#include "ShardingStrategy.h"
#include <spdlog/spdlog.h>
#include <spdlog/sinks/stdout_color_sinks.h>
#include <nlohmann/json.hpp>
#include <functional>
#include <cstdint>
#include <memory>
#include <unordered_map>
#include <vector>
#include <string>
#include <mutex>
#include <shared_mutex>

namespace risk_engine {
namespace infrastructure {

namespace {
    spdlog::logger* GetLogger() {
        static auto logger = spdlog::stdout_color_mt("ShardingStrategy");
        return logger.get();
    }
    
    constexpr uint64_t kDefaultShardCount = 16;
    constexpr uint64_t kMinShardCount = 1;
    constexpr uint64_t kMaxShardCount = 256;
}

ShardingStrategy::ShardingStrategy()
    : shard_count_(kDefaultShardCount)
    , strategy_type_(ShardingType::kConsistentHash)
    , enable_rebalance_(true)
    , rebalance_threshold_(0.2)
    , virtual_nodes_(150)
    , use_instrument_class_(false) {
    InitializeHashRing();
}

ShardingStrategy& ShardingStrategy::WithShardCount(uint64_t count) {
    if (count < kMinShardCount) {
        GetLogger()->warn("Shard count {} too small, using minimum {}", 
                          count, kMinShardCount);
        count = kMinShardCount;
    } else if (count > kMaxShardCount) {
        GetLogger()->warn("Shard count {} too large, using maximum {}", 
                          count, kMaxShardCount);
        count = kMaxShardCount;
    }
    shard_count_ = count;
    InitializeHashRing();
    return *this;
}

ShardingStrategy& ShardingStrategy::WithStrategy(ShardingType type) {
    strategy_type_ = type;
    InitializeHashRing();
    return *this;
}

ShardingStrategy& ShardingStrategy::WithRebalance(bool enable) {
    enable_rebalance_ = enable;
    return *this;
}

ShardingStrategy& ShardingStrategy::WithRebalanceThreshold(double threshold) {
    if (threshold < 0.0 || threshold > 1.0) {
        GetLogger()->warn("Rebalance threshold {} out of range [0,1], using 0.2", threshold);
        threshold = 0.2;
    }
    rebalance_threshold_ = threshold;
    return *this;
}

ShardingStrategy& ShardingStrategy::WithVirtualNodes(uint32_t vnodes) {
    virtual_nodes_ = vnodes;
    InitializeHashRing();
    return *this;
}

ShardingStrategy& ShardingStrategy::WithInstrumentClass(bool use_class) {
    use_instrument_class_ = use_class;
    return *this;
}

uint64_t ShardingStrategy::GetShardForInstrument(const std::string& instrument_id) const {
    switch (strategy_type_) {
        case ShardingType::kModulo:
            return GetShardModulo(instrument_id);
            
        case ShardingType::kConsistentHash:
            return GetShardConsistentHash(instrument_id);
            
        case ShardingType::kInstrumentClass:
            return GetShardByInstrumentClass(instrument_id);
            
        case ShardingType::kComposite:
            return GetShardComposite(instrument_id);
            
        default:
            GetLogger()->warn("Unknown sharding type, using modulo");
            return GetShardModulo(instrument_id);
    }
}

uint64_t ShardingStrategy::GetShardForOrder(
    const std::string& instrument_id,
    const std::string& trader_id,
    const std::string& strategy_id) const {
    
    if (strategy_type_ == ShardingType::kComposite) {
        std::string composite_key = instrument_id + "|" + trader_id + "|" + strategy_id;
        return GetShardConsistentHash(composite_key);
    }
    
    return GetShardForInstrument(instrument_id);
}

uint64_t ShardingStrategy::GetShardConsistentHash(const std::string& key) const {
    uint64_t hash = ComputeMurmurHash(key);
    
    auto it = hash_ring_.lower_bound(hash);
    if (it == hash_ring_.end()) {
        it = hash_ring_.begin();
    }
    
    return it->second;
}

uint64_t ShardingStrategy::GetShardModulo(const std::string& key) const {
    uint64_t hash = ComputeMurmurHash(key);
    return hash % shard_count_;
}

uint64_t ShardingStrategy::GetShardByInstrumentClass(const std::string& instrument_id) const {
    if (!use_instrument_class_) {
        return GetShardModulo(instrument_id);
    }
    
    if (instrument_id.length() < 2) {
        return GetShardModulo(instrument_id);
    }
    
    char prefix = instrument_id[0];
    
    if (prefix >= 'A' && prefix <= 'Z') {
        return (prefix - 'A') % shard_count_;
    }
    
    if (prefix >= '0' && prefix <= '9') {
        return (10 + (prefix - '0')) % shard_count_;
    }
    
    return GetShardModulo(instrument_id);
}

uint64_t ShardingStrategy::GetShardComposite(const std::string& instrument_id) const {
    return GetShardModulo(instrument_id);
}

uint64_t ShardingStrategy::ComputeMurmurHash(const std::string& key) const {
    const uint64_t m = 0xc6a4a7935bd1e995ULL;
    const int r = 47;
    
    uint64_t h = key.length() * m;
    
    const uint64_t* data = reinterpret_cast<const uint64_t*>(key.data());
    const uint64_t* end = data + (key.length() / 8);
    
    while (data != end) {
        uint64_t k = *data++;
        
        k *= m;
        k ^= k >> r;
        k *= m;
        
        h ^= k;
        h *= m;
    }
    
    const unsigned char* tail = reinterpret_cast<const unsigned char*>(data);
    
    switch (key.length() & 7) {
        case 7: h ^= uint64_t(tail[6]) << 48;
        [[fallthrough]];
        case 6: h ^= uint64_t(tail[5]) << 40;
        [[fallthrough]];
        case 5: h ^= uint64_t(tail[4]) << 32;
        [[fallthrough]];
        case 4: h ^= uint64_t(tail[3]) << 24;
        [[fallthrough]];
        case 3: h ^= uint64_t(tail[2]) << 16;
        [[fallthrough]];
        case 2: h ^= uint64_t(tail[1]) << 8;
        [[fallthrough]];
        case 1: h ^= uint64_t(tail[0]);
                h *= m;
    };
    
    h ^= h >> r;
    h *= m;
    h ^= h >> r;
    
    return h;
}

void ShardingStrategy::InitializeHashRing() {
    hash_ring_.clear();
    
    if (strategy_type_ != ShardingType::kConsistentHash) {
        return;
    }
    
    for (uint64_t shard = 0; shard < shard_count_; ++shard) {
        for (uint32_t vnode = 0; vnode < virtual_nodes_; ++vnode) {
            std::string key = "shard_" + std::to_string(shard) + 
                            "_vnode_" + std::to_string(vnode);
            uint64_t hash = ComputeMurmurHash(key);
            hash_ring_[hash] = shard;
        }
    }
    
    GetLogger()->debug("Hash ring inicializado con {} shards y {} virtual nodes por shard",
                      shard_count_, virtual_nodes_);
}

std::vector<uint64_t> ShardingStrategy::GetActiveShards() const {
    std::shared_lock<std::shared_mutex> lock(mutex_);
    
    std::vector<uint64_t> active_shards;
    active_shards.reserve(shard_metadata_.size());
    
    for (const auto& [shard_id, metadata] : shard_metadata_) {
        if (metadata.is_active) {
            active_shards.push_back(shard_id);
        }
    }
    
    return active_shards;
}

void ShardingStrategy::RegisterShard(uint64_t shard_id, ShardMetadata metadata) {
    std::unique_lock<std::shared_mutex> lock(mutex_);
    
    if (shard_id >= shard_count_) {
        GetLogger()->error("Shard ID {} excede el límite de shards {}", 
                          shard_id, shard_count_);
        return;
    }
    
    metadata.is_active = true;
    metadata.registered_at = std::chrono::steady_clock::now();
    shard_metadata_[shard_id] = metadata;
    
    GetLogger()->info("Shard {} registrado - Motor: {}, Capacity: {}",
                     shard_id, metadata.engine_id, metadata.capacity);
}

void ShardingStrategy::UnregisterShard(uint64_t shard_id) {
    std::unique_lock<std::shared_mutex> lock(mutex_);
    
    auto it = shard_metadata_.find(shard_id);
    if (it != shard_metadata_.end()) {
        it->second.is_active = false;
        it->second.unregistered_at = std::chrono::steady_clock::now();
        GetLogger()->warn("Shard {} desregistrado", shard_id);
    }
}

bool ShardingStrategy::ShouldRebalance() const {
    if (!enable_rebalance_) {
        return false;
    }
    
    std::shared_lock<std::shared_mutex> lock(mutex_);
    
    size_t active_count = 0;
    size_t total_capacity = 0;
    size_t total_load = 0;
    
    for (const auto& [shard_id, metadata] : shard_metadata_) {
        if (metadata.is_active) {
            active_count++;
            total_capacity += metadata.capacity;
            total_load += metadata.current_load;
        }
    }
    
    if (active_count == 0 || total_capacity == 0) {
        return false;
    }
    
    double avg_load = static_cast<double>(total_load) / active_count;
    double max_load = 0;
    
    for (const auto& [shard_id, metadata] : shard_metadata_) {
        if (metadata.is_active) {
            double load_ratio = static_cast<double>(metadata.current_load) / metadata.capacity;
            max_load = std::max(max_load, load_ratio);
        }
    }
    
    double imbalance = (max_load - avg_load) / avg_load;
    return imbalance > rebalance_threshold_;
}

bool ShardingStrategy::Validate() const {
    bool valid = true;
    
    if (shard_count_ < kMinShardCount || shard_count_ > kMaxShardCount) {
        GetLogger()->error("Shard count inválido: {}", shard_count_);
        valid = false;
    }
    
    if (strategy_type_ == ShardingType::kConsistentHash && virtual_nodes_ < 50) {
        GetLogger()->warn("Virtual nodes muy bajo ({}) para consistent hash", virtual_nodes_);
    }
    
    if (rebalance_threshold_ < 0.0 || rebalance_threshold_ > 1.0) {
        GetLogger()->error("Rebalance threshold inválido: {}", rebalance_threshold_);
        valid = false;
    }
    
    return valid;
}

ShardingStrategy ShardingStrategy::FromJson(const nlohmann::json& config) {
    ShardingStrategy builder;
    
    if (config.contains("shardCount")) {
        builder.WithShardCount(config["shardCount"].get<uint64_t>());
    }
    
    if (config.contains("strategy")) {
        std::string strategy = config["strategy"].get<std::string>();
        if (strategy == "modulo") builder.WithStrategy(ShardingType::kModulo);
        else if (strategy == "consistent") builder.WithStrategy(ShardingType::kConsistentHash);
        else if (strategy == "instrumentClass") builder.WithStrategy(ShardingType::kInstrumentClass);
        else if (strategy == "composite") builder.WithStrategy(ShardingType::kComposite);
    }
    
    if (config.contains("enableRebalance")) {
        builder.WithRebalance(config["enableRebalance"].get<bool>());
    }
    
    if (config.contains("rebalanceThreshold")) {
        builder.WithRebalanceThreshold(config["rebalanceThreshold"].get<double>());
    }
    
    if (config.contains("virtualNodes")) {
        builder.WithVirtualNodes(config["virtualNodes"].get<uint32_t>());
    }
    
    if (config.contains("useInstrumentClass")) {
        builder.WithInstrumentClass(config["useInstrumentClass"].get<bool>());
    }
    
    GetLogger()->info("ShardingStrategy cargado desde JSON");
    return builder;
}

ShardingStrategy::MetricSnapshot ShardingStrategy::GetMetrics() const {
    MetricSnapshot snapshot;
    snapshot.shard_count = shard_count_;
    snapshot.strategy_type = strategy_type_;
    snapshot.virtual_nodes = virtual_nodes_;
    snapshot.rebalance_enabled = enable_rebalance_;
    
    std::shared_lock<std::shared_mutex> lock(mutex_);
    snapshot.active_shards = 0;
    snapshot.total_capacity = 0;
    snapshot.total_load = 0;
    
    for (const auto& [shard_id, metadata] : shard_metadata_) {
        if (metadata.is_active) {
            snapshot.active_shards++;
            snapshot.total_capacity += metadata.capacity;
            snapshot.total_load += metadata.current_load;
        }
    }
    
    return snapshot;
}

} // namespace infrastructure
} // namespace risk_engine
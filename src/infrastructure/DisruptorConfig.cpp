#include "DisruptorConfig.h"
#include <disruptor/ring_buffer.h>
#include <disruptor/event_processor.h>
#include <disruptor/thread_affinity_calculator.h>
#include <spdlog/spdlog.h>
#include <spdlog/sinks/stdout_color_sinks.h>
#include <nlohmann/json.hpp>
#include <cstdint>
#include <thread>
#include <vector>
#include <memory>
#include <atomic>
#include <chrono>
#include <sched.h>

namespace risk_engine {
namespace infrastructure {

constexpr size_t kDefaultRingBufferSize = 1 << 20; // 1M eventos
constexpr size_t kMinRingBufferSize = 1 << 10;    // 1024 eventos mínimo
constexpr size_t kMaxRingBufferSize = 1 << 26;    // 64M eventos máximo

namespace {
    spdlog::logger* GetLogger() {
        static auto logger = spdlog::stdout_color_mt("DisruptorConfig");
        return logger.get();
    }
}

DisruptorConfig::DisruptorConfig() 
    : ring_buffer_size_(kDefaultRingBufferSize)
    , wait_strategy_type_(WaitStrategyType::kBlocking)
    , enable_thread_affinity_(true)
    , consumer_count_(std::thread::hardware_concurrency())
    , producer_type_(ProducerType::kMulti)
    , enable_metrics_(true)
    , spin_threshold_us_(0)
    , log_level_(spdlog::level::info) {
}

DisruptorConfig& DisruptorConfig::WithRingBufferSize(size_t size) {
    if (size < kMinRingBufferSize) {
        GetLogger()->warn("Ring buffer size {} too small, using minimum {}", 
                          size, kMinRingBufferSize);
        size = kMinRingBufferSize;
    } else if (size > kMaxRingBufferSize) {
        GetLogger()->warn("Ring buffer size {} too large, using maximum {}", 
                          size, kMaxRingBufferSize);
        size = kMaxRingBufferSize;
    } else if ((size & (size - 1)) != 0) {
        GetLogger()->warn("Ring buffer size {} not power of 2, rounding up", size);
        size = 1ULL << (64 - __builtin_clzll(size));
    }
    ring_buffer_size_ = size;
    return *this;
}

DisruptorConfig& DisruptorConfig::WithWaitStrategy(WaitStrategyType strategy) {
    wait_strategy_type_ = strategy;
    return *this;
}

DisruptorConfig& DisruptorConfig::WithThreadAffinity(bool enable) {
    enable_thread_affinity_ = enable;
    return *this;
}

DisruptorConfig& DisruptorConfig::WithConsumerCount(size_t count) {
    if (count == 0) {
        GetLogger()->warn("Consumer count cannot be 0, using 1");
        count = 1;
    }
    consumer_count_ = count;
    return *this;
}

DisruptorConfig& DisruptorConfig::WithProducerType(ProducerType type) {
    producer_type_ = type;
    return *this;
}

DisruptorConfig& DisruptorConfig::WithMetrics(bool enable) {
    enable_metrics_ = enable;
    return *this;
}

DisruptorConfig& DisruptorConfig::WithSpinThreshold(uint64_t microseconds) {
    spin_threshold_us_ = microseconds;
    return *this;
}

DisruptorConfig& DisruptorConfig::WithLogLevel(spdlog::level::level_enum level) {
    log_level_ = level;
    GetLogger()->set_level(level);
    return *this;
}

std::unique_ptr<disruptor::RingBuffer<RiskEvent>> 
DisruptorConfig::BuildRingBuffer() const {
    auto ring_buffer = std::make_unique<disruptor::RingBuffer<RiskEvent>>(
        ring_buffer_size_, 
        producer_type_ == ProducerType::kSingle 
            ? disruptor::ProducerType::Single 
            : disruptor::ProducerType::Multi);
    
    GetLogger()->info("Ring buffer creado con tamaño {} para {} consumidores",
                      ring_buffer_size_, consumer_count_);
    return ring_buffer;
}

std::unique_ptr<disruptor::WaitStrategy> 
DisruptorConfig::BuildWaitStrategy() const {
    std::unique_ptr<disruptor::WaitStrategy> strategy;
    
    switch (wait_strategy_type_) {
        case WaitStrategyType::kBlocking:
            strategy = std::make_unique<disruptor::BlockingWaitStrategy>();
            GetLogger()->debug("Estrategia de espera: Blocking");
            break;
            
        case WaitStrategyType::kSpinWait:
            strategy = std::make_unique<disruptor::SpinWaitWaitStrategy>();
            GetLogger()->debug("Estrategia de espera: SpinWait");
            break;
            
        case WaitStrategyType::kSpinYield:
            strategy = std::make_unique<disruptor::YieldingWaitStrategy>();
            GetLogger()->debug("Estrategia de espera: Yielding");
            break;
            
        case WaitStrategyType::kSleepingWait:
            strategy = std::make_unique<disruptor::SleepingWaitStrategy>();
            GetLogger()->debug("Estrategia de espera: Sleeping");
            break;
            
        case WaitStrategyType::kPhasedBackoff:
            strategy = std::make_unique<disruptor::PhasedBackoffWaitStrategy>(
                spin_threshold_us_, 
                std::chrono::milliseconds(100),
                std::chrono::microseconds(1000));
            GetLogger()->debug("Estrategia de espera: PhasedBackoff");
            break;
            
        default:
            GetLogger()->warn("Estrategia desconocida, usando Blocking por defecto");
            strategy = std::make_unique<disruptor::BlockingWaitStrategy>();
    }
    
    return strategy;
}

std::vector<std::thread> 
DisruptorConfig::StartConsumers(
    RingBufferPtr ring_buffer,
    std::function<void(RiskEvent&)> handler) const {
    
    std::vector<std::thread> threads;
    threads.reserve(consumer_count_);
    
    for (size_t i = 0; i < consumer_count_; ++i) {
        threads.emplace_back([this, ring_buffer, handler, i]() {
            if (enable_thread_affinity_) {
                ConfigureThreadAffinity(i);
            }
            
            auto sequence = ring_buffer->NewConsumerBarrier();
            
            while (true) {
                auto available = sequence->WaitFor(ring_buffer->GetCursor());
                
                while (sequence->GetCurrent() < available) {
                    auto event = ring_buffer->Get(sequence->GetCurrent());
                    try {
                        handler(*event);
                    } catch (const std::exception& e) {
                        GetLogger()->error("Error procesando evento en consumer {}: {}", 
                                          i, e.what());
                    }
                    sequence->Increment();
                }
                
                if (enable_metrics_) {
                    RecordConsumerMetrics(i, available - sequence->GetCurrent());
                }
            }
        });
    }
    
    GetLogger()->info("Iniciados {} consumidores con thread affinity {}",
                      consumer_count_, enable_thread_affinity_ ? "habilitado" : "deshabilitado");
    return threads;
}

void DisruptorConfig::ConfigureThreadAffinity(size_t consumer_id) const {
    cpu_set_t cpuset;
    CPU_ZERO(&cpuset);
    
    size_t core = (consumer_id * 2) % std::thread::hardware_concurrency();
    CPU_SET(core, &cpuset);
    
    pthread_t current_thread = pthread_self();
    int result = pthread_setaffinity_np(current_thread, sizeof(cpu_set_t), &cpuset);
    
    if (result != 0) {
        GetLogger()->warn("Error configurando thread affinity para consumer {}: {}",
                          consumer_id, result);
    } else {
        GetLogger()->debug("Consumer {} asignado al core {}", consumer_id, core);
    }
}

void DisruptorConfig::RecordConsumerMetrics(size_t consumer_id, size_t batch_size) const {
    static std::atomic<uint64_t> total_events{0};
    static std::atomic<uint64_t> total_batches{0};
    
    total_events.fetch_add(batch_size, std::memory_order_relaxed);
    total_batches.fetch_add(1, std::memory_order_relaxed);
    
    if (total_batches.load(std::memory_order_relaxed) % 10000 == 0) {
        GetLogger()->info("Consumer {} - Lotes procesados: {}, Eventos totales: {}",
                          consumer_id, 
                          total_batches.load(std::memory_order_relaxed),
                          total_events.load(std::memory_order_relaxed));
    }
}

bool DisruptorConfig::Validate() const {
    bool valid = true;
    
    if (ring_buffer_size_ < kMinRingBufferSize || ring_buffer_size_ > kMaxRingBufferSize) {
        GetLogger()->error("Ring buffer size inválido: {}", ring_buffer_size_);
        valid = false;
    }
    
    if (consumer_count_ == 0) {
        GetLogger()->error("Consumer count no puede ser 0");
        valid = false;
    }
    
    if (wait_strategy_type_ == WaitStrategyType::kPhasedBackoff && spin_threshold_us_ == 0) {
        GetLogger()->warn("PhasedBackoff sin spin threshold puede degradar rendimiento");
    }
    
    return valid;
}

DisruptorConfig DisruptorConfig::FromJson(const nlohmann::json& config) {
    DisruptorConfig builder;
    
    if (config.contains("ringBufferSize")) {
        builder.WithRingBufferSize(config["ringBufferSize"].get<size_t>());
    }
    
    if (config.contains("waitStrategy")) {
        std::string strategy = config["waitStrategy"].get<std::string>();
        if (strategy == "blocking") builder.WithWaitStrategy(WaitStrategyType::kBlocking);
        else if (strategy == "spin") builder.WithWaitStrategy(WaitStrategyType::kSpinWait);
        else if (strategy == "yield") builder.WithWaitStrategy(WaitStrategyType::kSpinYield);
        else if (strategy == "sleep") builder.WithWaitStrategy(WaitStrategyType::kSleepingWait);
        else if (strategy == "phased") builder.WithWaitStrategy(WaitStrategyType::kPhasedBackoff);
    }
    
    if (config.contains("threadAffinity")) {
        builder.WithThreadAffinity(config["threadAffinity"].get<bool>());
    }
    
    if (config.contains("consumerCount")) {
        builder.WithConsumerCount(config["consumerCount"].get<size_t>());
    }
    
    if (config.contains("producerType")) {
        std::string type = config["producerType"].get<std::string>();
        builder.WithProducerType(type == "single" ? ProducerType::kSingle : ProducerType::kMulti);
    }
    
    if (config.contains("metrics")) {
        builder.WithMetrics(config["metrics"].get<bool>());
    }
    
    if (config.contains("spinThresholdUs")) {
        builder.WithSpinThreshold(config["spinThresholdUs"].get<uint64_t>());
    }
    
    GetLogger()->info("DisruptorConfig cargado desde JSON");
    return builder;
}

nDisruptorConfig::MetricSnapshot DisruptorConfig::GetMetrics() const {
    MetricSnapshot snapshot;
    snapshot.ring_buffer_size = ring_buffer_size_;
    snapshot.consumer_count = consumer_count_;
    snapshot.wait_strategy_type = wait_strategy_type_;
    snapshot.thread_affinity_enabled = enable_thread_affinity_;
    snapshot.producer_type = producer_type_;
    return snapshot;
}

} // namespace infrastructure
} // namespace risk_engine
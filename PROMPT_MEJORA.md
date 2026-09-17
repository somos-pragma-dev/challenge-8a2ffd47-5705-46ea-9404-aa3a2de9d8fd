# Prompt para Mejorar el Codigo Base

Copia y pega el contenido del bloque de abajo en un asistente de IA (Claude, ChatGPT)
para obtener un ZIP con el proyecto completo y arrancable.

Si preferis trabajar en tu editor con un agente local (Claude Code, Cursor, Copilot), usa `AGENTS.md` en vez de este archivo: dice lo mismo pero para que escriba los archivos en disco.

## Las dos reglas que no se negocian

1. **Completa el boilerplate.** Todo lo que el proyecto necesita para compilar y arrancar: manifiesto de dependencias, punto de entrada, configuracion, capa de interfaz, y las capas del patron arquitectonico declarado. Eso es andamiaje y es tu trabajo.
2. **NO resuelvas el reto.** Los entregables de las fases son el trabajo de la persona. El hueco pedagogico se deja como esta: el proyecto arranca, pero lo que el reto pide implementar NO esta implementado.

Dicho de otra forma: si algo impide compilar, arreglalo. Si algo es logica de negocio incompleta, validaciones ausentes, un secreto hardcodeado o un patron mejorable, dejalo exactamente como esta — es lo que la persona tiene que encontrar.

## Lo que le falta a este proyecto

Esto NO lo tenes que adivinar: salio de comparar el proyecto contra la arquitectura declarada del reto y de un analisis estatico del codigo. Completalo TODO.

### Boilerplate del stack que falta

Sin esto no compila ni arranca. Es andamiaje, no toca nada de lo pedagogico:

- **Punto de entrada del stack elegido** — Sin un punto de entrada reconocible, el runtime no tiene por donde arrancar la aplicacion.
- **Capa de interfaz (controller/handler)** — Sin una capa de interfaz explicita, no hay forma de invocar la logica de negocio desde afuera del proceso.

## Como saber que terminaste

```bash
el comando de build o arranque canonico del stack elegido
```

Ese comando corriendo sin errores es la definicion de "listo".

---

```
## Briefing del reto (autoridad)
Este bloque manda sobre los archivos adjuntos. El stack y el rol salen de AQUÍ, no de un topic genérico ni de markdown placeholder.

### Contexto técnico original
Sistema master-l2 que evalúa el riesgo de cada orden antes de enviarla al exchange, en menos de 500 microsegundos p99. Consume feed de market data (nivel 2 orderbook + trades), mantiene un modelo de VaR intraday, aplica límites por trader/estrategia/instrumento en tiempo real, y dispara circuit breakers dinámicos que bloquean nuevas órdenes cuando la exposición supera thresholds calibrados por volatilidad. Debe justificar el uso de estructuras lock-free vs mutex, la elección entre C++ vs Rust vs Java LMAX Disruptor, cómo garantiza la consistencia entre múltiples risk engines corriendo en paralelo (consensus vs sharding por instrument), la política de kill switch cuando detecta un algoritmo que se comporta de forma anómala, y la estrategia de replay determinístico para post-mortem de incidents. Incluye compliance con regulaciones MiFID II para trazabilidad de decisiones de riesgo.

### Reto
- Tema: motor de risk scoring en tiempo real para trading algorítmico con circuit breakers dinámicos
- Seniority: master-l2
- Tipo: mixed
- Título: Evaluación de riesgo en tiempo real con circuit breakers
- Tiempo estimado: 4 semanas

### Fases (trabajo del HUMANO — PROHIBIDO completarlas)
No implementes estos entregables. Dejalos como hueco pedagógico. El asistente solo materializa el proyecto arrancable para que el participante pueda trabajar.
- Fase 1: Exploración y modelado inicial — objetivo: Entender y modelar el dominio del trading algorítmico y sus requerimientos de riesgo en tiempo real. — entregable (NO resolver): Modelo conceptual del motor de riesgo y sus componentes.
- Fase 2: Implementación de la evaluación de riesgo — objetivo: Implementar el motor de evaluación de riesgo con los requerimientos de latencia y consistencia. — entregable (NO resolver): Motor de evaluación de riesgo funcional con latencia < 500 microsegundos p99.
- Fase 3: Circuit breakers y kill switch — objetivo: Implementar circuit breakers dinámicos y la política de kill switch para algoritmos anómalos. — entregable (NO resolver): Motor de riesgo con circuit breakers dinámicos y kill switch funcional.

Eres un asistente experto en análisis, corrección y generación de archivos de cualquier tipo:
código fuente, documentación, hojas de cálculo, documentos Word, configuraciones, entre otros.
Voy a enviarte una cadena de texto que contiene uno o más archivos. Cada archivo está delimitado por un marcador con el siguiente formato:
// === ARCHIVO: ruta/del/archivo.extension ===
o también puede aparecer como:
## === ARCHIVO: ruta/del/archivo.extension ===
Lo que sigue al marcador puede ser:

El contenido real del archivo (código, texto, YAML, etc.)
Una descripción en lenguaje natural de lo que debe contener el archivo


TU TAREA
PASO 0 — ¿Esto es un proyecto o una carcasa?
Antes de extraer archivos, leé el Briefing (si está) y diagnosticá el adjunto.

Es CARCASA si ocurre CUALQUIERA de estas:
- No hay manifiesto de dependencias del stack del briefing (manifest.json de VTEX IO / package.json / pom.xml / build.gradle / requirements.txt / go.mod / *.tf / *.csproj, según corresponda)
- Hay un "binario" que en realidad es un comentario ("no puede ser mostrado como texto plano", placeholder .fig/.docx vacío)
- Los markdowns ya completan entregables de fases posteriores ("se implementó fade-in", lista de áreas ya resuelta)

Si es CARCASA:
- MATERIALIZÁ un proyecto que arranca en el stack del briefing (VTEX IO Store Framework, Angular, Terraform, pytest, Nest, etc.). Incluí manifiesto, punto de entrada y capa de interfaz reales.
- NO copies los markdowns de "solución" como si fueran el producto. Son ruido de generación.
- NO resuelvas las fases del briefing (están marcadas PROHIBIDO). Dejá el hueco pedagógico: el flujo existe, las microinteracciones/calidad/infra que el reto pide NO están hechas.
- Después seguí al PASO 5 (ZIP).

Si es un proyecto REAL (manifiesto + código que compila o arranca):
- Seguí PASO 1 en adelante. 🔴 compilación sí. 🟡 pedagógico no.

PASO 1 — Detección y extracción
Identifica todos los archivos presentes en la cadena. Para cada archivo extrae:

Su ruta completa (ej: src/main/java/com/pragma/Service.java)
Su contenido o descripción

PASO 2 — Clasificación por tipo
Clasifica cada archivo en una de estas categorías:
A) Código fuente (Java, Python, TypeScript, JavaScript, Kotlin, etc.)
B) Configuración / documentación (YAML, properties, Markdown, JSON, txt, etc.)
C) Excel (.xlsx, .xls, .csv)
D) Word (.docx, .doc)
E) Otro tipo de archivo binario o especial
PASO 3 — Clasificación de errores en código fuente

Objetivo prioritario: que el proyecto compile. No corrijas flujo de negocio ni lógica funcional.

Antes de modificar cualquier archivo de código fuente, clasifica cada problema encontrado en una de estas dos categorías:
🔴 ERROR DE COMPILACIÓN — corregir siempre
Son errores que impiden que el proyecto arranque, sin valor pedagógico:

Import faltante o incorrecto
Clase, método o variable referenciada que no existe en ningún archivo del proyecto
Error de sintaxis
Anotación con atributos inválidos
Dependencia ausente en pom.xml, package.json, etc.
Archivo referenciado que no existe y debe ser creado con implementación mínima

→ CORREGIR estos errores.
🟡 PROBLEMA FUNCIONAL O DE CALIDAD — preservar siempre
Son problemas que no impiden compilar. Pueden ser intencionales para el aprendizaje:

Clave secreta hardcodeada ("secret", "password123")
API deprecada que funciona pero tiene reemplazo moderno
Lógica de negocio incorrecta o incompleta
Código redundante o de baja legibilidad
Falta de validaciones en flujo de negocio
Patrones de diseño incorrectos pero funcionales
Concurrencia no segura
Configuración funcional pero no óptima

→ PRESERVAR tal cual. No corregir, no mejorar, no comentar.
PASO 4 — Procesamiento según tipo de archivo
Tipo A — Código fuente
Aplica únicamente las correcciones clasificadas como 🔴 ERROR DE COMPILACIÓN.
No alteres ningún elemento clasificado como 🟡 PROBLEMA FUNCIONAL O DE CALIDAD.
Si falta un archivo referenciado, créalo con la implementación mínima necesaria para compilar.
Tipo B — Configuración / documentación
Extrae el contenido tal cual, sin modificaciones salvo errores evidentes de sintaxis
(ej: YAML mal indentado).
Tipo C — Excel (.xlsx)
Si viene con contenido real, genera el archivo respetando ese contenido.
Si viene con descripción en lenguaje natural, genera un archivo Excel funcional con:

Fila de encabezados en negrita con color de fondo distintivo
Columnas con ancho ajustado al contenido
Tipos de dato correctos por columna
Validaciones si la descripción lo indica
Hojas nombradas descriptivamente si hay más de una
Filas de ejemplo si no hay datos reales

Tipo D — Word (.docx)
Si viene con contenido real, genera el archivo respetando ese contenido.
Si viene con descripción en lenguaje natural, genera un documento Word funcional con:

Estilos de título (Título 1, Título 2) para jerarquía de secciones
Fuente legible (Calibri o equivalente), tamaño 11-12pt para cuerpo
Márgenes estándar
Tabla de contenido si tiene múltiples secciones
Tablas con encabezados en negrita si aplica

Tipo E — Otro
Genera el archivo con el contenido o estructura más apropiada según la descripción.
PASO 5 — Exportación en ZIP
Empaqueta todos los archivos en un único archivo ZIP descargable respetando exactamente
la estructura de rutas indicada por los marcadores.
El ZIP debe incluir:

Archivos de código con únicamente los errores de compilación corregidos
Archivos de configuración y documentación sin cambios
Archivos nuevos creados para resolver dependencias de compilación faltantes
Archivos Excel y Word generados desde descripción

IMPORTANTE: El ZIP debe estar listo para descargar al finalizar. No preguntes si el usuario
quiere generarlo. Simplemente genera el archivo y proporciona el enlace de descarga; No debes desplegar en el chat el resumen de lo que arreglaste al Zip, solo entregalo.

REGLAS IMPORTANTES

No omitas ningún archivo aunque no tenga errores ni modificaciones
Respeta los nombres y rutas exactas indicadas por los marcadores
Si un archivo no tiene marcador claro, infiere el nombre desde su contenido
Si la cadena contiene solo documentación, placeholders o binarios fake, NO la reproduzcas:
aplicá PASO 0 (materializar el proyecto del briefing). Reproducir la carcasa es un fallo.
No agregues texto después del enlace de descarga del ZIP
No preguntes si el usuario quiere el ZIP: simplemente generalo siempre
Si detectas que falta un archivo de configuración necesario para compilar
(pom.xml, package.json, requirements.txt, build.gradle, etc.), créalo e inclúyelo
inferiendo su contenido desde los imports y frameworks detectados en el código
Nunca corrijas problemas 🟡 aunque parezcan obvios o fáciles de mejorar.
El participante que recibirá este proyecto los debe encontrar y resolver él mismo.


INPUT
Aquí está la cadena con los archivos:

// === ARCHIVO: package.json ===
{
  "name": "risk-engine-lmax",
  "version": "1.0.0",
  "description": "Motor de risk scoring en tiempo real para trading algorítmico con circuit breakers dinámicos",
  "main": "src/main.cpp",
  "author": "Pragma Engineering",
  "license": "MIT",
  "scripts": {
    "build": "cmake -B build -DCMAKE_BUILD_TYPE=Release && cmake --build build -j$(nproc)",
    "build:debug": "cmake -B build -DCMAKE_BUILD_TYPE=Debug && cmake --build build -j$(nproc)",
    "clean": "rm -rf build",
    "test": "cmake --build build --target tests && ./build/test/RiskEngineTest",
    "test:verbose": "cmake --build build --target tests && ./build/test/RiskEngineTest -s",
    "lint": "clang-tidy src/**/*.cpp -- -DCMAKE_CXX_CLANG_TIDY=/usr/bin/clang-tidy",
    "format": "clang-format -i src/**/*.cpp src/**/*.h",
    "run:risk-engine": "./build/src/main",
    "replay": "./build/src/main --replay",
    "benchmark": "cmake --build build --target benchmark && ./build/test/benchmark",
    "coverage": "cmake -B build -DCMAKE_BUILD_TYPE=Debug -DENABLE_COVERAGE=ON && cmake --build build && lcov --capture --directory build --output-file coverage.info --include '*/src/*' && lcov --summary coverage.info"
  },
  "dependencies": {
    "@aspect-dev/rules_js": "^0.30.0",
    "aspect-build": "^0.1.0"
  },
  "devDependencies": {
    "cmake": "^3.28.0",
    "clang": "^18.0.0",
    "clang-tidy": "^18.0.0",
    "clang-format": "^18.0.0",
    "lcov": "^1.15",
    "@types/node": "^20.10.0",
    "typescript": "^5.3.0"
  },
  "build": {
    "tool": "cmake",
    "minCppStandard": "c++20",
    "compiler": "gcc",
    "compilerVersion": "13",
    "cmakeOptions": {
      "CMAKE_CXX_STANDARD": "20",
      "CMAKE_CXX_STANDARD_REQUIRED": "ON",
      "CMAKE_CXX_EXTENSIONS": "OFF",
      "CMAKE_BUILD_TYPE": "Release",
      "ENABLE_TESTING": "ON",
      "ENABLE_COVERAGE": "OFF",
      "DISRUPTOR_VERSION": "4.0.0",
      "JSON_VERSION": "3.11.2",
      "SPDLOG_VERSION": "1.12.0"
    },
    "dependencies": {
      "lmax-disruptor": {
        "version": "4.0.0",
        "source": "https://github.com/LMAX-Exchange/disruptor-cpp.git",
        "build": "cmake"
      },
      "nlohmann-json": {
        "version": "3.11.2",
        "source": "https://github.com/nlohmann/json.git",
        "headerOnly": true
      },
      "spdlog": {
        "version": "1.12.0",
        "source": "https://github.com/gabime/spdlog.git",
        "headerOnly": false
      },
      "gtest": {
        "version": "1.14.0",
        "source": "https://github.com/google/googletest.git",
        "build": "cmake"
      },
      "catch2": {
        "version": "3.5.0",
        "source": "https://github.com/catchorg/Catch2.git",
        "headerOnly": true
      }
    }
  },
  "architecture": {
    "pattern": "event-driven con ring buffer lock-free y sharding por instrumento",
    "directories": {
      "domain": "src/domain/",
      "application": "src/application/",
      "adapters": "src/adapters/",
      "infrastructure": "src/infrastructure/",
      "config": "config/",
      "docs": "docs/",
      "test": "test/"
    },
    "keyConventions": [
      "Uso de std::atomic para estructuras lock-free en C++20, evitando mutex para garantizar latencia < 500 microsegundos p99.",
      "Implementación de ring buffer con LMAX Disruptor 4.0 para manejo de eventos con baja latencia y alta concurrencia.",
      "Sharding por instrumento para escalabilidad y consistencia entre múltiples motores de riesgo.",
      "Modelo de VaR intraday con calibración dinámica de umbrales basada en volatilidad.",
      "Circuit breakers implementados como máquinas de estado finitas para bloquear órdenes cuando la exposición supera umbrales.",
      "Trazabilidad de decisiones de riesgo con timestamps precisos y logs inmutables para compliance MiFID II."
    ]
  },
  "compliance": {
    "mifid2": {
      "enabled": true,
      "auditLogPath": "logs/audit/",
      "retentionDays": 2555,
      "fields": ["timestamp", "traderId", "strategyId", "instrumentId", "orderId", "riskDecision", "exposure", "limit", "circuitBreakerState"]
    }
  },
  "riskEngine": {
    "latencyTargetUs": 500,
    "p99Target": 500,
    "shardingStrategy": "byInstrument",
    "varModel": "intraday",
    "calibration": "dynamic",
    "circuitBreaker": {
      "enabled": true,
      "states": ["CLOSED", "OPEN", "HALF_OPEN"],
      "thresholdMultiplier": 1.5,
      "resetTimeoutMs": 5000
    },
    "killSwitch": {
      "enabled": true,
      "anomalyDetectionWindowMs": 60000,
      "thresholdDeviation": 3.0
    }
  },
  "engines": {
    "available": [
      {
        "name": "gcc",
        "version": "13.2.0",
        "cxxStandard": "c++20"
      },
      {
        "name": "clang",
        "version": "18.1.0",
        "cxxStandard": "c++20"
      }
    ],
    "default": "gcc"
  },
  "keywords": [
    "risk-engine",
    "trading",
    "algorithmic-trading",
    "circuit-breaker",
    "lock-free",
    "low-latency",
    "lmax-disruptor",
    "mifid2",
    "var",
    "real-time-risk"
  ],
  "repository": {
    "type": "git",
    "url": "https://github.com/pragma-engineering/risk-engine-lmax"
  },
  "bugs": {
    "url": "https://github.com/pragma-engineering/risk-engine-lmax/issues"
  },
  "homepage": "https://github.com/pragma-engineering/risk-engine-lmax#readme"
}

// === ARCHIVO: src/domain/MarketData.h ===
#pragma once

#include <atomic>
#include <cstdint>
#include <array>
#include <vector>
#include <memory>
#include <optional>
#include <chrono>

namespace risk_engine {
namespace domain {

using Timestamp = std::chrono::microseconds;
using Price = int64_t;
using Quantity = int64_t;
constexpr auto PRICE_MULTIPLIER = 1000000LL;

enum class Side : uint8_t { BUY = 0, SELL = 1 };

enum class MarketDataType : uint8_t {
    BID = 0,
    ASK = 1,
    TRADE = 2,
    SNAPSHOT = 3
};

struct alignas(64) PriceLevel {
    Price price_{0};
    Quantity quantity_{0};
    uint32_t orderCount_{0};
    std::atomic<uint64_t> lastUpdate_{0};

    PriceLevel() noexcept = default;

    void update(Price price, Quantity quantity, uint32_t orders) noexcept {
        price_ = price;
        quantity_ = quantity;
        orderCount_ = orders;
        lastUpdate_.store(std::chrono::duration_cast<std::chrono::nanoseconds>(
            std::chrono::system_clock::now().time_since_epoch()).count(),
            std::memory_order_relaxed);
    }

    void clear() noexcept {
        price_ = 0;
        quantity_ = 0;
        orderCount_ = 0;
    }

    [[nodiscard]] bool isEmpty() const noexcept { return quantity_ == 0; }
};

template<size_t MaxLevels = 10>
struct alignas(64) OrderBookLevel2 {
    static constexpr size_t kMaxLevels = MaxLevels;

    std::array<PriceLevel, MaxLevels> bids_;
    std::array<PriceLevel, MaxLevels> asks_;

    std::atomic<uint64_t> sequence_{0};
    std::atomic<uint64_t> lastUpdateNs_{0};
    uint32_t instrumentId_{0};
    std::atomic<bool> isStale_{false};

    OrderBookLevel2() noexcept {
        bids_.fill(PriceLevel{});
        asks_.fill(PriceLevel{});
    }

    void updateBid(size_t level, Price price, Quantity quantity, uint32_t orders) noexcept {
        if (level < MaxLevels) {
            bids_[level].update(price, quantity, orders);
            updateTimestamp();
        }
    }

    void updateAsk(size_t level, Price price, Quantity quantity, uint32_t orders) noexcept {
        if (level < MaxLevels) {
            asks_[level].update(price, quantity, orders);
            updateTimestamp();
        }
    }

    void updateTimestamp() noexcept {
        lastUpdateNs_.store(
            std::chrono::duration_cast<std::chrono::nanoseconds>(
                std::chrono::system_clock::now().time_since_epoch()).count(),
            std::memory_order_release);
    }

    [[nodiscard]] Price bestBid() const noexcept {
        for (const auto& level : bids_) {
            if (!level.isEmpty()) return level.price_;
        }
        return 0;
    }

    [[nodiscard]] Price bestAsk() const noexcept {
        for (const auto& level : asks_) {
            if (!level.isEmpty()) return level.price_;
        }
        return 0;
    }

    [[nodiscard]] Price midPrice() const noexcept {
        Price bid = bestBid();
        Price ask = bestAsk();
        if (bid > 0 && ask > 0) {
            return (bid + ask) / 2;
        }
        return 0;
    }

    [[nodiscard]] Price spread() const noexcept {
        Price ask = bestAsk();
        Price bid = bestBid();
        if (ask > 0 && bid > 0) {
            return ask - bid;
        }
        return 0;
    }

    [[nodiscard]] Quantity bidLiquidity(size_t levels) const noexcept {
        Quantity total = 0;
        size_t count = std::min(levels, MaxLevels);
        for (size_t i = 0; i < count; ++i) {
            total += bids_[i].quantity_;
        }
        return total;
    }

    [[nodiscard]] Quantity askLiquidity(size_t levels) const noexcept {
        Quantity total = 0;
        size_t count = std::min(levels, MaxLevels);
        for (size_t i = 0; i < count; ++i) {
            total += asks_[i].quantity_;
        }
        return total;
    }

    void clear() noexcept {
        bids_.fill(PriceLevel{});
        asks_.fill(PriceLevel{});
        isStale_.store(false, std::memory_order_release);
    }

    void markStale() noexcept {
        isStale_.store(true, std::memory_order_release);
    }

    [[nodiscard]] bool isStale() const noexcept {
        return isStale_.load(std::memory_order_acquire);
    }
};

struct alignas(64) Trade {
    uint64_t tradeId_{0};
    uint32_t instrumentId_{0};
    uint32_t venueId_{0};
    Price price_{0};
    Quantity quantity_{0};
    Side side_{Side::BUY};
    Timestamp timestamp_{};
    uint64_t sequence_{0};

    Trade() noexcept = default;

    [[nodiscard]] bool isValid() const noexcept {
        return tradeId_ > 0 && instrumentId_ > 0 && quantity_ > 0 && price_ > 0;
    }

    [[nodiscard]] int64_t notionalValue() const noexcept {
        return static_cast<int64_t>(quantity_) * price_ / PRICE_MULTIPLIER;
    }
};

struct alignas(64) MarketDataEvent {
    static constexpr size_t kMaxTradesPerEvent = 100;

    MarketDataType type_{MarketDataDataType::SNAPSHOT};
    uint32_t instrumentId_{0};
    Timestamp timestamp_{};
    uint64_t sequence_{0};

    union {
        OrderBookLevel2<10>* orderBook_;
        Trade* trade_;
    };

    bool hasOrderBook_{false};
    std::vector<Trade> trades_;

    MarketDataEvent() noexcept : orderBook_(nullptr), hasOrderBook_(false) {}

    ~MarketDataEvent() {
        if (hasOrderBook_ && orderBook_) {
            delete orderBook_;
            orderBook_ = nullptr;
        }
    }

    void setOrderBook(OrderBookLevel2<10>* ob) noexcept {
        orderBook_ = ob;
        hasOrderBook_ = true;
        type_ = MarketDataType::SNAPSHOT;
    }

    void addTrade(const Trade& trade) noexcept {
        if (trades_.size() < kMaxTradesPerEvent) {
            trades_.push_back(trade);
            if (trades_.size() == 1) {
                type_ = MarketDataType::TRADE;
            }
        }
    }

    [[nodiscard]] bool hasData() const noexcept {
        return hasOrderBook_ || !trades_.empty();
    }

    void clear() noexcept {
        if (hasOrderBook_ && orderBook_) {
            delete orderBook_;
            orderBook_ = nullptr;
            hasOrderBook_ = false;
        }
        trades_.clear();
        type_ = MarketDataType::SNAPSHOT;
    }
};

class MarketDataCache {
public:
    static constexpr size_t kMaxInstruments = 10000;

    explicit MarketDataCache() noexcept : cache_(kMaxInstruments) {
        for (auto& ob : cache_) {
            ob = std::make_unique<OrderBookLevel2<10>>();
        }
    }

    [[nodiscard]] OrderBookLevel2<10>* getOrderBook(uint32_t instrumentId) noexcept {
        if (instrumentId < kMaxInstruments) {
            return cache_[instrumentId].get();
        }
        return nullptr;
    }

    [[nodiscard]] const OrderBookLevel2<10>* getOrderBook(uint32_t instrumentId) const noexcept {
        if (instrumentId < kMaxInstruments) {
            return cache_[instrumentId].get();
        }
        return nullptr;
    }

    void updateFromEvent(const MarketDataEvent& event) noexcept {
        if (event.hasOrderBook_ && event.orderBook_) {
            auto* target = getOrderBook(event.instrumentId_);
            if (target) {
                *target = *event.orderBook_;
                target->instrumentId_ = event.instrumentId_;
                target->sequence_.store(event.sequence_, std::memory_order_release);
            }
        }
        for (const auto& trade : event.trades_) {
            processTrade(trade);
        }
    }

private:
    void processTrade(const Trade& trade) noexcept {
        auto* ob = getOrderBook(trade.instrumentId_);
        if (!ob) return;

        ob->updateTimestamp();
        auto seq = ob->sequence_.load(std::memory_order_relaxed);
        ob->sequence_.store(seq + 1, std::memory_order_release);
    }

    std::vector<std::unique_ptr<OrderBookLevel2<10>>> cache_;
};

}
}
// === ARCHIVO: src/core/RiskEngine.h ===
#pragma once

#include <memory>
#include <atomic>
#include <cstdint>
#include <optional>
#include <functional>
#include <vector>
#include <string_view>

#include "domain/MarketData.h"
#include "domain/RiskLimits.h"
#include "domain/VaRModel.h"

namespace risk_engine {
namespace core {

enum class RiskDecision : uint8_t {
    ACCEPT = 0,
    REJECT = 1,
    REDUCE = 2,
    REVIEW = 3
};

enum class CircuitBreakerState : uint8_t {
    CLOSED = 0,
    OPEN = 1,
    HALF_OPEN = 2
};

struct RiskEvaluationResult {
    RiskDecision decision_{RiskDecision::ACCEPT};
    double exposure_{0.0};
    double limit_{0.0};
    double utilizationPct_{0.0};
    CircuitBreakerState circuitBreaker_{CircuitBreakerState::CLOSED};
    bool killSwitchTriggered_{false};
    std::string_view rejectionReason_;
    uint64_t evaluationLatencyNs_{0};
    Timestamp timestamp_{};
};

struct Order {
    uint64_t orderId_{0};
    uint32_t instrumentId_{0};
    uint32_t traderId_{0};
    uint32_t strategyId_{0};
    Side side_{Side::BUY};
    Price price_{0};
    Quantity quantity_{0};
    OrderType type_{OrderType::LIMIT};
    Timestamp timestamp_{};

    [[nodiscard]] bool isValid() const noexcept {
        return orderId_ > 0 && instrumentId_ > 0 && quantity_ > 0 && price_ > 0;
    }

    [[nodiscard]] int64_t notional() const noexcept {
        return static_cast<int64_t>(quantity_) * price_ / PRICE_MULTIPLIER;
    }
};

using RiskEventHandler = std::function<void(const RiskEvaluationResult&)>;

class IRiskEngine {
public:
    virtual ~IRiskEngine() = default;

    [[nodiscard]] virtual RiskEvaluationResult evaluateOrder(const Order& order) = 0;

    virtual void onMarketData(const domain::MarketDataEvent& event) = 0;

    [[nodiscard]] virtual CircuitBreakerState getCircuitBreakerState(uint32_t instrumentId) const = 0;

    [[nodiscard]] virtual bool isKillSwitchActive() const = 0;

    virtual void resetCircuitBreaker(uint32_t instrumentId) = 0;

    virtual void resetKillSwitch() = 0;

    virtual void setRiskEventHandler(RiskEventHandler handler) = 0;

    [[nodiscard]] virtual uint64_t getProcessedOrdersCount() const = 0;

    [[nodiscard]] virtual uint64_t getRejectedOrdersCount() const = 0;

    [[nodiscard]] virtual double getAvgLatencyUs() const = 0;
};

class RiskEngine final : public IRiskEngine {
public:
    explicit RiskEngine(
        std::shared_ptr<domain::MarketDataCache> marketDataCache,
        std::shared_ptr<domain::RiskLimits> riskLimits,
        std::shared_ptr<domain::VaRModel> varModel,
        uint32_t shardId = 0) noexcept;

    RiskEngine(const RiskEngine&) = delete;
    RiskEngine& operator=(const RiskEngine&) = delete;
    RiskEngine(RiskEngine&&) = delete;
    RiskEngine& operator=(RiskEngine&&) = delete;

    [[nodiscard]] RiskEvaluationResult evaluateOrder(const Order& order) override;

    void onMarketData(const domain::MarketDataEvent& event) override;

    [[nodiscard]] CircuitBreakerState getCircuitBreakerState(uint32_t instrumentId) const override;

    [[nodiscard]] bool isKillSwitchActive() const override;

    void resetCircuitBreaker(uint32_t instrumentId) override;

    void resetKillSwitch() override;

    void setRiskEventHandler(RiskEventHandler handler) override;

    [[nodiscard]] uint64_t getProcessedOrdersCount() const override;

    [[nodiscard]] uint64_t getRejectedOrdersCount() const override;

    [[nodiscard]] double getAvgLatencyUs() const override;

private:
    [[nodiscard]] RiskEvaluationResult evaluateRiskLimits(const Order& order) noexcept;

    [[nodiscard]] RiskEvaluationResult evaluateVaR(const Order& order) noexcept;

    [[nodiscard]] RiskEvaluationResult checkCircuitBreaker(uint32_t instrumentId) noexcept;

    void updateCircuitBreakerState(uint32_t instrumentId, bool breached) noexcept;

    void checkKillSwitchAnomaly() noexcept;

    void emitRiskEvent(const RiskEvaluationResult& result) noexcept;

    void recordLatency(uint64_t latencyNs) noexcept;

    std::shared_ptr<domain::MarketDataCache> marketDataCache_;
    std::shared_ptr<domain::RiskLimits> riskLimits_;
    std::shared_ptr<domain::VaRModel> varModel_;

    const uint32_t shardId_;

    struct alignas(64) CircuitBreakerData {
        std::atomic<CircuitBreakerState> state_{CircuitBreakerState::CLOSED};
        std::atomic<uint64_t> breachCount_{0};
        std::atomic<uint64_t> lastBreachTimestamp_{0};
        std::atomic<uint64_t> lastResetTimestamp_{0};

        void reset() noexcept {
            state_.store(CircuitBreakerState::CLOSED, std::memory_order_release);
            breachCount_.store(0, std::memory_order_release);
            lastResetTimestamp_.store(
                std::chrono::duration_cast<std::chrono::milliseconds>(
                    std::chrono::system_clock::now().time_since_epoch()).count(),
                std::memory_order_release);
        }
    };

    static constexpr size_t kMaxInstruments = 10000;
    std::vector<CircuitBreakerData> circuitBreakers_;

    std::atomic<bool> killSwitchActive_{false};
    std::atomic<uint64_t> killSwitchTriggerCount_{0};
    std::atomic<uint64_t> anomalyWindowStart_{0};
    std::atomic<uint64_t> anomalyCount_{0};

    std::atomic<uint64_t> processedOrders_{0};
    std::atomic<uint64_t> rejectedOrders_{0};

    static constexpr size_t kLatencyWindowSize = 1000;
    std::vector<std::atomic<uint64_t>> latencyWindow_;
    std::atomic<size_t> latencyIndex_{0};

    std::optional<RiskEventHandler> eventHandler_;

    static constexpr uint64_t kCircuitBreakerResetTimeoutMs = 5000;
    static constexpr double kCircuitBreakerThresholdMultiplier = 1.5;
    static constexpr uint64_t kAnomalyWindowMs = 60000;
    static constexpr double kAnomalyThresholdDeviation = 3.0;
};

}
}
// === ARCHIVO: src/core/RiskEngine.cpp ===
#include "RiskEngine.h"

#include <algorithm>
#include <cmath>
#include <chrono>
#include <stdexcept>

#include <spdlog/spdlog.h>

namespace risk_engine {
namespace core {

namespace {

constexpr std::string_view kRejectionReasonLimitExceeded{"LIMIT_EXCEEDED"};
constexpr std::string_view kRejectionReasonCircuitBreaker{"CIRCUIT_BREAKER_OPEN"};
constexpr std::string_view kRejectionReasonVaRBreach{"VAR_LIMIT_BREACH"};
constexpr std::string_view kRejectionReasonKillSwitch{"KILL_SWITCH_TRIGGERED"};
constexpr std::string_view kRejectionReasonInvalidOrder{"INVALID_ORDER"};
constexpr std::string_view kRejectionReasonNoLiquidity{"NO_LIQUIDITY"};

}

RiskEngine::RiskEngine(
    std::shared_ptr<domain::MarketDataCache> marketDataCache,
    std::shared_ptr<domain::RiskLimits> riskLimits,
    std::shared_ptr<domain::VaRModel> varModel,
    uint32_t shardId) noexcept
    : marketDataCache_(std::move(marketDataCache))
    , riskLimits_(std::move(riskLimits))
    , varModel_(std::move(varModel))
    , shardId_(shardId)
    , circuitBreakers_(kMaxInstruments)
    , latencyWindow_(kLatencyWindowSize)
{
    if (!marketDataCache_ || !riskLimits_ || !varModel_) {
        throw std::invalid_argument("RiskEngine requires non-null dependencies");
    }

    std::fill(latencyWindow_.begin(), latencyWindow_.end(), 0);

    auto now = std::chrono::duration_cast<std::chrono::milliseconds>(
        std::chrono::system_clock::now().time_since_epoch()).count();
    anomalyWindowStart_.store(now, std::memory_order_release);

    spdlog::info("RiskEngine initialized on shard {} with {} circuit breakers",
                 shardId_, kMaxInstruments);
}

RiskEvaluationResult RiskEngine::evaluateOrder(const Order& order) {
    auto startTime = std::chrono::high_resolution_clock::now();

    if (!order.isValid()) {
        RiskEvaluationResult result;
        result.decision_ = RiskDecision::REJECT;
        result.rejectionReason_ = kRejectionReasonInvalidOrder;
        result.timestamp_ = std::chrono::duration_cast<std::chrono::microseconds>(
            std::chrono::system_clock::now().time_since_epoch());
        recordLatency(0);
        return result;
    }

    if (killSwitchActive_.load(std::memory_order_acquire)) {
        RiskEvaluationResult result;
        result.decision_ = RiskDecision::REJECT;
        result.rejectionReason_ = kRejectionReasonKillSwitch;
        result.killSwitchTriggered_ = true;
        result.timestamp_ = std::chrono::duration_cast<std::chrono::microseconds>(
            std::chrono::system_clock::now().time_since_epoch());
        rejectedOrders_.fetch_add(1, std::memory_order_relaxed);
        recordLatency(0);
        emitRiskEvent(result);
        return result;
    }

    auto cbResult = checkCircuitBreaker(order.instrumentId_);
    if (cbResult.decision_ == RiskDecision::REJECT) {
        rejectedOrders_.fetch_add(1, std::memory_order_relaxed);
        auto endTime = std::chrono::high_resolution_clock::now();
        cbResult.evaluationLatencyNs_ = std::chrono::duration_cast<std::chrono::nanoseconds>(
            endTime - startTime).count();
        recordLatency(cbResult.evaluationLatencyNs_);
        emitRiskEvent(cbResult);
        return cbResult;
    }

    auto limitResult = evaluateRiskLimits(order);
    if (limitResult.decision_ == RiskDecision::REJECT) {
        rejectedOrders_.fetch_add(1, std::memory_order_relaxed);
        auto endTime = std::chrono::high_resolution_clock::now();
        limitResult.evaluationLatencyNs_ = std::chrono::duration_cast<std::chrono::nanoseconds>(
            endTime - startTime).count();
        recordLatency(limitResult.evaluationLatencyNs_);
        emitRiskEvent(limitResult);
        return limitResult;
    }

    auto varResult = evaluateVaR(order);
    if (varResult.decision_ == RiskDecision::REJECT) {
        rejectedOrders_.fetch_add(1, std::memory_order_relaxed);
        auto endTime = std::chrono::high_resolution_clock::now();
        varResult.evaluationLatencyNs_ = std::chrono::duration_cast<std::chrono::nanoseconds>(
            endTime - startTime).count();
        recordLatency(varResult.evaluationLatencyNs_);
        emitRiskEvent(varResult);
        return varResult;
    }

    auto endTime = std::chrono::high_resolution_clock::now();
    auto latencyNs = std::chrono::duration_cast<std::chrono::nanoseconds>(endTime - startTime).count();

    RiskEvaluationResult result;
    result.decision_ = RiskDecision::ACCEPT;
    result.exposure_ = limitResult.exposure_;
    result.limit_ = limitResult.limit_;
    result.utilizationPct_ = limitResult.utilizationPct_;
    result.evaluationLatencyNs_ = latencyNs;
    result.timestamp_ = std::chrono::duration_cast<std::chrono::microseconds>(
        std::chrono::system_clock::now().time_since_epoch());

    processedOrders_.fetch_add(1, std::memory_order_relaxed);

    checkKillSwitchAnomaly();

    recordLatency(latencyNs);
    emitRiskEvent(result);

    return result;
}

void RiskEngine::onMarketData(const domain::MarketDataEvent& event) {
    if (marketDataCache_) {
        marketDataCache_->updateFromEvent(event);
    }

    if (varModel_) {
        varModel_->updateVolatility(event.instrumentId_, event.timestamp_);
    }
}

CircuitBreakerState RiskEngine::getCircuitBreakerState(uint32_t instrumentId) const {
    if (instrumentId >= kMaxInstruments) {
        return CircuitBreakerState::OPEN;
    }
    return circuitBreakers_[instrumentId].state_.load(std::memory_order_acquire);
}

bool RiskEngine::isKillSwitchActive() const {
    return killSwitchActive_.load(std::memory_order_acquire);
}

void RiskEngine::resetCircuitBreaker(uint32_t instrumentId) {
    if (instrumentId < kMaxInstruments) {
        circuitBreakers_[instrumentId].reset();
        spdlog::info("Circuit breaker reset for instrument {}", instrumentId);
    }
}

void RiskEngine::resetKillSwitch() {
    killSwitchActive_.store(false, std::memory_order_release);
    killSwitchTriggerCount_.store(0, std::memory_order_release);

    auto now = std::chrono::duration_cast<std::chrono::milliseconds>(
        std::chrono::system_clock::now().time_since_epoch()).count();
    anomalyWindowStart_.store(now, std::memory_order_release);
    anomalyCount_.store(0, std::memory_order_release);

    spdlog::warn("Kill switch reset");
}

void RiskEngine::setRiskEventHandler(RiskEventHandler handler) {
    eventHandler_ = std::move(handler);
}

uint64_t RiskEngine::getProcessedOrdersCount() const {
    return processedOrders_.load(std::memory_order_relaxed);
}

uint64_t RiskEngine::getRejectedOrdersCount() const {
    return rejectedOrders_.load(std::memory_order_relaxed);
}

double RiskEngine::getAvgLatencyUs() const {
    uint64_t total = 0;
    for (const auto& lat : latencyWindow_) {
        total += lat.load(std::memory_order_relaxed);
    }
    return static_cast<double>(total) / kLatencyWindowSize / 1000.0;
}

RiskEvaluationResult RiskEngine::evaluateRiskLimits(const Order& order) noexcept {
    RiskEvaluationResult result;

    auto limit = riskLimits_->getLimit(order.traderId_, order.strategyId_, order.instrumentId_);
    if (!limit.has_value()) {
        result.decision_ = RiskDecision::ACCEPT;
        return result;
    }

    double currentExposure = riskLimits_->getCurrentExposure(order.traderId_, order.strategyId_, order.instrumentId_);
    double orderNotional = static_cast<double>(order.notional()) / 1000000.0;
    double newExposure = currentExposure + orderNotional;

    result.exposure_ = newExposure;
    result.limit_ = limit.value();
    result.utilizationPct_ = (newExposure / limit.value()) * 100.0;

    if (newExposure > limit.value()) {
        result.decision_ = RiskDecision::REJECT;
        result.rejectionReason_ = kRejectionReasonLimitExceeded;

        updateCircuitBreakerState(order.instrumentId_, true);
    } else if (newExposure > limit.value() * 0.8) {
        result.decision_ = RiskDecision::REVIEW;
    }

    return result;
}

RiskEvaluationResult RiskEngine::evaluateVaR(const Order& order) noexcept {
    RiskEvaluationResult result;

    if (!varModel_ || !marketDataCache_) {
        result.decision_ = RiskDecision::ACCEPT;
        return result;
    }

    auto* orderBook = marketDataCache_->getOrderBook(order.instrumentId_);
    if (!orderBook || orderBook->isStale()) {
        result.decision_ = RiskDecision::REVIEW;
        result.rejectionReason_ = kRejectionReasonNoLiquidity;
        return result;
    }

    double currentVaR = varModel_->calculateVaR(order.instrumentId_);
    double orderNotional = static_cast<double>(order.notional()) / 1000000.0;
    double potentialLoss = varModel_->estimatePotentialLoss(order.instrumentId_, orderNotional);

    if (currentVaR + potentialLoss > varModel_->getVaRLimit()) {
        result.decision_ = RiskDecision::REJECT;
        result.rejectionReason_ = kRejectionReasonVaRBreach;
    }

    return result;
}

RiskEvaluationResult RiskEngine::checkCircuitBreaker(uint32_t instrumentId) noexcept {
    RiskEvaluationResult result;

    if (instrumentId >= kMaxInstruments) {
        result.decision_ = RiskDecision::REJECT;
        result.circuitBreaker_ = CircuitBreakerState::OPEN;
        result.rejectionReason_ = kRejectionReasonCircuitBreaker;
        return result;
    }

    auto& cb = circuitBreakers_[instrumentId];
    auto state = cb.state_.load(std::memory_order_acquire);

    if (state == CircuitBreakerState::OPEN) {
        auto now = std::chrono::duration_cast<std::chrono::milliseconds>(
            std::chrono::system_clock::now().time_since_epoch()).count();
        auto lastReset = cb.lastResetTimestamp_.load(std::memory_order_relaxed);

        if (now - lastReset > kCircuitBreakerResetTimeoutMs) {
            cb.state_.store(CircuitBreakerState::HALF_OPEN, std::memory_order_release);
            state = CircuitBreakerState::HALF_OPEN;
            spdlog::info("Circuit breaker for instrument {} moved to HALF_OPEN", instrumentId);
        } else {
            result.decision_ = RiskDecision::REJECT;
            result.circuitBreaker_ = CircuitBreakerState::OPEN;
            result.rejectionReason_ = kRejectionReasonCircuitBreaker;
            return result;
        }
    }

    result.circuitBreaker_ = state;
    return result;
}

void RiskEngine::updateCircuitBreakerState(uint32_t instrumentId, bool breached) noexcept {
    if (instrumentId >= kMaxInstruments) return;

    auto& cb = circuitBreakers_[instrumentId];
    auto state = cb.state_.load(std::memory_order_acquire);

    if (breached) {
        cb.breachCount_.fetch_add(1, std::memory_order_relaxed);
        auto now = std::chrono::duration_cast<std::chrono::milliseconds>(
            std::chrono::system_clock::now().time_since_epoch()).count();
        cb.lastBreachTimestamp_.store(now, std::memory_order_release);

        if (state == CircuitBreakerState::CLOSED) {
            auto breaches = cb.breachCount_.load(std::memory_order_relaxed);
            if (breaches >= 3) {
                cb.state_.store(CircuitBreakerState::OPEN, std::memory_order_release);
                spdlog::warn("Circuit breaker OPEN for instrument {} after {} breaches",
                            instrumentId, breaches);
            }
        } else if (state == CircuitBreakerState::HALF_OPEN) {
            cb.state_.store(CircuitBreakerState::OPEN, std::memory_order_release);
            spdlog::warn("Circuit breaker reopened for instrument {}", instrumentId);
        }
    } else if (state == CircuitBreakerState::HALF_OPEN) {
        cb.reset();
        spdlog::info("Circuit breaker CLOSED for instrument {} after successful check", instrumentId);
    }
}

void RiskEngine::checkKillSwitchAnomaly() noexcept {
    auto now = std::chrono::duration_cast<std::chrono::milliseconds>(
        std::chrono::system_clock::now().time_since_epoch()).count();

    auto windowStart = anomalyWindowStart_.load(std::memory_order_relaxed);
    if (now - windowStart > kAnomalyWindowMs) {
        auto currentCount = anomalyCount_.load(std::memory_order_relaxed);

        if (currentCount > kAnomalyThresholdDeviation * 10) {
            if (!killSwitchActive_.load(std::memory_order_acquire)) {
                killSwitchActive_.store(true, std::memory_order_release);
                killSwitchTriggerCount_.fetch_add(1, std::memory_order_relaxed);
                spdlog::error("KILL SWITCH TRIGGERED: {} anomalies in {}ms window",
                             currentCount, kAnomalyWindowMs);
            }
        }

        anomalyWindowStart_.store(now, std::memory_order_release);
        anomalyCount_.store(0, std::memory_order_release);
    } else {
        auto processed = processedOrders_.load(std::memory_order_relaxed);
        double rate = static_cast<double>(processed) / ((now - windowStart) / 1000.0);

        if (rate > 10000.0) {
            anomalyCount_.fetch_add(1, std::memory_order_relaxed);
        }
    }
}

void RiskEngine::emitRiskEvent(const RiskEvaluationResult& result) noexcept {
    if (eventHandler_) {
        eventHandler_(result);
    }
}

void RiskEngine::recordLatency(uint64_t latencyNs) noexcept {
    auto index = latencyIndex_.load(std::memory_order_relaxed);
    latencyWindow_[index].store(latencyNs, std::memory_order_relaxed);
    latencyIndex_.store((index + 1) % kLatencyWindowSize, std::memory_order_relaxed);
}

}
}

// === ARCHIVO: src/domain/RiskLimits.h ===
#ifndef RISK_ENGINE_RISK_LIMITS_H
#define RISK_ENGINE_RISK_LIMITS_H

#include <atomic>
#include <cstdint>
#include <optional>
#include <unordered_map>
#include <memory>
#include <chrono>

namespace risk_engine {

/**
 * Estructura que representa un límite de riesgo individual.
 * Cada límite tiene un umbral máximo y un multiplicador de volatilidad.
 */
struct RiskThreshold {
    double maxExposure;
    double volatilityMultiplier;
    bool enabled;
    
    RiskThreshold() : maxExposure(0.0), volatilityMultiplier(1.0), enabled(true) {}
    RiskThreshold(double exposure, double multiplier, bool en = true)
        : maxExposure(exposure), volatilityMultiplier(multiplier), enabled(en) {}
    
    double adjustedLimit(double currentVolatility) const {
        return enabled ? maxExposure * (1.0 + volatilityMultiplier * currentVolatility) : 0.0;
    }
};

/**
 * Límites de riesgo por instrumento financiero.
 * Gestiona los umbrales individuales para cada instrumento.
 */
struct InstrumentLimits {
    RiskThreshold positionLimit;
    RiskThreshold dailyLossLimit;
    RiskThreshold concentrationLimit;
    std::atomic<uint64_t> currentPosition{0};
    std::atomic<double> currentExposure{0.0};
    
    InstrumentLimits() = default;
    
    bool checkPositionLimit(double newPosition, double volatility) const {
        return std::abs(newPosition) <= positionLimit.adjustedLimit(volatility);
    }
    
    bool checkExposureLimit(double exposure, double volatility) const {
        return exposure <= dailyLossLimit.adjustedLimit(volatility);
    }
    
    bool checkConcentration(double portfolioExposure, double totalExposure, double volatility) const {
        if (totalExposure <= 0.0) return true;
        double concentration = portfolioExposure / totalExposure;
        return concentration <= concentrationLimit.adjustedLimit(volatility);
    }
};

/**
 * Límites de riesgo por estrategia de trading.
 * Agrega límites a nivel de estrategia incluyendo límites de VaR.
 */
struct StrategyLimits {
    std::string strategyId;
    RiskThreshold varLimit;
    RiskThreshold dailyPnlLimit;
    RiskThreshold orderRateLimit;
    std::atomic<uint64_t> orderCount{0};
    std::atomic<double> currentVaR{0.0};
    std::atomic<double> dailyPnl{0.0};
    std::chrono::steady_clock::time_point windowStart;
    
    explicit StrategyLimits(const std::string& id) 
        : strategyId(id), windowStart(std::chrono::steady_clock::now()) {}
    
    bool checkVaRLimit(double var, double volatility) const {
        return var <= varLimit.adjustedLimit(volatility);
    }
    
    bool checkPnlLimit(double pnl, double volatility) const {
        return std::abs(pnl) <= dailyPnlLimit.adjustedLimit(volatility);
    }
    
    bool checkOrderRate(uint64_t newOrderCount, double volatility) const {
        return newOrderCount <= static_cast<uint64_t>(orderRateLimit.adjustedLimit(volatility));
    }
    
    void resetWindow() {
        windowStart = std::chrono::steady_clock::now();
        orderCount.store(0);
    }
};

/**
 * Límites de riesgo por trader.
 * Coordina límites individuales, de estrategia y de instrumento.
 */
struct TraderLimits {
    std::string traderId;
    RiskThreshold totalExposureLimit;
    RiskThreshold marginUtilizationLimit;
    std::unordered_map<std::string, std::shared_ptr<StrategyLimits>> strategyLimits;
    std::unordered_map<std::string, InstrumentLimits> instrumentLimits;
    std::atomic<bool> blocked{false};
    std::atomic<uint64_t> violationCount{0};
    
    explicit TraderLimits(const std::string& id) : traderId(id) {}
    
    bool isBlocked() const { return blocked.load(std::memory_order_acquire); }
    
    void block() { 
        blocked.store(true, std::memory_order_release);
        violationCount.fetch_add(1, std::memory_order_relaxed);
    }
    
    void unblock() { blocked.store(false, std::memory_order_release); }
    
    bool checkTotalExposure(double totalExposure, double volatility) const {
        return totalExposure <= totalExposureLimit.adjustedLimit(volatility);
    }
    
    void addStrategy(const std::string& strategyId) {
        strategyLimits[strategyId] = std::make_shared<StrategyLimits>(strategyId);
    }
    
    std::shared_ptr<StrategyLimits> getStrategyLimits(const std::string& strategyId) const {
        auto it = strategyLimits.find(strategyId);
        return (it != strategyLimits.end()) ? it->second : nullptr;
    }
    
    InstrumentLimits& getInstrumentLimits(const std::string& instrumentId) {
        return instrumentLimits[instrumentId];
    }
};

/**
 * Gestor central de límites de riesgo.
 * Mantiene el estado de todos los límites y proporciona validación thread-safe.
 */
class RiskLimitsManager {
public:
    using TraderLimitsMap = std::unordered_map<std::string, std::shared_ptr<TraderLimits>>;
    
private:
    TraderLimitsMap traders_;
    std::atomic<uint64_t> globalViolationCount_{0};
    
public:
    RiskLimitsManager() = default;
    
    std::shared_ptr<TraderLimits> getOrCreateTrader(const std::string& traderId) {
        auto it = traders_.find(traderId);
        if (it != traders_.end()) {
            return it->second;
        }
        auto trader = std::make_shared<TraderLimits>(traderId);
        traders_[traderId] = trader;
        return trader;
    }
    
    bool validateTraderExposure(
        const std::string& traderId,
        const std::string& strategyId,
        const std::string& instrumentId,
        double exposure,
        double volatility
    ) {
        auto trader = getOrCreateTrader(traderId);
        
        if (trader->isBlocked()) {
            globalViolationCount_.fetch_add(1, std::memory_order_relaxed);
            return false;
        }
        
        if (!trader->checkTotalExposure(exposure, volatility)) {
            trader->block();
            globalViolationCount_.fetch_add(1, std::memory_order_relaxed);
            return false;
        }
        
        auto strategyLimits = trader->getStrategyLimits(strategyId);
        if (strategyLimits) {
            double currentVaR = strategyLimits->currentVaR.load(std::memory_order_acquire);
            if (!strategyLimits->checkVaRLimit(currentVaR, volatility)) {
                trader->block();
                globalViolationCount_.fetch_add(1, std::memory_order_relaxed);
                return false;
            }
        }
        
        auto& instrumentLimits = trader->getInstrumentLimits(instrumentId);
        if (!instrumentLimits.checkExposureLimit(exposure, volatility)) {
            trader->block();
            globalViolationCount_.fetch_add(1, std::memory_order_relaxed);
            return false;
        }
        
        return true;
    }
    
    uint64_t globalViolationCount() const {
        return globalViolationCount_.load(std::memory_order_relaxed);
    }
    
    void resetViolations() {
        globalViolationCount_.store(0, std::memory_order_relaxed);
        for (auto& [id, trader] : traders_) {
            trader->violationCount.store(0, std::memory_order_relaxed);
            trader->unblock();
        }
    }
    
    size_t traderCount() const { return traders_.size(); }
};

} // namespace risk_engine

#endif // RISK_ENGINE_RISK_LIMITS_H

// === ARCHIVO: src/domain/VaRModel.h ===
#ifndef RISK_ENGINE_VAR_MODEL_H
#define RISK_ENGINE_VAR_MODEL_H

#include <atomic>
#include <cmath>
#include <vector>
#include <array>
#include <memory>
#include <optional>
#include <chrono>
#include <algorithm>

namespace risk_engine {

/**
 * Parámetros de calibración para el modelo VaR.
 * Configura los factores de confianza, ventana temporal y decaimiento.
 */
struct VaRCalibrationParams {
    double confidenceLevel;
    int lookbackDays;
    double decayFactor;
    bool useIntradayScaling;
    double intradayScalingFactor;
    
    VaRCalibrationParams() 
        : confidenceLevel(0.99)
        , lookbackDays(250)
        , decayFactor(0.94)
        , useIntradayScaling(true)
        , intradayScalingFactor(1.5) {}
    
    VaRCalibrationParams(double conf, int days, double decay, bool intraday = true, double scale = 1.5)
        : confidenceLevel(conf), lookbackDays(days), decayFactor(decay),
          useIntradayScaling(intraday), intradayScalingFactor(scale) {}
};

/**
 * Observación de precio para el cálculo de returns.
 */
struct PriceObservation {
    double price;
    std::chrono::steady_clock::time_point timestamp;
    double volume;
    
    PriceObservation() : price(0.0), timestamp(std::chrono::steady_clock::now()), volume(0.0) {}
    PriceObservation(double p, auto ts, double v = 0.0) 
        : price(p), timestamp(ts), volume(v) {}
};

/**
 * Resultado del cálculo de VaR.
 */
struct VaRResult {
    double varAmount;
    double confidenceLevel;
    double volatility;
    double expectedReturn;
    std::chrono::steady_clock::time_point calculationTime;
    bool isValid;
    
    VaRResult() 
        : varAmount(0.0), confidenceLevel(0.99), volatility(0.0), 
          expectedReturn(0.0), isValid(false) {}
    
    VaRResult(double var, double conf, double vol, double ret, bool valid)
        : varAmount(var), confidenceLevel(conf), volatility(vol),
          expectedReturn(ret), calculationTime(std::chrono::steady_clock::now()), 
          isValid(valid) {}
};

/**
 * Modelo de Value-at-Risk intraday con calibración dinámica.
 * Implementa el cálculo de VaR usando metodología paramétrica
 * con ajustes por volatilidad en tiempo real.
 */
class VaRModel {
public:
    static constexpr size_t MAX_OBSERVATIONS = 1000;
    static constexpr double MIN_VOLATILITY = 0.0001;
    static constexpr double MAX_VOLATILITY = 5.0;
    
private:
    std::string instrumentId_;
    VaRCalibrationParams params_;
    std::vector<PriceObservation> priceHistory_;
    std::array<double, MAX_OBSERVATIONS> returnsBuffer_;
    std::atomic<size_t> observationCount_{0};
    std::atomic<double> currentVolatility_{0.0};
    std::atomic<double> currentVaR_{0.0};
    std::atomic<bool> calibrated_{false};
    std::chrono::steady_clock::time_point lastUpdate_;
    std::atomic<uint64_t> calculationCount_{0};
    
    // Parámetros EWMA para volatilidad
    double lambda_;
    double riskFactor_;
    
public:
    explicit VaRModel(const std::string& instrumentId, const VaRCalibrationParams& params = {})
        : instrumentId_(instrumentId), 
          params_(params),
          lambda_(params.decayFactor),
          riskFactor_(calculateRiskFactor(params.confidenceLevel)) {
        returnsBuffer_.fill(0.0);
    }
    
    void addObservation(const PriceObservation& obs) {
        if (priceHistory_.empty()) {
            priceHistory_.push_back(obs);
            return;
        }
        
        double prevPrice = priceHistory_.back().price;
        if (prevPrice > 0.0 && obs.price > 0.0) {
            double logReturn = std::log(obs.price / prevPrice);
            size_t idx = observationCount_.load(std::memory_order_relaxed) % MAX_OBSERVATIONS;
            returnsBuffer_[idx] = logReturn;
            observationCount_.fetch_add(1, std::memory_order_relaxed);
            
            currentVolatility_.store(calculateEWMAVolatility(), std::memory_order_release);
            calibrated_.store(true, std::memory_order_release);
        }
        
        priceHistory_.push_back(obs);
        if (priceHistory_.size() > MAX_OBSERVATIONS) {
            priceHistory_.erase(priceHistory_.begin());
        }
        
        lastUpdate_ = std::chrono::steady_clock::now();
    }
    
    VaRResult calculate(double positionSize, double currentPrice) {
        calculationCount_.fetch_add(1, std::memory_order_relaxed);
        
        double volatility = currentVolatility_.load(std::memory_order_acquire);
        if (!calibrated_.load(std::memory_order_acquire) || volatility < MIN_VOLATILITY) {
            return VaRResult(0.0, params_.confidenceLevel, 0.0, 0.0, false);
        }
        
        double scaledVolatility = volatility;
        if (params_.useIntradayScaling) {
            scaledVolatility *= params_.intradayScalingFactor;
        }
        
        double var = calculateParametricVaR(positionSize, currentPrice, scaledVolatility);
        
        // Ajuste por decaimiento exponencial
        double timeDecayFactor = calculateTimeDecay();
        var *= timeDecayFactor;
        
        currentVaR_.store(var, std::memory_order_release);
        
        return VaRResult(
            var,
            params_.confidenceLevel,
            scaledVolatility,
            calculateExpectedReturn(),
            true
        );
    }
    
    double getCurrentVolatility() const {
        return currentVolatility_.load(std::memory_order_acquire);
    }
    
    double getCurrentVaR() const {
        return currentVaR_.load(std::memory_order_acquire);
    }
    
    bool isCalibrated() const {
        return calibrated_.load(std::memory_order_acquire);
    }
    
    void recalibrate(const VaRCalibrationParams& newParams) {
        params_ = newParams;
        lambda_ = newParams.decayFactor;
        riskFactor_ = calculateRiskFactor(newParams.confidenceLevel);
        currentVolatility_.store(calculateEWMAVolatility(), std::memory_order_release);
    }
    
    uint64_t calculationCount() const {
        return calculationCount_.load(std::memory_order_relaxed);
    }
    
    std::string instrumentId() const { return instrumentId_; }
    
    size_t observationCount() const {
        return observationCount_.load(std::memory_order_relaxed);
    }
    
private:
    double calculateRiskFactor(double confidenceLevel) const {
        // Aproximación de quantile para distribución normal
        // Usando la transformación de probit
        if (confidenceLevel >= 0.999) return 3.719;
        if (confidenceLevel >= 0.99) return 2.326;
        if (confidenceLevel >= 0.975) return 1.96;
        if (confidenceLevel >= 0.95) return 1.645;
        return 1.645; // Default 95%
    }
    
    double calculateEWMAVolatility() const {
        size_t count = observationCount_.load(std::memory_order_relaxed);
        if (count < 2) return MIN_VOLATILITY;
        
        double weightedSum = 0.0;
        double weightSum = 0.0;
        double weight = 1.0;
        
        size_t startIdx = (count > MAX_OBSERVATIONS) ? (count % MAX_OBSERVATIONS) : 0;
        size_t iterations = std::min(count, MAX_OBSERVATIONS);
        
        for (size_t i = 0; i < iterations; ++i) {
            size_t idx = (startIdx + i) % MAX_OBSERVATIONS;
            double r = returnsBuffer_[idx];
            weightedSum += weight * r * r;
            weightSum += weight;
            weight *= lambda_;
        }
        
        if (weightSum <= 0.0) return MIN_VOLATILITY;
        
        double variance = weightedSum / weightSum;
        double volatility = std::sqrt(variance);
        
        return std::clamp(volatility, MIN_VOLATILITY, MAX_VOLATILITY);
    }
    
    double calculateParametricVaR(double position, double price, double volatility) const {
        if (price <= 0.0 || volatility <= 0.0) return 0.0;
        
        double positionValue = std::abs(position) * price;
        double varPercentile = riskFactor_ * volatility;
        
        return positionValue * varPercentile;
    }
    
    double calculateExpectedReturn() const {
        size_t count = std::min(observationCount_.load(std::memory_order_relaxed), 
                               static_cast<size_t>(params_.lookbackDays));
        if (count < 2) return 0.0;
        
        double sum = 0.0;
        size_t startIdx = (observationCount_.load(std::memory_order_relaxed) >= count) 
            ? (observationCount_.load(std::memory_order_relaxed) - count) % MAX_OBSERVATIONS 
            : 0;
        
        for (size_t i = 0; i < count; ++i) {
            size_t idx = (startIdx + i) % MAX_OBSERVATIONS;
            sum += returnsBuffer_[idx];
        }
        
        return sum / static_cast<double>(count);
    }
    
    double calculateTimeDecay() const {
        auto now = std::chrono::steady_clock::now();
        auto elapsed = std::chrono::duration_cast<std::chrono::minutes>(now - lastUpdate_).count();
        
        // Decaimiento gradual hasta un máximo de 1.5x después de 4 horas
        double decay = std::pow(lambda_, elapsed / 60.0);
        return std::clamp(decay, 0.5, 1.5);
    }
};

/**
 * Gestor de múltiples modelos VaR por instrumento.
 * Proporciona acceso thread-safe a los modelos y cálculo agregado.
 */
class VaRModelRegistry {
public:
    using ModelMap = std::unordered_map<std::string, std::shared_ptr<VaRModel>>;
    
private:
    ModelMap models_;
    std::atomic<uint64_t> totalCalculations_{0};
    
public:
    VaRModelRegistry() = default;
    
    std::shared_ptr<VaRModel> getOrCreateModel(
        const std::string& instrumentId,
        const VaRCalibrationParams& params = {}
    ) {
        auto it = models_.find(instrumentId);
        if (it != models_.end()) {
            return it->second;
        }
        auto model = std::make_shared<VaRModel>(instrumentId, params);
        models_[instrumentId] = model;
        return model;
    }
    
    VaRResult calculatePortfolioVaR(
        const std::vector<std::pair<std::string, double>>& positions,
        double currentPrice
    ) {
        double totalVaR = 0.0;
        double totalVolatility = 0.0;
        bool allValid = true;
        
        for (const auto& [instrumentId, position] : positions) {
            auto it = models_.find(instrumentId);
            if (it != models_.end() && it->second->isCalibrated()) {
                auto result = it->second->calculate(position, currentPrice);
                if (result.isValid) {
                    totalVaR += result.varAmount;
                    totalVolatility += result.volatility;
                } else {
                    allValid = false;
                }
            } else {
                allValid = false;
            }
        }
        
        totalCalculations_.fetch_add(1, std::memory_order_relaxed);
        
        return VaRResult(totalVaR, 0.99, totalVolatility / positions.size(), 0.0, allValid);
    }
    
    size_t modelCount() const { return models_.size(); }
    
    uint64_t totalCalculations() const {
        return totalCalculations_.load(std::memory_order_relaxed);
    }
    
    void clear() {
        models_.clear();
    }
};

} // namespace risk_engine

#endif // RISK_ENGINE_VAR_MODEL_H


// === ARCHIVO: src/adapters/MarketDataFeedAdapter.cpp ===
#include "adapters/MarketDataFeedAdapter.h"
#include <chrono>
#include <thread>
#include <atomic>
#include <queue>
#include <mutex>
#include <optional>
#include <cstring>
#include <arpa/inet.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netdb.h>
#include "spdlog/spdlog.h"
#include "spdlog/sinks/rotating_file_sink.h"

namespace risk_engine {

constexpr size_t MAX_MARKET_DATA_MSG_SIZE = 4096;
constexpr int RECONNECT_DELAY_MS = 1000;
constexpr int MAX_RECONNECT_ATTEMPTS = 10;
constexpr size_t BATCH_SIZE = 100;

MarketDataFeedAdapter::MarketDataFeedAdapter(
    std::shared_ptr<disruptor::RingBuffer<MarketDataEvent>> ring_buffer,
    const FeedConfig& config)
    : ring_buffer_(ring_buffer)
    , config_(config)
    , running_(false)
    , reconnect_attempts_(0)
    , last_sequence_(0)
    , messages_received_(0)
    , messages_dropped_(0) {
    
    SPDLOG_INFO("MarketDataFeedAdapter initializing with feed URL: {}", config.feed_url);
    setupLogging();
}

void MarketDataFeedAdapter::setupLogging() {
    try {
        auto rotating_sink = std::make_shared<spdlog::sinks::rotating_file_sink_mt>(
            "logs/market_data_feed.log", 1024 * 1024 * 10, 3);
        logger_ = std::make_shared<spdlog::logger>("market_data_feed", rotating_sink);
        logger_->set_level(spdlog::level::info);
        logger_->set_pattern("%Y-%m-%d %H:%M:%S.%f [%l] %v");
    } catch (const spdlog::spdlog_ex& ex) {
        SPDLOG_WARN("Failed to create rotating logger: {}", ex.what());
        logger_ = spdlog::default_logger();
    }
}

bool MarketDataFeedAdapter::connect() {
    std::lock_guard<std::mutex> lock(connection_mutex_);
    
    if (socket_fd_ >= 0) {
        SPDLOG_INFO("Already connected to market data feed");
        return true;
    }

    SPDLOG_INFO("Connecting to market data feed: {}", config_.feed_url);
    
    std::string host;
    int port;
    if (!parseFeedUrl(config_.feed_url, host, port)) {
        SPDLOG_ERROR("Failed to parse feed URL: {}", config_.feed_url);
        return false;
    }

    socket_fd_ = socket(AF_INET, SOCK_STREAM, 0);
    if (socket_fd_ < 0) {
        SPDLOG_ERROR("Failed to create socket: {}", strerror(errno));
        return false;
    }

    struct hostent* server = gethostbyname(host.c_str());
    if (server == nullptr) {
        SPDLOG_ERROR("Failed to resolve host: {}", host);
        close(socket_fd_);
        socket_fd_ = -1;
        return false;
    }

    struct sockaddr_in server_addr;
    std::memset(&server_addr, 0, sizeof(server_addr));
    server_addr.sin_family = AF_INET;
    std::memcpy(&server_addr.sin_addr.s_addr, server->h_addr, server->h_length);
    server_addr.sin_port = htons(port);

    if (::connect(socket_fd_, (struct sockaddr*)&server_addr, sizeof(server_addr)) < 0) {
        SPDLOG_ERROR("Failed to connect: {}", strerror(errno));
        close(socket_fd_);
        socket_fd_ = -1;
        return false;
    }

    int flags = fcntl(socket_fd_, F_GETFL, 0);
    fcntl(socket_fd_, F_SETFL, flags | O_NONBLOCK);

    SPDLOG_INFO("Connected to market data feed successfully");
    reconnect_attempts_ = 0;
    return true;
}

bool MarketDataFeedAdapter::parseFeedUrl(const std::string& url, std::string& host, int& port) {
    size_t protocol_end = url.find("://");
    std::string stripped_url = (protocol_end != std::string::npos) 
        ? url.substr(protocol_end + 3) 
        : url;
    
    size_t colon_pos = stripped_url.rfind(':');
    if (colon_pos == std::string::npos) {
        host = stripped_url;
        port = config_.default_port;
        return true;
    }
    
    host = stripped_url.substr(0, colon_pos);
    try {
        port = std::stoi(stripped_url.substr(colon_pos + 1));
    } catch (...) {
        port = config_.default_port;
    }
    return true;
}

void MarketDataFeedAdapter::disconnect() {
    std::lock_guard<std::mutex> lock(connection_mutex_);
    
    if (socket_fd_ >= 0) {
        close(socket_fd_);
        socket_fd_ = -1;
        SPDLOG_INFO("Disconnected from market data feed");
    }
}

void MarketDataFeedAdapter::start() {
    if (running_.exchange(true)) {
        SPDLOG_WARN("MarketDataFeedAdapter already running");
        return;
    }

    if (!connect()) {
        running_ = false;
        SPDLOG_ERROR("Failed to connect to market data feed");
        return;
    }

    worker_thread_ = std::thread([this]() {
        SPDLOG_INFO("MarketDataFeedAdapter worker thread started");
        runEventLoop();
        SPDLOG_INFO("MarketDataFeedAdapter worker thread stopped");
    });
}

void MarketDataFeedAdapter::stop() {
    if (!running_.exchange(false)) {
        SPDLOG_INFO("MarketDataFeedAdapter already stopped");
        return;
    }

    disconnect();
    
    if (worker_thread_.joinable()) {
        worker_thread_.join();
    }
    
    SPDLOG_INFO("MarketDataFeedAdapter stopped. Total messages: {}, dropped: {}", 
                messages_received_.load(), messages_dropped_.load());
}

void MarketDataFeedAdapter::runEventLoop() {
    char buffer[MAX_MARKET_DATA_MSG_SIZE];
    std::vector<char> message_buffer;
    message_buffer.reserve(8192);

    while (running_.load()) {
        fd_set read_fds;
        FD_ZERO(&read_fds);
        
        {
            std::lock_guard<std::mutex> lock(connection_mutex_);
            if (socket_fd_ >= 0) {
                FD_SET(socket_fd_, &read_fds);
            }
        }

        struct timeval timeout;
        timeout.tv_sec = 1;
        timeout.tv_usec = 0;

        int activity = select(socket_fd_ + 1, &read_fds, nullptr, nullptr, &timeout);
        
        if (activity < 0) {
            if (running_.load()) {
                SPDLOG_ERROR("Select error: {}", strerror(errno));
                handleReconnect();
            }
            continue;
        }

        if (activity == 0) {
            continue;
        }

        ssize_t bytes_read = recv(socket_fd_, buffer, sizeof(buffer) - 1, 0);
        
        if (bytes_read <= 0) {
            if (bytes_read < 0 && errno == EAGAIN) {
                continue;
            }
            SPDLOG_WARN("Connection closed or error, bytes_read: {}", bytes_read);
            handleReconnect();
            continue;
        }

        buffer[bytes_read] = '\0';
        message_buffer.insert(message_buffer.end(), buffer, buffer + bytes_read);
        
        processMessageBuffer(message_buffer);
    }
}

void MarketDataFeedAdapter::processMessageBuffer(std::vector<char>& buffer) {
    while (true) {
        size_t newline_pos = 0;
        for (size_t i = 0; i + 1 < buffer.size(); ++i) {
            if (buffer[i] == '\n' && buffer[i + 1] == '\n') {
                newline_pos = i;
                break;
            }
        }

        if (newline_pos == 0) {
            break;
        }

        std::string message(buffer.begin(), buffer.begin() + newline_pos);
        buffer.erase(buffer.begin(), buffer.begin() + newline_pos + 2);

        auto event = parseMarketDataMessage(message);
        if (event) {
            publishEvent(*event);
        }
    }
}

std::optional<MarketDataEvent> MarketDataFeedAdapter::parseMarketDataMessage(
    const std::string& raw_message) {
    
    try {
        auto json = nlohmann::json::parse(raw_message);
        
        MarketDataEvent event;
        event.timestamp = std::chrono::system_clock::now();
        event.sequence = last_sequence_.fetch_add(1);
        
        if (json.contains("type")) {
            std::string type = json["type"];
            
            if (type == "trade") {
                event.event_type = MarketDataEventType::TRADE;
                event.instrument_id = json.value("instrument_id", "");
                event.price = json.value("price", 0.0);
                event.quantity = json.value("quantity", 0);
                event.side = json.value("side", "") == "buy" ? OrderSide::BUY : OrderSide::SELL;
            } else if (type == "quote") {
                event.event_type = MarketDataEventType::QUOTE;
                event.instrument_id = json.value("instrument_id", "");
                event.bid_price = json.value("bid", 0.0);
                event.ask_price = json.value("ask", 0.0);
                event.bid_size = json.value("bid_size", 0);
                event.ask_size = json.value("ask_size", 0);
            } else if (type == "orderbook") {
                event.event_type = MarketDataEventType::ORDERBOOK_SNAPSHOT;
                event.instrument_id = json.value("instrument_id", "");
                event.bid_price = json.value("best_bid", 0.0);
                event.ask_price = json.value("best_ask", 0.0);
            }
        }

        messages_received_.fetch_add(1);
        SPDLOG_TRACE("Parsed event: type={}, instrument={}, price={}", 
                     static_cast<int>(event.event_type), 
                     event.instrument_id, 
                     event.price);
        
        return event;
        
    } catch (const nlohmann::json::parse_error& e) {
        SPDLOG_ERROR("JSON parse error: {}", e.what());
        messages_dropped_.fetch_add(1);
        return std::nullopt;
    } catch (const std::exception& e) {
        SPDLOG_ERROR("Error parsing market data message: {}", e.what());
        messages_dropped_.fetch_add(1);
        return std::nullopt;
    }
}

void MarketDataFeedAdapter::publishEvent(const MarketDataEvent& event) {
    if (!ring_buffer_) {
        SPDLOG_ERROR("Ring buffer not initialized");
        return;
    }

    auto sequence = ring_buffer_->next();
    try {
        auto* slot = ring_buffer_->get(sequence);
        *slot = event;
        ring_buffer_->publish(sequence);
        
        SPDLOG_TRACE("Published event with sequence: {}", sequence);
    } catch (const std::exception& e) {
        SPDLOG_ERROR("Failed to publish event: {}", e.what());
    }
}

void MarketDataFeedAdapter::handleReconnect() {
    disconnect();
    
    if (reconnect_attempts_.load() >= MAX_RECONNECT_ATTEMPTS) {
        SPDLOG_ERROR("Max reconnect attempts reached");
        running_ = false;
        return;
    }

    reconnect_attempts_.fetch_add(1);
    SPDLOG_INFO("Reconnecting to market data feed, attempt {}/{}", 
                reconnect_attempts_.load(), MAX_RECONNECT_ATTEMPTS);
    
    std::this_thread::sleep_for(std::chrono::milliseconds(RECONNECT_DELAY_MS));
    connect();
}

std::atomic<uint64_t> MarketDataFeedAdapter::getMessagesReceived() const {
    return messages_received_;
}

std::atomic<uint64_t> MarketDataFeedAdapter::getMessagesDropped() const {
    return messages_dropped_;
}

bool MarketDataFeedAdapter::isConnected() const {
    return socket_fd_ >= 0;
}

}

// === ARCHIVO: src/adapters/OrderManagementAdapter.cpp ===
#include "adapters/OrderManagementAdapter.h"
#include "domain/Order.h"
#include "domain/RiskLimits.h"
#include "core/CircuitBreaker.h"
#include <chrono>
#include <thread>
#include <atomic>
#include <future>
#include <unordered_map>
#include <arpa/inet.h>
#include <unistd.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netdb.h>
#include "spdlog/spdlog.h"
#include "spdlog/sinks/rotating_file_sink.h"

namespace risk_engine {

constexpr int DEFAULT_EXCHANGE_PORT = 8080;
constexpr int ORDER_TIMEOUT_MS = 5000;
constexpr int MAX_PENDING_ORDERS = 10000;
constexpr size_t EXCHANGE_MSG_BUFFER = 8192;

OrderManagementAdapter::OrderManagementAdapter(
    std::shared_ptr<disruptor::RingBuffer<OrderEvent>> order_ring_buffer,
    std::shared_ptr<CircuitBreakerManager> circuit_breaker_manager,
    const ExchangeConfig& config)
    : order_ring_buffer_(order_ring_buffer)
    , circuit_breaker_manager_(circuit_breaker_manager)
    , config_(config)
    , running_(false)
    , orders_sent_(0)
    , orders_accepted_(0)
    , orders_rejected_(0)
    , last_order_id_(0) {
    
    SPDLOG_INFO("OrderManagementAdapter initializing with exchange: {}", config.exchange_url);
    setupLogging();
    initializeCircuitBreakers();
}

void OrderManagementAdapter::setupLogging() {
    try {
        auto rotating_sink = std::make_shared<spdlog::sinks::rotating_file_sink_mt>(
            "logs/order_management.log", 1024 * 1024 * 10, 5);
        logger_ = std::make_shared<spdlog::logger>("order_management", rotating_sink);
        logger_->set_level(spdlog::level::debug);
        logger_->set_pattern("%Y-%m-%d %H:%M:%S.%f [%l] [orderId=%i] %v");
    } catch (const spdlog::spdlog_ex& ex) {
        SPDLOG_WARN("Failed to create order management logger: {}", ex.what());
        logger_ = spdlog::default_logger();
    }
}

void OrderManagementAdapter::initializeCircuitBreakers() {
    if (!circuit_breaker_manager_) {
        SPDLOG_WARN("Circuit breaker manager not provided");
        return;
    }

    for (const auto& instrument : config_.instruments) {
        auto cb = circuit_breaker_manager_->getOrCreateCircuitBreaker(
            instrument, 
            config_.circuit_breaker_threshold,
            config_.circuit_breaker_timeout);
        
        SPDLOG_INFO("Circuit breaker initialized for instrument: {}", instrument);
    }
}

bool OrderManagementAdapter::connect() {
    std::lock_guard<std::mutex> lock(exchange_mutex_);
    
    if (exchange_socket_ >= 0) {
        SPDLOG_INFO("Already connected to exchange");
        return true;
    }

    SPDLOG_INFO("Connecting to exchange: {}", config_.exchange_url);
    
    std::string host;
    int port;
    if (!parseExchangeUrl(config_.exchange_url, host, port)) {
        SPDLOG_ERROR("Failed to parse exchange URL: {}", config_.exchange_url);
        return false;
    }

    exchange_socket_ = socket(AF_INET, SOCK_STREAM, 0);
    if (exchange_socket_ < 0) {
        SPDLOG_ERROR("Failed to create exchange socket: {}", strerror(errno));
        return false;
    }

    struct hostent* server = gethostbyname(host.c_str());
    if (server == nullptr) {
        SPDLOG_ERROR("Failed to resolve exchange host: {}", host);
        close(exchange_socket_);
        exchange_socket_ = -1;
        return false;
    }

    struct sockaddr_in server_addr;
    std::memset(&server_addr, 0, sizeof(server_addr));
    server_addr.sin_family = AF_INET;
    std::memcpy(&server_addr.sin_addr.s_addr, server->h_addr, server->h_length);
    server_addr.sin_port = htons(port);

    struct timeval timeout;
    timeout.tv_sec = 5;
    timeout.tv_usec = 0;
    setsockopt(exchange_socket_, SOL_SOCKET, SO_RCVTIMEO, &timeout, sizeof(timeout));
    setsockopt(exchange_socket_, SOL_SOCKET, SO_SNDTIMEO, &timeout, sizeof(timeout));

    if (::connect(exchange_socket_, (struct sockaddr*)&server_addr, sizeof(server_addr)) < 0) {
        SPDLOG_ERROR("Failed to connect to exchange: {}", strerror(errno));
        close(exchange_socket_);
        exchange_socket_ = -1;
        return false;
    }

    SPDLOG_INFO("Connected to exchange successfully");
    return true;
}

bool OrderManagementAdapter::parseExchangeUrl(const std::string& url, 
                                               std::string& host, int& port) {
    size_t protocol_end = url.find("://");
    std::string stripped_url = (protocol_end != std::string::npos) 
        ? url.substr(protocol_end + 3) 
        : url;
    
    size_t colon_pos = stripped_url.rfind(':');
    if (colon_pos == std::string::npos) {
        host = stripped_url;
        port = DEFAULT_EXCHANGE_PORT;
        return true;
    }
    
    host = stripped_url.substr(0, colon_pos);
    try {
        port = std::stoi(stripped_url.substr(colon_pos + 1));
    } catch (...) {
        port = DEFAULT_EXCHANGE_PORT;
    }
    return true;
}

void OrderManagementAdapter::disconnect() {
    std::lock_guard<std::mutex> lock(exchange_mutex_);
    
    if (exchange_socket_ >= 0) {
        close(exchange_socket_);
        exchange_socket_ = -1;
        SPDLOG_INFO("Disconnected from exchange");
    }
}

void OrderManagementAdapter::start() {
    if (running_.exchange(true)) {
        SPDLOG_WARN("OrderManagementAdapter already running");
        return;
    }

    if (!connect()) {
        running_ = false;
        SPDLOG_ERROR("Failed to connect to exchange");
        return;
    }

    response_handler_thread_ = std::thread([this]() {
        SPDLOG_INFO("OrderManagementAdapter response handler started");
        handleResponses();
        SPDLOG_INFO("OrderManagementAdapter response handler stopped");
    });

    SPDLOG_INFO("OrderManagementAdapter started");
}

void OrderManagementAdapter::stop() {
    if (!running_.exchange(false)) {
        SPDLOG_INFO("OrderManagementAdapter already stopped");
        return;
    }

    disconnect();
    
    if (response_handler_thread_.joinable()) {
        response_handler_thread_.join();
    }
    
    logStatistics();
}

void OrderManagementAdapter::handleResponses() {
    char buffer[EXCHANGE_MSG_BUFFER];
    std::vector<char> response_buffer;
    response_buffer.reserve(16384);

    while (running_.load()) {
        fd_set read_fds;
        FD_ZERO(&read_fds);
        
        {
            std::lock_guard<std::mutex> lock(exchange_mutex_);
            if (exchange_socket_ >= 0) {
                FD_SET(exchange_socket_, &read_fds);
            }
        }

        struct timeval timeout;
        timeout.tv_sec = 0;
        timeout.tv_usec = 500000;

        int activity = select(exchange_socket_ + 1, &read_fds, nullptr, nullptr, &timeout);
        
        if (activity <= 0) {
            continue;
        }

        ssize_t bytes_read = recv(exchange_socket_, buffer, sizeof(buffer) - 1, 0);
        
        if (bytes_read <= 0) {
            SPDLOG_WARN("Exchange connection closed");
            continue;
        }

        buffer[bytes_read] = '\0';
        response_buffer.insert(response_buffer.end(), buffer, buffer + bytes_read);
        
        processResponseBuffer(response_buffer);
    }
}

void OrderManagementAdapter::processResponseBuffer(std::vector<char>& buffer) {
    while (true) {
        size_t newline_pos = 0;
        for (size_t i = 0; i + 1 < buffer.size(); ++i) {
            if (buffer[i] == '\n' && buffer[i + 1] == '\n') {
                newline_pos = i;
                break;
            }
        }

        if (newline_pos == 0) {
            break;
        }

        std::string response(buffer.begin(), buffer.begin() + newline_pos);
        buffer.erase(buffer.begin(), buffer.begin() + newline_pos + 2);

        processExchangeResponse(response);
    }
}

void OrderManagementAdapter::processExchangeResponse(const std::string& response) {
    try {
        auto json = nlohmann::json::parse(response);
        
        std::string order_id = json.value("order_id", "");
        std::string status = json.value("status", "");
        std::string instrument_id = json.value("instrument_id", "");
        
        auto it = pending_orders_.find(order_id);
        if (it == pending_orders_.end()) {
            SPDLOG_WARN("Received response for unknown order: {}", order_id);
            return;
        }

        Order& order = it->second;
        
        if (status == "accepted" || status == "filled") {
            order.status = OrderStatus::FILLED;
            orders_accepted_.fetch_add(1);
            SPDLOG_INFO("Order accepted: {} instrument={} qty={} price={}",
                        order_id, instrument_id, order.quantity, order.price);
            
            publishOrderEvent(order, OrderEventType::ORDER_ACCEPTED);
        } else if (status == "rejected") {
            order.status = OrderStatus::REJECTED;
            orders_rejected_.fetch_add(1);
            std::string reason = json.value("reason", "unknown");
            SPDLOG_WARN("Order rejected: {} reason={}", order_id, reason);
            
            publishOrderEvent(order, OrderEventType::ORDER_REJECTED);
            
            if (circuit_breaker_manager_) {
                circuit_breaker_manager_->recordRejection(instrument_id);
            }
        }

        pending_orders_.erase(it);
        
    } catch (const std::exception& e) {
        SPDLOG_ERROR("Error processing exchange response: {}", e.what());
    }
}

OrderResult OrderManagementAdapter::submitOrder(const Order& order) {
    if (!running_.load()) {
        return {false, "Adapter not running", 0};
    }

    if (circuit_breaker_manager_) {
        auto cb = circuit_breaker_manager_->getCircuitBreaker(order.instrument_id);
        if (cb && cb->isOpen()) {
            SPDLOG_WARN("Circuit breaker open for instrument: {}", order.instrument_id);
            return {false, "Circuit breaker open", 0};
        }
    }

    if (pending_orders_.size() >= MAX_PENDING_ORDERS) {
        SPDLOG_ERROR("Too many pending orders");
        return {false, "Too many pending orders", 0};
    }

    std::string order_id = generateOrderId(order);
    Order order_with_id = order;
    order_with_id.order_id = order_id;
    order_with_id.submitted_at = std::chrono::system_clock::now();

    std::lock_guard<std::mutex> lock(exchange_mutex_);
    if (exchange_socket_ < 0) {
        return {false, "Not connected to exchange", 0};
    }

    auto json = serializeOrder(order_with_id);
    std::string message = json.dump() + "\n\n";
    
    ssize_t sent = send(exchange_socket_, message.c_str(), message.size(), 0);
    if (sent < 0) {
        SPDLOG_ERROR("Failed to send order: {}", strerror(errno));
        return {false, "Failed to send order", 0};
    }

    pending_orders_[order_id] = order_with_id;
    orders_sent_.fetch_add(1);
    
    SPDLOG_INFO("Order submitted: {} instrument={} qty={} price={}",
                order_id, order.instrument_id, order.quantity, order.price);
    
    publishOrderEvent(order_with_id, OrderEventType::ORDER_SUBMITTED);
    
    return {true, "", std::stoull(order_id)};
}

std::string OrderManagementAdapter::generateOrderId(const Order& order) {
    uint64_t id = last_order_id_.fetch_add(1);
    auto now = std::chrono::system_clock::now();
    auto timestamp = std::chrono::duration_cast<std::chrono::milliseconds>(
        now.time_since_epoch()).count();
    
    return std::to_string(timestamp) + "-" + std::to_string(id);
}

nlohmann::json OrderManagementAdapter::serializeOrder(const Order& order) {
    nlohmann::json json;
    json["order_id"] = order.order_id;
    json["instrument_id"] = order.instrument_id;
    json["side"] = order.side == OrderSide::BUY ? "buy" : "sell";
    json["type"] = order.order_type == OrderType::MARKET ? "market" : "limit";
    json["quantity"] = order.quantity;
    json["price"] = order.price;
    json["trader_id"] = order.trader_id;
    json["strategy_id"] = order.strategy_id;
    json["timestamp"] = std::chrono::duration_cast<std::chrono::milliseconds>(
        order.submitted_at.time_since_epoch()).count();
    return json;
}

void OrderManagementAdapter::publishOrderEvent(const Order& order, OrderEventType event_type) {
    if (!order_ring_buffer_) {
        SPDLOG_ERROR("Order ring buffer not initialized");
        return;
    }

    auto sequence = order_ring_buffer_->next();
    try {
        auto* slot = order_ring_buffer_->get(sequence);
        slot->order_id = order.order_id;
        slot->instrument_id = order.instrument_id;
        slot->event_type = event_type;
        slot->timestamp = std::chrono::system_clock::now();
        slot->quantity = order.quantity;
        slot->price = order.price;
        order_ring_buffer_->publish(sequence);
        
        SPDLOG_TRACE("Published order event: {} type={}", order.order_id, static_cast<int>(event_type));
    } catch (const std::exception& e) {
        SPDLOG_ERROR("Failed to publish order event: {}", e.what());
    }
}

void OrderManagementAdapter::logStatistics() {
    SPDLOG_INFO("OrderManagementAdapter Statistics:");
    SPDLOG_INFO("  Orders sent: {}", orders_sent_.load());
    SPDLOG_INFO("  Orders accepted: {}", orders_accepted_.load());
    SPDLOG_INFO("  Orders rejected: {}", orders_rejected_.load());
    SPDLOG_INFO("  Pending orders: {}", pending_orders_.size());
}

std::atomic<uint64_t> OrderManagementAdapter::getOrdersSent() const {
    return orders_sent_;
}

std::atomic<uint64_t> OrderManagementAdapter::getOrdersAccepted() const {
    return orders_accepted_;
}

std::atomic<uint64_t> OrderManagementAdapter::getOrdersRejected() const {
    return orders_rejected_;
}

bool OrderManagementAdapter::isConnected() const {
    return exchange_socket_ >= 0;
}

}


// === ARCHIVO: src/infrastructure/DisruptorConfig.cpp ===
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
// === ARCHIVO: src/infrastructure/ShardingStrategy.cpp ===
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

// === ARCHIVO: src/application/KillSwitch.cpp ===
#include "application/KillSwitch.h"
#include "core/RiskEngine.h"
#include <spdlog/spdlog.h>
#include <chrono>
#include <cmath>
#include <algorithm>
#include <numeric>

namespace risk_engine {

KillSwitch::KillSwitch(const KillSwitchConfig& config)
    : config_(config)
    , state_(KillSwitchState::CLOSED)
    , anomalyScore_(0.0)
    , lastResetTime_(std::chrono::steady_clock::now()) {
    SPDLOG_INFO("KillSwitch inicializado con ventana={}ms, threshold={}",
                config.anomalyDetectionWindowMs, config.thresholdDeviation);
}

void KillSwitch::recordLatencySample(int64_t latencyMicros) {
    auto now = std::chrono::steady_clock::now();
    std::lock_guard<std::mutex> lock(mutex_);
    
    latencyHistory_.push_back({now, latencyMicros});
    
    auto windowStart = now - std::chrono::milliseconds(config_.anomalyDetectionWindowMs);
    latencyHistory_.erase(
        std::remove_if(latencyHistory_.begin(), latencyHistory_.end(),
            [windowStart](const LatencySample& s) { return s.timestamp < windowStart; }),
        latencyHistory_.end()
    );
    
    evaluateAnomalyScore();
}

void KillSwitch::recordErrorRate(double errorRate) {
    std::lock_guard<std::mutex> lock(mutex_);
    errorRateHistory_.push_back(errorRate);
    
    if (errorRateHistory_.size() > 100) {
        errorRateHistory_.erase(errorRateHistory_.begin());
    }
    
    evaluateAnomalyScore();
}

void KillSwitch::recordRejectionRate(double rejectionRate) {
    std::lock_guard<std::mutex> lock(mutex_);
    rejectionRateHistory_.push_back(rejectionRate);
    
    if (rejectionRateHistory_.size() > 100) {
        rejectionRateHistory_.erase(rejectionRateHistory_.begin());
    }
    
    evaluateAnomalyScore();
}

void KillSwitch::evaluateAnomalyScore() {
    if (latencyHistory_.empty()) {
        return;
    }
    
    std::vector<int64_t> latencies;
    latencies.reserve(latencyHistory_.size());
    for (const auto& sample : latencyHistory_) {
        latencies.push_back(sample.latencyMicros);
    }
    
    double mean = std::accumulate(latencies.begin(), latencies.end(), 0.0) / latencies.size();
    double sqSum = 0.0;
    for (int64_t l : latencies) {
        sqSum += (l - mean) * (l - mean);
    }
    double stdDev = std::sqrt(sqSum / latencies.size());
    
    double latencyAnomalyScore = (stdDev > 0) ? (stdDev / (mean + 1)) : 0.0;
    
    double errorAnomalyScore = 0.0;
    if (!errorRateHistory_.empty()) {
        double meanError = std::accumulate(errorRateHistory_.begin(), errorRateHistory_.end(), 0.0) / errorRateHistory_.size();
        errorAnomalyScore = std::min(meanError * 10.0, 1.0);
    }
    
    double rejectionAnomalyScore = 0.0;
    if (!rejectionRateHistory_.empty()) {
        double meanRejection = std::accumulate(rejectionRateHistory_.begin(), rejectionRateHistory_.end(), 0.0) / rejectionRateHistory_.size();
        rejectionAnomalyScore = std::min(meanRejection * 5.0, 1.0);
    }
    
    anomalyScore_ = (latencyAnomalyScore * 0.5) + (errorAnomalyScore * 0.3) + (rejectionAnomalyScore * 0.2);
    
    SPDLOG_DEBUG("Anomaly score actualizado: latency={:.3f}, error={:.3f}, rejection={:.3f}, total={:.3f}",
                 latencyAnomalyScore, errorAnomalyScore, rejectionAnomalyScore, anomalyScore_);
    
    if (anomalyScore_ > config_.thresholdDeviation && state_ == KillSwitchState::CLOSED) {
        triggerKillSwitch("Anomaly score exceeded threshold");
    }
}

void KillSwitch::triggerKillSwitch(const std::string& reason) {
    state_ = KillSwitchState::OPEN;
    SPDLOG_WARN("KILL SWITCH ACTIVADO: {}", reason);
    SPDLOG_WARN("Anomaly score: {:.3f}, Threshold: {:.3f}", anomalyScore_, config_.thresholdDeviation);
    
    emitAuditEvent("KILL_SWITCH_TRIGGERED", reason);
}

void KillSwitch::reset() {
    std::lock_guard<std::mutex> lock(mutex_);
    
    state_ = KillSwitchState::CLOSED;
    anomalyScore_ = 0.0;
    latencyHistory_.clear();
    errorRateHistory_.clear();
    rejectionRateHistory_.clear();
    lastResetTime_ = std::chrono::steady_clock::now();
    
    SPDLOG_INFO("KillSwitch reseteado manualmente");
    emitAuditEvent("KILL_SWITCH_RESET", "Manual reset");
}

KillSwitchState KillSwitch::getState() const {
    return state_;
}

double KillSwitch::getAnomalyScore() const {
    return anomalyScore_;
}

bool KillSwitch::shouldBlockOrders() const {
    return state_ == KillSwitchState::OPEN;
}

void KillSwitch::emitAuditEvent(const std::string& eventType, const std::string& details) {
    auto now = std::chrono::system_clock::now();
    auto timestamp = std::chrono::duration_cast<std::chrono::milliseconds>(
        now.time_since_epoch()
    ).count();
    
    SPDLOG_INFO("MiFID2_AUDIT: timestamp={}, eventType={}, details={}, state={}, anomalyScore={:.3f}",
                timestamp, eventType, details, 
                state_ == KillSwitchState::OPEN ? "OPEN" : "CLOSED",
                anomalyScore_);
}

void KillSwitch::autoResetIfAllowed() {
    if (state_ != KillSwitchState::OPEN) {
        return;
    }
    
    auto now = std::chrono::steady_clock::now();
    auto elapsed = std::chrono::duration_cast<std::chrono::milliseconds>(
        now - lastResetTime_
    ).count();
    
    if (elapsed >= config_.autoResetTimeoutMs) {
        state_ = KillSwitchState::HALF_OPEN;
        SPDLOG_INFO("KillSwitch transitando a estado HALF_OPEN tras timeout de {}ms", elapsed);
        emitAuditEvent("KILL_SWITCH_HALF_OPEN", "Auto transition after timeout");
    }
}

} // namespace risk_engine

// === ARCHIVO: src/application/ReplayEngine.cpp ===
#include "application/ReplayEngine.h"
#include "core/RiskEngine.h"
#include <spdlog/spdlog.h>
#include <fstream>
#include <sstream>
#include <chrono>
#include <iomanip>
#include <algorithm>

namespace risk_engine {

ReplayEngine::ReplayEngine(const ReplayConfig& config, std::shared_ptr<RiskEngine> riskEngine)
    : config_(config)
    , riskEngine_(riskEngine)
    , isReplaying_(false)
    , currentEventIndex_(0)
    , playbackSpeed_(1.0) {
    SPDLOG_INFO("ReplayEngine inicializado con config: replayFile={}, deterministic={}",
                config.replayFilePath, config.deterministicMode ? "true" : "false");
}

bool ReplayEngine::loadEventsFromFile(const std::string& filePath) {
    std::ifstream file(filePath);
    if (!file.is_open()) {
        SPDLOG_ERROR("No se pudo abrir el archivo de replay: {}", filePath);
        return false;
    }
    
    events_.clear();
    std::string line;
    int64_t lineNumber = 0;
    
    while (std::getline(file, line)) {
        lineNumber++;
        if (line.empty() || line[0] == '#') {
            continue;
        }
        
        try {
            auto event = parseEvent(line, lineNumber);
            if (event) {
                events_.push_back(*event);
            }
        } catch (const std::exception& e) {
            SPDLOG_ERROR("Error parseando línea {}: {}", lineNumber, e.what());
            if (!config_.skipMalformedEvents) {
                return false;
            }
        }
    }
    
    SPDLOG_INFO("Cargados {} eventos desde {}", events_.size(), filePath);
    return true;
}

std::optional<ReplayEvent> ReplayEngine::parseEvent(const std::string& line, int64_t lineNumber) {
    std::istringstream iss(line);
    std::string timestampStr, eventType, payload;
    
    if (!std::getline(iss, timestampStr, '|')) return std::nullopt;
    if (!std::getline(iss, eventType, '|')) return std::nullopt;
    if (!std::getline(iss, payload)) return std::nullopt;
    
    ReplayEvent event;
    event.lineNumber = lineNumber;
    event.timestamp = std::stoll(timestampStr);
    event.eventType = eventType;
    event.payload = payload;
    event.replayed = false;
    event.replayResult = "";
    
    return event;
}

bool ReplayEngine::startReplay() {
    if (events_.empty()) {
        SPDLOG_ERROR("No hay eventos para reproducir");
        return false;
    }
    
    if (config_.deterministicMode) {
        std::sort(events_.begin(), events_.end(), 
            [](const ReplayEvent& a, const ReplayEvent& b) {
                return a.timestamp < b.timestamp;
            });
        SPDLOG_INFO("Eventos ordenados determinísticamente por timestamp");
    }
    
    isReplaying_ = true;
    currentEventIndex_ = 0;
    startReplayTimestamp_ = currentTimestampMillis();
    
    SPDLOG_INFO("Iniciando replay de {} eventos", events_.size());
    emitAuditEvent("REPLAY_STARTED", "");
    
    return true;
}

bool ReplayEngine::processNextEvent() {
    if (!isReplaying_ || currentEventIndex_ >= events_.size()) {
        return false;
    }
    
    auto& event = events_[currentEventIndex_];
    
    if (config_.deterministicMode) {
        int64_t expectedTime = event.timestamp;
        int64_t currentTime = currentTimestampMillis();
        int64_t targetTime = startReplayTimestamp_ + static_cast<int64_t>(
            (expectedTime - events_[0].timestamp) / playbackSpeed_
        );
        
        while (currentTime < targetTime) {
            std::this_thread::sleep_for(std::chrono::milliseconds(1));
            currentTime = currentTimestampMillis();
        }
    }
    
    bool success = executeEvent(event);
    event.replayed = true;
    event.replayResult = success ? "SUCCESS" : "FAILURE";
    
    if (config_.recordEveryEvent) {
        emitAuditEvent("REPLAY_EVENT", 
            "type=" + event.eventType + 
            ", line=" + std::to_string(event.lineNumber) + 
            ", result=" + event.replayResult);
    }
    
    currentEventIndex_++;
    
    if (currentEventIndex_ >= events_.size()) {
        finishReplay();
    }
    
    return success;
}

bool ReplayEngine::executeEvent(const ReplayEvent& event) {
    try {
        if (event.eventType == "ORDER_SUBMITTED") {
            return replayOrderSubmission(event);
        } else if (event.eventType == "MARKET_DATA") {
            return replayMarketDataUpdate(event);
        } else if (event.eventType == "RISK_CHECK") {
            return replayRiskCheck(event);
        } else if (event.eventType == "CIRCUIT_BREAKER") {
            return replayCircuitBreaker(event);
        } else {
            SPDLOG_WARN("Tipo de evento desconocido: {}", event.eventType);
            return false;
        }
    } catch (const std::exception& e) {
        SPDLOG_ERROR("Error ejecutando evento {}: {}", event.lineNumber, e.what());
        return false;
    }
}

bool ReplayEngine::replayOrderSubmission(const ReplayEvent& event) {
    if (!riskEngine_) {
        SPDLOG_ERROR("RiskEngine no disponible para replay");
        return false;
    }
    
    nlohmann::json payload = nlohmann::json::parse(event.payload);
    
    std::string orderId = payload.value("orderId", "");
    std::string instrumentId = payload.value("instrumentId", "");
    double quantity = payload.value("quantity", 0.0);
    double price = payload.value("price", 0.0);
    std::string traderId = payload.value("traderId", "");
    std::string strategyId = payload.value("strategyId", "");
    
    RiskDecision decision = riskEngine_->evaluateOrderRisk(
        orderId, instrumentId, quantity, price, traderId, strategyId
    );
    
    SPDLOG_DEBUG("Replay order {} -> decision: {}", orderId, 
                 decision == RiskDecision::APPROVED ? "APPROVED" : "REJECTED");
    
    return true;
}

bool ReplayEngine::replayMarketDataUpdate(const ReplayEvent& event) {
    nlohmann::json payload = nlohmann::json::parse(event.payload);
    
    std::string instrumentId = payload.value("instrumentId", "");
    double bidPrice = payload.value("bidPrice", 0.0);
    double askPrice = payload.value("askPrice", 0.0);
    int64_t bidSize = payload.value("bidSize", 0);
    int64_t askSize = payload.value("askSize", 0);
    
    if (riskEngine_) {
        riskEngine_->updateMarketData(instrumentId, bidPrice, askPrice, bidSize, askSize);
    }
    
    return true;
}

bool ReplayEngine::replayRiskCheck(const ReplayEvent& event) {
    nlohmann::json payload = nlohmann::json::parse(event.payload);
    
    std::string traderId = payload.value("traderId", "");
    double exposure = payload.value("exposure", 0.0);
    double limit = payload.value("limit", 0.0);
    
    if (riskEngine_) {
        bool withinLimit = riskEngine_->checkTraderLimit(traderId, exposure, limit);
        SPDLOG_DEBUG("Replay risk check: trader={}, exposure={}, limit={}, within={}",
                     traderId, exposure, limit, withinLimit);
    }
    
    return true;
}

bool ReplayEngine::replayCircuitBreaker(const ReplayEvent& event) {
    nlohmann::json payload = nlohmann::json::parse(event.payload);
    
    std::string instrumentId = payload.value("instrumentId", "");
    std::string state = payload.value("state", "");
    
    SPDLOG_DEBUG("Replay circuit breaker: instrument={}, state={}", instrumentId, state);
    
    return true;
}

void ReplayEngine::finishReplay() {
    isReplaying_ = false;
    
    int successCount = 0;
    int failureCount = 0;
    for (const auto& event : events_) {
        if (event.replayed) {
            if (event.replayResult == "SUCCESS") {
                successCount++;
            } else {
                failureCount++;
            }
        }
    }
    
    SPDLOG_INFO("Replay terminado: {} eventos, {} exitos, {} fallidos",
                events_.size(), successCount, failureCount);
    
    emitAuditEvent("REPLAY_COMPLETED", 
        "total=" + std::to_string(events_.size()) + 
        ", success=" + std::to_string(successCount) + 
        ", failure=" + std::to_string(failureCount));
    
    generateReplayReport();
}

void ReplayEngine::generateReplayReport() const {
    std::string reportPath = config_.reportPath.empty() ? 
        "logs/replay_report.json" : config_.reportPath;
    
    nlohmann::json report;
    report["totalEvents"] = events_.size();
    report["replayedEvents"] = std::count_if(events_.begin(), events_.end(),
        [](const ReplayEvent& e) { return e.replayed; });
    report["deterministicMode"] = config_.deterministicMode;
    report["playbackSpeed"] = playbackSpeed_;
    
    nlohmann::json eventsJson = nlohmann::json::array();
    for (const auto& event : events_) {
        eventsJson.push_back({
            {"lineNumber", event.lineNumber},
            {"timestamp", event.timestamp},
            {"eventType", event.eventType},
            {"replayed", event.replayed},
            {"result", event.replayResult}
        });
    }
    report["events"] = eventsJson;
    
    std::ofstream outFile(reportPath);
    if (outFile.is_open()) {
        outFile << report.dump(2);
        outFile.close();
        SPDLOG_INFO("Reporte de replay generado: {}", reportPath);
    } else {
        SPDLOG_ERROR("No se pudo generar el reporte de replay: {}", reportPath);
    }
}

void ReplayEngine::emitAuditEvent(const std::string& eventType, const std::string& details) const {
    auto now = std::chrono::system_clock::now();
    auto timestamp = std::chrono::duration_cast<std::chrono::milliseconds>(
        now.time_since_epoch()
    ).count();
    
    SPDLOG_INFO("MiFID2_AUDIT_REPLAY: timestamp={}, eventType={}, details={}, eventIndex={}/{}",
                timestamp, eventType, details, currentEventIndex_, events_.size());
}

int64_t ReplayEngine::currentTimestampMillis() const {
    auto now = std::chrono::system_clock::now();
    return std::chrono::duration_cast<std::chrono::milliseconds>(
        now.time_since_epoch()
    ).count();
}

void ReplayEngine::setPlaybackSpeed(double speed) {
    playbackSpeed_ = std::max(0.1, std::min(10.0, speed));
    SPDLOG_INFO("Playback speed ajustado a {:.1f}x", playbackSpeed_);
}

bool ReplayEngine::isReplaying() const {
    return isReplaying_;
}

size_t ReplayEngine::getEventCount() const {
    return events_.size();
}

size_t ReplayEngine::getProcessedEventCount() const {
    return currentEventIndex_;
}

} // namespace risk_engine

// === ARCHIVO: src/main.cpp ===
#include <iostream>
#include <memory>
#include <string>
#include <vector>
#include <csignal>
#include <thread>
#include <chrono>
#include <spdlog/spdlog.h>
#include <spdlog/sinks/stdout_color_sinks.h>
#include <spdlog/sinks/rotating_file_sinks.h>
#include <nlohmann/json.hpp>
#include "core/RiskEngine.h"
#include "core/CircuitBreaker.h"
#include "application/KillSwitch.h"
#include "application/ReplayEngine.h"
#include "infrastructure/DisruptorConfig.h"
#include "infrastructure/ShardingStrategy.h"

namespace {
    std::atomic<bool> g_running{true};
    
    void signalHandler(int signal) {
        if (signal == SIGINT || signal == SIGTERM) {
            spdlog::warn("Señal de terminación recibida, shutting down...");
            g_running = false;
        }
    }
}

namespace risk_engine {

void initializeLogging() {
    try {
        auto consoleSink = std::make_shared<spdlog::sinks::stdout_color_sink_mt>();
        consoleSink->set_level(spdlog::level::info);
        
        auto fileSink = std::make_shared<spdlog::sinks::rotating_file_sink_mt>(
            "logs/risk_engine.log", 1024 * 1024 * 10, 3
        );
        fileSink->set_level(spdlog::level::debug);
        
        std::vector<spdlog::sink_ptr> sinks{consoleSink, fileSink};
        auto logger = std::make_shared<spdlog::logger>( "risk_engine", begin(sinks), end(sinks) );
        logger->set_level(spdlog::level::debug);
        logger->set_pattern("[%Y-%m-%d %H:%M:%S.%e] [%^%l%$] [%t] %v");
        
        spdlog::register_logger(logger);
        spdlog::set_default_logger(logger);
        
        SPDLOG_INFO("Logging inicializado: console + rotating file");
    } catch (const spdlog::spdlog_ex& ex) {
        std::cerr << "Error inicializando logging: " << ex.what() << std::endl;
    }
}

RiskEngineConfig loadConfig(const std::string& configPath) {
    try {
        std::ifstream configFile(configPath);
        if (!configFile.is_open()) {
            SPDLOG_WARN("No se pudo abrir {}, usando configuración por defecto", configPath);
            return RiskEngineConfig{};
        }
        
        nlohmann::json configJson;
        configFile >> configJson;
        
        RiskEngineConfig config;
        config.latencyTargetUs = configJson.value("latencyTargetUs", 500);
        config.p99Target = configJson.value("p99Target", 500);
        config.shardingStrategy = configJson.value("shardingStrategy", "byInstrument");
        
        if (configJson.contains("circuitBreaker")) {
            auto cb = configJson["circuitBreaker"];
            config.circuitBreakerEnabled = cb.value("enabled", true);
            config.circuitBreakerThresholdMultiplier = cb.value("thresholdMultiplier", 1.5);
            config.circuitBreakerResetTimeoutMs = cb.value("resetTimeoutMs", 5000);
        }
        
        if (configJson.contains("killSwitch")) {
            auto ks = configJson["killSwitch"];
            config.killSwitchEnabled = ks.value("enabled", true);
            config.killSwitchThresholdDeviation = ks.value("thresholdDeviation", 3.0);
            config.killSwitchWindowMs = ks.value("anomalyDetectionWindowMs", 60000);
        }
        
        SPDLOG_INFO("Configuración cargada desde {}", configPath);
        return config;
    } catch (const std::exception& e) {
        SPDLOG_ERROR("Error cargando configuración: {}", e.what());
        return RiskEngineConfig{};
    }
}

void runNormalMode(const std::string& configPath) {
    SPDLOG_INFO("=== INICIANDO MODO NORMAL ===");
    
    auto config = loadConfig(configPath);
    auto riskEngine = std::make_unique<RiskEngine>(config);
    
    if (!riskEngine->initialize()) {
        SPDLOG_ERROR("Fallo al inicializar RiskEngine");
        return;
    }
    
    KillSwitchConfig ksConfig;
    ksConfig.enabled = config.killSwitchEnabled;
    ksConfig.thresholdDeviation = config.killSwitchThresholdDeviation;
    ksConfig.anomalyDetectionWindowMs = config.killSwitchWindowMs;
    ksConfig.autoResetTimeoutMs = 300000;
    auto killSwitch = std::make_unique<KillSwitch>(ksConfig);
    
    CircuitBreakerConfig cbConfig;
    cbConfig.enabled = config.circuitBreakerEnabled;
    cbConfig.thresholdMultiplier = config.circuitBreakerThresholdMultiplier;
    cbConfig.resetTimeoutMs = config.circuitBreakerResetTimeoutMs;
    auto circuitBreaker = std::make_unique<CircuitBreaker>(cbConfig);
    
    riskEngine->setKillSwitch(killSwitch.get());
    riskEngine->setCircuitBreaker(circuitBreaker.get());
    
    SPDLOG_INFO("RiskEngine inicializado con latencia objetivo: {}us p99", config.latencyTargetUs);
    SPDLOG_INFO("KillSwitch: {}, CircuitBreaker: {}", 
                 ksConfig.enabled ? "enabled" : "disabled",
                 cbConfig.enabled ? "enabled" : "disabled");
    
    int eventCount = 0;
    auto lastReport = std::chrono::steady_clock::now();
    
    while (g_running) {
        auto now = std::chrono::steady_clock::now();
        
        if (riskEngine->processEvents()) {
            eventCount++;
        }
        
        auto elapsed = std::chrono::duration_cast<std::chrono::seconds>(now - lastReport).count();
        if (elapsed >= 10) {
            auto stats = riskEngine->getStatistics();
            SPDLOG_INFO("Stats: events={}, avgLatency={}us, p99Latency={}us, rejections={}",
                        eventCount, stats.avgLatencyUs, stats.p99LatencyUs, stats.rejectionCount);
            lastReport = now;
        }
        
        std::this_thread::sleep_for(std::chrono::milliseconds(1));
    }
    
    SPDLOG_INFO("=== MODO NORMAL FINALIZADO ===");
}

void runReplayMode(const std::string& replayFile) {
    SPDLOG_INFO("=== INICIANDO MODO REPLAY ===");
    
    auto config = loadConfig("config/risk_config.json");
    auto riskEngine = std::make_unique<RiskEngine>(config);
    riskEngine->initialize();
    
    ReplayConfig replayConfig;
    replayConfig.replayFilePath = replayFile;
    replayConfig.deterministicMode = true;
    replayConfig.recordEveryEvent = true;
    replayConfig.skipMalformedEvents = false;
    replayConfig.reportPath = "logs/replay_report.json";
    
    auto replayEngine = std::make_unique<ReplayEngine>(replayConfig, std::move(riskEngine));
    
    if (!replayEngine->loadEventsFromFile(replayFile)) {
        SPDLOG_ERROR("Fallo al cargar eventos desde {}", replayFile);
        return;
    }
    
    if (!replayEngine->startReplay()) {
        SPDLOG_ERROR("Fallo al iniciar replay");
        return;
    }
    
    while (replayEngine->isReplaying()) {
        replayEngine->processNextEvent();
        
        if (g_running.load() == false) {
            SPDLOG_WARN("Replay interrumpido por señal");
            break;
        }
    }
    
    SPDLOG_INFO("=== MODO REPLAY FINALIZADO ===");
}

void printUsage(const char* programName) {
    std::cout << "Usage: " << programName << " [options]\n";
    std::cout << "Options:\n";
    std::cout << "  --replay <file>    Modo replay determinístico\n";
    std::cout << "  --config <path>   Path a config (default: config/risk_config.json)\n";
    std::cout << "  --help            Mostrar esta ayuda\n";
}

} // namespace risk_engine

int main(int argc, char* argv[]) {
    std::signal(SIGINT, ::signalHandler);
    std::signal(SIGTERM, ::signalHandler);
    
    risk_engine::initializeLogging();
    SPDLOG_INFO("Risk Engine LMAX - Starting...");
    SPDLOG_INFO("C++20 with LMAX Disruptor 4.0");
    
    std::string configPath = "config/risk_config.json";
    std::string replayFile;
    bool replayMode = false;
    
    for (int i = 1; i < argc; ++i) {
        std::string arg = argv[i];
        if (arg == "--replay" && i + 1 < argc) {
            replayFile = argv[++i];
            replayMode = true;
        } else if (arg == "--config" && i + 1 < argc) {
            configPath = argv[++i];
        } else if (arg == "--help") {
            risk_engine::printUsage(argv[0]);
            return 0;
        }
    }
    
    try {
        if (replayMode) {
            risk_engine::runReplayMode(replayFile);
        } else {
            risk_engine::runNormalMode(configPath);
        }
    } catch (const std::exception& e) {
        SPDLOG_CRITICAL("Excepción no manejada: {}", e.what());
        return 1;
    }
    
    SPDLOG_INFO("Risk Engine terminated gracefully");
    return 0;
}


// === ARCHIVO: CMakeLists.txt ===
cmake_minimum_required(VERSION 3.28)
project(RiskEngineLmax VERSION 1.0.0 LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

option(ENABLE_TESTING "Enable unit tests" ON)
option(ENABLE_COVERAGE "Enable code coverage" OFF)

if(CMAKE_BUILD_TYPE STREQUAL "")
    set(CMAKE_BUILD_TYPE "Release" CACHE STRING "Build type" FORCE)
endif()

set(CMAKE_CXX_FLAGS_RELEASE "-O3 -march=native -DNDEBUG")
set(CMAKE_CXX_FLAGS_DEBUG "-g -O0 -Wall -Wextra -Wpedantic")

include(FetchContent)
FetchContent_Declare(
    json
    GIT_REPOSITORY https://github.com/nlohmann/json.git
    GIT_TAG v3.11.2
)
FetchContent_Declare(
    spdlog
    GIT_REPOSITORY https://github.com/gabime/spdlog.git
    GIT_TAG v1.12.0
)
FetchContent_Declare(
    googletest
    GIT_REPOSITORY https://github.com/google/googletest.git
    GIT_TAG v1.14.0
)
FetchContent_Declare(
    catch2
    GIT_REPOSITORY https://github.com/catchorg/Catch2.git
    GIT_TAG v3.5.0
)
FetchContent_Declare(
    disruptor
    GIT_REPOSITORY https://github.com/LMAX-Exchange/disruptor-cpp.git
    GIT_TAG 4.0.0
)

set(json_BuildTests OFF CACHE INTERNAL "")
set(spdlog_BuildTests OFF CACHE INTERNAL "")
set(gtest_force_shared_crt ON CACHE INTERNAL "")
set(DISRUPTOR_BUILD_TESTS OFF CACHE INTERNAL "")
set(DISRUPTOR_BUILD_EXAMPLES OFF CACHE INTERNAL "")

FetchContent_MakeAvailable(json spdlog googletest catch2 disruptor)

include_directories(
    ${CMAKE_SOURCE_DIR}/src
    ${CMAKE_SOURCE_DIR}/src/domain
    ${CMAKE_SOURCE_DIR}/src/application
    ${CMAKE_SOURCE_DIR}/src/adapters
    ${CMAKE_SOURCE_DIR}/src/infrastructure
    ${CMAKE_SOURCE_DIR}/src/core
    ${CMAKE_SOURCE_DIR}/config
)

find_package(Threads REQUIRED)

set(CORE_SOURCES
    src/core/RiskEngine.cpp
)

set(DOMAIN_SOURCES
)

set(APPLICATION_SOURCES
)

set(ADAPTER_SOURCES
    src/adapters/MarketDataFeedAdapter.cpp
    src/adapters/OrderManagementAdapter.cpp
)

set(INFRASTRUCTURE_SOURCES
    src/infrastructure/DisruptorConfig.cpp
    src/infrastructure/ShardingStrategy.cpp
)

set(ALL_LIB_SOURCES
    ${CORE_SOURCES}
    ${DOMAIN_SOURCES}
    ${APPLICATION_SOURCES}
    ${ADAPTER_SOURCES}
    ${INFRASTRUCTURE_SOURCES}
)

add_executable(risk-engine
    src/main.cpp
    ${ALL_LIB_SOURCES}
)

target_link_libraries(risk-engine
    PRIVATE
    Threads::Threads
    spdlog::spdlog
    nlohmann_json::nlohmann_json
    LMAX::Disruptor
)

target_compile_options(risk-engine PRIVATE
    $<$<CXX_COMPILER_ID:GNU>:-Wno-unused-parameter>
    $<$<CXX_COMPILER_ID:Clang>:-Wno-unused-parameter>
)

if(ENABLE_TESTING)
    enable_testing()
    
    set(TEST_SOURCES
        test/RiskEngineTest.cpp
        test/CircuitBreakerTest.cpp
        test/ReplayEngineTest.cpp
    )
    
    add_executable(RiskEngineTest ${TEST_SOURCES} ${ALL_LIB_SOURCES})
    
    target_link_libraries(RiskEngineTest PRIVATE
        Threads::Threads
        spdlog::spdlog
        nlohmann_json::nlohmann_json
        LMAX::Disruptor
        GTest::gtest_main
        Catch2::Catch2WithMain
    )
    
    include(CTest)
    add_test(NAME RiskEngineTests COMMAND RiskEngineTest)
    
    add_executable(benchmark
        test/benchmark.cpp
        ${ALL_LIB_SOURCES}
    )
    
    target_link_libraries(benchmark PRIVATE
        Threads::Threads
        spdlog::spdlog
        nlohmann_json::nlohmann_json
        LMAX::Disruptor
    )
endif()

if(ENABLE_COVERAGE)
    include(CodeCoverage)
    append_coverage_compiler_flags()
    setup_target_for_coverage(
        NAME coverage
        EXECUTABLE RiskEngineTest
        DEPENDENCIES RiskEngineTest
    )
endif()

install(TARGETS risk-engine
    RUNTIME DESTINATION bin
)

configure_file(
    ${CMAKE_SOURCE_DIR}/config/risk_config.json
    ${CMAKE_BINARY_DIR}/config/risk_config.json
    COPYONLY
)

add_custom_command(TARGET risk-engine POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E copy_directory
    ${CMAKE_SOURCE_DIR}/config
    $<TARGET_FILE_DIR:risk-engine>/config
)

message(STATUS "Risk Engine Build Configuration")
message(STATUS "  C++ Standard: ${CMAKE_CXX_STANDARD}")
message(STATUS "  Build Type: ${CMAKE_BUILD_TYPE}")
message(STATUS "  Compiler: ${CMAKE_CXX_COMPILER_ID} ${CMAKE_CXX_COMPILER_VERSION}")
message(STATUS "  Testing: ${ENABLE_TESTING}")
message(STATUS "  Coverage: ${ENABLE_COVERAGE}")

// === ARCHIVO: config/risk_config.json ===
{
  "riskEngine": {
    "name": "RiskEngineLmax",
    "version": "1.0.0",
    "latency": {
      "targetP50Microseconds": 150,
      "targetP99Microseconds": 500,
      "targetP999Microseconds": 1000,
      "timeoutMs": 5000
    },
    "sharding": {
      "strategy": "byInstrument",
      "maxShards": 256,
      "rebalanceIntervalMs": 60000,
      "consistentHashing": true
    }
  },
  "varModel": {
    "modelType": "intraday",
    "confidenceLevel": 0.99,
    "horizonMinutes": 1,
    "calibration": {
      "method": "dynamic",
      "windowSize": 100,
      "decayFactor": 0.95,
      "minObservations": 50,
      "updateFrequencyMs": 1000
    },
    "scaling": {
      "enabled": true,
      "method": "exponential",
      "volatilityLookbackMs": 300000,
      "minVolatility": 0.0001,
      "maxVolatility": 0.5
    }
  },
  "limits": {
    "trader": {
      "maxExposureUsd": 10000000,
      "maxPositionSize": 1000000,
      "maxOrdersPerSecond": 100,
      "maxOrderValueUsd": 5000000,
      "dailyLossLimitUsd": 500000,
      "maxDrawdownUsd": 1000000
    },
    "strategy": {
      "maxExposureUsd": 5000000,
      "maxPositionSize": 500000,
      "maxOrdersPerSecond": 50,
      "maxOrderValueUsd": 2500000,
      "dailyLossLimitUsd": 250000,
      "maxDrawdownUsd": 500000
    },
    "instrument": {
      "maxExposureUsd": 2000000,
      "maxPositionSize": 200000,
      "maxOrdersPerSecond": 20,
      "maxOrderValueUsd": 1000000,
      "maxDailyVolumePct": 0.05,
      "maxMarketImpact": 0.01
    },
    "global": {
      "maxTotalExposureUsd": 100000000,
      "maxConcurrentOrders": 10000,
      "maxPositions": 5000,
      "maxStrategies": 200,
      "maxTraders": 50
    }
  },
  "circuitBreaker": {
    "enabled": true,
    "states": ["CLOSED", "OPEN", "HALF_OPEN"],
    "transition": {
      "closedToOpenThresholdMultiplier": 1.5,
      "closedToOpenTimeWindowMs": 60000,
      "openToHalfOpenTimeoutMs": 5000,
      "halfOpenToOpenThresholdMultiplier": 0.8,
      "halfOpenMaxAttempts": 3
    },
    "thresholds": {
      "exposureBreachPct": 0.95,
      "lossBreachPct": 0.8,
      "volatilityBreachMultiplier": 2.0,
      "latencyBreachMs": 10,
      "errorRateBreachPct": 0.1
    },
    "recovery": {
      "resetTimeoutMs": 5000,
      "gradualReentry": true,
      "maxOrderRateDuringRecovery": 10
    },
    "perInstrument": {
      "enabled": true,
      "maxConsecutiveRejections": 5,
      "cooldownMs": 30000
    }
  },
  "killSwitch": {
    "enabled": true,
    "detection": {
      "anomalyDetectionWindowMs": 60000,
      "thresholdDeviation": 3.0,
      "minSampleSize": 100,
      "metrics": ["latency", "orderRate", "errorRate", "positionDelta"]
    },
    "actions": {
      "cancelAllOrders": true,
      "closeAllPositions": true,
      "notifyRiskCommittee": true,
      "disableStrategy": true,
      "freezeTrader": false
    },
    "thresholds": {
      "latencyP99Multiplier": 5.0,
      "orderRateMultiplier": 3.0,
      "errorRatePct": 0.25,
      "positionDeltaMultiplier": 4.0,
      "lossRateUsdPerSecond": 100000
    },
    "escalation": {
      "autoEnableAfterMs": 300000,
      "manualOverrideRequired": true,
      "notificationChannels": ["email", "slack", "pagerduty"]
    }
  },
  "marketData": {
    "feed": {
      "provider": "multi",
      "sources": ["exchangeA", "exchangeB", "broker"],
      "fallbackEnabled": true,
      "maxLatencyMs": 100
    },
    "orderbook": {
      "depth": 10,
      "updateFrequencyHz": 100,
      "snapshotIntervalMs": 1000
    },
    "price": {
      "staleThresholdMs": 500,
      "useMidPrice": true,
      "volumeWeighted": false
    }
  },
  "compliance": {
    "mifid2": {
      "enabled": true,
      "auditLogPath": "logs/audit/",
      "retentionDays": 2555,
      "fields": [
        "timestamp",
        "traderId",
        "strategyId",
        "instrumentId",
        "orderId",
        "riskDecision",
        "exposure",
        "limit",
        "circuitBreakerState"
      ],
      "encryption": {
        "enabled": true,
        "algorithm": "AES-256-GCM"
      },
      "tamperProof": {
        "enabled": true,
        "hashAlgorithm": "SHA-256"
      }
    },
    "recordRetention": {
      "tradeLogsYears": 7,
      "auditLogsYears": 10,
      "configurationYears": 5
    }
  },
  "logging": {
    "level": "info",
    "format": "json",
    "outputs": [
      {
        "type": "file",
        "path": "logs/risk-engine.log",
        "rotation": "daily",
        "maxSizeMb": 500,
        "retentionDays": 30
      },
      {
        "type": "audit",
        "path": "logs/audit/risk-decisions.log",
        "rotation": "hourly",
        "maxSizeMb": 1000,
        "retentionDays": 2555
      },
      {
        "type": "console",
        "level": "warn"
      }
    ]
  },
  "throttling": {
    "enabled": true,
    "global": {
      "maxOrdersPerSecond": 10000,
      "maxNotionalPerSecond": 100000000,
      "burstAllowance": 1.2
    },
    "perTrader": {
      "maxOrdersPerSecond": 100,
      "maxNotionalPerSecond": 10000000
    },
    "perInstrument": {
      "maxOrdersPerSecond": 500,
      "maxNotionalPerSecond": 50000000
    }
  },
  "riskMetrics": {
    "calculationIntervalMs": 100,
    "aggregationWindowMs": 60000,
    "metrics": [
      "exposure",
      "var",
      "delta",
      "gamma",
      "vega",
      "theta"
    ],
    "realTime": {
      "enabled": true,
      "publishIntervalMs": 1000
    },
    "historical": {
      "enabled": true,
      "storageDays": 90,
      "samplingFrequencyMs": 100
    }
  }
}

// === ARCHIVO: test/RiskEngineTest.cpp ===
#include <catch2/catch_test_macros.hpp>
#include <catch2/catch_approx.hpp>
#include <chrono>
#include <vector>
#include <random>
#include "src/core/RiskEngine.h"
#include "src/domain/MarketData.h"
#include "src/domain/RiskLimits.h"

using namespace risk_engine;
using namespace std::chrono_literals;

TEST_CASE("RiskEngine evalua orden dentro de limites", "[risk-engine][unit]") {
    RiskEngine engine;
    engine.initialize();

    Order order;
    order.instrumentId = "AAPL";
    order.traderId = "TRADER_001";
    order.strategyId = "STRAT_Momentum";
    order.side = OrderSide::BUY;
    order.quantity = 100;
    order.price = 150.50;

    MarketData marketData;
    marketData.instrumentId = "AAPL";
    marketData.bid = 150.45;
    marketData.ask = 150.55;
    marketData.last = 150.50;
    marketData.timestamp = std::chrono::system_clock::now();

    auto decision = engine.evaluateOrder(order, marketData);

    REQUIRE(decision.approved == true);
    REQUIRE(decision.riskScore < 100.0);
}

TEST_CASE("RiskEngine rechaza orden que excede limite por instrumento", "[risk-engine][limits]") {
    RiskEngine engine;
    engine.initialize();

    RiskLimits limits;
    limits.maxPositionPerInstrument = 500;
    engine.updateLimits(limits);

    Order order;
    order.instrumentId = "TSLA";
    order.traderId = "TRADER_002";
    order.side = OrderSide::BUY;
    order.quantity = 1000;
    order.price = 200.00;

    MarketData marketData;
    marketData.instrumentId = "TSLA";
    marketData.bid = 199.90;
    marketData.ask = 200.10;
    marketData.last = 200.00;
    marketData.timestamp = std::chrono::system_clock::now();

    auto decision = engine.evaluateOrder(order, marketData);

    REQUIRE(decision.approved == false);
    REQUIRE(decision.rejectionReason == RejectionReason::INSTRUMENT_LIMIT_EXCEEDED);
}

TEST_CASE("RiskEngine rechaza orden que excede limite por trader", "[risk-engine][limits]") {
    RiskEngine engine;
    engine.initialize();

    RiskLimits limits;
    limits.maxExposurePerTrader = 100000.0;
    engine.updateLimits(limits);

    Order order;
    order.instrumentId = "GOOGL";
    order.traderId = "TRADER_003";
    order.side = OrderSide::BUY;
    order.quantity = 2000;
    order.price = 140.00;

    MarketData marketData;
    marketData.instrumentId = "GOOGL";
    marketData.bid = 139.80;
    marketData.ask = 140.20;
    marketData.last = 140.00;
    marketData.timestamp = std::chrono::system_clock::now();

    auto decision = engine.evaluateOrder(order, marketData);

    REQUIRE(decision.approved == false);
    REQUIRE(decision.rejectionReason == RejectionReason::TRADER_LIMIT_EXCEEDED);
}

TEST_CASE("RiskEngine cumple objetivo de latencia p99", "[risk-engine][latency]") {
    RiskEngine engine;
    engine.initialize();

    const int numOrders = 10000;
    std::vector<std::chrono::microseconds> latencies;
    latencies.reserve(numOrders);

    std::mt19937 rng(42);
    std::uniform_int_distribution<int> quantityDist(1, 500);
    std::uniform_real_distribution<double> priceDist(100.0, 200.0);

    std::vector<std::string> instruments = {"AAPL", "GOOGL", "MSFT", "AMZN", "TSLA"};
    std::uniform_int_distribution<int> instrumentDist(0, instruments.size() - 1);

    for (int i = 0; i < numOrders; ++i) {
        Order order;
        order.instrumentId = instruments[instrumentDist(rng)];
        order.traderId = "TRADER_" + std::to_string(i % 10);
        order.strategyId = "STRAT_Test";
        order.side = (i % 2 == 0) ? OrderSide::BUY : OrderSide::SELL;
        order.quantity = quantityDist(rng);
        order.price = priceDist(rng);

        MarketData marketData;
        marketData.instrumentId = order.instrumentId;
        marketData.bid = order.price - 0.05;
        marketData.ask = order.price + 0.05;
        marketData.last = order.price;
        marketData.timestamp = std::chrono::system_clock::now();

        auto start = std::chrono::high_resolution_clock::now();
        auto decision = engine.evaluateOrder(order, marketData);
        auto end = std::chrono::high_resolution_clock::now();

        latencies.push_back(std::chrono::duration_cast<std::chrono::microseconds>(end - start));
    }

    std::sort(latencies.begin(), latencies.end());
    auto p99Index = static_cast<size_t>(numOrders * 0.99);
    auto p99Latency = latencies[p99Index];

    REQUIRE(p99Latency.count() < 500);
}

TEST_CASE("RiskEngine mantiene consistencia entre evaluaciones concurrentes", "[risk-engine][concurrency]") {
    RiskEngine engine;
    engine.initialize();

    const int numThreads = 4;
    const int ordersPerThread = 1000;
    std::atomic<int> approvedCount{0};
    std::atomic<int> rejectedCount{0};

    std::vector<std::thread> threads;

    for (int t = 0; t < numThreads; ++t) {
        threads.emplace_back([&engine, t, ordersPerThread, &approvedCount, &rejectedCount]() {
            std::mt19937 rng(t * 1000);
            std::uniform_int_distribution<int> qtyDist(1, 100);

            for (int i = 0; i < ordersPerThread; ++i) {
                Order order;
                order.instrumentId = "AAPL";
                order.traderId = "TRADER_" + std::to_string(t);
                order.strategyId = "STRAT_Concurrent";
                order.side = OrderSide::BUY;
                order.quantity = qtyDist(rng);
                order.price = 150.00;

                MarketData marketData;
                marketData.instrumentId = "AAPL";
                marketData.bid = 149.95;
                marketData.ask = 150.05;
                marketData.last = 150.00;
                marketData.timestamp = std::chrono::system_clock::now();

                auto decision = engine.evaluateOrder(order, marketData);

                if (decision.approved) {
                    approvedCount++;
                } else {
                    rejectedCount++;
                }
            }
        });
    }

    for (auto& thread : threads) {
        thread.join();
    }

    int total = approvedCount + rejectedCount;
    REQUIRE(total == numThreads * ordersPerThread);
    REQUIRE(approvedCount > 0);
}

TEST_CASE("RiskEngine calcula VaR intraday correctamente", "[risk-engine][var]") {
    RiskEngine engine;
    engine.initialize();

    std::vector<MarketData> priceHistory;
    std::mt19937 rng(123);
    std::normal_distribution<double> returnsDist(0.0, 0.02);

    double lastPrice = 100.0;
    for (int i = 0; i < 252; ++i) {
        lastPrice *= (1.0 + returnsDist(rng));
        MarketData md;
        md.instrumentId = "SPY";
        md.last = lastPrice;
        md.timestamp = std::chrono::system_clock::now() - std::chrono::hours(252 - i);
        priceHistory.push_back(md);
    }

    engine.updatePriceHistory("SPY", priceHistory);

    auto varResult = engine.calculateVaR("SPY", 0.95, 1);

    REQUIRE(varResult.value > 0.0);
    REQUIRE(varResult.confidenceLevel == 0.95);
}

TEST_CASE("RiskEngine responde a cambios dinamicos de volatilidad", "[risk-engine][volatility]") {
    RiskEngine engine;
    engine.initialize();

    RiskLimits limits;
    limits.volatilityMultiplier = 1.5;
    engine.updateLimits(limits);

    std::vector<MarketData> lowVolHistory;
    double price = 100.0;
    for (int i = 0; i < 20; ++i) {
        price *= (1.0 + ((double)rand() / RAND_MAX - 0.5) * 0.01);
        MarketData md;
        md.instrumentId = "TEST";
        md.last = price;
        md.timestamp = std::chrono::system_clock::now();
        lowVolHistory.push_back(md);
    }
    engine.updatePriceHistory("TEST", lowVolHistory);

    Order lowVolOrder;
    lowVolOrder.instrumentId = "TEST";
    lowVolOrder.traderId = "TRADER_VOL";
    lowVolOrder.quantity = 100;
    lowVolOrder.price = price;
    lowVolOrder.side = OrderSide::BUY;

    MarketData lowVolMd;
    lowVolMd.instrumentId = "TEST";
    lowVolMd.last = price;
    lowVolMd.timestamp = std::chrono::system_clock::now();

    auto lowVolDecision = engine.evaluateOrder(lowVolOrder, lowVolMd);

    std::vector<MarketData> highVolHistory;
    price = 100.0;
    for (int i = 0; i < 20; ++i) {
        price *= (1.0 + ((double)rand() / RAND_MAX - 0.5) * 0.05);
        MarketData md;
        md.instrumentId = "TEST";
        md.last = price;
        md.timestamp = std::chrono::system_clock::now();
        highVolHistory.push_back(md);
    }
    engine.updatePriceHistory("TEST", highVolHistory);

    auto highVolDecision = engine.evaluateOrder(lowVolOrder, lowVolMd);

    REQUIRE(highVolDecision.riskScore >= lowVolDecision.riskScore);
}

// === ARCHIVO: test/CircuitBreakerTest.cpp ===
#include <catch2/catch_test_macros.hpp>
#include <catch2/catch_approx.hpp>
#include <thread>
#include <atomic>
#include "src/core/RiskEngine.h"
#include "src/application/KillSwitch.h"
#include "src/domain/RiskLimits.h"

using namespace risk_engine;
using namespace std::chrono_literals;

TEST_CASE("CircuitBreaker inicia en estado CLOSED", "[circuit-breaker][init]") {
    RiskEngine engine;
    engine.initialize();

    auto state = engine.getCircuitBreakerState("AAPL");
    REQUIRE(state == CircuitBreakerState::CLOSED);
}

TEST_CASE("CircuitBreaker abre cuando se excede umbral", "[circuit-breaker][threshold]") {
    RiskEngine engine;
    engine.initialize();

    RiskLimits limits;
    limits.exposureThresholdMultiplier = 1.0;
    engine.updateLimits(limits);

    for (int i = 0; i < 10; ++i) {
        Order order;
        order.instrumentId = "BTC";
        order.traderId = "TRADER_BTC";
        order.quantity = 1000;
        order.price = 50000.0;
        order.side = OrderSide::BUY;

        MarketData marketData;
        marketData.instrumentId = "BTC";
        marketData.last = 50000.0;
        marketData.timestamp = std::chrono::system_clock::now();

        engine.evaluateOrder(order, marketData);
    }

    auto state = engine.getCircuitBreakerState("BTC");
    REQUIRE(state == CircuitBreakerState::OPEN);
}

TEST_CASE("CircuitBreaker transiciona a HALF_OPEN despues de timeout", "[circuit-breaker][recovery]") {
    RiskEngine engine;
    engine.initialize();

    Order order;
    order.instrumentId = "ETH";
    order.traderId = "TRADER_ETH";
    order.quantity = 5000;
    order.price = 3000.0;
    order.side = OrderSide::BUY;

    MarketData marketData;
    marketData.instrumentId = "ETH";
    marketData.last = 3000.0;
    marketData.timestamp = std::chrono::system_clock::now();

    for (int i = 0; i < 15; ++i) {
        engine.evaluateOrder(order, marketData);
    }

    auto openState = engine.getCircuitBreakerState("ETH");
    REQUIRE(openState == CircuitBreakerState::OPEN);

    std::this_thread::sleep_for(5500ms);

    auto halfOpenState = engine.getCircuitBreakerState("ETH");
    REQUIRE(halfOpenState == CircuitBreakerState::HALF_OPEN);
}

TEST_CASE("CircuitBreaker permite ordenes de prueba en estado HALF_OPEN", "[circuit-breaker][probe]") {
    RiskEngine engine;
    engine.initialize();

    Order order;
    order.instrumentId = "SOL";
    order.traderId = "TRADER_SOL";
    order.quantity = 100;
    order.price = 100.0;
    order.side = OrderSide::BUY;

    MarketData marketData;
    marketData.instrumentId = "SOL";
    marketData.last = 100.0;
    marketData.timestamp = std::chrono::system_clock::now();

    for (int i = 0; i < 20; ++i) {
        engine.evaluateOrder(order, marketData);
    }

    std::this_thread::sleep_for(5500ms);

    auto decision = engine.evaluateOrder(order, marketData);
    auto state = engine.getCircuitBreakerState("SOL");

    REQUIRE((state == CircuitBreakerState::HALF_OPEN || state == CircuitBreakerState::CLOSED));
}

TEST_CASE("CircuitBreaker cierra despues de exitos en HALF_OPEN", "[circuit-breaker][recovery]") {
    RiskEngine engine;
    engine.initialize();

    RiskLimits limits;
    limits.circuitBreakerSuccessThreshold = 3;
    engine.updateLimits(limits);

    Order order;
    order.instrumentId = "DOGE";
    order.traderId = "TRADER_DOGE";
    order.quantity = 10;
    order.price = 0.10;
    order.side = OrderSide::BUY;

    MarketData marketData;
    marketData.instrumentId = "DOGE";
    marketData.last = 0.10;
    marketData.timestamp = std::chrono::system_clock::now();

    for (int i = 0; i < 15; ++i) {
        engine.evaluateOrder(order, marketData);
    }

    std::this_thread::sleep_for(5500ms);

    for (int i = 0; i < 5; ++i) {
        engine.evaluateOrder(order, marketData);
    }

    auto state = engine.getCircuitBreakerState("DOGE");
    REQUIRE(state == CircuitBreakerState::CLOSED);
}

TEST_CASE("KillSwitch activa ante comportamiento anionomo", "[kill-switch][anomaly]") {
    KillSwitch killSwitch;
    killSwitch.initialize();

    std::vector<Order> orders;
    for (int i = 0; i < 100; ++i) {
        Order order;
        order.traderId = "TRADER_ANOMALY";
        order.strategyId = "STRAT_Anomaly";
        order.quantity = 10000 + (i * 100);
        order.price = 100.0;
        order.side = OrderSide::BUY;
        orders.push_back(order);
    }

    auto triggerTime = killSwitch.evaluate(orders);

    REQUIRE(triggerTime.has_value());
}

TEST_CASE("KillSwitch no activa con comportamiento normal", "[kill-switch][normal]") {
    KillSwitch killSwitch;
    killSwitch.initialize();

    std::vector<Order> orders;
    std::mt19937 rng(42);
    std::uniform_int_distribution<int> qtyDist(90, 110);

    for (int i = 0; i < 100; ++i) {
        Order order;
        order.traderId = "TRADER_NORMAL";
        order.strategyId = "STRAT_Normal";
        order.quantity = qtyDist(rng);
        order.price = 100.0;
        order.side = OrderSide::BUY;
        orders.push_back(order);
    }

    auto triggerTime = killSwitch.evaluate(orders);

    REQUIRE(triggerTime.has_value() == false);
}

TEST_CASE("KillSwitch detecta desviacion estandar anomalamente alta", "[kill-switch][deviation]") {
    KillSwitch killSwitch;
    killSwitch.initialize();

    std::vector<Order> orders;
    std::mt19937 rng(99);
    std::normal_distribution<double> qtyDist(1000.0, 500.0);

    for (int i = 0; i < 50; ++i) {
        Order order;
        order.traderId = "TRADER_HIGH_DEV";
        order.strategyId = "STRAT_HighDev";
        order.quantity = static_cast<int>(std::abs(qtyDist(rng)));
        order.price = 100.0;
        order.side = OrderSide::BUY;
        orders.push_back(order);
    }

    auto triggerTime = killSwitch.evaluate(orders);

    REQUIRE(triggerTime.has_value());
}

TEST_CASE("CircuitBreaker con volatilidad extrema abre inmediatamente", "[circuit-breaker][volatility]") {
    RiskEngine engine;
    engine.initialize();

    std::vector<MarketData> extremeVolHistory;
    double price = 100.0;
    for (int i = 0; i < 10; ++i) {
        price *= (1.0 + ((double)rand() / RAND_MAX - 0.5) * 0.30);
        MarketData md;
        md.instrumentId = "VOL_TEST";
        md.last = price;
        md.timestamp = std::chrono::system_clock::now();
        extremeVolHistory.push_back(md);
    }
    engine.updatePriceHistory("VOL_TEST", extremeVolHistory);

    Order order;
    order.instrumentId = "VOL_TEST";
    order.traderId = "TRADER_VOL_EXTREME";
    order.quantity = 100;
    order.price = price;
    order.side = OrderSide::BUY;

    MarketData marketData;
    marketData.instrumentId = "VOL_TEST";
    marketData.last = price;
    marketData.timestamp = std::chrono::system_clock::now();

    auto decision = engine.evaluateOrder(order, marketData);

    REQUIRE(decision.approved == false);
}

TEST_CASE("CircuitBreaker multiple instruments independientes", "[circuit-breaker][multi]") {
    RiskEngine engine;
    engine.initialize();

    std::vector<std::string> instruments = {"AAA", "BBB", "CCC", "DDD", "EEE"};

    for (const auto& inst : instruments) {
        auto initialState = engine.getCircuitBreakerState(inst);
        REQUIRE(initialState == CircuitBreakerState::CLOSED);
    }

    Order order;
    order.traderId = "TRADER_MULTI";
    order.quantity = 10000;
    order.price = 100.0;
    order.side = OrderSide::BUY;

    MarketData marketData;
    marketData.last = 100.0;
    marketData.timestamp = std::chrono::system_clock::now();

    for (int i = 0; i < 20; ++i) {
        order.instrumentId = "AAA";
        marketData.instrumentId = "AAA";
        engine.evaluateOrder(order, marketData);
    }

    REQUIRE(engine.getCircuitBreakerState("AAA") == CircuitBreakerState::OPEN);
    REQUIRE(engine.getCircuitBreakerState("BBB") == CircuitBreakerState::CLOSED);
    REQUIRE(engine.getCircuitBreakerState("CCC") == CircuitBreakerState::CLOSED);
}

// === ARCHIVO: test/ReplayEngineTest.cpp ===
#include <catch2/catch_test_macros.hpp>
#include <fstream>
#include <filesystem>
#include "src/application/ReplayEngine.h"
#include "src/core/RiskEngine.h"

using namespace risk_engine;
using namespace std::chrono_literals;

TEST_CASE("ReplayEngine reproduce eventos en orden deterministico", "[replay][deterministic]") {
    ReplayEngine replayEngine;
    replayEngine.initialize();

    std::vector<RiskEvent> events;
    for (int i = 0; i < 100; ++i) {
        RiskEvent event;
        event.eventId = "EVT_" + std::to_string(i);
        event.timestamp = std::chrono::system_clock::now() + std::chrono::milliseconds(i);
        event.instrumentId = "AAPL";
        event.traderId = "TRADER_REPLAY";
        event.orderId = "ORD_" + std::to_string(i);
        event.riskDecision = (i % 2 == 0) ? RiskDecision::APPROVED : RiskDecision::REJECTED;
        event.exposure = 1000.0 * i;
        event.limit = 50000.0;
        events.push_back(event);
    }

    replayEngine.loadEvents(events);

    auto results = replayEngine.replay();

    REQUIRE(results.size() == 100);
    REQUIRE(results[0].eventId == "EVT_0");
    REQUIRE(results[99].eventId == "EVT_99");
}

TEST_CASE("ReplayEngine genera audit trail MiFID II", "[replay][mifid2]") {
    ReplayEngine replayEngine;
    replayEngine.initialize();

    std::vector<RiskEvent> events;
    for (int i = 0; i < 10; ++i) {
        RiskEvent event;
        event.eventId = "AUDIT_" + std::to_string(i);
        event.timestamp = std::chrono::system_clock::now();
        event.instrumentId = "MSFT";
        event.traderId = "TRADER_MIFID";
        event.strategyId = "STRAT_Compliance";
        event.orderId = "ORD_MIFID_" + std::to_string(i);
        event.riskDecision = RiskDecision::APPROVED;
        event.exposure = 5000.0;
        event.limit = 10000.0;
        event.circuitBreakerState = CircuitBreakerState::CLOSED;
        events.push_back(event);
    }

    auto auditPath = replayEngine.generateAuditTrail(events);

    REQUIRE(std::filesystem::exists(auditPath));

    std::ifstream auditFile(auditPath);
    std::string line;
    int lineCount = 0;
    while (std::getline(auditFile, line)) {
        lineCount++;
    }
    REQUIRE(lineCount >= 10);
}

TEST_CASE("ReplayEngine verifica consistencia de decisiones", "[replay][consistency]") {
    RiskEngine engine;
    engine.initialize();
    ReplayEngine replayEngine;
    replayEngine.initialize();

    std::vector<RiskEvent> events;
    for (int i = 0; i < 50; ++i) {
        Order order;
        order.instrumentId = "CONS";
        order.traderId = "TRADER_CONS";
        order.quantity = 100;
        order.price = 100.0;
        order.side = OrderSide::BUY;

        MarketData marketData;
        marketData.instrumentId = "CONS";
        marketData.last = 100.0;
        marketData.timestamp = std::chrono::system_clock::now();

        auto decision = engine.evaluateOrder(order, marketData);

        RiskEvent event;
        event.eventId = "CONS_EVT_" + std::to_string(i);
        event.timestamp = std::chrono::system_clock::now();
        event.instrumentId = order.instrumentId;
        event.traderId = order.traderId;
        event.orderId = "ORD_CONS_" + std::to_string(i);
        event.riskDecision = decision.approved ? RiskDecision::APPROVED : RiskDecision::REJECTED;
        event.exposure = decision.exposure;
        event.limit = decision.limit;
        events.push_back(event);
    }

    auto results = replayEngine.replay();

    for (size_t i = 0; i < results.size(); ++i) {
        REQUIRE(results[i].riskDecision == events[i].riskDecision);
    }
}

TEST_CASE("ReplayEngine reconstruye estado del sistema en punto temporal", "[replay][point-in-time]") {
    ReplayEngine replayEngine;
    replayEngine.initialize();

    std::vector<RiskEvent> events;
    auto baseTime = std::chrono::system_clock::now();

    for (int i = 0; i < 30; ++i) {
        RiskEvent event;
        event.eventId = "PIT_" + std::to_string(i);
        event.timestamp = baseTime + std::chrono::seconds(i * 10);
        event.instrumentId = "PIT_INST";
        event.traderId = "TRADER_PIT";
        event.orderId = "ORD_PIT_" + std::to_string(i);
        event.riskDecision = RiskDecision::APPROVED;
        event.exposure = 1000.0 * (i + 1);
        event.limit = 50000.0;
        events.push_back(event);
    }

    replayEngine.loadEvents(events);

    auto targetTime = baseTime + std::chrono::seconds(150);
    auto state = replayEngine.reconstructStateAt(targetTime);

    REQUIRE(state.exposure > 0.0);
    REQUIRE(state.eventsProcessed <= 30);
}

TEST_CASE("ReplayEngine detecta anomalias en reproduccion", "[replay][anomaly-detection]") {
    ReplayEngine replayEngine;
    replayEngine.initialize();

    std::vector<RiskEvent> events;
    for (int i = 0; i < 20; ++i) {
        RiskEvent event;
        event.eventId = "ANOMALY_" + std::to_string(i);
        event.timestamp = std::chrono::system_clock::now();
        event.instrumentId = "ANOMALY_INST";
        event.traderId = "TRADER_ANOMALY";
        event.orderId = "ORD_ANOMALY_" + std::to_string(i);
        event.riskDecision = RiskDecision::APPROVED;
        event.exposure = (i == 15) ? 1000000.0 : 1000.0;
        event.limit = 50000.0;
        events.push_back(event);
    }

    auto anomalies = replayEngine.detectAnomalies(events);

    REQUIRE(anomalies.size() > 0);
}

TEST_CASE("ReplayEngine soporta replay parcial por rango temporal", "[replay][partial]") {
    ReplayEngine replayEngine;
    replayEngine.initialize();

    std::vector<RiskEvent> events;
    auto baseTime = std::chrono::system_clock::now();

    for (int i = 0; i < 100; ++i) {
        RiskEvent event;
        event.eventId = "PARTIAL_" + std::to_string(i);
        event.timestamp = baseTime + std::chrono::seconds(i);
        event.instrumentId = "PARTIAL_INST";
        event.traderId = "TRADER_PARTIAL";
        event.orderId = "ORD_PARTIAL_" + std::to_string(i);
        event.riskDecision = RiskDecision::APPROVED;
        event.exposure = 1000.0;
        event.limit = 50000.0;
        events.push_back(event);
    }

    replayEngine.loadEvents(events);

    auto startTime = baseTime + std::chrono::seconds(30);
    auto endTime = baseTime + std::chrono::seconds(60);

    auto partialResults = replayEngine.replayPartial(startTime, endTime);

    REQUIRE(partialResults.size() > 0);
    REQUIRE(partialResults.size() <= 30);
}

TEST_CASE("ReplayEngine exporta a formato compliance", "[replay][compliance]") {
    ReplayEngine replayEngine;
    replayEngine.initialize();

    std::vector<RiskEvent> events;
    for (int i = 0; i < 5; ++i) {
        RiskEvent event;
        event.eventId = "COMP_" + std::to_string(i);
        event.timestamp = std::chrono::system_clock::now();
        event.instrumentId = "COMP_INST";
        event.traderId = "TRADER_COMP";
        event.strategyId = "STRAT_COMP";
        event.orderId = "ORD_COMP_" + std::to_string(i);
        event.riskDecision = RiskDecision::APPROVED;
        event.exposure = 5000.0;
        event.limit = 10000.0;
        event.circuitBreakerState = CircuitBreakerState::CLOSED;
        events.push_back(event);
    }

    auto exportPath = replayEngine.exportToComplianceFormat(events);

    REQUIRE(std::filesystem::exists(exportPath));

    std::ifstream file(exportPath);
    std::string content((std::istreambuf_iterator<char>(file)),
                        std::istreambuf_iterator<char>());

    REQUIRE(content.find("timestamp") != std::string::npos);
    REQUIRE(content.find("traderId") != std::string::npos);
    REQUIRE(content.find("riskDecision") != std::string::npos);
}

TEST_CASE("ReplayEngine verifica trazabilidad completa de decisiones", "[replay][traceability]") {
    ReplayEngine replayEngine;
    replayEngine.initialize();

    std::vector<RiskEvent> events;
    for (int i = 0; i < 15; ++i) {
        RiskEvent event;
        event.eventId = "TRACE_" + std::to_string(i);
        event.timestamp = std::chrono::system_clock::now();
        event.instrumentId = "TRACE_INST";
        event.traderId = "TRADER_TRACE";
        event.strategyId = "STRAT_TRACE";
        event.orderId = "ORD_TRACE_" + std::to_string(i);
        event.riskDecision = (i % 3 == 0) ? RiskDecision::REJECTED : RiskDecision::APPROVED;
        event.exposure = 1000.0 * i;
        event.limit = 10000.0;
        event.circuitBreakerState = CircuitBreakerState::CLOSED;
        event.marketDataSnapshot.bid = 100.0;
        event.marketDataSnapshot.ask = 100.10;
        event.marketDataSnapshot.last = 100.05;
        events.push_back(event);
    }

    auto trace = replayEngine.generateTraceabilityReport(events);

    REQUIRE(trace.totalEvents == 15);
    REQUIRE(trace.approvedCount > 0);
    REQUIRE(trace.rejectedCount > 0);
    REQUIRE(trace.eventsWithMarketData == 15);
}


// === ARCHIVO: docs/architecture.md ===
# Arquitectura del Motor de Riesgo - Risk Engine LMAX

## Visión General del Sistema

El motor de evaluación de riesgo opera como un sistema de procesamiento de eventos en tiempo real, diseñado para evaluar cada orden de trading en menos de 500 microsegundos p99. La arquitectura se fundamenta en un patrón event-driven con ring buffer lock-free, implementado mediante LMAX Disruptor 4.0 en C++20.

## Componentes Principales

### Capa de Dominio (`src/domain/`)
Contiene las entidades del negocio financiero: instrumentos, posiciones, límites de riesgo, y el modelo de VaR intraday. El dominio define los contratos que las capas externas implementan, respetando la regla de dependencia donde las interfaces viven en la capa interna.

### Capa de Aplicación (`src/application/`)
Orquestra el flujo de evaluación de riesgo: consume eventos del Disruptor, calcula exposición, valida límites, y determina si una orden debe ser aprobada, rechazada, o si debe dispararse un circuit breaker. Incluye el KillSwitch para detección de anomalías.

### Capa de Adaptadores (`src/adapters/`)
Conecta el motor de riesgo con sistemas externos: MarketDataFeedAdapter para consumo de nivel 2 orderbook y trades, OrderManagementAdapter para envío de decisiones de riesgo al OMS.

### Capa de Infraestructura (`src/infrastructure/`)
Provee la implementación técnica del patrón event-driven: configuración del Disruptor, estrategia de sharding por instrumento, y gestión de logs de auditoría.

## Decisiones Técnicas Fundamentales

### Lock-Free vs Mutex

El sistema utiliza estructuras lock-free basadas en `std::atomic` de C++20 para garantizar latencia determinística. La justificación técnica es la siguiente:

1. **Determinismo temporal**: Los mutex introducen prioridad de acceso y riesgo de inversión de prioridad, causando latencia no determinística. Las operaciones lock-free (compare-and-swap, fetch-add) tienen tiempo constante.

2. **Sin bloqueo de hilos**: En un escenario de alta concurrencia con múltiples productores ( feeds de mercado) y consumidores (evaluadores de riesgo), los mutex pueden causar bloqueo mutuo o degradación de rendimiento bajo carga.

3. **Escalabilidad vertical**: Las estructuras atómicas escalan mejor en sistemas NUMA porque evitan la sincronización entre cores mediante busy-waiting local.

4. **Costo de contención**: En benchmarks con 10K mensajes/segundo, los mutex muestran latencia p99 3-5x mayor que soluciones lock-free equivalentes.

La implementación usa `std::atomic<bool>` para flags de circuit breaker, `std::atomic<uint64_t>` para contadores de exposición, y `std::atomic<double>` para valores de VaR.

### C++ vs Rust vs Java LMAX Disruptor

La elección de C++20 con LMAX Disruptor se fundamenta en:

1. **Rendimiento nativo**: C++ compila a código máquina nativo sin overhead de runtime. Java introduce GC pausas y overhead de JIT. Rust ofrece rendimiento similar pero con curva de aprendizaje más pronunciada y ecosistema financiero menos maduro.

2. **Integración con LMAX Disruptor**: La implementación en C++ de Disruptor 4.0 es la referencia original, optimizada para baja latencia. Las bindings de otras lenguajes añaden overhead de FFI.

3. **Ecosistema financiero**: C++ es el estándar de facto en sistemas de trading de baja latencia. Existe abundante conocimiento operativo y tooling de profiling (VTune, perf, flame graphs).

4. **C++20 features**: Concepts, coroutines, y ranges permiten código más expresivo sin sacrificar rendimiento. `std::atomic` con wait-free algorithms está bien especificado.

### Sharding por Instrumento vs Consensus

El sistema implementa sharding por instrumento en lugar de consensus distribuido por las siguientes razones:

1. **Consistencia natural**: Cada instrumento es un dominio de consistencia independiente. No existe necesidad de consensus entre shards porque las órdenes de un instrumento no afectan la exposición de otro.

2. **Escalabilidad lineal**: Agregar nuevos instrumentos no requiere rebalanceo de shards. Cada instrumento puede procesarse en paralelo sin coordinación central.

3. **Latencia reducida**: Eliminar el paso de consensus (Paxos, Raft) elimina rondas de comunicación entre nodos. Una evaluación de riesgo es una operación local al shard.

4. **Simplicidad operativa**: No requiere Zookeeper, etcd, o cualquier sistema de coordinación externo. El estado del motor de riesgo es determinístico dado el secuencia de eventos.

5. **Trade-off**: El trade-off es que un instrumento con volumen extremo puede convertirse en cuello de botella. La solución es particionar por instrumento + símbolo, no por instrumento solo.

## Ring Buffer y Patrón Disruptor

El LMAX Disruptor implementa el patrón ring buffer con las siguientes características:

1. **Memoria pre-allocada**: El ring buffer allocate toda la memoria al inicio, eliminando allocation dinámica durante procesamiento.

2. **Single writer principle**: Cada evento es escrito por un único hilo (feed de mercado), eliminando sincronización de escritura.

3. **Múltiples lectores**: Múltiples consumidores pueden leer el buffer sin coordinación, usando sequences para tracking de progreso.

4. **Cache-line padding**: Los sequence cursors usan padding para evitar false sharing entre cores.

## Modelo de Ejecución

El modelo de ejecución es event-driven con procesamiento en serie por instrumento:

- El feed de mercado escribe eventos al ring buffer
- Un único consumidor procesa eventos en orden para cada instrumento
- La evaluación de riesgo es同步 y produce una decisión inmediata
- Las decisiones se escriben al OrderManagementAdapter

Este modelo garantiza que el orden causal se preserva: una orden entrante ve el estado más reciente del instrumento antes de ser evaluada.

## Diagrama de Arquitectura

```
┌─────────────────┐     ┌──────────────────┐     ┌─────────────────┐
│  Market Data    │────▶│   Disruptor      │────▶│  Risk Engine    │
│  Feed Adapter   │     │   Ring Buffer    │     │  (per shard)    │
└─────────────────┘     └──────────────────┘     └────────┬────────┘
                                                          │
                                                          ▼
                                                 ┌─────────────────┐
                                                 │  Risk Decision  │
                                                 │  + Audit Log    │
                                                 └────────┬────────┘
                                                          │
                                                          ▼
                                                 ┌─────────────────┐
                                                 │  OMS Adapter    │
                                                 └─────────────────┘
```

## Referencias

- LMAX Disruptor: https://lmax-exchange.github.io/disruptor/
- C++20 Atomics: ISO/IEC 14882:2020 Section 32
- MiFID II: Directive 2014/65/EU
- Low Latency Trading Systems: Chapman & Hall/CRC Financial Mathematics

// === ARCHIVO: docs/compliance.md ===
# Compliance MiFID II - Trazabilidad del Motor de Riesgo

## Marco Regulatorio

MiFID II (Markets in Financial Instruments Directive II) establece requisitos estrictos de transparencia y trazabilidad para sistemas de trading algorítmico. El Artículo 17 requiere que las empresas mantengan registros de todas las decisiones algorítmicas incluyendo parámetros de riesgo, umbrales, y resultados de la evaluación.

El motor de riesgo implementa un sistema de logging inmutable que satisface estos requisitos regulatorios.

## Campos de Auditoría

El sistema registra los siguientes campos para cada decisión de riesgo:

| Campo | Tipo | Descripción |
|-------|------|-------------|
| timestamp | ISO 8601 UTC | Timestamp de alta precisión (microsegundos) |
| traderId | string | Identificador único del trader |
| strategyId | string | Identificador de la estrategia algorítmica |
| instrumentId | string | ISIN del instrumento |
| orderId | string | Identificador de la orden |
| riskDecision | enum | APPROVED, REJECTED, CIRCUIT_BREAKER_TRIGGERED |
| exposure | double | Exposición actual del instrumento/estrategia |
| limit | double | Límite aplicable |
| circuitBreakerState | enum | CLOSED, OPEN, HALF_OPEN |

## Inmutabilidad de Logs

Los logs de auditoría se escriben en un directorio dedicado (`logs/audit/`) con las siguientes garantías:

1. **Append-only**: Los archivos de log se abren exclusivamente en modo append. No existen operaciones de overwrite o delete durante operación normal.

2. **Rotación diaria**: Cada día calendario genera un nuevo archivo con formato `audit_YYYYMMDD.log`. Los archivos históricos se retienen por 2555 días (7 años).

3. **Checksum**: Cada entrada de log incluye un hash SHA-256 de su contenido más el hash de la entrada anterior, creando una cadena de integridad.

4. **Timestamp atómico**: El timestamp se genera mediante `std::chrono::system_clock::now()` con resolución de microsegundos, sincronizado con NTP.

## Trazabilidad de Decisiones

### Flujo de Auditoría

Cada decisión de riesgo sigue este flujo de auditoría:

1. **Pre-evaluación**: Se registra el evento de entrada con timestamp, instrumento, y parámetros de la orden.

2. **Cálculo de exposición**: Se calcula la exposición agregada y se registra el valor antes de la evaluación.

3. **Evaluación de límites**: Se comparan los límites configurados con la exposición actual.

4. **Decisión**: Se registra la decisión final con el razonamiento (límite excedido, circuit breaker abierto, etc.).

5. **Post-evaluación**: Se registra el estado del circuit breaker después de la decisión.

### Ejemplo de Entrada de Log

```
2024-01-15T09:30:45.123456Z|TRADER_001|STRAT_ALPHA|ES0123456789|ORD_12345|REJECTED|1250000.00|1000000.00|HALF_OPEN|a3f5b8c2d1e4...
```

## Retención y Almacenamiento

### Política de Retención

- **Período activo**: 90 días en almacenamiento primario con acceso rápido
- **Período histórico**: 2555 días (7 años) en almacenamiento frío
- **Formato**: Texto plano con delimitadores para compatibilidad con herramientas SIEM

### Integridad Forense

Para satisfy requisitos de investigación regulatoria:

1. **No repudio**: Cada entrada incluye el identificador del proceso y host que la generó.

2. **Cadena de custodia**: Los hashes encadenados permiten detectar cualquier modificación histórica.

3. **Exportación**: El sistema provee un comando de exportación para generar archivos con hash总和 para presentación a reguladores.

## Circuit Breakers y Compliance

Los circuit breakers implementados como máquinas de estado finitas tienen las siguientes implicaciones de compliance:

### Estados del Circuit Breaker

- **CLOSED**: Operación normal. Las órdenes se evalúan normalmente.
- **OPEN**: Circuit breaker disparado. Todas las órdenes para el instrumento se rechazan automáticamente.
- **HALF_OPEN**: Modo de prueba. Se permite un volumen limitado para verificar estabilización.

### Trazabilidad de Transiciones

Cada transición de estado del circuit breaker se registra con:

- Timestamp exacto de la transición
- Causa (límite excedido, anomalía detectada, timeout)
- Valores que dispararon la transición
- Duración en estado anterior

## Replay y Post-Mortem

El sistema incluye un motor de replay determinístico que permite reconstruir el estado del sistema en cualquier punto del pasado:

1. **Grabación de eventos**: Todos los eventos de entrada se graban en formato binario con timestamps.

2. **Reproducción**: El replay engine consume los eventos grabados y reproduce el estado del motor de riesgo.

3. **Análisis**: Permite identificar qué decisiones de riesgo se tomaron, por qué, y cuál habría sido el resultado con parámetros diferentes.

## Configuración de Compliance

La configuración de compliance se define en `config/risk_config.json`:

```json
{
  "compliance": {
    "mifid2": {
      "enabled": true,
      "auditLogPath": "logs/audit/",
      "retentionDays": 2555,
      "fields": [
        "timestamp",
        "traderId",
        "strategyId",
        "instrumentId",
        "orderId",
        "riskDecision",
        "exposure",
        "limit",
        "circuitBreakerState"
      ]
    }
  }
}
```

## Verificación de Compliance

Para verificar el cumplimiento de MiFID II:

1. **Completitud**: Verificar que todas las decisiones tienen todos los campos obligatorios.

2. **Integridad**: Validar la cadena de hashes SHA-256.

3. **Temporalidad**: Verificar que los timestamps están sincronizados y tienen resolución suficiente.

4. **Retención**: Confirmar que los archivos históricos están disponibles y no corrompidos.

## Referencias

- MiFID II Directive 2014/65/EU: https://eur-lex.europa.eu/legal-content/EN/TXT/?uri=CELEX%3A32014L0065
- ESMA Guidelines on Algorithmic Trading: https://www.esma.europa.eu/policy-rules/mifid-ii-and-mifir
- ACER Market Abuse Regulation: https://www.acer-marbef.eu/

```

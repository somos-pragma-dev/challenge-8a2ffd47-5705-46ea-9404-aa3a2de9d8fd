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
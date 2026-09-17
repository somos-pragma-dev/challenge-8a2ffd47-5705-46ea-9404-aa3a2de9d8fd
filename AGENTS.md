# AGENTS.md

Instrucciones para el agente de IA que abra este repositorio (Claude Code, Cursor, Codex, Copilot, Gemini). Se cargan solas: no hay que pegar nada en ningun chat.

## Que es este repositorio

Es el codigo base de un reto de aprendizaje de Pragma: **Evaluación de riesgo en tiempo real con circuit breakers**.

| | |
|---|---|
| Tema | motor de risk scoring en tiempo real para trading algorítmico con circuit breakers dinámicos |
| Nivel | master-l2 |
| Chapter | Generico |
| Especialidad | Inferido del contexto |
| Stack | C++20 / LMAX Disruptor 4.0 |
| Patron arquitectonico | event-driven con ring buffer lock-free y sharding por instrumento |
| Tiempo estimado | 4 semanas |

## Tu tarea

Dejar este proyecto en estado **verificable**: que el comando de verificacion corra sin errores. Escribi los archivos en disco, en este repositorio. No generes ZIPs ni archivos adjuntos.

En orden:

1. Corre `el comando de build o arranque canonico del stack elegido` y mira que falla.
2. Completa lo que falte de la lista de abajo: manifiesto de dependencias, punto de entrada, capa de interfaz y las capas del patron declarado.
3. Arregla SOLO los errores que impiden compilar o arrancar.
4. Volve a correr `el comando de build o arranque canonico del stack elegido` hasta que pase.
5. Pará ahí.

## Regla dura: las fases son trabajo del humano

**PROHIBIDO implementar los entregables de las fases.** El valor del reto esta en que la persona los resuelva. Tu trabajo es que tenga un proyecto que arranca; el hueco pedagogico se queda como esta.

No resuelvas nada de esto:

- **Fase 1 — Exploración y modelado inicial**: Modelo conceptual del motor de riesgo y sus componentes.
- **Fase 2 — Implementación de la evaluación de riesgo**: Motor de evaluación de riesgo funcional con latencia < 500 microsegundos p99.
- **Fase 3 — Circuit breakers y kill switch**: Motor de riesgo con circuit breakers dinámicos y kill switch funcional.

Distincion operativa:

- **Arreglar** (si): import faltante, tipo que no existe, dependencia sin declarar, error de sintaxis, archivo referenciado que no existe.
- **No tocar** (no): logica de negocio incompleta, validaciones ausentes, secretos hardcodeados, APIs deprecadas que funcionan, concurrencia insegura, patrones mejorables. Eso es lo que la persona tiene que encontrar.

## Lo que falta y tenes que completar

### 1. Boilerplate del stack (2)

Sin esto el proyecto no compila ni arranca. **Es tu trabajo crearlo**, y no toca nada de lo pedagogico: es andamiaje del stack.

- [ ] **Punto de entrada del stack elegido** — Sin un punto de entrada reconocible, el runtime no tiene por donde arrancar la aplicacion.
- [ ] **Capa de interfaz (controller/handler)** — Sin una capa de interfaz explicita, no hay forma de invocar la logica de negocio desde afuera del proceso.

### Presentes (20)

- `package.json`
- `src/domain/MarketData.h`
- `src/core/RiskEngine.h`
- `src/core/RiskEngine.cpp`
- `src/domain/RiskLimits.h`
- `src/domain/VaRModel.h`
- `src/adapters/MarketDataFeedAdapter.cpp`
- `src/adapters/OrderManagementAdapter.cpp`
- `src/infrastructure/DisruptorConfig.cpp`
- `src/infrastructure/ShardingStrategy.cpp`
- `src/application/KillSwitch.cpp`
- `src/application/ReplayEngine.cpp`
- `src/main.cpp`
- `CMakeLists.txt`
- `config/risk_config.json`
- `test/RiskEngineTest.cpp`
- `test/CircuitBreakerTest.cpp`
- `test/ReplayEngineTest.cpp`
- `docs/architecture.md`
- `docs/compliance.md`

### Capas del patron declarado

Cada una tiene que existir como directorio real con al menos un archivo. Codigo plano en la raiz no satisface el patron.

- `src/core`
- `src/adapters`
- `src/infrastructure`
- `src/domain`
- `src/application`
- `test`
- `config`
- `docs`

## Verificacion

```bash
el comando de build o arranque canonico del stack elegido
```

Ese comando pasando es la definicion de "terminado" para vos.

## Convenciones que tenes que respetar

- Un solo ecosistema: no declares librerias de otro lenguaje ni mezcles gestores de paquetes.
- Toda libreria que uses tiene que estar declarada en el manifiesto de dependencias.
- Todo import declarado tiene que usarse; todo tipo usado tiene que existir o venir de una dependencia declarada.
- El patron es **event-driven con ring buffer lock-free y sharding por instrumento**: los contratos (interfaces, puertos) los define la capa interna y los implementa la externa, nunca al revés.
- Los archivos que crees llevan implementacion real, no stubs: sin `TODO`, sin cuerpos vacios, sin `// getters y setters`.

## Contexto del candidato

Sirve para calibrar el nivel del codigo, no para resolver las fases.

- Brecha que el reto ataca: Sistema master-l2 que evalúa el riesgo de cada orden antes de enviarla al exchange, en menos de 500 microsegundos p99. Consume feed de market data (nivel 2 orderbook + trades), mantiene un modelo de VaR intraday, aplica límites por trader/estrategia/instrumento en tiempo real, y dispara circuit breakers dinámicos que bloquean nuevas órdenes cuando la exposición supera thresholds calibrados por volatilidad. Debe justificar el uso de estructuras lock-free vs mutex, la elección entre C++ vs Rust vs Java LMAX Disruptor, cómo garantiza la consistencia entre múltiples risk engines corriendo en paralelo (consensus vs sharding por instrument), la política de kill switch cuando detecta un algoritmo que se comporta de forma anómala, y la estrategia de replay determinístico para post-mortem de incidents. Incluye compliance con regulaciones MiFID II para trazabilidad de decisiones de riesgo.

---

*Generado por Challenge Generator — Pragma. `README.md` tiene el enunciado completo del reto para la persona. `PROMPT_MEJORA.md` es la variante para pegar en un chat, si se prefiere ese flujo.*

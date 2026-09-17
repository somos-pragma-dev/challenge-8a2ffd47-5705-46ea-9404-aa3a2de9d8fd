# Evaluación de riesgo en tiempo real con circuit breakers

Diseña un motor de evaluación de riesgo para órdenes de trading que opere en menos de 500 microsegundos p99. El sistema consume un feed de datos de mercado (nivel 2 orderbook + trades), mantiene un modelo de VaR intraday, aplica límites por trader/estrategia/instrumento en tiempo real, y dispara circuit breakers dinámicos cuando la exposición supera umbrales calibrados por volatilidad. Debes justificar el uso de estructuras lock-free vs mutex, la elección entre C++ vs Rust vs Java LMAX Disruptor, cómo garantizar la consistencia entre múltiples motores de riesgo corriendo en paralelo (consensus vs sharding por instrumento), la política de kill switch cuando detecta un algoritmo que se comporta de forma anómala, y la estrategia de replay determinístico para post-mortem de incidentes. Incluye compliance con regulaciones MiFID II para trazabilidad de decisiones de riesgo.

## Informacion General

| Campo | Valor |
|-------|-------|
| **Tema** | motor de risk scoring en tiempo real para trading algorítmico con circuit breakers dinámicos |
| **Nivel** | master-l2 |
| **Tipo** | mixed |
| **Tiempo estimado** | 4 semanas |

## Fases del Reto

### Fase 0: Configuración del Proyecto

**Objetivo:** Obtener el proyecto base funcional enviando el Código Base a un asistente de IA, que lo analizará, corregirá errores y generará un ZIP listo para usar.

**Tiempo estimado:** 15-30 minutos

**Instrucciones:**

- Asegúrate de tener instalado para ejecutar el proyecto: Node.js 18+, npm, VS Code o similar.
- Copia todo el contenido del campo **Código Base** de este reto — incluyendo el texto de instrucciones que aparece al inicio.
- Abre un asistente de IA (Claude en claude.ai, ChatGPT o Gemini — se recomienda Claude), pega el contenido copiado en el chat y envíalo.
- El asistente analizará los archivos, corregirá errores y generará un archivo ZIP descargable. Descárgalo y extráelo en la carpeta donde quieras trabajar.
- Ejecuta `npm install && npm run build` (o `npm start`). Si no hay errores, estás listo.

**Entregable:** El proyecto compila/arranca sin errores.

<details>
<summary>Pistas de conocimiento</summary>

- Copia el Código Base completo incluyendo el texto de instrucciones al inicio — esas instrucciones le indican al asistente exactamente qué hacer con los archivos.
- Si el asistente no genera el ZIP automáticamente al terminar el análisis, escríbele: "genera el ZIP ahora".
- Si el proyecto tiene errores al arrancar, comparte el mensaje de error con el mismo asistente para que lo corrija.

</details>

### Fase 1: Exploración y modelado inicial

**Objetivo:** Entender y modelar el dominio del trading algorítmico y sus requerimientos de riesgo en tiempo real.

**Tiempo estimado:** 1 semana

**Instrucciones:**

- Identifica las fuentes de datos de mercado y sus características.
- Modela los límites de riesgo por trader, estrategia e instrumento.
- Define los umbrales de volatilidad para los circuit breakers.

**Entregable:** Modelo conceptual del motor de riesgo y sus componentes.

<details>
<summary>Pistas de conocimiento</summary>

- Considera la latencia y la consistencia como propiedades críticas.
- Evalúa la complejidad de los algoritmos de evaluación de riesgo.

</details>

### Fase 2: Implementación de la evaluación de riesgo

**Objetivo:** Implementar el motor de evaluación de riesgo con los requerimientos de latencia y consistencia.

**Tiempo estimado:** 2 semanas

**Instrucciones:**

- Elige entre C++ vs Rust vs Java LMAX Disruptor para la implementación.
- Implementa la lógica de evaluación de riesgo con los límites definidos.
- Garantiza la consistencia entre múltiples motores de riesgo corriendo en paralelo.

**Entregable:** Motor de evaluación de riesgo funcional con latencia < 500 microsegundos p99.

<details>
<summary>Pistas de conocimiento</summary>

- Evalúa el uso de estructuras lock-free vs mutex para la concurrencia.
- Considera sharding por instrumento para mejorar la escalabilidad.

</details>

### Fase 3: Circuit breakers y kill switch

**Objetivo:** Implementar circuit breakers dinámicos y la política de kill switch para algoritmos anómalos.

**Tiempo estimado:** 1 semana

**Instrucciones:**

- Diseña los circuit breakers dinámicos que bloquean nuevas órdenes cuando la exposición supera los umbrales.
- Implementa la política de kill switch cuando detecta un algoritmo que se comporta de forma anómala.
- Define la estrategia de replay determinístico para post-mortem de incidentes.

**Entregable:** Motor de riesgo con circuit breakers dinámicos y kill switch funcional.

<details>
<summary>Pistas de conocimiento</summary>

- Evalúa la efectividad de los circuit breakers en escenarios de alta volatilidad.
- Considera la trazabilidad de decisiones de riesgo para compliance con MiFID II.

</details>

## Dimensiones Evaluadas

- **queEs**: ¿Qué es un motor de evaluación de riesgo en tiempo real y cuáles son sus componentes?
- **paraQueSirve**: ¿Para qué sirve la evaluación de riesgo en el trading algorítmico y cuáles son sus beneficios?
- **comoSeUsa**: ¿Cómo se usa un motor de riesgo para garantizar la consistencia y la latencia en escenarios de alta concurrencia?
- **erroresComunes**: ¿Cuáles son los errores comunes en la implementación de motores de riesgo y cómo se pueden evitar?
- **queDecisionesImplica**: ¿Qué decisiones implica la elección de tecnología y la implementación de circuit breakers y kill switch?

## Criterios de Evaluacion

- Modelo conceptual del motor de riesgo y sus componentes.
- Motor de evaluación de riesgo funcional con latencia < 500 microsegundos p99.
- Motor de riesgo con circuit breakers dinámicos y kill switch funcional.
- Estrategia de replay determinístico para post-mortem de incidentes.
- Compliance con regulaciones MiFID II para trazabilidad de decisiones de riesgo.

## Como trabajar con un asistente de IA

Hay dos caminos, elegi uno:

- **AGENTS.md** (recomendado) — instrucciones nativas del repo. Abri esta carpeta con tu agente local (Claude Code, Cursor, Codex, Copilot, Gemini) y las carga solo. Sabe que archivos faltan y con que comando se verifica, y completa el scaffold escribiendo en disco.
- **PROMPT_MEJORA.md** — para copiar y pegar en un chat (claude.ai, ChatGPT). Devuelve un ZIP con el proyecto. Sirve si no tenes un agente en el IDE.

Ninguno de los dos resuelve las fases del reto: eso es tu trabajo.

## Verificacion

El proyecto esta listo para trabajar cuando este comando corre sin errores:

```bash
el comando de build o arranque canonico del stack elegido
```

---

*Reto generado automaticamente por Challenge Generator - Pragma*

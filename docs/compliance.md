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
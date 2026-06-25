# Reflex

**Reflex** es un juego de reflejos para dos jugadores desarrollado con Arduino.

El sistema inicia mediante un botón central, muestra una secuencia de preparación con LEDs y buzzer, puede generar señales falsas y luego activa una señal real para detectar cuál de los dos jugadores presiona primero su pulsador.

## Objetivo general

Desarrollar un prototipo funcional con Arduino que permita medir la reacción de dos jugadores ante una señal visual y sonora, incorporando señales falsas para evitar respuestas anticipadas.

## Funcionalidades principales

* Inicio de partida mediante botón central.
* Secuencia visual y sonora de preparación.
* Señales falsas para dificultar la reacción anticipada.
* Señal real mediante LED y buzzer.
* Detección del jugador que presiona primero.
* Detección de fallo si un jugador se adelanta.
* Indicación del resultado mediante LEDs, buzzer y monitor serial.

## Componentes principales

* Arduino Uno.
* Protoboard.
* 3 pulsadores.
* LED rojo.
* LED amarillo.
* LED verde.
* Buzzer.
* Resistencias.
* Cables de conexión.
* Cable USB A/B.

## Documentación

* Descripción del proyecto: `docs/proyecto.md`
* Armado del prototipo: `docs/componentes.md`
* Funcionamiento y lógica: `docs/logica.md`
* Diagrama de conexión: `docs/img/diagrama-conexion-v2.0.png`
* Diagrama de flujo: `docs/img/diagrama-flujo.png`

## Enlaces

* Simulación Wokwi: `https://wokwi.com/projects/467573832377455617`
* Código fuente: `src/reflex.ino`
* Documentación: carpeta `docs/`

## Estado del proyecto

Versión estable del prototipo con lógica de duelo de reflejos, señales falsas, detección de ganador y detección de fallo por anticipación.
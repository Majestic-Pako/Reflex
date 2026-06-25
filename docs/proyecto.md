# - Descripción del proyecto

## Identificación del problema

En actividades donde intervienen reflejos y tiempo de reacción, muchas veces se necesita una forma simple de detectar quién responde primero ante una señal.

Este proyecto busca resolver esa necesidad mediante un sistema interactivo para dos jugadores, donde ambos deben presionar su pulsador lo más rápido posible cuando el sistema indique el momento correcto.

Además, para hacerlo más desafiante, el sistema puede emitir señales falsas antes de la señal real. Si un jugador se adelanta y presiona antes del momento correcto, pierde la ronda.

## Objetivo del proyecto

El objetivo del proyecto **Reflex** es desarrollar un prototipo con Arduino que funcione como un juego de reflejos para dos jugadores.

El sistema permite:

* Iniciar la partida con un botón central.
* Mostrar una secuencia de preparación mediante LEDs y sonido.
* Generar señales falsas para dificultar la reacción anticipada.
* Activar una señal real visual y sonora para indicar el momento correcto de respuesta.
* Detectar cuál jugador presiona primero.
* Detectar si un jugador se adelanta antes de la señal correcta.
* Mostrar el resultado final mediante LEDs, buzzer y mensajes en el monitor serial.

## Justificación de la solución

Arduino es una tecnología adecuada para este proyecto porque permite controlar entradas y salidas digitales en tiempo real.

Los pulsadores funcionan como entradas del sistema, mientras que los LEDs y el buzzer funcionan como salidas. Esto permite aplicar conceptos básicos de sistemas embebidos, lectura de entradas digitales, control de actuadores, temporización y toma de decisiones mediante lógica de programación.

La implementación con señales falsas permite que el prototipo no sea solamente una reacción directa ante un LED, sino que incorpore una condición extra de validación: diferenciar entre una señal falsa y una señal real.

Además, el proyecto es simple de montar, fácil de simular en Wokwi y permite demostrar de manera clara el funcionamiento de un circuito interactivo.

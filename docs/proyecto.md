# Descripción del proyecto

## Problema

Los juegos de memoria y reflejos suelen limitarse a mostrar estímulos y medir respuestas, sin registrar los resultados ni permitir el seguimiento del rendimiento. **Reflex** aborda esta limitación con una experiencia individual que combina memoria, velocidad de respuesta y almacenamiento de datos.

En cada partida, el jugador memoriza una secuencia numérica mostrada en una pantalla LCD y la repite con un control remoto infrarrojo antes de que termine el tiempo. La dificultad aumenta en cada ronda superada. La partida finaliza ante una respuesta incorrecta o cuando se agota el tiempo.

## Objetivo

El objetivo de **Reflex** es desarrollar un sistema interactivo basado en ESP32 que permita ejercitar y evaluar la memoria y los reflejos mediante secuencias numéricas de dificultad progresiva.

El sistema debe permitir:

- Ingresar el nombre del jugador con un control remoto infrarrojo.
- Mostrar instrucciones, secuencias, tiempo y resultados en una pantalla LCD 16x2 con I2C.
- Generar secuencias numéricas aleatorias y validar la respuesta dentro de un tiempo limitado.
- Aumentar la longitud de la secuencia después de cada ronda superada.
- Brindar retroalimentación mediante un LED verde, un LED rojo y un buzzer.
- Calcular el puntaje según las rondas superadas.
- Enviar el resultado por WiFi y mostrarlo en una aplicación web.

## Solución propuesta

El ESP32 administra la partida y la interacción con el hardware. Al iniciar, solicita el nombre del jugador en la pantalla LCD. Luego genera una secuencia formada por los números 1, 2 y 3, la muestra durante unos segundos y habilita la respuesta mediante el control remoto.

Una respuesta completa y correcta activa el LED verde y un sonido de confirmación. Una respuesta incorrecta o el vencimiento del tiempo activa el LED rojo y un sonido de error. Al finalizar, el sistema muestra el puntaje y envía los datos para su almacenamiento y consulta.

La arquitectura general es:

**ESP32 → API en Vercel → Supabase → Aplicación web**

La API valida y procesa los resultados antes de almacenarlos en Supabase. La aplicación web consulta esos datos y presenta las partidas y el ranking.

## Justificación

El ESP32 reúne el control de entradas y salidas con conectividad WiFi, por lo que integra el juego con los servicios web sin requerir un módulo de red adicional. El control remoto infrarrojo concentra el ingreso del nombre y las respuestas en un único dispositivo.

La pantalla LCD 16x2 comunica los estados principales: ingreso del nombre, preparación, secuencia, progreso, tiempo restante, resultado y estado del guardado. Los LEDs y el buzzer complementan esa información con señales visuales y sonoras inmediatas.

La API intermedia evita que el ESP32 acceda directamente a la base de datos y separa las responsabilidades del dispositivo, el procesamiento, la persistencia y la visualización.

## Simulación

El proyecto se implementa en Wokwi con una placa ESP32 y componentes virtuales. La simulación permite verificar el receptor infrarrojo, la pantalla LCD, los LEDs, el buzzer, el temporizador, la generación de secuencias, la conexión WiFi y el envío de resultados mediante HTTPS.

Este entorno facilita la demostración, la detección de errores y la repetición de pruebas sin depender de la disponibilidad inmediata de los componentes físicos.

## Resultado esperado

El resultado esperado es un sistema en el que una persona pueda:

1. Ingresar su nombre e iniciar una partida.
2. Memorizar y repetir secuencias antes de que finalice el tiempo.
3. Superar rondas de dificultad progresiva y obtener un puntaje.
4. Guardar el resultado automáticamente.
5. Consultar las partidas y el ranking desde la aplicación web.

Así, **Reflex** integra programación, electrónica, simulación, conectividad y persistencia de datos en una solución única.

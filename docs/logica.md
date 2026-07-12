# Funcionamiento y lógica

## Flujo general

**Reflex** es un juego individual de memoria y reflejos. Su flujo principal es:

1. El jugador ingresa su nombre con el control remoto infrarrojo.
2. El sistema genera y muestra una secuencia aleatoria.
3. El jugador repite la secuencia antes de que termine el tiempo.
4. El sistema valida cada número ingresado.
5. Si la respuesta es correcta, aumenta la dificultad y comienza otra ronda.
6. Si hay un error o se agota el tiempo, calcula y envía el resultado.

Durante la respuesta, la pantalla LCD muestra el progreso y una barra con el tiempo restante. El LED verde y un sonido confirman una ronda superada; el LED rojo y un sonido de error indican el final de la partida.

## Ingreso del nombre

Las teclas 2 a 9 utilizan una lógica similar a la de los teléfonos celulares tradicionales:

| Tecla | Letras |
| ----- | ------ |
| 2     | A, B, C |
| 3     | D, E, F |
| 4     | G, H, I |
| 5     | J, K, L |
| 6     | M, N, O |
| 7     | P, Q, R, S |
| 8     | T, U, V |
| 9     | W, X, Y, Z |

Las pulsaciones sucesivas de una misma tecla recorren sus letras. La tecla de aceptación confirma la letra pendiente o, si no hay una, el nombre completo. La tecla 1 borra la selección actual o el último carácter confirmado. El nombre admite hasta cuatro caracteres.

## Secuencias y dificultad

La primera ronda utiliza una secuencia de tres valores. Cada valor se genera aleatoriamente entre 1, 2 y 3. Después de cada ronda superada, la longitud aumenta en una posición hasta un máximo de ocho valores.

| Ronda | Longitud de la secuencia |
| ----: | -----------------------: |
| 1     | 3 |
| 2     | 4 |
| 3     | 5 |
| 4     | 6 |
| 5     | 7 |
| 6 y siguientes | 8 |

El tiempo de respuesta se mantiene constante, por lo que las secuencias más largas incrementan la dificultad.

## Validación y temporizador

Cuando la secuencia deja de mostrarse, comienza el tiempo de respuesta y se habilita el control remoto. Cada número ingresado se compara con la posición correspondiente:

- Si coincide, el sistema avanza y actualiza el progreso.
- Si no coincide, la partida finaliza por error.
- Si se completan todas las posiciones, la ronda se considera superada.
- Si el tiempo se agota antes de completar la secuencia, la partida finaliza por tiempo.

La barra de la pantalla LCD disminuye durante el período de respuesta para representar el tiempo restante.

## Puntaje

Cada ronda superada otorga 100 puntos:

**Puntaje = rondas acertadas × 100**

La ronda alcanzada es aquella en la que termina la partida. Por ejemplo, si el jugador supera tres rondas y pierde en la cuarta, obtiene 300 puntos, registra tres rondas acertadas y alcanza la ronda 4.

## Estados del sistema

El programa utiliza una máquina de seis estados:

### 1. Ingreso de nombre

Procesa las teclas alfabéticas y permite confirmar o borrar caracteres. Un nombre válido inicia la partida.

### 2. Preparación de ronda

Muestra el número de ronda y el nombre del jugador, reinicia las variables necesarias y genera una nueva secuencia.

### 3. Mostrar secuencia

Presenta la secuencia en la pantalla LCD durante el período de memorización. En este estado no se aceptan respuestas.

### 4. Esperar respuesta

Oculta la secuencia, inicia el temporizador y compara cada entrada con el valor esperado. La pantalla actualiza el progreso y el tiempo restante.

### 5. Ronda superada

Activa el LED verde y el sonido de confirmación, incrementa las rondas acertadas y aumenta la longitud de la secuencia cuando todavía no alcanzó el máximo.

### 6. Juego terminado

Activa el LED rojo y el sonido de error, informa el motivo de finalización, calcula el resultado e inicia su envío. Después queda disponible el comienzo de una nueva partida.

## Envío del resultado

Al finalizar, el ESP32 realiza una solicitud HTTPS `POST` a la API con el nombre, el puntaje, las rondas acertadas y la ronda alcanzada. Los datos se envían en formato JSON:

```json
{
  "nombre": "AGUS",
  "puntaje": 300,
  "rondas_acertadas": 3,
  "ronda_alcanzada": 4
}
```

La pantalla informa si el guardado fue correcto o si ocurrió un error de conexión o del servidor. Para evitar registros duplicados, el envío se intenta una sola vez por partida.

## Ciclo del programa

La función `setup()` configura el monitor serial, los LEDs, el buzzer, la comunicación I2C, la pantalla LCD, el receptor infrarrojo y la generación de números aleatorios. Luego muestra la pantalla inicial y habilita el ingreso del nombre.

La función `loop()` actualiza el temporizador, recibe comandos infrarrojos, ignora repeticiones automáticas y ejecuta la acción correspondiente al estado actual mediante una estructura `switch`.

## Diagrama de flujo

El diagrama de flujo debe representar el proceso general descrito en este documento:

[`docs/img/diagrama-flujo.png`](img/diagrama-flujo.png)

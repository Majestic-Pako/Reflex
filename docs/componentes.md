# - Armado del prototipo

## Lista de componentes

| Componente         | Cantidad | Función                           |
| ------------------ | -------: | --------------------------------- |
| Arduino Uno        |        1 | Controlador principal             |
| Protoboard         |        1 | Base de conexión                  |
| Pulsadores         |        3 | Inicio, jugador 1 y jugador 2     |
| LED rojo           |        1 | Estado inicial o preparación      |
| LED amarillo       |        1 | Estado de atención / señal falsa  |
| LED verde          |        1 | Señal real de reacción            |
| Buzzer             |        1 | Señales sonoras del sistema       |
| Resistencias       |   Varias | Protección de LEDs                |
| Cables macho-macho |   Varios | Conexiones entre componentes      |
| Cable USB A/B      |        1 | Alimentación y carga del programa |

## Descripción general de conexiones

| Elemento        | Tipo de pin     | Función                         |
| --------------- | --------------- | ------------------------------- |
| Botón central   | Entrada digital | Inicia la partida               |
| Botón jugador 1 | Entrada digital | Detecta respuesta del jugador 1 |
| Botón jugador 2 | Entrada digital | Detecta respuesta del jugador 2 |
| LED rojo        | Salida digital  | Estado inicial o preparación    |
| LED amarillo    | Salida digital  | Estado de atención/señal falsa  |
| LED verde       | Salida digital  | Señal real para reaccionar      |
| Buzzer          | Salida digital  | Señal sonora                    |

## Proceso de armado

1. Se coloca el Arduino Uno junto a la protoboard.
2. Se conectan los LEDs a pines digitales de salida.
3. Se agregan resistencias para proteger los LEDs.
4. Se conecta el buzzer a un pin digital y a GND.
5. Se conectan los tres pulsadores:

   * Botón central de inicio.
   * Botón del jugador 1.
   * Botón del jugador 2.
6. Se verifica que todos los componentes compartan la misma línea de GND.
7. Se carga el código en Arduino o en la simulación de Wokwi.
8. Se prueba el funcionamiento mediante los botones, LEDs, buzzer y monitor serial.
9. Se valida que el sistema diferencie correctamente entre señal falsa, señal real, victoria y fallo por adelantarse.

## Nota sobre los pulsadores

Los pulsadores se configuran con `INPUT_PULLUP`.

En este modo, cada botón se conecta entre el pin digital correspondiente y GND. Cuando el botón no está presionado, Arduino lee `HIGH`. Cuando se presiona, Arduino lee `LOW`.

Esta configuración evita lecturas inestables y permite simplificar el armado del circuito, ya que se utiliza la resistencia interna de pull-up del Arduino.

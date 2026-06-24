# 02 - Armado del prototipo

## Lista de componentes

| Componente         | Cantidad | Función                               |
| ------------------ | -------: | ------------------------------------- |
| Arduino Uno        |        1 | Controlador principal                 |
| Protoboard         |        1 | Base de conexión                      |
| Pulsadores         |        3 | Inicio, jugador 1 y jugador 2         |
| LEDs               |  3 o más | Indicación visual                     |
| Buzzer             |        1 | Señal sonora                          |
| Resistencias       |   Varias | Protección y estabilidad del circuito |
| Cables macho-macho |   Varios | Conexiones entre componentes          |
| Cable USB A/B      |        1 | Alimentación y carga del programa     |

## Descripción general de conexiones

| Elemento        | Tipo de pin     | Función                         |
| --------------- | --------------- | ------------------------------- |
| Botón central   | Entrada digital | Inicia la partida               |
| Botón jugador 1 | Entrada digital | Detecta respuesta del jugador 1 |
| Botón jugador 2 | Entrada digital | Detecta respuesta del jugador 2 |
| LED rojo        | Salida digital  | Estado inicial o preparación    |
| LED amarillo    | Salida digital  | Estado de atención              |
| LED verde       | Salida digital  | Señal para reaccionar           |
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
8. Se prueba el funcionamiento mediante los botones y el monitor serial.

## Nota sobre los pulsadores

Para evitar lecturas inestables, los pulsadores pueden configurarse con `INPUT_PULLUP`.

En ese caso, cada botón se conecta entre el pin digital correspondiente y GND. Cuando el botón no está presionado, Arduino lee `HIGH`. Cuando se presiona, lee `LOW`.

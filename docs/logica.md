# - Funcionamiento y lógica del sistema

## Funcionamiento general

El proyecto **Reflex** funciona como un duelo de reflejos entre dos jugadores.

La partida comienza cuando se presiona el botón central. Luego, el sistema muestra una secuencia de preparación con LEDs y sonidos. Durante esta etapa, los jugadores deben esperar y no presionar sus botones.

Antes de la señal real, el sistema puede emitir señales falsas. Estas señales buscan confundir a los jugadores. Si un jugador presiona durante una señal falsa o antes de la señal real, el sistema detecta el fallo y finaliza la ronda.

Cuando aparece la señal real, indicada principalmente por el LED verde y el buzzer, ambos jugadores pueden presionar su botón. El primer jugador detectado por el sistema es declarado ganador.

## Estados principales

1. **Espera de inicio**

   * El sistema queda detenido hasta que se presiona el botón central.

2. **Preparación**

   * Se activan señales visuales y sonoras para indicar que la ronda está por comenzar.

3. **Señales falsas**

   * El sistema puede emitir una o más señales que no son válidas para reaccionar.
   * Si un jugador presiona en esta etapa, pierde la ronda.

4. **Señal real**

   * El sistema activa la señal correcta para reaccionar.
   * Los jugadores deben presionar lo más rápido posible.

5. **Resultado**

   * Si un jugador presiona primero luego de la señal real, gana.
   * Si un jugador se adelanta, se informa el fallo.
   * El resultado se muestra mediante LEDs, buzzer y monitor serial.

## Diagrama de flujo

El diagrama de flujo representa la lógica principal del programa:

Inicio → Esperar botón central → Preparación → Señal falsa o señal real → Verificar botones → Detectar fallo o ganador → Mostrar resultado → Reiniciar ronda.

La imagen del diagrama se encuentra en:

[`/docs/img/diagrama-flujo.png`](/docs/img/diagrama-conexion-v2.0.png)
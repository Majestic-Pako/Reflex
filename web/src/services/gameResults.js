/**
 * Prepara los datos de una partida sin asumir nombres de tablas o columnas.
 * La persistencia se agregara cuando el esquema de Supabase este definido.
 */
export function createGameResultPayload({
  jugadores,
  partida,
  patronMostrado,
  patronIngresado,
  tiempo,
  aciertos,
  errores,
  precision,
  puntaje,
  ganador,
  fechaHora,
}) {
  return {
    jugadores,
    partida,
    patronMostrado,
    patronIngresado,
    tiempo,
    aciertos,
    errores,
    precision,
    puntaje,
    ganador,
    fechaHora,
  }
}

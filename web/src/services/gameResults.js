import { supabase } from './supabase'

export async function obtenerJugadores() {
  const { data, error } = await supabase
    .from('jugadores')
    .select('id, nombre, puntaje, rondas_acertadas, ronda_alcanzada, updated_at')
    .order('puntaje', { ascending: false })
    .order('ronda_alcanzada', { ascending: false })

  if (error) {
    throw error
  }

  return data ?? []
}

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

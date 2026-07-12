import { supabase } from './supabase'

export async function obtenerJugadores() {
  const { data, error } = await supabase
    .from('jugadores')
    .select('*')

  if (error) {
    console.error('Error al obtener jugadores:', error)
    throw error
  }

  return data ?? []
}

export function subscribeToGameResults(onInsert, onStatusChange) {
  return supabase
    .channel('jugadores-inserts')
    .on(
      'postgres_changes',
      {
        event: 'INSERT',
        schema: 'public',
        table: 'jugadores',
      },
      (payload) => {
        console.info('Nueva partida recibida en tiempo real:', payload.new)
        onInsert?.(payload.new)
      },
    )
    .subscribe((status, error) => {
      console.info('Estado Supabase Realtime:', status)

      if (error) {
        console.error('Error en Supabase Realtime:', error)
      } else if (['CHANNEL_ERROR', 'TIMED_OUT', 'CLOSED'].includes(status)) {
        console.error(`Supabase Realtime finalizo con estado: ${status}`)
      }

      onStatusChange?.(status, error)
    })
}

export async function unsubscribeFromGameResults(channel) {
  if (!channel) return

  await supabase.removeChannel(channel)
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

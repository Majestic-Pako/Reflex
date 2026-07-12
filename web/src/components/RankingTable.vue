<script setup>
import { Trophy } from 'lucide-vue-next'

defineProps({
  results: { type: Array, required: true },
  loading: { type: Boolean, default: false },
})
</script>

<template>
  <div class="ranking-panel" :aria-busy="loading">
    <div v-if="loading" class="loading-state" aria-live="polite">
      <span class="spinner" aria-hidden="true"></span>
      Actualizando resultados…
    </div>

    <div v-else-if="results.length" class="ranking-table-wrap">
      <table class="ranking-table">
        <thead>
          <tr>
            <th scope="col">Posición</th>
            <th scope="col">Jugador</th>
            <th scope="col">Puntaje</th>
            <th scope="col">Rondas acertadas</th>
            <th scope="col">Ronda alcanzada</th>
          </tr>
        </thead>
        <tbody>
          <tr v-for="(result, index) in results" :key="result.id ?? `${result.nombre}-${index}`">
            <td data-label="Posición">
              <span class="rank-position" :class="{ 'rank-position--first': index === 0 }">
                <Trophy v-if="index === 0" :size="17" aria-label="Primer puesto" />
                <span v-else>#{{ index + 1 }}</span>
              </span>
            </td>
            <th scope="row" data-label="Jugador">{{ result.nombre || 'Sin nombre' }}</th>
            <td data-label="Puntaje"><strong>{{ result.puntaje ?? '—' }}</strong></td>
            <td data-label="Rondas acertadas">{{ result.rondas_acertadas ?? '—' }}</td>
            <td data-label="Ronda alcanzada">{{ result.ronda_alcanzada ?? '—' }}</td>
          </tr>
        </tbody>
      </table>
    </div>

    <div v-else class="empty-state">
      <span class="empty-state__icon"><Trophy :size="27" /></span>
      <h3>Todavía no hay partidas registradas</h3>
      <p>Los resultados aparecerán acá cuando un jugador termine una partida.</p>
    </div>
  </div>
</template>

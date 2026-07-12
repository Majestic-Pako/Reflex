<script setup>
import { computed } from 'vue'
import { Activity, Target, Trophy, Users } from 'lucide-vue-next'

const props = defineProps({
  results: { type: Array, required: true },
  loading: { type: Boolean, default: false },
})

const stats = computed(() => {
  const scores = props.results.map(({ puntaje }) => Number(puntaje)).filter(Number.isFinite)
  const rounds = props.results
    .map(({ ronda_alcanzada }) => Number(ronda_alcanzada))
    .filter(Number.isFinite)
  const players = new Set(
    props.results.map(({ nombre }) => String(nombre ?? '').trim().toLowerCase()).filter(Boolean),
  )

  return [
    { label: 'Partidas registradas', value: props.results.length, icon: Activity },
    { label: 'Mejor puntaje', value: scores.length ? Math.max(...scores) : '—', icon: Trophy },
    { label: 'Ronda máxima', value: rounds.length ? Math.max(...rounds) : '—', icon: Target },
    { label: 'Jugadores registrados', value: players.size, icon: Users },
  ]
})
</script>

<template>
  <div class="stats-grid" :aria-busy="loading">
    <article v-for="stat in stats" :key="stat.label" class="stat-card">
      <span class="icon-box"><component :is="stat.icon" :size="21" /></span>
      <div>
        <p>{{ stat.label }}</p>
        <strong :class="{ skeleton: loading }">{{ loading ? '00' : stat.value }}</strong>
      </div>
    </article>
  </div>
</template>

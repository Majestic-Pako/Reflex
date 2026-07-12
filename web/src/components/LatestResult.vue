<script setup>
import { Activity, Clock3, Target, Trophy, UserRound } from 'lucide-vue-next'

defineProps({
  result: { type: Object, default: null },
  loading: { type: Boolean, default: false },
})

const fields = [
  { key: 'nombre', label: 'Jugador', icon: UserRound },
  { key: 'puntaje', label: 'Puntaje', icon: Trophy },
  { key: 'rondas_acertadas', label: 'Rondas acertadas', icon: Activity },
  { key: 'ronda_alcanzada', label: 'Ronda alcanzada', icon: Target },
]
</script>

<template>
  <div class="latest-card" :aria-busy="loading">
    <div v-if="loading" class="loading-state" aria-live="polite">
      <span class="spinner" aria-hidden="true"></span> Buscando la última partida…
    </div>
    <template v-else-if="result">
      <div class="latest-card__icon"><Clock3 :size="25" /></div>
      <dl>
        <div v-for="field in fields" :key="field.key">
          <dt><component :is="field.icon" :size="16" /> {{ field.label }}</dt>
          <dd>{{ result[field.key] ?? '—' }}</dd>
        </div>
      </dl>
    </template>
    <div v-else class="latest-empty"><Clock3 :size="21" /> No se registraron partidas todavía.</div>
  </div>
</template>

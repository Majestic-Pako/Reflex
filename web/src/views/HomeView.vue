<script setup>
import { onMounted, ref } from 'vue'
import { obtenerJugadores } from '../services/gameResults'

const jugadores = ref([])
const isLoading = ref(true)
const errorMessage = ref('')

onMounted(async () => {
  try {
    jugadores.value = await obtenerJugadores()
  } catch (error) {
    console.error('Error al cargar los jugadores:', error)
    errorMessage.value = 'No se pudieron cargar los resultados.'
  } finally {
    isLoading.value = false
  }
})
</script>

<template>
  <main class="home">
    <section class="panel" aria-labelledby="project-title">
      <p class="eyebrow">Panel de resultados del juego</p>
      <h1 id="project-title">Reflex Code VS</h1>
      <p class="status">Frontend en preparación</p>
    </section>

    <p v-if="isLoading" class="feedback" aria-live="polite">
      Cargando resultados...
    </p>

    <p v-else-if="errorMessage" class="feedback feedback--error" role="alert">
      {{ errorMessage }}
    </p>

    <section
      v-else-if="jugadores.length > 0"
      class="ranking"
      aria-labelledby="ranking-title"
    >
      <h2 id="ranking-title">Ranking de jugadores</h2>

      <ol class="ranking-list">
        <li
          v-for="(jugador, index) in jugadores"
          :key="jugador.id"
          class="ranking-item"
        >
          <div class="player">
            <span class="position" aria-label="Posición">{{ index + 1 }}.</span>
            <span class="player-name">{{ jugador.nombre }}</span>
          </div>

          <dl class="player-stats">
            <div>
              <dt>Puntaje</dt>
              <dd>{{ jugador.puntaje }}</dd>
            </div>
            <div>
              <dt>Errores</dt>
              <dd>{{ jugador.errores }}</dd>
            </div>
            <div>
              <dt>Victorias</dt>
              <dd>{{ jugador.victorias }}</dd>
            </div>
          </dl>
        </li>
      </ol>
    </section>
  </main>
</template>

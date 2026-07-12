<script setup>
import { computed, onMounted, ref } from 'vue'
import {
  Activity,
  Clock3,
  Cpu,
  Database,
  Eye,
  Gamepad2,
  Keyboard,
  Monitor,
  Radio,
  Trophy,
} from 'lucide-vue-next'
import AppFooter from '../components/AppFooter.vue'
import AppHeader from '../components/AppHeader.vue'
import LatestResult from '../components/LatestResult.vue'
import RankingTable from '../components/RankingTable.vue'
import StatsOverview from '../components/StatsOverview.vue'
import { obtenerJugadores } from '../services/gameResults'

const resultados = ref([])
const isLoading = ref(true)
const errorMessage = ref('')

const resultadosOrdenados = computed(() =>
  [...resultados.value].sort(
    (a, b) =>
      Number(b.puntaje ?? 0) - Number(a.puntaje ?? 0) ||
      Number(b.ronda_alcanzada ?? 0) - Number(a.ronda_alcanzada ?? 0),
  ),
)

const ultimaPartida = computed(() => {
  if (!resultados.value.length) return null

  const dateFields = ['updated_at', 'created_at', 'fecha', 'fecha_hora']
  const datedResults = resultados.value
    .map((result, index) => {
      const rawDate = dateFields.map((field) => result[field]).find(Boolean)
      const timestamp = rawDate ? Date.parse(rawDate) : Number.NaN
      return { result, index, timestamp }
    })
    .filter(({ timestamp }) => Number.isFinite(timestamp))

  if (datedResults.length) {
    return datedResults.reduce((latest, current) =>
      current.timestamp > latest.timestamp ? current : latest,
    ).result
  }

  const resultsWithNumericId = resultados.value.filter((result) =>
    Number.isFinite(Number(result.id)),
  )

  if (resultsWithNumericId.length) {
    return resultsWithNumericId.reduce((latest, current) =>
      Number(current.id) > Number(latest.id) ? current : latest,
    )
  }

  return resultados.value.at(-1)
})

onMounted(async () => {
  try {
    const data = await obtenerJugadores()
    resultados.value = Array.isArray(data) ? data : []
    console.info(`Resultados reales cargados desde Supabase: ${resultados.value.length}`)
  } catch (error) {
    console.error('Error al cargar los resultados de Reflex Code:', error)
    errorMessage.value = 'No pudimos actualizar los resultados. El resto del sitio sigue disponible.'
  } finally {
    isLoading.value = false
  }
})

const steps = [
  {
    title: 'Observá',
    description: 'El sistema presenta una secuencia de números que tenés que memorizar.',
    icon: Eye,
  },
  {
    title: 'Repetí',
    description: 'Ingresá la secuencia en el mismo orden mediante el control remoto.',
    icon: Gamepad2,
  },
  {
    title: 'Avanzá',
    description: 'Cada respuesta correcta aumenta la dificultad y suma más puntos.',
    icon: Activity,
  },
]

const technologies = [
  { label: 'ESP32', icon: Cpu },
  { label: 'Control infrarrojo', icon: Radio },
  { label: 'Vue', icon: Monitor },
  { label: 'Supabase', icon: Database },
  { label: 'Wokwi', icon: Keyboard },
]
</script>

<template>
  <AppHeader />

  <main>
    <section id="inicio" class="hero section-shell" aria-labelledby="hero-title">
      <div class="hero__content">
        <p class="eyebrow"><Gamepad2 :size="16" /> Juego de memoria y reflejos</p>
        <h1 id="hero-title">Reflex <span>Code</span></h1>
        <p class="hero__lead">Poné a prueba tu memoria, velocidad y precisión.</p>
        <p class="hero__description">
          Observá la secuencia, repetila con el control remoto y avanzá de ronda para
          conseguir el mejor puntaje.
        </p>
        <div class="hero__actions">
          <a class="button button--primary" href="#ranking">
            <Trophy :size="18" /> Ver ranking
          </a>
          <a class="button button--secondary" href="#funcionamiento">Cómo funciona</a>
        </div>
      </div>

      <aside class="hero-result-card" aria-label="Resumen de la última partida registrada">
        <div v-if="isLoading" class="hero-result-card__state" aria-live="polite">
          <span class="spinner" aria-hidden="true"></span>
          <p>Cargando última partida...</p>
        </div>

        <template v-else-if="ultimaPartida">
          <div class="hero-result-card__header">
            <div>
              <span class="hero-result-card__label">Última partida</span>
              <h2>Resultado reciente</h2>
            </div>
            <Clock3 :size="24" aria-hidden="true" />
          </div>
          <dl class="hero-result-card__stats">
            <div><dt>Jugador</dt><dd>{{ ultimaPartida.nombre || 'Sin nombre' }}</dd></div>
            <div><dt>Puntaje</dt><dd>{{ ultimaPartida.puntaje }}</dd></div>
            <div><dt>Rondas acertadas</dt><dd>{{ ultimaPartida.rondas_acertadas }}</dd></div>
            <div><dt>Ronda alcanzada</dt><dd>{{ ultimaPartida.ronda_alcanzada }}</dd></div>
          </dl>
        </template>

        <div v-else class="hero-result-card__state hero-result-card__state--empty">
          <span class="hero-result-card__empty-icon"><Gamepad2 :size="27" /></span>
          <div>
            <h2>Todavía no hay partidas</h2>
            <p>La última partida registrada aparecerá acá.</p>
          </div>
        </div>
      </aside>
    </section>

    <section class="section-shell section-block" aria-labelledby="stats-title">
      <div class="section-heading section-heading--compact">
        <p class="eyebrow">Resumen</p>
        <h2 id="stats-title">El juego, de un vistazo</h2>
      </div>
      <StatsOverview :results="resultados" :loading="isLoading" />
    </section>

    <section id="ranking" class="section-shell section-block" aria-labelledby="ranking-title">
      <div class="section-heading">
        <div>
          <p class="eyebrow"><Trophy :size="16" /> Clasificación</p>
          <h2 id="ranking-title">Ranking de jugadores</h2>
        </div>
        <p>Los mejores resultados, ordenados por puntaje y ronda alcanzada.</p>
      </div>

      <p v-if="errorMessage" class="data-notice" role="status">{{ errorMessage }}</p>
      <RankingTable :results="resultadosOrdenados" :loading="isLoading" />
    </section>

    <section class="section-shell section-block latest-section" aria-labelledby="latest-title">
      <div class="section-heading section-heading--compact">
        <p class="eyebrow">Actividad reciente</p>
        <h2 id="latest-title">Última partida</h2>
      </div>
      <LatestResult :result="ultimaPartida" :loading="isLoading" />
    </section>

    <section id="funcionamiento" class="section-shell section-block" aria-labelledby="how-title">
      <div class="section-heading section-heading--centered">
        <p class="eyebrow">Dinámica del juego</p>
        <h2 id="how-title">Cómo funciona</h2>
        <p>Tres pasos simples. Cada ronda, un desafío un poco mayor.</p>
      </div>
      <ol class="steps-list">
        <li v-for="(step, index) in steps" :key="step.title" class="step-card">
          <div class="step-card__top">
            <span class="icon-box"><component :is="step.icon" :size="22" /></span>
            <span class="step-number">0{{ index + 1 }}</span>
          </div>
          <h3>{{ step.title }}</h3>
          <p>{{ step.description }}</p>
        </li>
      </ol>
    </section>

    <section class="section-shell section-block technologies" aria-labelledby="tech-title">
      <div>
        <p class="eyebrow">Tecnologías</p>
        <h2 id="tech-title">Un sistema conectado de punta a punta</h2>
      </div>
      <ul class="tech-list" aria-label="Tecnologías utilizadas">
        <li v-for="technology in technologies" :key="technology.label">
          <component :is="technology.icon" :size="19" /> {{ technology.label }}
        </li>
      </ul>
    </section>
  </main>

  <AppFooter />
</template>

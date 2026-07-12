<script setup>
import { computed, onMounted, onUnmounted, ref } from 'vue'
import {
  Activity,
  Clock3,
  Cpu,
  Database,
  Eye,
  ExternalLink,
  Gamepad2,
  Keyboard,
  Monitor,
  Radio,
  Trophy,
  Zap,
} from 'lucide-vue-next'
import AppFooter from '../components/AppFooter.vue'
import AppHeader from '../components/AppHeader.vue'
import RankingTable from '../components/RankingTable.vue'
import StatsOverview from '../components/StatsOverview.vue'
import {
  obtenerJugadores,
  subscribeToGameResults,
  unsubscribeFromGameResults,
} from '../services/gameResults'

const resultados = ref([])
const isLoading = ref(true)
const errorMessage = ref('')
let resultsChannel = null
let loadingResultsPromise = null
let realtimeResynced = false
let viewIsMounted = false

const normalizeResult = (row) => ({
  ...row,
  puntaje: Number(row.puntaje ?? 0),
  rondas_acertadas: Number(row.rondas_acertadas ?? 0),
  ronda_alcanzada: Number(row.ronda_alcanzada ?? 0),
})

const mergeResultsById = (currentResults, incomingResults) => {
  const merged = [...currentResults]
  const indexesById = new Map(
    merged
      .map((result, index) => [result.id, index])
      .filter(([id]) => id != null)
      .map(([id, index]) => [String(id), index]),
  )

  for (const row of incomingResults) {
    const result = normalizeResult(row)
    const id = result.id == null ? null : String(result.id)

    if (id !== null && indexesById.has(id)) {
      merged[indexesById.get(id)] = result
    } else {
      if (id !== null) indexesById.set(id, merged.length)
      merged.push(result)
    }
  }

  return merged
}

const loadResults = () => {
  if (loadingResultsPromise) return loadingResultsPromise

  loadingResultsPromise = (async () => {
    try {
      const data = await obtenerJugadores()
      if (!viewIsMounted) return

      resultados.value = mergeResultsById(
        resultados.value,
        Array.isArray(data) ? data : [],
      )
      errorMessage.value = ''
      console.info(`Resultados reales cargados desde Supabase: ${resultados.value.length}`)
    } catch (error) {
      console.error('Error al cargar los resultados de Reflex:', error)
      if (viewIsMounted) {
        errorMessage.value = 'No pudimos actualizar los resultados. El resto del sitio sigue disponible.'
      }
    } finally {
      if (viewIsMounted) isLoading.value = false
      loadingResultsPromise = null
    }
  })()

  return loadingResultsPromise
}

const addRealtimeResult = (row) => {
  if (!viewIsMounted) return
  resultados.value = mergeResultsById(resultados.value, [row])
}

const resultadosOrdenados = computed(() =>
  [...resultados.value].sort(
    (a, b) =>
      Number(b.puntaje ?? 0) - Number(a.puntaje ?? 0) ||
      Number(b.ronda_alcanzada ?? 0) - Number(a.ronda_alcanzada ?? 0),
  ),
)

const mejorResultado = computed(() => resultadosOrdenados.value[0] ?? null)

const diferenciaConSegundo = computed(() => {
  const [primero, segundo] = resultadosOrdenados.value

  if (!primero || !segundo) return null

  return Number(primero.puntaje ?? 0) - Number(segundo.puntaje ?? 0)
})

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

onMounted(() => {
  viewIsMounted = true
  resultsChannel = subscribeToGameResults(addRealtimeResult, async (status) => {
    if (status === 'SUBSCRIBED' && !realtimeResynced) {
      realtimeResynced = true
      const initialLoadWasInProgress = loadingResultsPromise !== null
      await loadResults()
      if (initialLoadWasInProgress && viewIsMounted) await loadResults()
    }
  })

  loadResults()
})

onUnmounted(async () => {
  viewIsMounted = false
  await unsubscribeFromGameResults(resultsChannel)
  resultsChannel = null
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

const wokwiUrl = 'https://wokwi.com/projects/469231128370292737'

</script>

<template>
  <AppHeader />

  <main>
    <section id="inicio" class="hero section-shell" aria-labelledby="hero-title">
      <div class="hero__content">
        <p class="eyebrow"><Gamepad2 :size="16" /> Juego de memoria y reflejos</p>
        <h1 id="hero-title" class="brand-title" aria-label="Reflex">
          <Zap class="brand-title__icon" :size="34" aria-hidden="true" />
          <span class="brand-title__accent">Ref</span><span class="brand-title__neutral">lex</span>
        </h1>
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

    <section class="section-shell section-block record-section" aria-labelledby="record-title">
      <div class="record-card" :aria-busy="isLoading">
        <div class="record-card__content">
          <p class="eyebrow"><Trophy :size="16" /> Récord actual</p>
          <h2 id="record-title">
            {{ mejorResultado?.nombre || 'Todavía no hay líder' }}
          </h2>
          <p v-if="mejorResultado">La mejor marca registrada hasta el momento.</p>
          <p v-else>El primer resultado destacado aparecerá acá.</p>
        </div>

        <dl v-if="mejorResultado" class="record-card__stats">
          <div>
            <dt>Puntaje</dt>
            <dd>{{ mejorResultado.puntaje }}</dd>
          </div>
          <div>
            <dt>Ronda</dt>
            <dd>{{ mejorResultado.ronda_alcanzada }}</dd>
          </div>
          <div v-if="diferenciaConSegundo !== null">
            <dt>Ventaja</dt>
            <dd>+{{ diferenciaConSegundo }}</dd>
          </div>
        </dl>
      </div>
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
      <div class="technologies__resources">
        <ul class="tech-list" aria-label="Tecnologías utilizadas">
          <li v-for="technology in technologies" :key="technology.label">
            <component :is="technology.icon" :size="19" /> {{ technology.label }}
          </li>
        </ul>
        <a
          class="technology-link"
          :href="wokwiUrl"
          target="_blank"
          rel="noopener noreferrer"
        >
          <ExternalLink :size="17" aria-hidden="true" />
          Abrir simulación en Wokwi
        </a>
      </div>
    </section>
  </main>

  <AppFooter />
</template>

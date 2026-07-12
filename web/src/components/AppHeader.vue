<script setup>
import { Menu, Zap } from 'lucide-vue-next'
</script>

<template>
  <header class="site-header">
    <div class="site-header__inner section-shell">
      <a class="brand" href="#inicio" aria-label="Ir al inicio de Reflex">
        <Zap class="brand__icon" :size="22" aria-hidden="true" />
        <span class="brand__accent">Ref</span><span class="brand__neutral">lex</span>
      </a>

      <nav class="desktop-nav" aria-label="Navegación principal">
        <a href="#inicio">Inicio</a>
        <a href="#ranking">Ranking</a>
        <a href="#funcionamiento">Funcionamiento</a>
      </nav>

      <details class="mobile-nav">
        <summary aria-label="Abrir navegación"><Menu :size="21" /></summary>
        <nav aria-label="Navegación móvil">
          <a href="#inicio">Inicio</a>
          <a href="#ranking">Ranking</a>
          <a href="#funcionamiento">Funcionamiento</a>
        </nav>
      </details>
    </div>
  </header>
</template>

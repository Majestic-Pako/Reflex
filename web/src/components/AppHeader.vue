<script setup>
import { Gamepad2, Menu } from 'lucide-vue-next'
</script>

<template>
  <header class="site-header">
    <div class="site-header__inner section-shell">
      <a class="brand" href="#inicio" aria-label="Reflex Code, volver al inicio">
        <span class="brand__mark"><Gamepad2 :size="20" /></span>
        Reflex Code
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

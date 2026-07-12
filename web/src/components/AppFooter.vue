<script setup>
import { Zap } from 'lucide-vue-next'
</script>

<template>
  <footer class="site-footer">
    <div class="section-shell">
      <strong class="footer-brand" aria-label="Reflex, Proyecto de Integración Tecnológica">
        <span class="footer-brand__name">
          <Zap class="footer-brand__icon" :size="17" aria-hidden="true" />
          <span class="footer-brand__accent">Ref</span><span class="footer-brand__neutral">lex</span>
        </span>
        <span aria-hidden="true"> · </span>
        <span>Proyecto de Integración Tecnológica</span>
      </strong>
      <p>Desarrollado con ESP32, Vue y Supabase.</p>
    </div>
  </footer>
</template>

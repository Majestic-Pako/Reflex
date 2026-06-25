// Proyecto: Reflex
// Version v2.0 - Duelo de reflejos con senales falsas
// Arduino UNO compatible

// Botones
const int BOTON_J1 = 2;
const int BOTON_J2 = 3;
const int BOTON_START = 4;

// LEDs
const int LED_ROJO = 8;
const int LED_AMARILLO = 6;
const int LED_VERDE = 7;

// Buzzer
const int BUZZER = 11;

// false = usar tone()
// true  = usar HIGH/LOW si el buzzer fisico fuera activo
const bool BUZZER_ACTIVO = false;

// Frecuencia unica confirmada para el buzzer fisico
const int FREQ_BUZZER = 250;

void setup() {
  pinMode(BOTON_J1, INPUT_PULLUP);
  pinMode(BOTON_J2, INPUT_PULLUP);
  pinMode(BOTON_START, INPUT_PULLUP);

  pinMode(LED_ROJO, OUTPUT);
  pinMode(LED_AMARILLO, OUTPUT);
  pinMode(LED_VERDE, OUTPUT);
  pinMode(BUZZER, OUTPUT);

  randomSeed(analogRead(A0));

  apagarTodo();
}

void loop() {
  while (digitalRead(BOTON_START) == HIGH) {
    // Espera hasta que se presione START
  }

  delay(300);

  iniciarRonda();

  delay(3000);
  apagarTodo();
}

void iniciarRonda() {
  apagarTodo();

  // Estado 1: preparados
  digitalWrite(LED_ROJO, HIGH);
  emitirPitidos(1, 300, 150);
  delay(700);
  digitalWrite(LED_ROJO, LOW);

  // Estado 2: atentos
  digitalWrite(LED_AMARILLO, HIGH);
  emitirPitidos(2, 260, 150);
  delay(700);
  digitalWrite(LED_AMARILLO, LOW);

  // Senales falsas antes de la senal real
  if (ejecutarSenialesFalsas()) {
    return;
  }

  // Pre-senal verdadera: trampa sonora antes del LED verde
  if (emitirPitidosConDeteccion(4, 220, 140)) {
    return;
  }

  if (esperarConDeteccion(300)) {
    return;
  }

  // Senal real: recien aca se habilita la respuesta
  digitalWrite(LED_VERDE, HIGH);
  iniciarSonido();

  unsigned long tiempoInicio = millis();
  unsigned long inicioPitidoReal = millis();
  bool pitidoRealActivo = true;

  while (true) {
    // Apaga el pitido real sin bloquear la lectura de botones
    if (pitidoRealActivo && millis() - inicioPitidoReal >= 900) {
      detenerSonido();
      pitidoRealActivo = false;
    }

    if (digitalRead(BOTON_J1) == LOW) {
      ganador(1, tiempoInicio);
      return;
    }

    if (digitalRead(BOTON_J2) == LOW) {
      ganador(2, tiempoInicio);
      return;
    }
  }
}

bool ejecutarSenialesFalsas() {
  int cantidadSenialesFalsas = random(2, 5); // entre 2 y 4

  for (int i = 0; i < cantidadSenialesFalsas; i++) {
    int tiempoAntesFalsa = random(700, 1700);

    if (esperarConDeteccion(tiempoAntesFalsa)) {
      return true;
    }

    if (mostrarSenialFalsa()) {
      return true;
    }

    if (esperarConDeteccion(400)) {
      return true;
    }
  }

  int esperaFinal = random(1000, 2200);

  if (esperarConDeteccion(esperaFinal)) {
    return true;
  }

  return false;
}

bool mostrarSenialFalsa() {
  // Senal falsa:
  // Amarillo titila 3 veces y el buzzer suena 3 veces.
  // No habilita la respuesta.

  for (int i = 0; i < 3; i++) {
    digitalWrite(LED_AMARILLO, HIGH);
    iniciarSonido();

    if (esperarConDeteccion(220)) {
      detenerSonido();
      digitalWrite(LED_AMARILLO, LOW);
      return true;
    }

    detenerSonido();
    digitalWrite(LED_AMARILLO, LOW);

    if (esperarConDeteccion(130)) {
      return true;
    }
  }

  return false;
}

bool esperarConDeteccion(int duracion) {
  unsigned long inicio = millis();

  while (millis() - inicio < duracion) {
    if (digitalRead(BOTON_J1) == LOW) {
      salidaFalsa(1);
      return true;
    }

    if (digitalRead(BOTON_J2) == LOW) {
      salidaFalsa(2);
      return true;
    }
  }

  return false;
}

bool emitirPitidosConDeteccion(int cantidad, int duracionPitido, int pausa) {
  for (int i = 0; i < cantidad; i++) {
    iniciarSonido();

    if (esperarConDeteccion(duracionPitido)) {
      detenerSonido();
      return true;
    }

    detenerSonido();

    if (esperarConDeteccion(pausa)) {
      return true;
    }
  }

  return false;
}

void emitirPitidos(int cantidad, int duracionPitido, int pausa) {
  for (int i = 0; i < cantidad; i++) {
    iniciarSonido();
    delay(duracionPitido);
    detenerSonido();
    delay(pausa);
  }
}

void iniciarSonido() {
  if (BUZZER_ACTIVO) {
    digitalWrite(BUZZER, HIGH);
  } else {
    tone(BUZZER, FREQ_BUZZER);
  }
}

void detenerSonido() {
  if (BUZZER_ACTIVO) {
    digitalWrite(BUZZER, LOW);
  } else {
    noTone(BUZZER);
  }
}

void ganador(int jugador, unsigned long tiempoInicio) {
  apagarTodo();

  if (jugador == 1) {
    digitalWrite(LED_ROJO, HIGH);
  } else {
    digitalWrite(LED_AMARILLO, HIGH);
  }
}

void salidaFalsa(int jugador) {
  apagarTodo();

  // Ante una salida falsa se indica error con LED rojo.
  // El ganador es el jugador contrario.
  digitalWrite(LED_ROJO, HIGH);
}

void apagarTodo() {
  digitalWrite(LED_ROJO, LOW);
  digitalWrite(LED_AMARILLO, LOW);
  digitalWrite(LED_VERDE, LOW);
  detenerSonido();
}
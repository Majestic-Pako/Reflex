// Proyecto: Reflex
// Version v0.1 - Duelo de reflejos simple

// Botones
const int BOTON_J1 = 2;
const int BOTON_J2 = 3;
const int BOTON_START = 4;

// LEDs
const int LED_ROJO = 8;
const int LED_AMARILLO = 9;
const int LED_VERDE = 10;

// Buzzer
const int BUZZER = 11;

void setup() {
  pinMode(BOTON_J1, INPUT_PULLUP);
  pinMode(BOTON_J2, INPUT_PULLUP);
  pinMode(BOTON_START, INPUT_PULLUP);

  pinMode(LED_ROJO, OUTPUT);
  pinMode(LED_AMARILLO, OUTPUT);
  pinMode(LED_VERDE, OUTPUT);
  pinMode(BUZZER, OUTPUT);

  Serial.begin(9600);

  randomSeed(analogRead(A0));

  apagarTodo();

  Serial.println("=== REFLEX v0.1 ===");
  Serial.println("Presiona START para comenzar.");
}

void loop() {
  Serial.println();
  Serial.println("Esperando START...");

  while (digitalRead(BOTON_START) == HIGH) {
    // Espera hasta que se presione START
  }

  delay(300);

  Serial.println("Ronda iniciada.");
  iniciarRonda();

  delay(3000);

  apagarTodo();
  Serial.println("Ronda finalizada.");
}

void iniciarRonda() {
  apagarTodo();

  // Estado 1: preparados
  digitalWrite(LED_ROJO, HIGH);
  tone(BUZZER, 500, 200);
  Serial.println("Preparados...");
  delay(1000);

  digitalWrite(LED_ROJO, LOW);

  // Estado 2: atentos
  digitalWrite(LED_AMARILLO, HIGH);
  tone(BUZZER, 700, 200);
  Serial.println("Atentos...");
  delay(1000);

  digitalWrite(LED_AMARILLO, LOW);

  // Espera aleatoria antes del verde
  int tiempoEspera = random(1500, 4000);
  unsigned long inicioEspera = millis();

  Serial.println("Esperando señal verde...");

  while (millis() - inicioEspera < tiempoEspera) {
    if (digitalRead(BOTON_J1) == LOW) {
      salidaFalsa(1);
      return;
    }

    if (digitalRead(BOTON_J2) == LOW) {
      salidaFalsa(2);
      return;
    }
  }

  // Señal para presionar
  digitalWrite(LED_VERDE, HIGH);
  tone(BUZZER, 1000, 200);

  Serial.println("¡YA!");
  Serial.println("Presionen ahora.");

  unsigned long tiempoInicio = millis();

  while (true) {
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

void ganador(int jugador, unsigned long tiempoInicio) {
  unsigned long tiempoReaccion = millis() - tiempoInicio;

  Serial.print("Ganador: Jugador ");
  Serial.println(jugador);

  Serial.print("Tiempo de reaccion: ");
  Serial.print(tiempoReaccion);
  Serial.println(" ms");

  tone(BUZZER, 1200, 300);
}

void salidaFalsa(int jugador) {
  apagarTodo();

  digitalWrite(LED_ROJO, HIGH);
  tone(BUZZER, 300, 700);

  Serial.print("Salida falsa del Jugador ");
  Serial.println(jugador);

  if (jugador == 1) {
    Serial.println("Ganador: Jugador 2");
  } else {
    Serial.println("Ganador: Jugador 1");
  }
}

void apagarTodo() {
  digitalWrite(LED_ROJO, LOW);
  digitalWrite(LED_AMARILLO, LOW);
  digitalWrite(LED_VERDE, LOW);
  noTone(BUZZER);
}
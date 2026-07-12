/*
  Proyecto: Reflex
  Plataforma: ESP32

  Juego de memoria y reflejos controlado mediante un control infrarrojo.
  El jugador ingresa su nombre, memoriza secuencias numéricas y debe
  repetirlas antes de que finalice el tiempo disponible.

  Al terminar la partida, el resultado se envía mediante HTTPS a una
  API web para su almacenamiento.
*/

// Comunicación I2C con la pantalla LCD.
#include <Wire.h>
#include <LiquidCrystal_I2C.h>

// Lectura del control remoto infrarrojo.
#include <IRremote.hpp>

// Conexión WiFi y solicitudes HTTPS.
#include <WiFi.h>
#include <HTTPClient.h>
#include <WiFiClientSecure.h>

// Configuración de red.
const char* WIFI_SSID = "Wokwi-GUEST";
const char* WIFI_PASSWORD = "";

// API utilizada para guardar los resultados.
const char* API_RESULTADOS_URL =
  "https://reflex-pied.vercel.app/api/resultados";

// Pines del ESP32.
const int PIN_IR = 33;
const int LED_ROJO = 25;
const int LED_VERDE = 26;
const int BUZZER = 27;

// Códigos del control infrarrojo.
const uint16_t TECLA_1 = 0x30;
const uint16_t TECLA_2 = 0x18;
const uint16_t TECLA_3 = 0x7A;
const uint16_t TECLA_4 = 0x10;
const uint16_t TECLA_5 = 0x38;
const uint16_t TECLA_6 = 0x5A;
const uint16_t TECLA_7 = 0x42;
const uint16_t TECLA_8 = 0x4A;
const uint16_t TECLA_9 = 0x52;
const uint16_t TECLA_ACEPTAR = 0xA8;

// Configuración del juego.
const int LONGITUD_INICIAL = 3;
const int LONGITUD_MAXIMA = 8;
const unsigned long TIEMPO_MEMORIZACION = 3000;
const unsigned long TIEMPO_RESPUESTA = 3000;
const unsigned long INTERVALO_BARRA = 100;
const int TAMANIO_BARRA = 8;
const int PUNTOS_POR_RONDA = 100;

// Configuración del ingreso de nombre.
const int LONGITUD_NOMBRE_MAXIMA = 4;

const char* LETRAS_TECLA_2 = "ABC";
const char* LETRAS_TECLA_3 = "DEF";
const char* LETRAS_TECLA_4 = "GHI";
const char* LETRAS_TECLA_5 = "JKL";
const char* LETRAS_TECLA_6 = "MNO";
const char* LETRAS_TECLA_7 = "PQRS";
const char* LETRAS_TECLA_8 = "TUV";
const char* LETRAS_TECLA_9 = "WXYZ";

// Estados posibles del juego.
enum EstadoJuego {
  INGRESANDO_NOMBRE,
  PREPARANDO_RONDA,
  MOSTRANDO_SECUENCIA,
  ESPERANDO_RESPUESTA,
  RONDA_SUPERADA,
  JUEGO_TERMINADO
};

// Motivos posibles de finalización.
enum MotivoFin {
  SIN_MOTIVO,
  FIN_POR_ERROR,
  FIN_POR_TIEMPO
};

// Información utilizada durante el ingreso del nombre.
struct DatosJugador {
  char nombre[LONGITUD_NOMBRE_MAXIMA + 1];
  int longitudNombre;
  char letraPendiente;
  uint16_t ultimaTeclaNombre;
  int indiceLetraPendiente;
};

// Información interna de la partida.
struct DatosPartida {
  int secuencia[LONGITUD_MAXIMA];
  int longitudSecuencia;
  int posicionActual;
  int rondasAcertadas;
  unsigned long inicioRespuesta;
  unsigned long ultimaActualizacionBarra;
  MotivoFin motivoFin;
  int teclaEsperadaFinal;
  int teclaRecibidaFinal;
};

// Información enviada a la API.
struct ResultadoPartida {
  String nombre;
  int puntaje;
  int rondasAcertadas;
  int rondaAlcanzada;
};

// Estado general del programa.
EstadoJuego estadoActual = INGRESANDO_NOMBRE;
DatosJugador jugador = {};
DatosPartida partida = {};

bool resultadoGuardado = false;
bool envioIntentado = false;

// Pantalla LCD 16x2 con dirección I2C 0x27.
LiquidCrystal_I2C lcd(0x27, 16, 2);

// Prototipos de utilidades.
void apagarSalidas();
void escribirLineaLCD(uint8_t fila, const char* texto);
void mostrarMensaje(const char* linea1, const char* linea2);
void reiniciarJugador();
void reiniciarPartida();

int calcularPuntaje(int rondasAcertadas);
const char* textoMotivoFin(MotivoFin motivo);
int obtenerNumeroTecla(uint16_t comando);
const char* obtenerLetrasPorTecla(uint16_t comando);
bool esTeclaNombre(uint16_t comando);

// Prototipos de conexión y persistencia.
bool conectarWiFi();
bool enviarResultado(const ResultadoPartida& resultado);

// Prototipos del ingreso de nombre.
void mostrarPantallaNombre();
void iniciarIngresoNombre();
void seleccionarLetra(uint16_t comando);
void confirmarLetraPendiente();
void borrarUltimaLetra();
void procesarAceptarNombre();

// Prototipos de la partida.
void generarSecuencia();
void mostrarSecuencia();
void mostrarBarraTiempo();
void mostrarProgresoRespuesta();
void iniciarPartida();
void iniciarRonda();
void iniciarTiempoRespuesta();
void actualizarTiempoRespuesta();
void procesarRespuesta(int numeroPulsado);
void superarRonda();
void perderPorError(int numeroPulsado, int numeroEsperado);
void perderPorTiempo();

void finalizarPartida(
  MotivoFin motivo,
  int teclaEsperada,
  int teclaRecibida
);

// Prototipos del resultado final.
ResultadoPartida construirResultadoFinal();
void mostrarResultadoLCD(const ResultadoPartida& resultado);
void mostrarResultadoSerial(const ResultadoPartida& resultado);
void procesarResultadoFinal(const ResultadoPartida& resultado);

// ------------------------------------------------------------
// Utilidades de hardware y estado
// ------------------------------------------------------------

void apagarSalidas() {
  digitalWrite(LED_ROJO, LOW);
  digitalWrite(LED_VERDE, LOW);
  noTone(BUZZER);
}

void escribirLineaLCD(uint8_t fila, const char* texto) {
  lcd.setCursor(0, fila);

  int columna = 0;

  while (columna < 16 && texto[columna] != '\0') {
    lcd.print(texto[columna]);
    columna++;
  }

  while (columna < 16) {
    lcd.print(' ');
    columna++;
  }
}

void mostrarMensaje(const char* linea1, const char* linea2) {
  lcd.clear();
  escribirLineaLCD(0, linea1);
  escribirLineaLCD(1, linea2);
}

void reiniciarJugador() {
  jugador.nombre[0] = '\0';
  jugador.longitudNombre = 0;
  jugador.letraPendiente = '\0';
  jugador.ultimaTeclaNombre = 0;
  jugador.indiceLetraPendiente = 0;
}

void reiniciarPartida() {
  partida.longitudSecuencia = LONGITUD_INICIAL;
  partida.posicionActual = 0;
  partida.rondasAcertadas = 0;
  partida.inicioRespuesta = 0;
  partida.ultimaActualizacionBarra = 0;
  partida.motivoFin = SIN_MOTIVO;
  partida.teclaEsperadaFinal = -1;
  partida.teclaRecibidaFinal = -1;

  resultadoGuardado = false;
  envioIntentado = false;
}

int calcularPuntaje(int rondasAcertadas) {
  return rondasAcertadas * PUNTOS_POR_RONDA;
}

const char* textoMotivoFin(MotivoFin motivo) {
  switch (motivo) {
    case FIN_POR_ERROR:
      return "ERROR";

    case FIN_POR_TIEMPO:
      return "TIEMPO";

    default:
      return "SIN DEFINIR";
  }
}

int obtenerNumeroTecla(uint16_t comando) {
  switch (comando) {
    case TECLA_1:
      return 1;

    case TECLA_2:
      return 2;

    case TECLA_3:
      return 3;

    default:
      return -1;
  }
}

const char* obtenerLetrasPorTecla(uint16_t comando) {
  switch (comando) {
    case TECLA_2:
      return LETRAS_TECLA_2;

    case TECLA_3:
      return LETRAS_TECLA_3;

    case TECLA_4:
      return LETRAS_TECLA_4;

    case TECLA_5:
      return LETRAS_TECLA_5;

    case TECLA_6:
      return LETRAS_TECLA_6;

    case TECLA_7:
      return LETRAS_TECLA_7;

    case TECLA_8:
      return LETRAS_TECLA_8;

    case TECLA_9:
      return LETRAS_TECLA_9;

    default:
      return nullptr;
  }
}

bool esTeclaNombre(uint16_t comando) {
  return obtenerLetrasPorTecla(comando) != nullptr;
}

// ------------------------------------------------------------
// Conexión WiFi y envío de resultados
// ------------------------------------------------------------

bool conectarWiFi() {
  if (WiFi.status() == WL_CONNECTED) {
    return true;
  }

  Serial.println("Conectando a WiFi...");

  WiFi.mode(WIFI_STA);
  WiFi.begin(WIFI_SSID, WIFI_PASSWORD);

  const unsigned long TIMEOUT_WIFI = 15000;
  unsigned long inicio = millis();

  while (
    WiFi.status() != WL_CONNECTED &&
    millis() - inicio < TIMEOUT_WIFI
  ) {
    delay(250);
  }

  if (WiFi.status() != WL_CONNECTED) {
    Serial.println("No se pudo conectar a WiFi");
    return false;
  }

  Serial.println("WiFi conectado");

  return true;
}

bool enviarResultado(const ResultadoPartida& resultado) {
  if (!conectarWiFi()) {
    return false;
  }

  String jsonBody =
    "{\"nombre\":\"" +
    resultado.nombre +
    "\",\"puntaje\":" +
    String(resultado.puntaje) +
    ",\"rondas_acertadas\":" +
    String(resultado.rondasAcertadas) +
    ",\"ronda_alcanzada\":" +
    String(resultado.rondaAlcanzada) +
    "}";

  Serial.println("Enviando resultado...");

  WiFiClientSecure client;

  /*
    La simulación de Wokwi no dispone del certificado raíz
    utilizado por la API. Se mantiene HTTPS, pero se omite
    la validación local del certificado.
  */
  client.setInsecure();

  HTTPClient http;

  if (!http.begin(client, API_RESULTADOS_URL)) {
    Serial.println("No se pudo iniciar la conexion HTTPS");
    return false;
  }

  http.setConnectTimeout(15000);
  http.setTimeout(15000);
  http.setFollowRedirects(HTTPC_STRICT_FOLLOW_REDIRECTS);

  http.addHeader("Content-Type", "application/json");

  int httpCode = http.POST(jsonBody);

  bool guardado =
    httpCode >= 200 &&
    httpCode < 300;

  if (httpCode > 0) {
    Serial.print("Respuesta del servidor: ");
    Serial.println(httpCode);
  } else {
    Serial.print("Error de conexion: ");
    Serial.println(http.errorToString(httpCode));
  }

  if (guardado) {
    Serial.println("Resultado guardado");
  } else {
    Serial.println("No se pudo guardar el resultado");
  }

  http.end();

  return guardado;
}

// ------------------------------------------------------------
// Ingreso de nombre
// ------------------------------------------------------------

void mostrarPantallaNombre() {
  char lineaNombre[LONGITUD_NOMBRE_MAXIMA + 1];

  for (int i = 0; i < LONGITUD_NOMBRE_MAXIMA; i++) {
    if (i < jugador.longitudNombre) {
      lineaNombre[i] = jugador.nombre[i];
    } else if (
      i == jugador.longitudNombre &&
      jugador.letraPendiente != '\0'
    ) {
      lineaNombre[i] = jugador.letraPendiente;
    } else {
      lineaNombre[i] = '_';
    }
  }

  lineaNombre[LONGITUD_NOMBRE_MAXIMA] = '\0';

  mostrarMensaje("Nombre:", lineaNombre);
}

void iniciarIngresoNombre() {
  estadoActual = INGRESANDO_NOMBRE;

  apagarSalidas();
  reiniciarJugador();
  reiniciarPartida();
  mostrarPantallaNombre();

  Serial.println();
  Serial.println("Ingresa tu nombre");
}

void seleccionarLetra(uint16_t comando) {
  if (jugador.longitudNombre >= LONGITUD_NOMBRE_MAXIMA) {
    return;
  }

  const char* letras = obtenerLetrasPorTecla(comando);

  if (letras == nullptr) {
    return;
  }

  int cantidadLetras = strlen(letras);

  if (
    jugador.ultimaTeclaNombre == comando &&
    jugador.letraPendiente != '\0'
  ) {
    jugador.indiceLetraPendiente =
      (jugador.indiceLetraPendiente + 1) % cantidadLetras;
  } else {
    jugador.ultimaTeclaNombre = comando;
    jugador.indiceLetraPendiente = 0;
  }

  jugador.letraPendiente =
    letras[jugador.indiceLetraPendiente];

  mostrarPantallaNombre();
}

void confirmarLetraPendiente() {
  if (
    jugador.letraPendiente == '\0' ||
    jugador.longitudNombre >= LONGITUD_NOMBRE_MAXIMA
  ) {
    return;
  }

  jugador.nombre[jugador.longitudNombre] =
    jugador.letraPendiente;

  jugador.longitudNombre++;
  jugador.nombre[jugador.longitudNombre] = '\0';

  jugador.letraPendiente = '\0';
  jugador.ultimaTeclaNombre = 0;
  jugador.indiceLetraPendiente = 0;

  mostrarPantallaNombre();
}

void borrarUltimaLetra() {
  if (jugador.letraPendiente != '\0') {
    jugador.letraPendiente = '\0';
    jugador.ultimaTeclaNombre = 0;
    jugador.indiceLetraPendiente = 0;

    mostrarPantallaNombre();
    return;
  }

  if (jugador.longitudNombre == 0) {
    return;
  }

  jugador.longitudNombre--;
  jugador.nombre[jugador.longitudNombre] = '\0';

  mostrarPantallaNombre();
}

void procesarAceptarNombre() {
  if (jugador.letraPendiente != '\0') {
    confirmarLetraPendiente();
    return;
  }

  if (jugador.longitudNombre == 0) {
    mostrarMensaje("Nombre vacio", "Ingresa letras");
    delay(800);
    mostrarPantallaNombre();
    return;
  }

  Serial.print("Nombre confirmado: ");
  Serial.println(jugador.nombre);

  iniciarPartida();
}

// ------------------------------------------------------------
// Secuencias y rondas
// ------------------------------------------------------------

void generarSecuencia() {
  for (int i = 0; i < partida.longitudSecuencia; i++) {
    partida.secuencia[i] = random(1, 4);
  }
}

void mostrarSecuencia() {
  char lineaSecuencia[17];
  int indice = 0;

  for (
    int i = 0;
    i < partida.longitudSecuencia && indice < 16;
    i++
  ) {
    lineaSecuencia[indice++] =
      '0' + partida.secuencia[i];

    if (
      i < partida.longitudSecuencia - 1 &&
      indice < 16
    ) {
      lineaSecuencia[indice++] = ' ';
    }
  }

  lineaSecuencia[indice] = '\0';

  estadoActual = MOSTRANDO_SECUENCIA;

  mostrarMensaje("Memoriza:", lineaSecuencia);

  delay(TIEMPO_MEMORIZACION);

  iniciarTiempoRespuesta();
}

void mostrarBarraTiempo() {
  unsigned long tiempoTranscurrido =
    millis() - partida.inicioRespuesta;

  if (tiempoTranscurrido > TIEMPO_RESPUESTA) {
    tiempoTranscurrido = TIEMPO_RESPUESTA;
  }

  unsigned long tiempoRestante =
    TIEMPO_RESPUESTA - tiempoTranscurrido;

  int bloquesActivos = map(
    tiempoRestante,
    0,
    TIEMPO_RESPUESTA,
    0,
    TAMANIO_BARRA
  );

  char barra[13] = "T:[--------]";

  for (int i = 0; i < TAMANIO_BARRA; i++) {
    if (i < bloquesActivos) {
      barra[3 + i] = '#';
    }
  }

  escribirLineaLCD(1, barra);
}

void mostrarProgresoRespuesta() {
  char progreso[17] = "Repite: ";

  for (int i = 0; i < partida.longitudSecuencia; i++) {
    progreso[8 + i] =
      i < partida.posicionActual ? '*' : '_';
  }

  progreso[8 + partida.longitudSecuencia] = '\0';

  escribirLineaLCD(0, progreso);
}

void iniciarPartida() {
  apagarSalidas();
  reiniciarPartida();

  Serial.println("Partida iniciada");

  char lineaJugador[17];

  snprintf(
    lineaJugador,
    sizeof(lineaJugador),
    "Jugador:%s",
    jugador.nombre
  );

  mostrarMensaje(lineaJugador, "Preparate...");

  delay(1500);

  iniciarRonda();
}

void iniciarRonda() {
  apagarSalidas();

  partida.posicionActual = 0;
  estadoActual = PREPARANDO_RONDA;

  char lineaRonda[17];

  snprintf(
    lineaRonda,
    sizeof(lineaRonda),
    "Ronda %d",
    partida.rondasAcertadas + 1
  );

  mostrarMensaje(lineaRonda, jugador.nombre);

  Serial.print("Ronda ");
  Serial.println(partida.rondasAcertadas + 1);

  delay(1000);

  generarSecuencia();
  mostrarSecuencia();
}

void iniciarTiempoRespuesta() {
  lcd.clear();

  partida.posicionActual = 0;
  partida.inicioRespuesta = millis();
  partida.ultimaActualizacionBarra = 0;
  estadoActual = ESPERANDO_RESPUESTA;

  mostrarProgresoRespuesta();
  mostrarBarraTiempo();
}

// ------------------------------------------------------------
// Temporizador y evaluación de respuestas
// ------------------------------------------------------------

void actualizarTiempoRespuesta() {
  if (estadoActual != ESPERANDO_RESPUESTA) {
    return;
  }

  unsigned long ahora = millis();

  if (ahora - partida.inicioRespuesta >= TIEMPO_RESPUESTA) {
    perderPorTiempo();
    return;
  }

  if (
    ahora - partida.ultimaActualizacionBarra >=
    INTERVALO_BARRA
  ) {
    partida.ultimaActualizacionBarra = ahora;
    mostrarBarraTiempo();
  }
}

void procesarRespuesta(int numeroPulsado) {
  if (
    estadoActual != ESPERANDO_RESPUESTA ||
    partida.posicionActual < 0 ||
    partida.posicionActual >= partida.longitudSecuencia
  ) {
    return;
  }

  int numeroEsperado =
    partida.secuencia[partida.posicionActual];

  if (numeroPulsado != numeroEsperado) {
    perderPorError(numeroPulsado, numeroEsperado);
    return;
  }

  partida.posicionActual++;

  mostrarProgresoRespuesta();

  if (partida.posicionActual >= partida.longitudSecuencia) {
    superarRonda();
  }
}

void superarRonda() {
  estadoActual = RONDA_SUPERADA;
  partida.rondasAcertadas++;

  apagarSalidas();

  digitalWrite(LED_VERDE, HIGH);

  tone(BUZZER, 1200);
  delay(150);
  noTone(BUZZER);

  char lineaRondas[17];

  snprintf(
    lineaRondas,
    sizeof(lineaRondas),
    "Rondas: %d",
    partida.rondasAcertadas
  );

  mostrarMensaje("CORRECTO!", lineaRondas);

  Serial.println("Ronda superada");

  delay(1500);

  digitalWrite(LED_VERDE, LOW);

  if (partida.longitudSecuencia < LONGITUD_MAXIMA) {
    partida.longitudSecuencia++;
  }

  iniciarRonda();
}

void perderPorError(int numeroPulsado, int numeroEsperado) {
  finalizarPartida(
    FIN_POR_ERROR,
    numeroEsperado,
    numeroPulsado
  );
}

void perderPorTiempo() {
  int teclaEsperada = -1;

  if (
    partida.posicionActual >= 0 &&
    partida.posicionActual < partida.longitudSecuencia
  ) {
    teclaEsperada =
      partida.secuencia[partida.posicionActual];
  }

  finalizarPartida(
    FIN_POR_TIEMPO,
    teclaEsperada,
    -1
  );
}

void finalizarPartida(
  MotivoFin motivo,
  int teclaEsperada,
  int teclaRecibida
) {
  estadoActual = JUEGO_TERMINADO;

  partida.motivoFin = motivo;
  partida.teclaEsperadaFinal = teclaEsperada;
  partida.teclaRecibidaFinal = teclaRecibida;

  apagarSalidas();

  digitalWrite(LED_ROJO, HIGH);

  tone(BUZZER, 300);
  delay(400);
  noTone(BUZZER);

  if (motivo == FIN_POR_ERROR) {
    mostrarMensaje("INCORRECTO", "Fin del juego");
    Serial.println("Fin del juego por error");
  } else {
    mostrarMensaje("SIN TIEMPO", "Fin del juego");
    Serial.println("Fin del juego por tiempo");
  }

  delay(1600);

  digitalWrite(LED_ROJO, LOW);

  ResultadoPartida resultado =
    construirResultadoFinal();

  procesarResultadoFinal(resultado);
}

// ------------------------------------------------------------
// Resultado final y persistencia
// ------------------------------------------------------------

ResultadoPartida construirResultadoFinal() {
  ResultadoPartida resultado = {};

  resultado.nombre = jugador.nombre;
  resultado.puntaje =
    calcularPuntaje(partida.rondasAcertadas);

  resultado.rondasAcertadas =
    partida.rondasAcertadas;

  resultado.rondaAlcanzada =
    partida.rondasAcertadas + 1;

  return resultado;
}

void mostrarResultadoLCD(
  const ResultadoPartida& resultado
) {
  char lineaPuntaje[17];
  char lineaRondas[17];

  snprintf(
    lineaPuntaje,
    sizeof(lineaPuntaje),
    "%s Pts:%d",
    resultado.nombre.c_str(),
    resultado.puntaje
  );

  snprintf(
    lineaRondas,
    sizeof(lineaRondas),
    "Rondas:%d",
    resultado.rondasAcertadas
  );

  mostrarMensaje(lineaPuntaje, lineaRondas);
}

void mostrarResultadoSerial(
  const ResultadoPartida& resultado
) {
  Serial.println();
  Serial.println("========== RESULTADO ==========");
  Serial.print("Jugador: ");
  Serial.println(resultado.nombre);

  Serial.print("Puntaje: ");
  Serial.println(resultado.puntaje);

  Serial.print("Rondas acertadas: ");
  Serial.println(resultado.rondasAcertadas);

  Serial.print("Ronda alcanzada: ");
  Serial.println(resultado.rondaAlcanzada);

  Serial.print("Finalizacion: ");
  Serial.println(textoMotivoFin(partida.motivoFin));

  Serial.println("===============================");
}

void procesarResultadoFinal(
  const ResultadoPartida& resultado
) {
  // Muestra primero el resultado obtenido.
  mostrarResultadoLCD(resultado);
  mostrarResultadoSerial(resultado);

  delay(1800);

  // Informa al jugador que se está realizando el envío.
  if (!envioIntentado) {
    envioIntentado = true;

    mostrarMensaje("Guardando...", "Por favor");

    resultadoGuardado =
      enviarResultado(resultado);
  }

  // Muestra el estado final del guardado.
  if (resultadoGuardado) {
    mostrarMensaje("Resultado", "guardado");
  } else {
    mostrarMensaje("No se guardo", "Intenta luego");
  }

  delay(1800);

  Serial.println("Pulsa START para jugar otra vez");

  char lineaRondas[17];

  snprintf(
    lineaRondas,
    sizeof(lineaRondas),
    "Rondas: %d",
    resultado.rondasAcertadas
  );

  mostrarMensaje(lineaRondas, "START: nuevo");
}

// ------------------------------------------------------------
// Configuración inicial
// ------------------------------------------------------------

void setup() {
  // Inicializa el monitor serial.
  Serial.begin(115200);

  // Configura LEDs y buzzer como salidas.
  pinMode(LED_ROJO, OUTPUT);
  pinMode(LED_VERDE, OUTPUT);
  pinMode(BUZZER, OUTPUT);

  apagarSalidas();

  // Inicializa la pantalla LCD mediante I2C.
  Wire.begin(21, 22);
  lcd.init();
  lcd.backlight();

  // Inicializa el receptor infrarrojo.
  IrReceiver.begin(
    PIN_IR,
    DISABLE_LED_FEEDBACK
  );

  // Inicializa la generación aleatoria de secuencias.
  randomSeed(micros());

  Serial.println("Reflex iniciado");

  mostrarMensaje("Reflex", "Cargando...");

  delay(1000);

  iniciarIngresoNombre();
}

// ------------------------------------------------------------
// Bucle principal
// ------------------------------------------------------------

void loop() {
  // Actualiza el temporizador mientras el jugador responde.
  actualizarTiempoRespuesta();

  // Termina esta iteración si no se recibió una señal IR.
  if (!IrReceiver.decode()) {
    return;
  }

  uint16_t comando =
    IrReceiver.decodedIRData.command;

  bool esRepeticion =
    IrReceiver.decodedIRData.flags &
    IRDATA_FLAGS_IS_REPEAT;

  // Prepara el receptor para recibir el siguiente comando.
  IrReceiver.resume();

  // Descarta repeticiones automáticas del control remoto.
  if (esRepeticion) {
    return;
  }

  // Procesa el comando según el estado actual del juego.
  switch (estadoActual) {
    case INGRESANDO_NOMBRE:
      if (comando == TECLA_ACEPTAR) {
        procesarAceptarNombre();
      } else if (comando == TECLA_1) {
        borrarUltimaLetra();
      } else if (esTeclaNombre(comando)) {
        seleccionarLetra(comando);
      }
      break;

    case ESPERANDO_RESPUESTA: {
      int numeroPulsado =
        obtenerNumeroTecla(comando);

      if (numeroPulsado != -1) {
        procesarRespuesta(numeroPulsado);
      }

      break;
    }

    case JUEGO_TERMINADO:
      if (comando == TECLA_ACEPTAR) {
        iniciarIngresoNombre();
      }
      break;

    case PREPARANDO_RONDA:
    case MOSTRANDO_SECUENCIA:
    case RONDA_SUPERADA:
      break;
  }
}
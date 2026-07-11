#include <Wire.h>
#include <LiquidCrystal_I2C.h>
#include <IRremote.hpp>

// Pines
const int PIN_IR = 33;
const int LED_ROJO = 25;
const int LED_VERDE = 26;
const int BUZZER = 27;

// Códigos del control remoto
const uint16_t TECLA_1 = 0x30;
const uint16_t TECLA_2 = 0x18;
const uint16_t TECLA_3 = 0x7A;

// LCD 16x2 I2C
LiquidCrystal_I2C lcd(0x27, 16, 2);

void apagarSalidas() {
  digitalWrite(LED_ROJO, LOW);
  digitalWrite(LED_VERDE, LOW);
  noTone(BUZZER);
}

void mostrarMensaje(const char* linea1, const char* linea2) {
  lcd.clear();

  lcd.setCursor(0, 0);
  lcd.print(linea1);

  lcd.setCursor(0, 1);
  lcd.print(linea2);
}

void probarLedRojo() {
  apagarSalidas();

  Serial.println("Tecla 1: LED rojo");

  mostrarMensaje("Tecla 1", "LED rojo");

  digitalWrite(LED_ROJO, HIGH);
  delay(1000);
  digitalWrite(LED_ROJO, LOW);
}

void probarLedVerde() {
  apagarSalidas();

  Serial.println("Tecla 2: LED verde");

  mostrarMensaje("Tecla 2", "LED verde");

  digitalWrite(LED_VERDE, HIGH);
  delay(1000);
  digitalWrite(LED_VERDE, LOW);
}

void probarBuzzer() {
  apagarSalidas();

  Serial.println("Tecla 3: Buzzer");

  mostrarMensaje("Tecla 3", "Buzzer");

  tone(BUZZER, 1000);
  delay(500);
  noTone(BUZZER);
}

void setup() {
  Serial.begin(115200);

  pinMode(LED_ROJO, OUTPUT);
  pinMode(LED_VERDE, OUTPUT);
  pinMode(BUZZER, OUTPUT);

  apagarSalidas();

  // I2C del ESP32
  Wire.begin(21, 22);

  lcd.init();
  lcd.backlight();

  mostrarMensaje("Reflex Code VS", "Usa teclas 1-3");

  // Receptor IR
  IrReceiver.begin(PIN_IR, DISABLE_LED_FEEDBACK);

  Serial.println("================================");
  Serial.println("Prueba de componentes");
  Serial.println("1: LED rojo");
  Serial.println("2: LED verde");
  Serial.println("3: Buzzer");
  Serial.println("================================");
}

void loop() {
  if (IrReceiver.decode()) {
    uint16_t comando = IrReceiver.decodedIRData.command;

    bool esRepeticion =
      IrReceiver.decodedIRData.flags & IRDATA_FLAGS_IS_REPEAT;

    if (!esRepeticion) {
      Serial.print("Comando recibido: 0x");
      Serial.println(comando, HEX);

      switch (comando) {
        case TECLA_1:
          probarLedRojo();
          break;

        case TECLA_2:
          probarLedVerde();
          break;

        case TECLA_3:
          probarBuzzer();
          break;

        default:
          Serial.println("Tecla no configurada");

          mostrarMensaje(
            "Tecla ignorada",
            "Usa 1, 2 o 3"
          );

          delay(600);
          break;
      }

      mostrarMensaje(
        "Reflex Code VS",
        "Usa teclas 1-3"
      );
    }

    IrReceiver.resume();
  }
}
# Componentes, conexiones y servicios

## Hardware

| Cantidad | Componente | Función principal |
| -------: | ---------- | ----------------- |
| 1 | ESP32 | Ejecuta la lógica y proporciona conexión WiFi |
| 1 | Pantalla LCD 16x2 con I2C | Muestra instrucciones, secuencias y resultados |
| 1 | Control remoto infrarrojo | Permite ingresar el nombre y responder |
| 1 | Receptor infrarrojo | Recibe los comandos del control |
| 1 | LED verde | Indica una ronda superada |
| 1 | LED rojo | Indica un error o el final de la partida |
| 1 | Buzzer | Proporciona señales sonoras |
| 2 | Resistencias de 220 Ω | Limitan la corriente de los LEDs |
| 1 | Protoboard | Permite montar el circuito sin soldaduras |
| Varias | Cables | Conectan los componentes y la alimentación |

### ESP32

El ESP32 controla la lógica del juego, procesa las señales del receptor IR, actualiza las salidas y calcula el resultado. Su conectividad WiFi integrada permite enviar los datos a la API mediante HTTPS sin un módulo de red adicional.

### Pantalla LCD 16x2 con I2C

La pantalla comunica el ingreso del nombre, la ronda, la secuencia, el progreso, el tiempo restante, el resultado y el estado del guardado. El módulo I2C reduce la conexión a las líneas SDA y SCL. La dirección configurada es `0x27`.

### Control y receptor infrarrojos

El control remoto es la entrada principal: permite escribir el nombre, confirmar o borrar caracteres, responder con las teclas 1, 2 y 3 e iniciar una nueva partida. El receptor decodifica cada comando y lo entrega al ESP32 mediante GPIO 33.

### Indicadores

- El LED verde, conectado a GPIO 26, confirma una ronda superada.
- El LED rojo, conectado a GPIO 25, indica una respuesta incorrecta, el vencimiento del tiempo o el final de la partida.
- El buzzer, conectado a GPIO 27, diferencia la confirmación y el error mediante tonos.

Cada LED utiliza una resistencia de aproximadamente 220 Ω. Todos los componentes deben compartir una conexión común a GND.

## Conexiones

### Resumen de pines

| Componente | Pin ESP32 |
| ---------- | --------- |
| Receptor infrarrojo | GPIO 33 |
| LED rojo | GPIO 25 |
| LED verde | GPIO 26 |
| Buzzer | GPIO 27 |
| LCD SDA | GPIO 21 |
| LCD SCL | GPIO 22 |

### Pantalla LCD

| Pin LCD I2C | Conexión ESP32 |
| ----------- | -------------- |
| VCC | 5V |
| GND | GND |
| SDA | GPIO 21 |
| SCL | GPIO 22 |

```cpp
Wire.begin(21, 22);
LiquidCrystal_I2C lcd(0x27, 16, 2);
```

### Receptor IR y salidas

```cpp
const int PIN_IR = 33;
const int LED_ROJO = 25;
const int LED_VERDE = 26;
const int BUZZER = 27;
```

La alimentación del receptor debe respetar la tensión admitida por el módulo utilizado. Su pin de señal se conecta a GPIO 33.

## Conectividad y servicios

### WiFi

La conectividad está integrada en el ESP32. En la simulación se utiliza la red:

```cpp
Wokwi-GUEST
```

Al finalizar la partida, el ESP32 verifica la conexión y realiza una solicitud HTTPS a la API de resultados.

### Wokwi

Wokwi simula el ESP32 y los componentes electrónicos. Permite probar la entrada infrarroja, la pantalla, los indicadores, el temporizador, la conexión WiFi y las solicitudes HTTPS.

### Vercel, Supabase y aplicación web

Vercel aloja la API que recibe y procesa los resultados. Supabase almacena el nombre, el puntaje, las rondas acertadas, la ronda alcanzada y la fecha del registro. La aplicación web consulta estos datos para presentar las partidas y el ranking.

La integración sigue esta arquitectura:

**ESP32 → API en Vercel → Supabase → Aplicación web**

## Librerías

| Librería | Uso |
| -------- | --- |
| `Wire.h` | Comunicación I2C con la pantalla |
| `LiquidCrystal_I2C.h` | Control de la pantalla LCD 16x2 |
| `IRremote.hpp` | Recepción y decodificación de comandos IR |
| `WiFi.h` | Conexión del ESP32 a la red WiFi |
| `HTTPClient.h` | Envío de la solicitud `POST` |
| `WiFiClientSecure.h` | Creación de la conexión HTTPS |

```cpp
#include <Wire.h>
#include <LiquidCrystal_I2C.h>
#include <IRremote.hpp>
#include <WiFi.h>
#include <HTTPClient.h>
#include <WiFiClientSecure.h>
```

### Inicialización de periféricos

El receptor IR se inicializa sin retroalimentación luminosa propia:

```cpp
IrReceiver.begin(PIN_IR, DISABLE_LED_FEEDBACK);
```

La conexión WiFi utiliza el ESP32 en modo estación:

```cpp
WiFi.mode(WIFI_STA);
WiFi.begin(WIFI_SSID, WIFI_PASSWORD);
```

### Solicitud HTTPS

`HTTPClient` envía el resultado como JSON:

```cpp
http.addHeader("Content-Type", "application/json");
int httpCode = http.POST(jsonBody);
```

En Wokwi, el cliente seguro se configura de la siguiente manera:

```cpp
WiFiClientSecure client;
client.setInsecure();
```

Esta configuración conserva HTTPS, pero omite la validación local del certificado porque la simulación no dispone del certificado raíz necesario. En un entorno de producción debe utilizarse el certificado correspondiente.

## Funciones del entorno Arduino

El programa también utiliza funciones provistas por Arduino y ESP32, entre ellas `pinMode()`, `digitalWrite()`, `delay()`, `millis()`, `tone()`, `noTone()`, `random()`, `randomSeed()`, `map()`, `snprintf()` y las funciones de `Serial`.

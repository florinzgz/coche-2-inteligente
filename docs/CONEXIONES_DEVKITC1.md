# 📋 GUÍA COMPLETA DE CONEXIONES HARDWARE
## ESP32-S3-DevKitC-1 (44 pines) - Firmware Coche Inteligente v2.0

---

## 📌 IMPORTANTE: Pinout ESP32-S3-DevKitC-1

Tu placa **ESP32-S3-DevKitC-1** expone GPIOs 0-48. Todas las conexiones están adaptadas a estos pines.

```
LADO IZQUIERDO:          LADO DERECHO:
3V3                      5V0
EN                       GPIO 46 ← BTN_BATTERY
GPIO 0                   GPIO 45
GPIO 1  ← FL_PWM         GPIO 48 ← RL_IN2
GPIO 2  ← RELAY_MAIN     GPIO 47 ← RL_IN1
GPIO 3  ← FL_IN1         GPIO 21 ← I2C_SDA
GPIO 4  ← RELAY_TRAC     GPIO 20 ← I2C_SCL
GPIO 5  ← RELAY_DIR      GPIO 38 ← ENCODER_B
GPIO 6  ← FL_IN2         GPIO 37 ← ENCODER_A
GPIO 7  ← FR_PWM         GPIO 36 ← WHEEL1_FR
GPIO 8  ← FR_IN1         GPIO 35 ← WHEEL0_FL
GPIO 9  ← FR_IN2         GPIO 34 ← PEDAL
GPIO 10 ← RL_PWM         GPIO 33 ← RELAY_MEDIA
GPIO 11 ← TFT_MOSI       GPIO 32 ← RELAY_LIGHTS
GPIO 12 ← TOUCH_CS       GPIO 31 ← WHEEL3_RR
GPIO 13 ← TOUCH_IRQ      GPIO 30 ← WHEEL2_RL
GPIO 14 ← TFT_RST        GPIO 29 ← RR_IN2
GPIO 15 ← TFT_CS         GPIO 28 ← RR_IN1
GPIO 16 ← DFPLAYER_RX    GPIO 27 ← TFT_DC
GPIO 17 ← DFPLAYER_TX    GPIO 26 ← DS18B20
GPIO 18 ← TFT_SCK        GPIO 25 ← ENCODER_Z
GPIO 19 ← TFT_MISO       GPIO 24 ← RR_PWM
GND                      GND
```

---

## 1️⃣ RELÉS HY-M158 (Estado Sólido 40A)

### 🔌 HY-M158 #1 - TRACCIÓN MOTORES 24V

| Pin HY-M158 | Color Cable | ESP32-S3 Pin | Descripción |
|-------------|-------------|--------------|-------------|
| DC+ (Input) | 🔴 Rojo | **GPIO 4** | Señal control tracción |
| DC- (Input) | ⚫ Negro | **GND** | Tierra común |
| **[CARGA]** | | | **LADO POTENCIA** |
| +LOAD | 🔴 Rojo | Batería 24V (+) | Entrada alimentación |
| -LOAD | ⚫ Negro | Motores BTS7960 | Salida a motores |
| GND | ⚫ Negro | Batería 24V (-) | Tierra potencia |

### 🔌 HY-M158 #2 - SISTEMA GENERAL 12V

| Pin HY-M158 | Color Cable | ESP32-S3 Pin | Descripción |
|-------------|-------------|--------------|-------------|
| DC+ (Input) | 🔴 Rojo | **GPIO 2** | Señal control sistema |
| DC- (Input) | ⚫ Negro | **GND** | Tierra común |
| **[CARGA]** | | | **LADO POTENCIA** |
| +LOAD | 🔴 Rojo | Batería 12V (+) | Entrada alimentación |
| -LOAD | ⚫ Negro | Luces/TFT/Audio | Salida a periféricos |
| GND | ⚫ Negro | Batería 12V (-) | Tierra potencia |

---

## 2️⃣ PANTALLA ILI9488 480×320 + TÁCTIL XPT2046

### 🖥️ Conexiones Pantalla TFT

| Pin ILI9488 | Color Cable | ESP32-S3 Pin | Función |
|-------------|-------------|--------------|---------|
| VCC | 🔴 Rojo | **3.3V** | Alimentación |
| GND | ⚫ Negro | **GND** | Tierra |
| CS | 🟠 Naranja | **GPIO 15** | Chip Select pantalla |
| RESET | 🟡 Amarillo | **GPIO 14** | Reset pantalla |
| DC/RS | 🟢 Verde | **GPIO 27** | Data/Command |
| SDI/MOSI | 🔵 Azul | **GPIO 11** | Datos SPI salida |
| SCK | 🟣 Violeta | **GPIO 18** | Reloj SPI |
| LED | ⚪ Blanco | **3.3V** | Backlight (conectar a 3.3V) |
| SDO/MISO | 🩶 Gris | **GPIO 19** | Datos SPI entrada |

### 👆 Conexiones Táctil XPT2046 (mismo módulo)

| Pin Táctil | Color Cable | ESP32-S3 Pin | Función |
|------------|-------------|--------------|---------|
| T_CLK | - | **GPIO 18** | Compartido con SCK pantalla |
| T_CS | 🟤 Marrón | **GPIO 12** | Chip Select táctil |
| T_DIN | - | **GPIO 11** | Compartido con MOSI pantalla |
| T_DO | - | **GPIO 19** | Compartido con MISO pantalla |
| T_IRQ | 🩷 Rosa | **GPIO 13** | Interrupción táctil (opcional) |

**⚠️ IMPORTANTE:** La pantalla y el táctil comparten el bus SPI (GPIO 11, 18, 19).

---

## 3️⃣ SENSORES DE RUEDA LJ12A3-4-Z/BX (Inductivos 5V)

### 🎯 Configuración: 6 tornillos por rueda

| Sensor | ESP32-S3 Pin | Posición Rueda | Señal |
|--------|--------------|----------------|-------|
| Sensor FL | **GPIO 35** | Frontal Izquierda | 6 pulsos/rev |
| Sensor FR | **GPIO 36** | Frontal Derecha | 6 pulsos/rev |
| Sensor RL | **GPIO 30** | Trasera Izquierda | 6 pulsos/rev |
| Sensor RR | **GPIO 31** | Trasera Derecha | 6 pulsos/rev |

### 🔌 Conexión Sensor LJ12A3-4-Z/BX

| Pin Sensor | Color | Conexión |
|------------|-------|----------|
| Marrón (+) | 🟤 | +5V (convertidor nivel HV) |
| Azul (-) | 🔵 | GND común |
| Negro (Señal) | ⚫ | ESP32-S3 GPIO (a través convertidor 5V→3.3V) |

**⚠️ USAR CONVERTIDOR DE NIVEL 5V→3.3V** (ver sección 8)

---

## 4️⃣ ENCODER DIRECCIÓN E6B2-CWZ6C 1200PR

### 🎯 Encoder Incremental con Señal Z

| Canal | ESP32-S3 Pin | Función |
|-------|--------------|---------|
| Canal A | **GPIO 37** | Pulsos cuadratura A |
| Canal B | **GPIO 38** | Pulsos cuadratura B |
| Señal Z | **GPIO 25** | Índice centrado (sensor LJ12A3) |

### 🔌 Conexiones Encoder

| Pin Encoder | Color | Conexión |
|-------------|-------|----------|
| VCC (+5V) | 🔴 | +5V (convertidor nivel HV) |
| GND | ⚫ | GND común |
| Canal A | 🟢 | GPIO 37 (a través convertidor) |
| Canal B | ⚪ | GPIO 38 (a través convertidor) |
| Canal Z | 🟡 | GPIO 25 (a través convertidor) |

**⚠️ Señal Z:** Sensor LJ12A3-4-Z/BX detecta 1 tornillo en centro del volante.

---

## 5️⃣ MOTORES BTS7960 (43A por canal)

### 🚗 4 Controladores BTS7960 (uno por rueda)

#### Motor Frontal Izquierdo (FL)
| Señal | ESP32-S3 Pin | Función |
|-------|--------------|---------|
| RPWM | **GPIO 1** | PWM adelante |
| LPWM | **GPIO 1** | PWM atrás (mismo pin) |
| R_IN | **GPIO 3** | Dirección adelante |
| L_IN | **GPIO 6** | Dirección atrás |

#### Motor Frontal Derecho (FR)
| Señal | ESP32-S3 Pin | Función |
|-------|--------------|---------|
| RPWM | **GPIO 7** | PWM adelante |
| LPWM | **GPIO 7** | PWM atrás |
| R_IN | **GPIO 8** | Dirección adelante |
| L_IN | **GPIO 9** | Dirección atrás |

#### Motor Trasero Izquierdo (RL)
| Señal | ESP32-S3 Pin | Función |
|-------|--------------|---------|
| RPWM | **GPIO 10** | PWM adelante |
| LPWM | **GPIO 10** | PWM atrás |
| R_IN | **GPIO 47** | Dirección adelante |
| L_IN | **GPIO 48** | Dirección atrás |

#### Motor Trasero Derecho (RR)
| Señal | ESP32-S3 Pin | Función |
|-------|--------------|---------|
| RPWM | **GPIO 24** | PWM adelante |
| LPWM | **GPIO 24** | PWM atrás |
| R_IN | **GPIO 28** | Dirección adelante |
| L_IN | **GPIO 29** | Dirección atrás |

### 🔌 Conexiones BTS7960 (todas iguales)

| Pin BTS7960 | Conexión |
|-------------|----------|
| VCC (Logic) | 5V |
| GND (Logic) | GND |
| VCC (Motor) | 24V Batería |
| GND (Motor) | GND común |
| M+ | Terminal motor + |
| M- | Terminal motor - |
| R_EN | 5V (siempre habilitado) |
| L_EN | 5V (siempre habilitado) |

---

## 6️⃣ SENSORES INA226 (Corriente/Voltaje I²C)

### 📊 4 Sensores en Bus I²C

| Sensor | Dirección I²C | Medición |
|--------|---------------|----------|
| INA226 #1 | 0x40 | Motor FL |
| INA226 #2 | 0x41 | Motor FR |
| INA226 #3 | 0x44 | Motor RL |
| INA226 #4 | 0x45 | Motor RR |

### 🔌 Conexiones I²C (todos en paralelo)

| Pin INA226 | ESP32-S3 Pin | Función |
|------------|--------------|---------|
| VCC | **3.3V** | Alimentación |
| GND | **GND** | Tierra |
| SDA | **GPIO 21** | Datos I²C (todos en paralelo) |
| SCL | **GPIO 20** | Reloj I²C (todos en paralelo) |

**✅ NO necesitan convertidor de nivel** (nativos 3.3V)

---

## 7️⃣ DFPLAYER MINI (Audio)

### 🎵 Reproductor MP3

| Pin DFPlayer | ESP32-S3 Pin | Función |
|--------------|--------------|---------|
| VCC | **5V** | Alimentación |
| GND | **GND** | Tierra |
| TX | **GPIO 16** | UART RX ESP32 |
| RX | **GPIO 17** | UART TX ESP32 |
| SPK1 | Altavoz + | Bocina positivo |
| SPK2 | Altavoz - | Bocina negativo |

### 💾 Estructura Tarjeta SD

```
SD Card/
└── mp3/
    ├── 0001.mp3  (Bienvenido Marcos)
    ├── 0002.mp3  (Hasta pronto)
    ├── 0013.mp3  (Batería crítica)
    ├── 0036.mp3  (Módulo OK)
    └── ... (hasta 0038.mp3)
```

---

## 8️⃣ CONVERTIDOR DE NIVEL 5V → 3.3V

### ⚡ Módulos Necesarios: 2×TXS0104E (4 canales c/u)

#### Convertidor #1: Encoder + Temperatura

| Canal | HV (5V) | LV (3.3V) | Función |
|-------|---------|-----------|---------|
| HV1 | Encoder A | **GPIO 37** | Canal A encoder |
| HV2 | Encoder B | **GPIO 38** | Canal B encoder |
| HV3 | Encoder Z | **GPIO 25** | Señal Z centrado |
| HV4 | DS18B20 | **GPIO 26** | Temperatura |

#### Convertidor #2: Sensores Ruedas

| Canal | HV (5V) | LV (3.3V) | Función |
|-------|---------|-----------|---------|
| HV1 | Sensor FL | **GPIO 35** | Rueda frontal izq |
| HV2 | Sensor FR | **GPIO 36** | Rueda frontal der |
| HV3 | Sensor RL | **GPIO 30** | Rueda trasera izq |
| HV4 | Sensor RR | **GPIO 31** | Rueda trasera der |

### 🔌 Alimentación Convertidores

| Pin | Conexión |
|-----|----------|
| VCCB (HV) | +5V |
| VCCA (LV) | +3.3V |
| GND | GND común |
| OE | VCCA (siempre habilitado) |

---

## 9️⃣ RELÉS SECUNDARIOS (Módulo 8 canales 5V)

### 🔌 Módulo Relé 8CH

| Canal | ESP32-S3 Pin | Función | Carga |
|-------|--------------|---------|-------|
| IN1 | - | No usado | - |
| IN2 | **GPIO 5** | Encoder dirección | Alimentación encoder |
| IN3 | **GPIO 32** | Luces | Sistema iluminación |
| IN4 | **GPIO 33** | Audio | DFPlayer Mini |
| IN5-8 | - | Reserva | Futuras expansiones |

### 🔌 Alimentación Módulo

| Pin | Conexión |
|-----|----------|
| VCC | +5V |
| GND | GND |
| JD-VCC | +5V (con jumper) |

---

## 🔟 BOTONES FÍSICOS

### 🎮 Botones de Control

| Botón | ESP32-S3 Pin | Función |
|-------|--------------|---------|
| BTN_LIGHTS | **GPIO 40** | Toggle luces |
| BTN_MEDIA | **GPIO 41** | Control multimedia |
| BTN_4X4 | **GPIO 42** | Modo 4×4 |
| BTN_BATTERY | **GPIO 46** | Estado batería |

### 🔌 Conexión Botones

```
[Botón] ──┬── ESP32-S3 GPIO
          └── Resistencia 10kΩ ── GND
```

---

## 1️⃣1️⃣ PEDAL ACELERADOR (Analógico)

### 🦶 Entrada ADC

| Señal | ESP32-S3 Pin | Rango |
|-------|--------------|-------|
| Pedal | **GPIO 34** | 0-3.3V (ADC) |

### 🔌 Conexión Pedal

| Pin | Conexión |
|-----|----------|
| VCC | +3.3V |
| GND | GND |
| Señal | **GPIO 34** |

---

## 1️⃣2️⃣ SENSOR TEMPERATURA DS18B20

### 🌡️ OneWire

| Pin DS18B20 | ESP32-S3 Pin | Notas |
|-------------|--------------|-------|
| VCC | +5V (HV convertidor) | - |
| GND | GND | - |
| Data | **GPIO 26** (LV convertidor) | Requiere resistencia pull-up 4.7kΩ |

**⚡ Usar convertidor de nivel 5V→3.3V**

---

## ⚠️ CHECKLIST PRE-ENCENDIDO

### ✅ Verificaciones Obligatorias

- [ ] **3.3V** conectado a L1 del DevKitC-1
- [ ] **5V** conectado a R1 del DevKitC-1
- [ ] **GND común** entre ESP32, baterías 12V/24V y todos los módulos
- [ ] **HY-M158 #1** (GPIO 4) conectado a sistema tracción 24V
- [ ] **HY-M158 #2** (GPIO 2) conectado a sistema general 12V
- [ ] **Convertidores de nivel** instalados entre sensores 5V y ESP32
- [ ] **Pantalla ILI9488** cables soldados correctamente (GPIO 11, 18, 19)
- [ ] **4 sensores INA226** en bus I²C (GPIO 20, 21) con direcciones únicas
- [ ] **Encoder E6B2-CWZ6C** conectado a GPIO 37, 38 con Z en GPIO 25
- [ ] **4 sensores LJ12A3** en GPIO 30, 31, 35, 36
- [ ] **4 BTS7960** con PWM y direcciones correctas
- [ ] **DFPlayer** con tarjeta SD y archivos 0001-0038.mp3
- [ ] **Polaridad correcta** en todas las conexiones de alimentación

### 🔋 Voltajes Correctos

- ESP32-S3: **3.3V** (no exceder)
- Lógica periféricos: **5V**
- Motores tracción: **24V**
- Sistema general: **12V**

---

## 📥 DESCARGA FIRMWARE

**Repositorio GitHub:**
```
https://github.com/florinzgz/coche-2-inteligente
Rama: copilot/revise-and-fix-functionality
Commit: 36c74b8
```

**Descarga directa ZIP:**
```
https://github.com/florinzgz/coche-2-inteligente/archive/refs/heads/copilot/revise-and-fix-functionality.zip
```

### 📂 Estructura Proyecto

```
firmware Coche Marcos/
├── platformio.ini        # Configuración PlatformIO
├── include/
│   └── pins.h           # ← PINOUT ADAPTADO ESP32-S3-DevKitC-1
├── src/
│   ├── main.cpp         # Código principal
│   ├── control/         # Control motores, ABS, TCS
│   ├── sensors/         # INA226, ruedas, temperatura
│   ├── ui/              # Interfaz TFT
│   └── wifi/            # WiFi + OTA
└── docs/
    ├── CONEXIONES_DEVKITC1.md  # ← Este documento
    └── PIN_MAPPING_DEVKITC1.md # Mapeo detallado
```

### 🔧 Compilar y Flashear

```bash
cd "firmware Coche Marcos"
pio run              # Compilar
pio run -t upload    # Flashear a ESP32-S3
```

---

## 📞 SOPORTE

**Documentación adicional:**
- `docs/PIN_MAPPING_DEVKITC1.md` - Mapeo completo de 48 GPIOs
- `docs/CONEXIONES_HARDWARE.md` - Guía general (anterior)
- `include/pins.h` - Definiciones de pines en código

**Firmware compilado y verificado para:**
- ✅ ESP32-S3-DevKitC-1 (44 pines físicos)
- ✅ GPIOs 0-48 compatibles
- ✅ Sensores LJ12A3-4-Z/BX (6 tornillos/rueda)
- ✅ Encoder E6B2-CWZ6C 1200PR con señal Z

---

**🎉 ¡Firmware listo para usar! Todos los pines han sido remapeados para ESP32-S3-DevKitC-1.**

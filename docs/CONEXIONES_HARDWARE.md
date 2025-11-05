# 🔌 Guía de Conexiones Hardware - Coche Inteligente ESP32-S3

**Firmware Version:** v1.0  
**Board:** ESP32-S3-DevKitC-1  
**Display:** ILI9488 480x320 TFT + XPT2046 Touch  
**Fecha:** Noviembre 2024

---

## 📋 Tabla de Contenidos

1. [Pantalla TFT ILI9488](#1-pantalla-tft-ili9488)
2. [Táctil XPT2046](#2-táctil-xpt2046)
3. [Relés HY-M158](#3-relés-hy-m158)
4. [Relés Secundarios](#4-relés-secundarios-módulo-8-canales)
5. [DFPlayer Mini](#5-dfplayer-mini-audio)
6. [LEDs Inteligentes](#6-leds-inteligentes-fastled)
7. [Sensores de Corriente](#7-sensores-ina226)
8. [Encoders](#8-encoders-rotatorios)
9. [Pedal y Controles](#9-pedal-y-controles)
10. [Alimentación](#10-alimentación)

---

## 1. Pantalla TFT ILI9488

### Especificaciones
- Resolución: 480x320 píxeles
- Interfaz: SPI
- Voltaje: 3.3V
- Driver: ILI9488

### Conexiones Pin a Pin

| **Pin Pantalla** | **Color Cable** | **Pin ESP32-S3** | **Función** | **Notas** |
|------------------|-----------------|------------------|-------------|-----------|
| VCC | 🔴 Rojo | 3.3V | Alimentación | ⚠️ NO usar 5V |
| GND | ⚫ Negro | GND | Tierra | Común con ESP32 |
| CS | 🟠 Naranja | **GPIO 15** | Chip Select | Control SPI pantalla |
| RESET (RST) | 🟡 Amarillo | **GPIO 14** | Reset | Reinicio pantalla |
| DC (A0) | 🟢 Verde | **GPIO 27** | Data/Command | Selector modo datos |
| MOSI (SDI) | 🔵 Azul | **GPIO 23** | SPI Data Out | Datos ESP32→Pantalla |
| SCK (CLK) | 🟣 Violeta | **GPIO 18** | SPI Clock | Reloj sincronización |
| LED (BLK) | ⚪ Blanco | 3.3V | Backlight | Iluminación siempre ON |
| MISO (SDO) | ⚪ Gris | **GPIO 19** | SPI Data In | Datos Pantalla→ESP32 |

### Diagrama ASCII
```
┌─────────────────────┐
│   ILI9488 Display   │
│     480x320 TFT     │
├─────────────────────┤
│ VCC  → ESP32 3.3V   │
│ GND  → ESP32 GND    │
│ CS   → GPIO 15      │
│ RST  → GPIO 14      │
│ DC   → GPIO 27      │
│ MOSI → GPIO 23 ─────┼─┐
│ SCK  → GPIO 18 ─────┼─┤ Bus SPI
│ MISO → GPIO 19 ─────┼─┘ Compartido
│ LED  → 3.3V         │
└─────────────────────┘
```

---

## 2. Táctil XPT2046

### Especificaciones
- Controlador: XPT2046
- Interfaz: SPI (compartido con pantalla)
- Resolución: 12-bit ADC
- Voltaje: 3.3V

### Conexiones Pin a Pin

| **Pin Táctil** | **Color Cable** | **Pin ESP32-S3** | **Función** | **Notas** |
|----------------|-----------------|------------------|-------------|-----------|
| T_CLK | 🟣 (compartido) | **GPIO 18** | SPI Clock | Mismo que pantalla SCK |
| T_CS | 🟤 Marrón | **GPIO 12** | Chip Select Touch | Control SPI táctil |
| T_DIN (MOSI) | 🔵 (compartido) | **GPIO 23** | SPI Data Out | Mismo que pantalla MOSI |
| T_DO (MISO) | ⚪ (compartido) | **GPIO 19** | SPI Data In | Mismo que pantalla MISO |
| T_IRQ | 🌸 Rosa | **GPIO 13** | Interrupt | Detección toque (opcional) |
| VCC | 🔴 Rojo | 3.3V | Alimentación | Compartido con pantalla |
| GND | ⚫ Negro | GND | Tierra | Compartido con pantalla |

### Diagrama ASCII
```
┌─────────────────────┐
│   XPT2046 Touch     │
│   (en mismo PCB)    │
├─────────────────────┤
│ T_CS  → GPIO 12     │
│ T_IRQ → GPIO 13     │
│ T_CLK ← GPIO 18 ────┼─┐
│ T_DIN ← GPIO 23 ────┼─┤ Bus SPI
│ T_DO  → GPIO 19 ────┼─┘ Compartido
└─────────────────────┘
```

---

## 3. Relés HY-M158

### Especificaciones
- Modelo: HY-M158 SSR (Relé Estado Sólido)
- Entrada: 3-32V DC
- Salida: 5-220V DC / 24-380V AC
- Corriente máxima: 40A
- Optoacoplador: Integrado

### HY-M158 #1 - Tracción Motores 24V

| **Terminal HY-M158** | **Color Cable** | **Conexión** | **Función** | **Notas** |
|---------------------|-----------------|--------------|-------------|-----------|
| DC+ (Input) | 🔴 Rojo | **ESP32 GPIO 4** | Control ON/OFF | Pin control tracción |
| DC- (Input) | ⚫ Negro | **ESP32 GND** | Tierra control | Referencia común |
| + (Output) | 🔴 Rojo grueso | **Batería 24V (+)** | Alimentación | Cable 16 AWG mínimo |
| - (Output/Load) | ⚫ Negro grueso | **Controladores BTS7960** | Carga motores | 4x motores tracción |

### HY-M158 #2 - Sistema General 12V

| **Terminal HY-M158** | **Color Cable** | **Conexión** | **Función** | **Notas** |
|---------------------|-----------------|--------------|-------------|-----------|
| DC+ (Input) | 🔴 Rojo | **ESP32 GPIO 2** | Control ON/OFF | Pin sistema principal |
| DC- (Input) | ⚫ Negro | **ESP32 GND** | Tierra control | Referencia común |
| + (Output) | 🔴 Rojo grueso | **Batería 12V (+)** | Alimentación | Cable 14 AWG mínimo |
| - (Output/Load) | ⚫ Negro grueso | **Luces/TFT/Audio** | Carga sistema | Distribución 12V |

### Diagrama de Potencia
```
┌──────────────────────────────────────────┐
│         SISTEMA DE RELÉS HY-M158         │
├──────────────────────────────────────────┤
│                                          │
│  ┌────────────┐         ┌────────────┐  │
│  │ HY-M158 #1 │         │ HY-M158 #2 │  │
│  │  TRACCIÓN  │         │  SISTEMA   │  │
│  ├────────────┤         ├────────────┤  │
│  │ DC+ GPIO 4 │         │ DC+ GPIO 2 │  │
│  │ DC- GND    │         │ DC- GND    │  │
│  │            │         │            │  │
│  │ + ← 24V    │         │ + ← 12V    │  │
│  │ - → Motors │         │ - → Luces  │  │
│  └────────────┘         └────────────┘  │
│       ↓                       ↓          │
│  🚗 Motores          💡 Iluminación     │
│  Tracción 4x         🎵 Audio           │
│  BTS7960             📟 Pantalla        │
└──────────────────────────────────────────┘
```

---

## 4. Relés Secundarios (Módulo 8 Canales)

### Especificaciones
- Voltaje bobina: 5V
- Corriente máxima por relé: 10A
- Optoacopladores: Integrados
- Modelo recomendado: SainSmart 8-Channel

### Conexiones Alimentación Módulo

| **Terminal Módulo** | **Color Cable** | **Pin ESP32-S3** | **Función** |
|---------------------|-----------------|------------------|-------------|
| VCC | 🔴 Rojo | 5V | Alimentación módulo |
| GND | ⚫ Negro | GND | Tierra módulo |

### Asignación de Canales

| **Canal** | **Pin ESP32-S3** | **Función** | **Carga Conectada** | **Notas** |
|-----------|------------------|-------------|---------------------|-----------|
| IN1 | - | (No usado) | - | Reservado futuro |
| IN2 | **GPIO 5** | Encoder Dirección | Habilitar/Bloquear encoder | Seguridad dirección |
| IN3 | **GPIO 32** | Luces | Sistema iluminación | LEDs delanteros/traseros |
| IN4 | **GPIO 33** | Audio/Media | DFPlayer Mini | Multimedia |
| IN5 | - | (No usado) | - | Reservado futuro |
| IN6 | - | (No usado) | - | Reservado futuro |
| IN7 | - | (No usado) | - | Reservado futuro |
| IN8 | - | (No usado) | - | Reservado futuro |

### Diagrama Conexiones
```
┌─────────────────────────────────────────┐
│    Módulo Relé 8 Canales 5V             │
├─────────────────────────────────────────┤
│ VCC → ESP32 5V                          │
│ GND → ESP32 GND                         │
│                                         │
│ IN1 → (Reservado)                       │
│ IN2 → GPIO 5  → Encoder Dirección   🎯  │
│ IN3 → GPIO 32 → Luces Sistema       💡  │
│ IN4 → GPIO 33 → DFPlayer Audio      🎵  │
│ IN5-8 → (Reservados)                    │
└─────────────────────────────────────────┘
```

---

## 5. DFPlayer Mini (Audio)

### Especificaciones
- Módulo: DFPlayer Mini MP3
- Voltaje: 3.3-5V
- Interfaz: UART
- Almacenamiento: MicroSD Card (hasta 32GB)
- Formato audio: MP3

### Conexiones Pin a Pin

| **Pin DFPlayer** | **Color Cable** | **Pin ESP32-S3** | **Función** | **Notas** |
|------------------|-----------------|------------------|-------------|-----------|
| VCC | 🔴 Rojo | 5V | Alimentación | Estable 5V |
| GND | ⚫ Negro | GND | Tierra | Común ESP32 |
| TX | 🟡 Amarillo | **GPIO 17** | Transmisión datos | UART TX DFPlayer |
| RX | 🟢 Verde | **GPIO 16** | Recepción datos | UART RX DFPlayer |
| SPK1 | 🔵 Azul | Altavoz (+) | Salida audio | Canal 1 altavoz |
| SPK2 | 🟣 Violeta | Altavoz (-) | Salida audio | Canal 2 altavoz |
| BUSY | 🟠 Naranja | **GPIO 34** | Estado reproducción | Pin lectura estado |

### Estructura Tarjeta SD
```
📁 SD Card (FAT32)
└── 📁 mp3/
    ├── 0001.mp3  "Bienvenido Marcos"
    ├── 0002.mp3  "Hasta pronto"
    ├── 0003.mp3  "Cambio a marcha D1"
    ├── 0004.mp3  "Cambio a marcha D2"
    ├── 0005.mp3  "Cambio a reversa"
    ├── 0006.mp3  "Freno de parking activado"
    ├── 0007.mp3  "Freno de parking desactivado"
    ├── 0013.mp3  "Batería crítica"
    ├── 0036.mp3  "Módulo OK"
    └── ... (hasta 0038.mp3)
```

### Diagrama Conexiones
```
┌─────────────────────────────────────────┐
│        DFPlayer Mini MP3                │
├─────────────────────────────────────────┤
│ VCC  → 5V                               │
│ GND  → GND                              │
│ TX   → GPIO 17  (UART ESP32)            │
│ RX   → GPIO 16  (UART ESP32)            │
│ BUSY → GPIO 34  (Estado)                │
│                                         │
│ SPK1 ──┐                                │
│ SPK2 ──┼──→ 🔊 Altavoz 3W 4Ω          │
│        └─── (Con condensador 1000µF)    │
│                                         │
│ 📟 MicroSD Slot                         │
│    └─ Tarjeta FAT32 con /mp3/*.mp3     │
└─────────────────────────────────────────┘
```

---

## 6. LEDs Inteligentes (FastLED)

### Especificaciones
- Tipo: WS2812B / NeoPixel
- Voltaje: 5V
- Protocolo: Digital 800kHz
- Delanteros: 28 LEDs
- Traseros: 16 LEDs

### Conexiones

| **Tira LED** | **Pin LED** | **Color Cable** | **Pin ESP32-S3** | **Función** |
|--------------|-------------|-----------------|------------------|-------------|
| Delanteros | DIN | 🟢 Verde | **GPIO 25** | Datos LEDs frontales |
| Delanteros | VCC | 🔴 Rojo | 5V | Alimentación |
| Delanteros | GND | ⚫ Negro | GND | Tierra |
| Traseros | DIN | 🔵 Azul | **GPIO 26** | Datos LEDs traseros |
| Traseros | VCC | 🔴 Rojo | 5V | Alimentación |
| Traseros | GND | ⚫ Negro | GND | Tierra |

### Consideraciones de Alimentación
```
⚠️ IMPORTANTE: Alimentación LEDs
━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━

• 28 LEDs frontales @ 60mA máx = 1.68A
• 16 LEDs traseros @ 60mA máx = 0.96A
• Total máximo: 2.64A @ 5V

💡 Solución recomendada:
   Usar fuente 5V/3A dedicada para LEDs
   Conectar GND común con ESP32
```

### Diagrama Conexiones
```
┌─────────────────────────────────────────┐
│          Sistema de LEDs                │
├─────────────────────────────────────────┤
│                                         │
│  🚗 LEDs DELANTEROS (28 unidades)       │
│  ┌─────────────────────────┐            │
│  │ DIN  ← GPIO 25          │            │
│  │ VCC  ← 5V (fuente LED)  │            │
│  │ GND  → GND común        │            │
│  └─────────────────────────┘            │
│         ↓                               │
│  Efectos: KITT, Flash, Rainbow         │
│                                         │
│  🚗 LEDs TRASEROS (16 unidades)         │
│  ┌─────────────────────────┐            │
│  │ DIN  ← GPIO 26          │            │
│  │ VCC  ← 5V (fuente LED)  │            │
│  │ GND  → GND común        │            │
│  └─────────────────────────┘            │
│         ↓                               │
│  Efectos: Freno, Reversa, Parpadeo     │
└─────────────────────────────────────────┘
```

---

## 7. Sensores INA226

### Especificaciones
- Sensor: INA226 (Voltaje + Corriente)
- Interfaz: I2C
- Voltaje medición: 0-36V
- Corriente máxima: ±20A (con shunt 0.1Ω)
- Precisión: 16-bit ADC

### Conexiones Bus I2C

| **Pin INA226** | **Color Cable** | **Pin ESP32-S3** | **Función** | **Notas** |
|----------------|-----------------|------------------|-------------|-----------|
| VCC | 🔴 Rojo | 3.3V | Alimentación sensor | Lógica 3.3V |
| GND | ⚫ Negro | GND | Tierra | Común ESP32 |
| SDA | 🟢 Verde | **GPIO 21** | I2C Data | Bus compartido |
| SCL | 🟡 Amarillo | **GPIO 22** | I2C Clock | Bus compartido |

### Direcciones I2C

| **Sensor** | **Dirección I2C** | **Función** | **Voltaje Medido** |
|------------|-------------------|-------------|--------------------|
| INA226 #1 | 0x40 | Batería Principal | 24V tracción |
| INA226 #2 | 0x41 | Sistema 12V | Auxiliar 12V |
| INA226 #3 | 0x44 | Motor FL (Front Left) | 24V motor |
| INA226 #4 | 0x45 | Motor FR (Front Right) | 24V motor |

### Diagrama Bus I2C
```
┌─────────────────────────────────────────┐
│          Bus I2C Sensores               │
├─────────────────────────────────────────┤
│                                         │
│  ESP32-S3                               │
│  ┌──────────┐                           │
│  │ GPIO 21  ├──── SDA ───┬─────────┐   │
│  │ (SDA)    │            │         │   │
│  │          │         INA226    INA226 │
│  │ GPIO 22  ├──── SCL ───┤  0x40    │   │
│  │ (SCL)    │            │  Bat24V  │   │
│  └──────────┘            │         │   │
│                          │      INA226 │
│                          │       0x41  │
│                          │      12V    │
│                          │             │
│                       INA226   INA226  │
│                        0x44     0x45   │
│                       Motor FL  Motor FR│
└─────────────────────────────────────────┘
```

---

## 8. Encoders Rotatorios

### Especificaciones
- Tipo: Encoder incremental
- Pulsos: 20-24 PPR (pulsos por revolución)
- Voltaje: 3.3V-5V
- Salida: Digital (A/B phase)

### Encoder Dirección

| **Pin Encoder DIR** | **Color Cable** | **Pin ESP32-S3** | **Función** | **Notas** |
|---------------------|-----------------|------------------|-------------|-----------|
| VCC | 🔴 Rojo | 3.3V | Alimentación | Lógica 3.3V |
| GND | ⚫ Negro | GND | Tierra | Común ESP32 |
| A (CLK) | 🟢 Verde | **GPIO 35** | Phase A | Lectura posición |
| B (DT) | 🟡 Amarillo | **GPIO 36** | Phase B | Dirección giro |
| SW (Button) | 🔵 Azul | **GPIO 39** | Pulsador | Botón central (opcional) |

### Encoder Velocidad Rueda

| **Pin Encoder** | **Color Cable** | **Pin ESP32-S3** | **Función** | **Notas** |
|-----------------|-----------------|------------------|-------------|-----------|
| VCC | 🔴 Rojo | 3.3V | Alimentación | Lógica 3.3V |
| GND | ⚫ Negro | GND | Tierra | Común ESP32 |
| A (CLK) | 🟢 Verde | **GPIO 37** | Phase A | Medición RPM |
| B (DT) | 🟡 Amarillo | **GPIO 38** | Phase B | Sentido rotación |

### Diagrama Funcional
```
┌─────────────────────────────────────────┐
│         Sistema de Encoders             │
├─────────────────────────────────────────┤
│                                         │
│  🎯 ENCODER DIRECCIÓN                   │
│  ┌─────────────────────┐                │
│  │ A   → GPIO 35       │ ↻ Ángulo      │
│  │ B   → GPIO 36       │   Volante     │
│  │ SW  → GPIO 39       │ 🔘 Centro     │
│  └─────────────────────┘                │
│                                         │
│  🚗 ENCODER VELOCIDAD                   │
│  ┌─────────────────────┐                │
│  │ A   → GPIO 37       │ ⚡ RPM        │
│  │ B   → GPIO 38       │   Rueda       │
│  └─────────────────────┘                │
│     ↓                                   │
│  Cálculo velocidad km/h                 │
│  Sistema ABS/TCS                        │
└─────────────────────────────────────────┘
```

---

## 9. Pedal y Controles

### Pedal Acelerador (Potenciómetro)

| **Pin Potenciómetro** | **Color Cable** | **Pin ESP32-S3** | **Función** | **Notas** |
|-----------------------|-----------------|------------------|-------------|-----------|
| VCC | 🔴 Rojo | 3.3V | Alimentación | Tensión referencia |
| GND | ⚫ Negro | GND | Tierra | Común ESP32 |
| Signal (Wiper) | 🟢 Verde | **GPIO 1** | Lectura analógica | ADC 12-bit |

### Selector de Marchas

| **Posición** | **Pin ESP32-S3** | **Estado** | **Función** |
|--------------|------------------|------------|-------------|
| P (Parking) | **GPIO 6** | LOW | Parking mode |
| R (Reversa) | **GPIO 7** | LOW | Marcha atrás |
| N (Neutral) | **GPIO 8** | LOW | Punto muerto |
| D1 (Drive 1) | **GPIO 9** | LOW | Marcha adelante 1 |
| D2 (Drive 2) | **GPIO 10** | LOW | Marcha adelante 2 |

### Botones Adicionales

| **Botón** | **Pin ESP32-S3** | **Función** | **Notas** |
|-----------|------------------|-------------|-----------|
| Freno Parking | **GPIO 11** | Activar/Desactivar | Pull-up interno |
| Luces | **GPIO 40** | Toggle luces | Pull-up interno |
| Claxon | **GPIO 41** | Bocina | Activo bajo |
| Menu | **GPIO 42** | Menú pantalla | Pull-up interno |

### Diagrama Controles
```
┌─────────────────────────────────────────┐
│        Panel de Control                 │
├─────────────────────────────────────────┤
│                                         │
│  🦶 PEDAL ACELERADOR                    │
│     POT 0-100% → GPIO 1 (ADC)           │
│                                         │
│  ⚙️ SELECTOR MARCHAS                    │
│     [P] GPIO 6                          │
│     [R] GPIO 7   🔄 Solo una activa    │
│     [N] GPIO 8                          │
│     [D1] GPIO 9                         │
│     [D2] GPIO 10                        │
│                                         │
│  🔘 BOTONES                              │
│     🅿️  Parking  → GPIO 11              │
│     💡 Luces    → GPIO 40              │
│     📯 Claxon   → GPIO 41              │
│     📋 Menu     → GPIO 42              │
└─────────────────────────────────────────┘
```

---

## 10. Alimentación

### Diagrama Sistema de Potencia

```
┌──────────────────────────────────────────────────────────────┐
│              SISTEMA DE ALIMENTACIÓN                         │
├──────────────────────────────────────────────────────────────┤
│                                                              │
│  🔋 BATERÍA 24V (Principal)                                 │
│     │                                                        │
│     ├─→ [HY-M158 #1] GPIO 4 → Controladores BTS7960        │
│     │                          ↓                            │
│     │                      🚗 4x Motores Tracción          │
│     │                                                        │
│     └─→ [Buck 24V→12V] → Batería 12V auxiliar              │
│                                                              │
│  🔋 BATERÍA 12V (Auxiliar)                                  │
│     │                                                        │
│     ├─→ [HY-M158 #2] GPIO 2 → Sistema 12V                  │
│     │                          ├─→ 💡 Luces                │
│     │                          ├─→ 🎵 DFPlayer             │
│     │                          └─→ 📟 Pantalla TFT         │
│     │                                                        │
│     └─→ [Buck 12V→5V] → ESP32-S3 + Periféricos             │
│                          ├─→ 🧠 ESP32-S3 (5V)              │
│                          ├─→ 📡 Módulo Relés 8CH           │
│                          └─→ 💡 LEDs WS2812B               │
│                                                              │
│  ⚡ PROTECCIONES                                             │
│     • Fusible 60A en línea 24V principal                    │
│     • Fusible 30A en línea 12V auxiliar                     │
│     • Fusible 5A en línea 5V ESP32                          │
│     • Diodos flyback en todas las bobinas de relés         │
│                                                              │
└──────────────────────────────────────────────────────────────┘
```

### Especificaciones Buck Converters

| **Convertidor** | **Entrada** | **Salida** | **Corriente Max** | **Modelo Recomendado** |
|-----------------|-------------|------------|-------------------|------------------------|
| Buck 24V→12V | 24V | 12V | 10A | LM2596 Adjustable |
| Buck 12V→5V | 12V | 5V | 3A | MP1584EN / LM2596 |

### Cálculo de Consumos

```
📊 CONSUMO ESTIMADO TOTAL
━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━

24V Sistema:
• 4x Motores BTS7960      : 15A max c/u = 60A total
• Buck 24V→12V converter  : 2A
━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━
  TOTAL 24V               : ~62A máximo

12V Sistema:
• Luces halógenas        : 5A
• DFPlayer + Altavoz     : 1A
• Pantalla TFT ILI9488   : 0.5A
• Buck 12V→5V            : 1.5A
━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━
  TOTAL 12V              : ~8A máximo

5V Sistema:
• ESP32-S3               : 0.5A
• Módulo Relés 8CH       : 0.3A
• LEDs WS2812B (44 LEDs) : 2.6A max
• Sensores varios        : 0.1A
━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━
  TOTAL 5V               : ~3.5A máximo
```

---

## 📌 Notas Importantes

### ⚠️ Seguridad

1. **Nunca conectar 5V a pines 3.3V del ESP32-S3** - Puede dañar permanentemente el MCU
2. **Usar fusibles apropiados** en todas las líneas de alimentación
3. **GND común obligatorio** entre todos los módulos
4. **Aislar bien conexiones de potencia** (24V motores)
5. **No conectar/desconectar con sistema energizado**

### 🔧 Consejos de Montaje

1. **Cable AWG según corriente:**
   - 24V motores: AWG 12-14 (4-2.5mm²)
   - 12V sistema: AWG 16-18 (1.5-1mm²)
   - 5V/señales: AWG 22-24 (0.5-0.25mm²)

2. **Usar conectores adecuados:**
   - Motores: XT60/XT90 o Anderson PowerPole
   - Señales: DuPont 2.54mm
   - Pantalla: JST-XH o DuPont

3. **Organización de cables:**
   - Separar cables de potencia de señales
   - Usar canaletas/mangueras termorretráctiles
   - Etiquetar todos los cables

4. **Testing paso a paso:**
   - Primero alimentación 3.3V/5V solamente
   - Luego cargar firmware y verificar
   - Finalmente conectar cargas de potencia

### 📝 Checklist Pre-Encendido

- [ ] Todas las conexiones GND comunes verificadas
- [ ] Voltajes de alimentación correctos (multímetro)
- [ ] No hay cortocircuitos entre pines
- [ ] Fusibles instalados en todas las líneas
- [ ] Polaridad correcta en todos los módulos
- [ ] Firmware flasheado y verificado
- [ ] Tarjeta SD con archivos MP3 en DFPlayer
- [ ] HY-M158 en posición inicial (OFF)

---

## 📞 Soporte Técnico

**Firmware Version:** v1.0  
**Repositorio:** https://github.com/florinzgz/coche-2-inteligente  
**Documentación:** /docs/  

Para dudas o problemas:
- Revisar logs serie @ 115200 baud
- Verificar estado LEDs indicadores
- Comprobar voltajes en puntos de test

---

**Documento generado automáticamente**  
**Fecha:** Noviembre 2024  
**ESP32-S3 Coche Inteligente - Sistema Completo**

# GUÍA COMPLETA DE CONEXIONES - COCHE ELÉCTRICO INTELIGENTE
## ESP32-S3-DevKitC-1 (44 pines) - Sistema de Control Completo

---

## 📋 ÍNDICE

1. [Resumen del Hardware](#resumen-del-hardware)
2. [Alimentación del Sistema](#alimentación-del-sistema)
3. [Conexiones por Subsistemas](#conexiones-por-subsistemas)
   - [Relés de Potencia](#1-relés-de-potencia)
   - [Sensores de Entrada](#2-sensores-de-entrada)
   - [Bus I²C](#3-bus-i²c)
   - [Motores de Tracción](#4-motores-de-tracción)
   - [Motor de Dirección](#5-motor-de-dirección)
   - [Interfaz de Usuario](#6-interfaz-de-usuario)
   - [Audio](#7-audio)
   - [Iluminación](#8-iluminación)
4. [Tabla Resumen de Pines](#tabla-resumen-de-pines)
5. [Diagrama de Bloques](#diagrama-de-bloques)
6. [Notas de Seguridad](#notas-de-seguridad)

---

## RESUMEN DEL HARDWARE

### Componentes Principales
- **Controlador**: ESP32-S3-DevKitC-1 (44 pines, GPIOs 0-48)
- **Sensores de Corriente**: 6x INA226 con shunts CG FL-2C
  - 1x 100A (batería 24V)
  - 5x 50A (motores tracción + dirección)
  - Todos 75mV, Clase 0.5
- **Multiplexado I²C**: 5x TCA9548A (evitar conflictos dirección)
- **Driver PWM**: 1x PCA9685 (dirección 0x41, 16 canales)
- **Expansión GPIO**: 1x MCP23017 (16 pines I²C para relés)
- **Optoacopladores**: 2x HY-M158 PC817 (16 canales total, 12V→3.3V)
- **Drivers Motores**: 
  - 4x BTS7960 43A (tracción 4 ruedas)
  - 1x BTS7960 (dirección RS390 12V 6000RPM)
- **Encoder Dirección**: E6B2-CWZ6C 1200PR (ratio 1:1 al volante)
- **Sensores Inductivos**: 5x LJ12A3-4-Z/BX (4 ruedas + señal Z)
- **Sensor Pedal**: A1324LUA-T (Hall analógico)
- **Pantalla**: ILI9488 480x320 + táctil XPT2046
- **Audio**: DFPlayer Mini
- **LEDs**: Tira WS2812B
- **Conversores Nivel**: 3x bidireccionales 4 canales (5V↔3.3V)

### Relés del Sistema
1. **Módulo 2CH 5V SRD-05VDC-SL-C** (control lógico)
2. **SPDT 100A 24V** (batería principal)
3. **TAXNELE TN606 RL280-12** (12V 100-200A)
4. **Contactor HCH8s-25z** (25A 2NO DC12)
5. **Automotive Waterproof Relay 100A 12V SPDT**

---

## ALIMENTACIÓN DEL SISTEMA

### Esquema de Alimentación

```
Batería 24V (100Ah)
    │
    ├─→ SPDT 100A → Motores Tracción (4x BTS7960)
    │
    ├─→ Convertidor DC-DC 24V→12V (20A)
    │      │
    │      ├─→ Motor Dirección RS390 12V
    │      ├─→ Relés 12V
    │      ├─→ Módulo HY-M158 #1 y #2
    │      └─→ DFPlayer Mini
    │
    └─→ Convertidor DC-DC 24V→5V (10A)
           │
           ├─→ ESP32-S3-DevKitC-1 (vía USB o 5V pin)
           ├─→ Módulo Relés SRD-05VDC
           ├─→ Sensores LJ12A3 (5V)
           ├─→ Pantalla ILI9488 (5V)
           ├─→ PCA9685
           ├─→ MCP23017
           └─→ INA226 (6 unidades, vía 3.3V ESP32)

Señales Lógicas:
- Conversores Nivel 5V↔3.3V entre ESP32 y periféricos 5V
- HY-M158 optoacopladores para aislamiento 12V→3.3V
```

### Recomendaciones de Alimentación
- **ESP32**: USB 5V o pin 5V (máx 500mA)
- **Convertidor 24V→12V**: Mínimo 20A continuo
- **Convertidor 24V→5V**: Mínimo 10A continuo
- **Fusibles**: 
  - 100A en línea batería 24V
  - 30A en línea 12V
  - 15A en línea 5V

---

## CONEXIONES POR SUBSISTEMAS

### 1. RELÉS DE POTENCIA

#### Relé 1: Power Hold (PIN_RELAY_MAIN)
- **GPIO**: 2
- **Función**: Mantiene alimentación sistema después arranque
- **Control**: Módulo 2CH SRD-05VDC canal 1
- **Conexión**:
  ```
  ESP32 GPIO2 → IN1 Módulo Relés 5V
  Relé COM → 5V Sistema
  Relé NO → ESP32 5V Pin (auto-sostenimiento)
  ```

#### Relé 2: 12V Auxiliares (PIN_RELAY_TRAC)
- **GPIO**: 4
- **Función**: Activa circuito 12V (luces, audio, ventilación)
- **Control**: Módulo 2CH SRD-05VDC canal 2
- **Conexión**:
  ```
  ESP32 GPIO4 → IN2 Módulo Relés 5V
  Relé COM → Convertidor 24V→12V salida
  Relé NO → Bus 12V auxiliares
  ```

#### Relé 3: 24V Motores Tracción (PIN_RELAY_DIR)
- **GPIO**: 5
- **Función**: Conecta batería 24V a drivers BTS7960
- **Control**: SPDT 100A 24V (activado vía MCP23017 pin 0)
- **Conexión**:
  ```
  MCP23017 GP0 → Bobina Relé 100A (+ transistor driver)
  Relé COM → Batería 24V (+)
  Relé NO → Bus 24V motores tracción
  ```

#### Relé 4: Reserva (PIN_RELAY_SPARE)
- **GPIO**: 6
- **Función**: Expansión futura / Emergencia
- **Control**: MCP23017 pin 1
- **Conexión**:
  ```
  MCP23017 GP1 → Bobina Relé (+ transistor driver)
  Relé COM → TBD
  Relé NO → TBD
  ```

---

### 2. SENSORES DE ENTRADA

#### 2.1 Pedal Acelerador (PIN_PEDAL)
- **GPIO**: 35 (ADC compatible)
- **Sensor**: A1324LUA-T (Hall analógico)
- **Salida**: 0.5V - 4.5V (requiere divisor tensión o conversor nivel)
- **Conexión**:
  ```
  Sensor +5V → 5V regulado
  Sensor GND → GND común
  Sensor OUT → Divisor tensión (5V→3.3V) → GPIO35
  
  Divisor tensión:
    OUT ─┬─ R1 (2.2kΩ) ─ GPIO35
         └─ R2 (4.7kΩ) ─ GND
  ```

#### 2.2 Encoder Dirección (E6B2-CWZ6C 1200PR)
- **GPIO A**: 37 (Canal A)
- **GPIO B**: 38 (Canal B)
- **GPIO Z**: 39 (Señal Z centrado)
- **Aislamiento**: HY-M158 Módulo #1 CH1-3
- **Conexión**:
  ```
  Encoder VCC → 5V (requiere 4.5-30VDC, usar 5V)
  Encoder GND → GND
  Encoder A → HY-M158 #1 CH1 IN → CH1 OUT → GPIO37
  Encoder B → HY-M158 #1 CH2 IN → CH2 OUT → GPIO38
  Encoder Z → HY-M158 #1 CH3 IN → CH3 OUT → GPIO39
  ```

#### 2.3 Sensores de Rueda (LJ12A3-4-Z/BX Inductivos)
- **Tipo**: NPN NO, 6-36VDC, salida 5V
- **Conexión vía**: HY-M158 Módulo #1 CH4-7
- **Pines**:
  ```
  Rueda FL → HY-M158 #1 CH4 → GPIO20
  Rueda FR → HY-M158 #1 CH5 → GPIO21
  Rueda RL → HY-M158 #1 CH6 → GPIO36
  Rueda RR → HY-M158 #1 CH7 → GPIO17
  ```
- **Cableado por sensor**:
  ```
  Marrón (BN) → 5V
  Azul (BU) → GND
  Negro (BK) → HY-M158 IN (respectivo canal)
  ```

#### 2.4 Sensor Temperatura (DS18B20)
- **GPIO**: 15 (OneWire)
- **Conexión**:
  ```
  DS18B20 VCC → 3.3V (o 5V con pull-up)
  DS18B20 GND → GND
  DS18B20 DQ → GPIO15 + Pull-up 4.7kΩ a 3.3V
  ```

---

### 3. BUS I²C

#### 3.1 Configuración I²C Principal
- **SDA**: GPIO16
- **SCL**: GPIO9
- **Pull-ups**: 4.7kΩ a 3.3V en SDA y SCL
- **Hub**: Divisor 8 vías (conecta todos los dispositivos)

#### 3.2 Dispositivos I²C

##### TCA9548A Multiplexor #1 (Dirección 0x70)
```
Canal 0: INA226 Motor FL (0x40)
Canal 1: INA226 Motor FR (0x40)
Canal 2: INA226 Motor RL (0x40)
Canal 3: INA226 Motor RR (0x40)
Canal 4: INA226 Batería 24V (0x40)
Canal 5: INA226 Motor Dirección (0x40)
Canales 6-7: Reserva
```

**Conexión TCA9548A**:
```
VCC → 3.3V
GND → GND
SDA → GPIO16 (via hub)
SCL → GPIO9 (via hub)
A0-A2 → GND (dirección 0x70)
RESET → 3.3V
```

##### INA226 - Configuración de Shunts
Todos los INA226 usan dirección 0x40 (A0=A1=GND), diferenciados por canal TCA9548A.

**Shunt CG FL-2C Especificaciones**:
- Corriente nominal: 50A o 100A
- Caída tensión: 75mV @ corriente nominal
- Precisión: Clase 0.5 (±0.5%)
- Potencia disipación: 3.75W @ 100A

**Conexión INA226 (ejemplo Motor FL)**:
```
TCA9548A CH0 SDA/SCL → INA226 SDA/SCL
INA226 VCC → 3.3V
INA226 GND → GND
INA226 A0, A1 → GND (dirección 0x40)
INA226 IN+ → Shunt terminal 1 (lado batería)
INA226 IN- → Shunt terminal 2 (lado motor)
Motor + → Shunt terminal 2
Motor - → GND/Negativo batería

Calibración INA226:
- Shunt 50A: 0.0015Ω (75mV/50A)
- Shunt 100A: 0.00075Ω (75mV/100A)
- Current_LSB = MaxCurrent / 32768
- Cal_Value = 0.00512 / (Current_LSB * Rshunt)
```

##### PCA9685 PWM Driver (Dirección 0x41)
**Función**: Control motor dirección RS390 vía PWM
**Conexión**:
```
VCC → 5V (lógica)
GND → GND
SDA → GPIO16 (via hub)
SCL → GPIO9 (via hub)
A0 → 3.3V (dirección 0x41, cambiado de 0x40)
A1-A5 → GND
OE → GND (siempre activo)
V+ → 12V (alimentación servos/motores)

Canal 0 (PWM): Motor dirección velocidad
Canal 1 (PWM): Motor dirección sentido/freno
```

##### MCP23017 Expansor GPIO (Dirección 0x20)
**Función**: Control relés y salidas auxiliares
**Conexión**:
```
VCC → 5V
GND → GND
SDA → GPIO16 (via hub)
SCL → GPIO9 (via hub)
A0-A2 → GND (dirección 0x20)
RESET → 5V (pull-up 10kΩ)

Asignación pines:
GP0: Relé 24V motores tracción (+ transistor driver)
GP1: Relé reserva
GP2-GP7: Control luces LED WS2812B futuro
GP8-GP15: Reserva expansión
```

---

### 4. MOTORES DE TRACCIÓN

#### Configuración 4x BTS7960 (43A por canal)

##### Motor Frontal Izquierdo (FL)
```
BTS7960 #1:
  RPWM → GPIO23 (PIN_FL_PWM)
  LPWM → GND (no usado, control unidireccional)
  R_EN → GPIO24 (PIN_FL_IN1)
  L_EN → GPIO25 (PIN_FL_IN2)
  R_IS → No conectado (feedback corriente opcional)
  L_IS → No conectado
  VCC → 5V
  GND → GND común
  B+ → Motor FL terminal +
  B- → Motor FL terminal -
  Vmot → Bus 24V motores (post-relé)
  Vmot GND → GND batería 24V
```

##### Motor Frontal Derecho (FR)
```
BTS7960 #2:
  RPWM → GPIO26 (PIN_FR_PWM)
  LPWM → GND
  R_EN → GPIO27 (PIN_FR_IN1)
  L_EN → GPIO28 (PIN_FR_IN2)
  VCC → 5V
  GND → GND
  B+ → Motor FR terminal +
  B- → Motor FR terminal -
  Vmot → Bus 24V motores
  Vmot GND → GND batería
```

##### Motor Trasero Izquierdo (RL)
```
BTS7960 #3:
  RPWM → GPIO29 (PIN_RL_PWM)
  LPWM → GND
  R_EN → GPIO30 (PIN_RL_IN1)
  L_EN → GPIO31 (PIN_RL_IN2)
  VCC → 5V
  GND → GND
  B+ → Motor RL terminal +
  B- → Motor RL terminal -
  Vmot → Bus 24V motores
  Vmot GND → GND batería
```

##### Motor Trasero Derecho (RR)
```
BTS7960 #4:
  RPWM → GPIO32 (PIN_RR_PWM)
  LPWM → GND
  R_EN → GPIO33 (PIN_RR_IN1)
  L_EN → GPIO34 (PIN_RR_IN2)
  VCC → 5V
  GND → GND
  B+ → Motor RR terminal +
  B- → Motor RR terminal -
  Vmot → Bus 24V motores
  Vmot GND → GND batería
```

**Nota**: Shunts INA226 (50A) van en línea positiva entre Vmot y B+ de cada motor.

---

### 5. MOTOR DE DIRECCIÓN

#### Motor RS390 12V 6000RPM + Reductora 1:50
**Control**: PCA9685 canal 0 (PWM) + canal 1 (dirección)
**Driver**: BTS7960 #5
**Encoder**: E6B2-CWZ6C 1200PR montado en eje salida reductora

```
PCA9685 Canal 0 (PWM) → BTS7960 #5 RPWM
PCA9685 Canal 1 (DIR) → BTS7960 #5 R_EN/L_EN (logic)
BTS7960 #5 VCC → 5V
BTS7960 #5 GND → GND
BTS7960 #5 Vmot → Bus 12V (post-relé)
BTS7960 #5 B+/B- → Motor RS390 terminales
Shunt INA226 50A → Entre 12V bus y Vmot BTS7960
```

**Control Lógico**:
- PWM 0-255: Velocidad motor
- DIR HIGH: Giro derecha
- DIR LOW: Giro izquierda

---

### 6. INTERFAZ DE USUARIO

#### 6.1 Pantalla ILI9488 (480x320 SPI)
```
VCC → 5V
GND → GND
CS → GPIO8
DC → GPIO13
RST → GPIO14
MOSI → GPIO11
MISO → GPIO12
SCK → GPIO10
LED/BL → GPIO42 (PWM backlight via LEDC)
```

**Control Brillo Backlight**:
```cpp
ledcSetup(0, 5000, 8);  // Canal 0, 5kHz, 8-bit
ledcAttachPin(42, 0);   // GPIO42 → Canal 0
ledcWrite(0, 128);      // Brillo 0-255
```

#### 6.2 Táctil XPT2046
```
VCC → 3.3V
GND → GND
CS → GPIO22
IRQ → GPIO46
MOSI → GPIO11 (compartido SPI)
MISO → GPIO12 (compartido SPI)
SCK → GPIO10 (compartido SPI)
```

#### 6.3 Botones Físicos

##### Botón Luces (PIN_BTN_LIGHTS)
```
GPIO45 ← Switch → GND
Pull-up interno ESP32 activado
```

##### Botón Multimedia (PIN_BTN_MEDIA)
**Señal**: 12V (vía HY-M158 Módulo #2 CH8)
```
Switch 12V → HY-M158 #2 CH8 IN
HY-M158 #2 CH8 OUT → GPIO40
Pull-up interno ESP32
```

##### Botón/Switch 4x4 (PIN_BTN_4X4)
**Tipo**: Switch 2 posiciones, señal 12V
**Aislamiento**: HY-M158 Módulo #2 CH6
```
Posición 4x4 → HY-M158 #2 CH6 IN
HY-M158 #2 CH6 OUT → GPIO41
Pull-up interno ESP32 (HIGH=4x4, LOW=4x2)
```

#### 6.4 Palanca Cambios (5 posiciones)
**Señales**: 12V vía HY-M158 Módulo #2

```
Posición P → HY-M158 #2 CH1 → GPIO47
Posición D2 → HY-M158 #2 CH2 → GPIO48
Posición D1 → HY-M158 #2 CH3 → GPIO7
Posición N → HY-M158 #2 CH4 → GPIO18
Posición R → HY-M158 #2 CH5 → GPIO19

Configuración: Pull-up interno en todos los GPIOs
Lógica: Posición activa = LOW (cerrado a GND post-optoacoplador)
```

---

### 7. AUDIO

#### DFPlayer Mini
```
VCC → 5V (consumo 20mA idle, 200mA playing)
GND → GND
TX → GPIO44 (PIN_DFPLAYER_RX del ESP32)
RX → GPIO43 (PIN_DFPLAYER_TX del ESP32)
SPK+ → Altavoz + (8Ω 3W recomendado)
SPK- → Altavoz -
```

**Tarjeta microSD**:
- Formato: FAT32
- Archivos: 0001.mp3 a 0038.mp3 (38 pistas)
- Carpeta raíz o /mp3/

**Configuración UART**:
```cpp
Serial2.begin(9600, SERIAL_8N1, 44, 43); // RX=44, TX=43
```

---

### 8. ILUMINACIÓN

#### LEDs WS2812B (Futuro)
**Pin de datos**: Disponible en MCP23017 GP2-GP7
**Alternativa directa**: GPIO libre (ej. GPIO1, GPIO3)

```
5V → Tira LED VCC
GND → Tira LED GND
Dato → GPIO1 (ejemplo) o MCP23017 GP2

Configuración:
- Nivel lógico: 5V (requiere conversor 3.3V→5V o WS2812B compatible 3.3V)
- Corriente: 60mA por LED (calcular según cantidad)
```

---

## TABLA RESUMEN DE PINES

### GPIOs ESP32-S3-DevKitC-1 Asignados

| GPIO | Función | Periférico | Notas |
|------|---------|-----------|--------|
| 2 | Relé Main | SRD-05VDC #1 | Power Hold |
| 4 | Relé 12V Aux | SRD-05VDC #2 | Auxiliares |
| 5 | Relé Tracción | MCP23017 GP0 | Via I²C |
| 6 | Relé Reserva | MCP23017 GP1 | Via I²C |
| 7 | Shifter D1 | HY-M158 #2 | 12V→3.3V |
| 8 | TFT CS | ILI9488 | SPI |
| 9 | I2C SCL | Bus I²C | Pull-up 4.7kΩ |
| 10 | TFT SCK | ILI9488/XPT2046 | SPI Clock |
| 11 | TFT MOSI | ILI9488/XPT2046 | SPI MOSI |
| 12 | TFT MISO | ILI9488/XPT2046 | SPI MISO |
| 13 | TFT DC | ILI9488 | Data/Command |
| 14 | TFT RST | ILI9488 | Reset |
| 15 | OneWire | DS18B20 | Temperatura |
| 16 | I2C SDA | Bus I²C | Pull-up 4.7kΩ |
| 17 | Rueda RR | LJ12A3 #4 | Via HY-M158 |
| 18 | Shifter N | HY-M158 #2 | 12V→3.3V |
| 19 | Shifter R | HY-M158 #2 | 12V→3.3V |
| 20 | Rueda FL | LJ12A3 #1 | Via HY-M158 |
| 21 | Rueda FR | LJ12A3 #2 | Via HY-M158 |
| 22 | Touch CS | XPT2046 | SPI Táctil |
| 23 | FL PWM | BTS7960 #1 | Motor FL |
| 24 | FL IN1 | BTS7960 #1 | Motor FL |
| 25 | FL IN2 | BTS7960 #1 | Motor FL |
| 26 | FR PWM | BTS7960 #2 | Motor FR |
| 27 | FR IN1 | BTS7960 #2 | Motor FR |
| 28 | FR IN2 | BTS7960 #2 | Motor FR |
| 29 | RL PWM | BTS7960 #3 | Motor RL |
| 30 | RL IN1 | BTS7960 #3 | Motor RL |
| 31 | RL IN2 | BTS7960 #3 | Motor RL |
| 32 | RR PWM | BTS7960 #4 | Motor RR |
| 33 | RR IN1 | BTS7960 #4 | Motor RR |
| 34 | RR IN2 | BTS7960 #4 | Motor RR |
| 35 | Pedal | A1324LUA-T | ADC, divisor 5V→3.3V |
| 36 | Rueda RL | LJ12A3 #3 | Via HY-M158 |
| 37 | Encoder A | E6B2-CWZ6C | Via HY-M158 |
| 38 | Encoder B | E6B2-CWZ6C | Via HY-M158 |
| 39 | Encoder Z | E6B2-CWZ6C | Via HY-M158 |
| 40 | Btn Media | Switch 12V | Via HY-M158 |
| 41 | Btn 4x4 | Switch 12V | Via HY-M158 |
| 42 | TFT Backlight | LED PWM | LEDC 0-255 |
| 43 | DFPlayer TX | DFPlayer RX | UART Serial2 |
| 44 | DFPlayer RX | DFPlayer TX | UART Serial2 |
| 45 | Btn Lights | Switch GND | Pull-up interno |
| 46 | Touch IRQ | XPT2046 | Interrupción |
| 47 | Shifter P | HY-M158 #2 | 12V→3.3V |
| 48 | Shifter D2 | HY-M158 #2 | 12V→3.3V |

**Total GPIOs usados**: 47 de 48 disponibles (GPIO 0-21, 35-48)
**GPIOs libres**: 1, 3 (reservados comunicación/expansión)

---

## DIAGRAMA DE BLOQUES

```
┌─────────────────────────────────────────────────────────────────────┐
│                         BATERÍA 24V 100Ah                           │
└────┬─────────────────────────────────────────────────────────┬──────┘
     │                                                          │
     ├─→ SPDT 100A ──→ 4x BTS7960 ──→ Motores Tracción 24V   │
     │    (GPIO5)           (GPIO23-34)                         │
     │                                                          │
     ├─→ Conv 24V→12V ──┬→ Motor Dirección RS390 12V          │
     │                  ├→ Relés 12V                            │
     │                  ├→ HY-M158 Módulos #1 #2                │
     │                  └→ DFPlayer Mini                        │
     │                                                          │
     └─→ Conv 24V→5V ──┬→ ESP32-S3-DevKitC-1                   │
                       ├→ Módulo Relés SRD-05VDC                │
                       ├→ Sensores LJ12A3 (5V)                  │
                       ├→ Pantalla ILI9488                      │
                       └→ I²C Devices (PCA9685, MCP23017)       │
                                                                │
┌────────────────────────────────────────────────────────────────────┐
│                      ESP32-S3-DevKitC-1                            │
│  ┌──────────────────────────────────────────────────────────────┐ │
│  │  GPIO2-6: Relés    GPIO35: Pedal ADC                        │ │
│  │  GPIO16/9: I²C     GPIO37-39: Encoder                        │ │
│  │  GPIO23-34: BTS7960 GPIO40-41,45-48: Botones/Shifter        │ │
│  │  GPIO8-14: TFT     GPIO43-44: DFPlayer                      │ │
│  │  GPIO22,46: Touch  GPIO42: Backlight PWM                    │ │
│  └──────────────────────────────────────────────────────────────┘ │
└───────┬────────────────┬───────────────────┬──────────────────────┘
        │                │                   │
        ├─ I²C Bus ──────┼─────────┬─────────┼────────────────┐
        │   (SDA/SCL)    │         │         │                │
        │                │         │         │                │
    TCA9548A         PCA9685   MCP23017   Conversores     HY-M158
    (6x INA226)      (0x41)    (0x20)     Nivel 5V↔3.3V   x2 (16ch)
        │                │         │                           │
    Shunts 50A/100A   Motor Dir  Relés                   Encoder
                      Control    Adicionales              Sensores
                                                          Shifter
```

---

## NOTAS DE SEGURIDAD

### ⚠️ ADVERTENCIAS CRÍTICAS

1. **NUNCA** conectar/desconectar alimentación con motores en movimiento
2. **SIEMPRE** verificar polaridad antes de energizar sistema
3. **OBLIGATORIO** fusibles en todas las líneas de potencia
4. **VERIFICAR** aislamiento de shunts INA226 (cortocircuito = daño)
5. **CALIBRAR** encoders y sensores antes de pruebas con carga
6. **MONITOREAR** temperaturas drivers BTS7960 (disipadores requeridos)
7. **PROTEGER** ESP32 con regulador 5V estable (ripple < 50mV)

### Secuencia Arranque Seguro

1. Verificar todas las conexiones en reposo
2. Conectar batería 24V (sin carga)
3. Verificar conversores 12V y 5V estables
4. Energizar ESP32 (debería ejecutar auto-test)
5. Esperar inicialización I²C (LEDs status)
6. Calibrar encoder posición central
7. Probar pedal en vacío (sin motores)
8. Activar relé tracción (GPIO5)
9. Probar motores individuales a baja potencia
10. Incrementar carga progresivamente

### Protecciones Implementadas en Firmware

- Límite corriente por INA226 (corte 100A batería)
- Watchdog 5s (reset automático si falla loop)
- Timeout motores 2s sin señal pedal
- Anti-retroceso en pendiente (freno automático)
- Modo seguridad temperatura >80°C
- Desconexión emergencia botón luces x3 rápido

### Mantenimiento Preventivo

- **Semanal**: Inspección visual cables/conectores
- **Mensual**: Verificar apriete terminales batería/motores
- **Trimestral**: Lubricar encoder dirección
- **Semestral**: Calibración sensores INA226
- **Anual**: Reemplazo preventivo shunts alta corriente

---

## CONTACTO Y SOPORTE

**Firmware**: ESP32-S3-DevKitC-1 (Arduino/PlatformIO)
**Versión Hardware**: v2.0 (Diciembre 2024)
**Autor**: Sistema Integrado Coche Eléctrico Inteligente

Para soporte técnico o mejoras, consultar:
- `firmware Coche Marcos/docs/FIRMWARE_FINAL_STATUS.md`
- `firmware Coche Marcos/include/pins.h`
- Repositorio: https://github.com/florinzgz/coche-2-inteligente

---

**ÚLTIMA ACTUALIZACIÓN**: Noviembre 2024
**ESTADO**: ✅ Firmware 100% compilable - Production Ready

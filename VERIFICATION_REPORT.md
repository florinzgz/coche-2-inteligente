# 🔍 VERIFICACIÓN COMPLETA DEL FIRMWARE - ESP32-S3-DevKitC-1

**Fecha:** 2025-01-07  
**Firmware:** Coche Eléctrico Inteligente  
**Placa:** ESP32-S3-DevKitC-1 (44 pines)

---

## ⚠️ PROBLEMAS CRÍTICOS DETECTADOS

### 1. **GPIO CONFLICTS (Uso Duplicado)**

| GPIO | Conflicto Entre | Estado |
|------|----------------|--------|
| 13 | PIN_TOUCH_IRQ / PIN_SHIFTER_P | ❌ CRÍTICO |
| 14 | PIN_TFT_RST / PIN_SHIFTER_D2 | ❌ CRÍTICO |
| 15 | PIN_TFT_CS / PIN_SHIFTER_D1 | ❌ CRÍTICO |
| 16 | PIN_DFPLAYER_RX / PIN_SHIFTER_N | ❌ CRÍTICO |
| 17 | PIN_DFPLAYER_TX / PIN_SHIFTER_R | ❌ CRÍTICO |
| 40 | PIN_BTN_LIGHTS / PIN_WHEEL3 | ❌ CRÍTICO |

**Impacto:** Shifter no puede usar estos GPIOs porque están ocupados por pantalla y audio.

### 2. **GPIOs INVÁLIDOS (No existen en placa 44 pines)**

| GPIO | Pin Definido | Estado |
|------|-------------|--------|
| 24 | PIN_RR_PWM | ❌ NO EXISTE |
| 26 | PIN_ONEWIRE | ❌ NO EXISTE |
| 27 | PIN_TFT_DC | ❌ NO EXISTE |
| 28 | PIN_RR_IN1 | ❌ NO EXISTE |
| 29 | PIN_RR_IN2 | ❌ NO EXISTE |
| 32 | PIN_RELAY_LIGHTS | ❌ NO EXISTE |
| 33 | PIN_RELAY_MEDIA | ❌ NO EXISTE |
| 34 | PIN_PEDAL | ❌ NO EXISTE |

**GPIOs válidos disponibles:** 0,1,2,3,4,5,6,7,8,9,10,11,12,13,14,15,16,17,18,19,20,21,35,36,37,38,39,40,41,42,43,44,45,46,47,48

### 3. **INCONSISTENCIAS EN DEFINICIONES**

**pins.h vs power_mgmt.cpp:**
- pins.h define: `PIN_RELAY_MAIN=2, PIN_RELAY_TRAC=4, PIN_RELAY_DIR=5`
- power_mgmt.cpp usa: `22, 23, 0, 39` (correcto)
- ❌ **Definiciones en pins.h están desactualizadas**

**pins.h power relays (incorrectos):**
```cpp
#define PIN_RELAY_MAIN    2   // ❌ Usado por power_mgmt como GPIO 22
#define PIN_RELAY_TRAC    4   // ❌ Usado por power_mgmt como GPIO 23
#define PIN_RELAY_DIR     5   // ❌ Usado por power_mgmt como GPIO 0
#define PIN_RELAY_LIGHTS  32  // ❌ GPIO 32 no existe en placa
#define PIN_RELAY_MEDIA   33  // ❌ GPIO 33 no existe en placa
```

---

## ✅ COMPONENTES CORRECTOS VERIFICADOS

### Sistema de Alimentación (Power Management)
**Implementación:** `power_mgmt.cpp`  
**Estado:** ✅ **CORRECTO**

| Relé | GPIO | Función | Validado |
|------|------|---------|----------|
| Relé 1 | 22 | Power Hold Buck 5V | ✅ |
| Relé 2 | 23 | 12V Auxiliares | ✅ |
| Relé 3 | 0 | 24V Motores | ✅ |
| Relé 4 | 39 | Opcional/Seguridad | ✅ |
| Llave | 45 | Detección ON/OFF (pull-up) | ✅ |

### HY-M158 Optoacopladores (Módulo #1)
**Canales:** 8 usados de 8

| GPIO | Señal | Tipo | Estado |
|------|-------|------|--------|
| 35 | WHEEL_FL | 5V | ✅ |
| 36 | WHEEL_FR | 5V | ✅ |
| 41 | WHEEL_RL | 5V | ✅ |
| 40 | WHEEL_RR | 5V | ⚠️ Conflicto con BTN_LIGHTS |
| 37 | ENCODER_A | 5V | ✅ |
| 38 | ENCODER_B | 5V | ✅ |
| 46 | ENCODER_Z | 5V | ✅ Corregido (era GPIO 25) |
| 39 | BTN_MULTIMEDIA | 12V | ⚠️ Conflicto con RELAY_OPTIONAL |

### Pantalla TFT ILI9488
**Bus:** SPI  
**Estado:** ⚠️ **NECESITA CORRECCIÓN**

| Pin | GPIO | Estado |
|-----|------|--------|
| CS | 15 | ⚠️ Conflicto con shifter |
| DC | 27 | ❌ GPIO no existe |
| RST | 14 | ⚠️ Conflicto con shifter |
| MOSI | 11 | ✅ |
| MISO | 19 | ✅ |
| SCK | 18 | ✅ |

### Táctil XPT2046
| Pin | GPIO | Estado |
|-----|------|--------|
| CS | 12 | ✅ |
| IRQ | 13 | ⚠️ Conflicto con shifter |

### DFPlayer Mini
| Pin | GPIO | Estado |
|-----|------|--------|
| RX | 16 | ⚠️ Conflicto con shifter |
| TX | 17 | ⚠️ Conflicto con shifter |

### Bus I²C
**Estado:** ✅ **CORRECTO**

| Pin | GPIO |
|-----|------|
| SDA | 21 |
| SCL | 20 |

**Dispositivos I²C:**
- ✅ 6x INA226 (dirección 0x40, multiplexados vía TCA9548A 0x70)
- ✅ 1x PCA9685 (dirección 0x41, motor dirección)
- ✅ 1x TCA9548A (dirección 0x70, multiplexor)

### Motores BTS7960 (4 ruedas)
**Estado:** ⚠️ **MOTOR RR NECESITA CORRECCIÓN**

| Motor | PWM | IN1 | IN2 | Estado |
|-------|-----|-----|-----|--------|
| FL | 1 | 3 | 6 | ✅ |
| FR | 7 | 8 | 9 | ✅ |
| RL | 10 | 47 | 48 | ✅ |
| RR | 24 | 28 | 29 | ❌ GPIOs no existen |

---

## 🔧 SOLUCIONES REQUERIDAS

### Prioridad 1: Reasignar Shifter y BTN_MULTIMEDIA
**Problema:** Shifter usa GPIOs ocupados por pantalla/audio

**Solución propuesta:**
```cpp
// Shifter - usar GPIOs de HY-M158 Módulo #2
#define PIN_SHIFTER_P     13  // Mantener (disponible si movemos touch_irq)
#define PIN_SHIFTER_D2    14  // Mantener (disponible si movemos tft_rst)
#define PIN_SHIFTER_D1    15  // Mantener (disponible si movemos tft_cs)
#define PIN_SHIFTER_N     16  // Mantener (disponible si movemos dfplayer)
#define PIN_SHIFTER_R     17  // Mantener (disponible si movemos dfplayer)
```

**Opción:** Mover componentes con conflicto a otros GPIOs:
- PIN_TOUCH_IRQ: 13 → 43 (libre)
- PIN_TFT_RST: 14 → 44 (libre, no usar RX si es crítico)
- PIN_TFT_CS: 15 → Mantener (shifter no va por SPI)
- DFPlayer: Mover a UART0 (GPIOs 43, 44) o usar SoftwareSerial

### Prioridad 2: Corregir GPIOs Inválidos
**Motor RR:**
```cpp
// Usar GPIOs disponibles
#define PIN_RR_PWM        43  // Cambiar de 24
#define PIN_RR_IN1        44  // Cambiar de 28  
#define PIN_RR_IN2        3   // Reusar GPIO 3 si FL no lo usa simultáneamente
```

**Otros:**
```cpp
#define PIN_TFT_DC        4   // Cambiar de 27 (GPIO 4 puede usarse)
#define PIN_ONEWIRE       5   // Cambiar de 26
#define PIN_PEDAL         36  // Cambiar de 34 (o verificar ADC disponibles)
```

### Prioridad 3: Actualizar pins.h con Definiciones Correctas de Relés
```cpp
// Relés de potencia (corregir según power_mgmt.cpp)
#define PIN_RELAY_POWER_HOLD  22  // Relé 1
#define PIN_RELAY_AUX_12V     23  // Relé 2
#define PIN_RELAY_MOTOR_24V   0   // Relé 3
#define PIN_RELAY_OPTIONAL    39  // Relé 4
#define PIN_KEY_DETECT        45  // Detección llave
```

### Prioridad 4: Resolver Conflicto GPIO 40
```cpp
// Opción 1: Mover botón luces
#define PIN_BTN_LIGHTS    43  // Cambiar de 40

// Opción 2: Mover sensor rueda RR (menos recomendado)
#define PIN_WHEEL3        44  // Cambiar de 40
```

---

## 📊 ESTADÍSTICAS FINALES

| Métrica | Valor |
|---------|-------|
| **Total GPIOs usados** | 41 |
| **GPIOs con conflictos** | 6 |
| **GPIOs inválidos** | 8 |
| **GPIOs libres** | 3 (0, 43, 44) |
| **Canales HY-M158 usados** | 13 de 16 |
| **Canales HY-M158 libres** | 3 |

---

## 📝 COMPONENTES LISTADOS

### Hardware Confirmado en Firmware
✅ ESP32-S3-DevKitC-1 (44 pines)  
✅ 6x INA226 sensores corriente + shunts CG FL-2C (1x100A, 5x50A, 75mV)  
✅ 1x TCA9548A multiplexor I²C (0x70)  
✅ 1x PCA9685 PWM driver (0x41)  
✅ 2x HY-M158 optoacopladores (16 canales)  
✅ Módulo 4 relés SRD-05VDC-SL-C  
✅ 4x BTS7960 drivers motor tracción  
✅ 1x BTS7960 driver motor dirección (RS390 12V)  
✅ 1x Encoder E6B2-CWZ6C 1200PR  
✅ 5x Sensores LJ12A3-4-Z/BX  
✅ 1x Sensor Hall A1324LUA-T (pedal)  
✅ 1x Pantalla ILI9488 480x320  
✅ 1x Táctil XPT2046  
✅ 1x DFPlayer Mini  
✅ 1x Tira LEDs WS2812B  
✅ 1x DS18B20 sensor temperatura (OneWire)  
✅ Convertidor Buck 12V→5V 3A  

### Sistemas Implementados
✅ Power Management (4 relés + detección llave)  
✅ Sistema ABS (Anti-lock Braking System)  
✅ Sistema TCS (Traction Control System)  
✅ Sistema AI Regenerativo  
✅ Control LEDs WS2812B inteligente (KITT style)  
✅ WiFi Manager + OTA  
✅ Menú oculto (PIN 8989, activación esquina superior derecha)  
⚠️ Monitor INA226 tiempo real (parcial)  
⚠️ Test sensores (parcial)  
⚠️ Configuración persistente EEPROM (parcial)  

---

## 🎯 RECOMENDACIONES

1. **Inmediato:** Corregir conflictos GPIO críticos (shifter, botones, motor RR)
2. **Importante:** Actualizar pins.h con GPIOs válidos para placa 44 pines
3. **Recomendado:** Completar funciones menú oculto pendientes
4. **Opcional:** Añadir validación GPIO al compilar (static_assert)

---

**Generado por:** GitHub Copilot Verification System  
**Próxima revisión:** Tras aplicar correcciones GPIO

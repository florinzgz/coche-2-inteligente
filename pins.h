#pragma once
#include <stdint.h>

// ============================================================================
// pins.h - Asignación de pines para ESP32-S3-DevKitC-1 (44 pines)
// Compatible con GPIOs 0-48 (adaptado de versión N16R8)
// ============================================================================
//
// HARDWARE COMPLETO INTEGRADO:
// - ESP32-S3-DevKitC-1 (44 pines, GPIOs 0-48)
// - 6x INA226 con shunts externos CG FL-2C (1x100A + 5x50A, 75mV, 0.5 Class)
// - 5x TCA9548A multiplexores I²C
// - 1x PCA9685 PWM driver (dirección 0x41, motor dirección RS390)
// - 1x MCP23017 expansor GPIO I²C
// - 2x HY-M158 optoacopladores PC817
// - 4x BTS7960 drivers motor 43A (tracción 4 ruedas)
// - 1x BTS7960 driver motor dirección
// - 1x Encoder E6B2-CWZ6C 1200PR
// - 5x Sensores inductivos LJ12A3-4-Z/BX
// - 1x Sensor Hall A1324LUA-T (pedal analógico)
// - 1x Pantalla ILI9488 + táctil XPT2046
// - 1x DFPlayer Mini (audio)
// - 1x Tira LEDs WS2812B
// ============================================================================

// -----------------------
// Relés de potencia
// -----------------------
#define PIN_RELAY_MAIN    2
#define PIN_RELAY_TRAC    4
#define PIN_RELAY_DIR     5
#define PIN_RELAY_SPARE   6

// -----------------------
// Pedal acelerador (ADC)
// -----------------------
#define PIN_PEDAL         35  // GPIO 35 (ADC)

// -----------------------
// Encoder dirección
// -----------------------
#define PIN_ENCODER_A     37
#define PIN_ENCODER_B     38
#define PIN_ENCODER_Z     39

// -----------------------
// DFPlayer Mini (UART1)
// -----------------------
#define PIN_DFPLAYER_RX   44
#define PIN_DFPLAYER_TX   43

// -----------------------
// Pantalla TFT ILI9488 (SPI)
// -----------------------
#define PIN_TFT_CS        8
#define PIN_TFT_DC        13
#define PIN_TFT_RST       14
#define PIN_TFT_MOSI      11
#define PIN_TFT_MISO      12
#define PIN_TFT_SCK       10

// -----------------------
// Táctil (XPT2046 SPI)
// -----------------------
#define PIN_TOUCH_CS      22   // corregido: antes 9, ahora 22
#define PIN_TOUCH_IRQ     46

// -----------------------
// Botones físicos
// -----------------------
#define PIN_BTN_LIGHTS    45
#define PIN_BTN_MEDIA     40
#define PIN_BTN_4X4       41
// PIN_BTN_BATTERY eliminado (GPIO 42 sin función)

// -----------------------
// Palanca de cambios (Shifter)
// -----------------------
#define PIN_SHIFTER_P     47
#define PIN_SHIFTER_D2    48
#define PIN_SHIFTER_D1    7
#define PIN_SHIFTER_N     18
#define PIN_SHIFTER_R     19

// -----------------------
// Sensores de rueda
// -----------------------
#define PIN_WHEEL0        20
#define PIN_WHEEL1        21
#define PIN_WHEEL2        36
#define PIN_WHEEL3        17

// -----------------------
// DS18B20 (OneWire)
// -----------------------
#define PIN_ONEWIRE       15

// -----------------------
// I2C (INA226 + PCA9685 + MCP23017 + TCA9548A)
// -----------------------
#define PIN_I2C_SDA       16
#define PIN_I2C_SCL       9

// -----------------------
// BTS7960 – Motores de rueda
// -----------------------
#define PIN_FL_PWM        23
#define PIN_FL_IN1        24
#define PIN_FL_IN2        25

#define PIN_FR_PWM        26
#define PIN_FR_IN1        27
#define PIN_FR_IN2        28

#define PIN_RL_PWM        29
#define PIN_RL_IN1        30
#define PIN_RL_IN2        31

#define PIN_RR_PWM        32
#define PIN_RR_IN1        33
#define PIN_RR_IN2        34

// -----------------------
// Helpers
// -----------------------
static inline bool pin_is_assigned(uint8_t gpio) {
  switch (gpio) {
    case PIN_RELAY_MAIN:
    case PIN_RELAY_TRAC:
    case PIN_RELAY_DIR:
    case PIN_RELAY_SPARE:
    case PIN_PEDAL:
    case PIN_ENCODER_A:
    case PIN_ENCODER_B:
    case PIN_ENCODER_Z:
    case PIN_SHIFTER_P:
    case PIN_SHIFTER_D2:
    case PIN_SHIFTER_D1:
    case PIN_SHIFTER_N:
    case PIN_SHIFTER_R:
    case PIN_DFPLAYER_RX:
    case PIN_DFPLAYER_TX:
    case PIN_TFT_CS:
    case PIN_TFT_DC:
    case PIN_TFT_RST:
    case PIN_TFT_MOSI:
    case PIN_TFT_MISO:
    case PIN_TFT_SCK:
    case PIN_TOUCH_CS:
    case PIN_TOUCH_IRQ:
    case PIN_BTN_LIGHTS:
    case PIN_BTN_MEDIA:
    case PIN_BTN_4X4:
    case PIN_WHEEL0:
    case PIN_WHEEL1:
    case PIN_WHEEL2:
    case PIN_WHEEL3:
    case PIN_ONEWIRE:
    case PIN_I2C_SDA:
    case PIN_I2C_SCL:
    case PIN_FL_PWM: case PIN_FL_IN1: case PIN_FL_IN2:
    case PIN_FR_PWM: case PIN_FR_IN1: case PIN_FR_IN2:
    case PIN_RL_PWM: case PIN_RL_IN1: case PIN_RL_IN2:
    case PIN_RR_PWM: case PIN_RR_IN1: case PIN_RR_IN2:
      return true;
    default:
      return false;
  }
}
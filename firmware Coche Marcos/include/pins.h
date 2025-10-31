#pragma once
#include <stdint.h>

// ============================================================================
// pins.h - Asignación de pines para ESP32-S3-N16R8 (Freenove universal breakout)
// ============================================================================

// -----------------------
// Relés de potencia
// -----------------------
#define PIN_RELAY_MAIN    2
#define PIN_RELAY_TRAC    4
#define PIN_RELAY_DIR     5
#define PIN_RELAY_LIGHTS  32
#define PIN_RELAY_MEDIA   33

// -----------------------
// Pedal (ADC)
// -----------------------
#define PIN_PEDAL         34

// -----------------------
// Encoder dirección
// -----------------------
#define PIN_ENCODER_A     44
#define PIN_ENCODER_B     45
#define PIN_ENCODER_Z     25

// -----------------------
// DFPlayer Mini (UART1)
// -----------------------
#define PIN_DFPLAYER_RX   16
#define PIN_DFPLAYER_TX   17

// -----------------------
// Pantalla TFT ILI9488 (SPI)
// -----------------------
#define PIN_TFT_CS        15
#define PIN_TFT_DC        27
#define PIN_TFT_RST       14
#define PIN_TFT_MOSI      23
#define PIN_TFT_MISO      19
#define PIN_TFT_SCK       18

// -----------------------
// Táctil (XPT2046 SPI)
// -----------------------
#define PIN_TOUCH_CS      12   // Chip Select táctil
#define PIN_TOUCH_IRQ     13   // Interrupción táctil (opcional)

// -----------------------
// Botones físicos
// -----------------------
#define PIN_BTN_LIGHTS    35
#define PIN_BTN_MEDIA     36
#define PIN_BTN_4X4       39
#define PIN_BTN_BATTERY   46

// -----------------------
// Sensores de rueda (entradas digitales/inductivas)
// -----------------------
#define PIN_WHEEL0        35  // FL
#define PIN_WHEEL1        36  // FR
#define PIN_WHEEL2        39  // RL
#define PIN_WHEEL3        46  // RR

// -----------------------
// DS18B20 (OneWire)
// -----------------------
#define PIN_ONEWIRE       26

// -----------------------
// I2C (INA226 + PCA9685)
// -----------------------
#define PIN_I2C_SDA       21
#define PIN_I2C_SCL       22

// -----------------------
// BTS7960 – Motores de rueda (PWM + IN1/IN2)
// -----------------------
#define PIN_FL_PWM        47
#define PIN_FL_IN1        48
#define PIN_FL_IN2        49

#define PIN_FR_PWM        50
#define PIN_FR_IN1        51
#define PIN_FR_IN2        52

#define PIN_RL_PWM        53
#define PIN_RL_IN1        54
#define PIN_RL_IN2        55

#define PIN_RR_PWM        56
#define PIN_RR_IN1        57
#define PIN_RR_IN2        58

// -----------------------
// Helpers
// -----------------------
static inline bool pin_is_assigned(uint8_t gpio) {
  switch (gpio) {
    case PIN_RELAY_MAIN:
    case PIN_RELAY_TRAC:
    case PIN_RELAY_DIR:
    case PIN_RELAY_LIGHTS:
    case PIN_RELAY_MEDIA:
    case PIN_PEDAL:
    case PIN_ENCODER_A:
    case PIN_ENCODER_B:
    case PIN_ENCODER_Z:
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
    case PIN_BTN_BATTERY:
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
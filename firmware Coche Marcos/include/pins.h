#pragma once
#include <stdint.h>

// ============================================================================
// pins.h - Asignación de pines para ESP32-S3-DevKitC-1 (44 pines)
// Compatible con GPIOs 0-48 (adaptado de versión N16R8)
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
// Remapeado para ESP32-S3-DevKitC-1
// -----------------------
#define PIN_ENCODER_A     37  // Canal A (cambiado de 44)
#define PIN_ENCODER_B     38  // Canal B (cambiado de 45)
#define PIN_ENCODER_Z     25  // Señal Z (centrado)

// -----------------------
// DFPlayer Mini (UART1)
// -----------------------
#define PIN_DFPLAYER_RX   16
#define PIN_DFPLAYER_TX   17

// -----------------------
// Pantalla TFT ILI9488 (SPI)
// Remapeado para ESP32-S3-DevKitC-1
// -----------------------
#define PIN_TFT_CS        15
#define PIN_TFT_DC        27
#define PIN_TFT_RST       14
#define PIN_TFT_MOSI      11  // SPI MOSI (cambiado de 23)
#define PIN_TFT_MISO      19
#define PIN_TFT_SCK       18

// -----------------------
// Táctil (XPT2046 SPI)
// -----------------------
#define PIN_TOUCH_CS      12   // Chip Select táctil
#define PIN_TOUCH_IRQ     13   // Interrupción táctil (opcional)

// -----------------------
// Botones físicos
// Remapeados para ESP32-S3-DevKitC-1
// -----------------------
#define PIN_BTN_LIGHTS    40  // Botón luces (cambiado de 35)
#define PIN_BTN_MEDIA     41  // Botón media (cambiado de 36)
#define PIN_BTN_4X4       42  // Botón 4x4 (cambiado de 39)
#define PIN_BTN_BATTERY   46  // Botón batería

// -----------------------
// Sensores de rueda (entradas digitales/inductivas LJ12A3-4-Z/BX)
// Remapeados para ESP32-S3-DevKitC-1
// -----------------------
#define PIN_WHEEL0        35  // FL (Frontal Izquierda)
#define PIN_WHEEL1        36  // FR (Frontal Derecha)
#define PIN_WHEEL2        30  // RL (Trasera Izquierda)
#define PIN_WHEEL3        31  // RR (Trasera Derecha)

// -----------------------
// DS18B20 (OneWire)
// -----------------------
#define PIN_ONEWIRE       26

// -----------------------
// I2C (INA226 + PCA9685)
// Remapeado para ESP32-S3-DevKitC-1
// -----------------------
#define PIN_I2C_SDA       21
#define PIN_I2C_SCL       20  // Cambiado de 22

// -----------------------
// BTS7960 – Motores de rueda (PWM + IN1/IN2)
// Remapeado para ESP32-S3-DevKitC-1 (GPIOs 0-48)
// -----------------------
#define PIN_FL_PWM        1   // Frontal Izquierda PWM
#define PIN_FL_IN1        3   // Frontal Izquierda IN1
#define PIN_FL_IN2        6   // Frontal Izquierda IN2

#define PIN_FR_PWM        7   // Frontal Derecha PWM
#define PIN_FR_IN1        8   // Frontal Derecha IN1
#define PIN_FR_IN2        9   // Frontal Derecha IN2

#define PIN_RL_PWM        10  // Trasera Izquierda PWM
#define PIN_RL_IN1        47  // Trasera Izquierda IN1
#define PIN_RL_IN2        48  // Trasera Izquierda IN2

#define PIN_RR_PWM        24  // Trasera Derecha PWM
#define PIN_RR_IN1        28  // Trasera Derecha IN1
#define PIN_RR_IN2        29  // Trasera Derecha IN2

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
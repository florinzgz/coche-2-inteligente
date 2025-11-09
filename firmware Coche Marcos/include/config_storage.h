#pragma once

#include <Arduino.h>
#include <Preferences.h>

namespace ConfigStorage {
    // Storage namespace
    const char* const NAMESPACE = "car_config";
    
    // EEPROM Configuration structure
    struct Config {
        // Sensor enable/disable states
        bool sensorFL_enabled;
        bool sensorFR_enabled;
        bool sensorRL_enabled;
        bool sensorRR_enabled;
        bool ina226_enabled;
        
        // Encoder calibration
        int16_t encoder_center;
        int16_t encoder_left_limit;
        int16_t encoder_right_limit;
        
        // LED configuration
        uint8_t led_pattern;        // 0-7 pattern index
        uint8_t led_brightness;     // 0-255
        uint8_t led_speed;          // 0-255
        uint32_t led_color;         // RGB color
        
        // Power management
        uint16_t power_hold_delay;  // ms
        uint16_t aux_delay;         // ms
        uint16_t traction_delay;    // ms
        
        // System flags
        bool abs_enabled;
        bool tcs_enabled;
        bool regen_enabled;
        bool wifi_enabled;
        
        // Bluetooth controller
        char bt_mac_address[18];    // MAC address of paired controller (XX:XX:XX:XX:XX:XX)
        bool bt_auto_reconnect;     // Auto-reconnect on power up
        
        // Checksum for validation
        uint32_t checksum;
    };
    
    // Default configuration values
    const Config DEFAULT_CONFIG = {
        // Sensors enabled by default
        .sensorFL_enabled = true,
        .sensorFR_enabled = true,
        .sensorRL_enabled = true,
        .sensorRR_enabled = true,
        .ina226_enabled = true,
        
        // Encoder defaults (will be calibrated)
        .encoder_center = 600,
        .encoder_left_limit = 0,
        .encoder_right_limit = 1200,
        
        // LED defaults
        .led_pattern = 0,      // Rainbow
        .led_brightness = 128, // 50%
        .led_speed = 128,      // Medium
        .led_color = 0xFF0000, // Red
        
        // Power management defaults (ms)
        .power_hold_delay = 5000,
        .aux_delay = 100,
        .traction_delay = 500,
        
        // Systems enabled by default
        .abs_enabled = true,
        .tcs_enabled = true,
        .regen_enabled = true,
        .wifi_enabled = true,
        
        // Bluetooth defaults
        .bt_mac_address = "",      // Empty until paired
        .bt_auto_reconnect = true,
        
        .checksum = 0
    };
    
    // Initialize storage system
    void init();
    
    // Load configuration from EEPROM
    bool load(Config& config);
    
    // Save configuration to EEPROM
    bool save(const Config& config);
    
    // Reset to factory defaults
    void resetToDefaults();
    
    // Validate configuration checksum
    bool isValid(const Config& config);
    
    // Calculate checksum
    uint32_t calculateChecksum(const Config& config);
    
    // Get current configuration
    Config& getCurrentConfig();
}

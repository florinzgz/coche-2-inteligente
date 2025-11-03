#pragma once
#include <Arduino.h>

namespace TCSSystem {
    
    // TCS configuration parameters
    struct Config {
        bool enabled;              // TCS enable/disable
        float slipThreshold;       // Wheel slip threshold (%) to trigger TCS
        float minSpeedKmh;         // Minimum speed to activate TCS (km/h)
        float reductionRate;       // Power reduction rate per cycle (%)
        float recoveryRate;        // Power recovery rate per cycle (%)
        uint16_t updateIntervalMs; // TCS update interval (ms)
    };
    
    // TCS state per wheel
    struct WheelTCSState {
        bool active;               // TCS currently active on this wheel
        float slipRatio;           // Current slip ratio (%)
        float powerReduction;      // Current power reduction (0-100%)
        uint32_t lastActiveMs;     // Last time TCS was active
    };
    
    // Global TCS state
    struct State {
        bool systemActive;         // At least one wheel has TCS active
        WheelTCSState wheels[4];   // State for each wheel (FL, FR, RL, RR)
        uint32_t activationCount;  // Total TCS activations
        float avgSlipRatio;        // Average slip ratio across all wheels
        float totalPowerReduction; // Total power reduction applied (%)
    };
    
    // Initialize TCS system
    void init();
    
    // Update TCS system (call from main loop)
    void update();
    
    // Enable/disable TCS
    void setEnabled(bool enabled);
    
    // Get current TCS state
    const State& getState();
    
    // Check if TCS is active on any wheel
    bool isActive();
    
    // Get configuration
    Config& getConfig();
    
    // Apply TCS modulation to power demand for a specific wheel
    // Returns modulated power demand (0-100%)
    float modulatePower(int wheel, float powerDemand);
}

#include "abs_system.h"
#include "wheels.h"
#include "logger.h"
#include "alerts.h"
#include <cmath>

namespace ABSSystem {

static Config config = {
    .enabled = true,
    .slipThreshold = 20.0f,      // 20% slip triggers ABS
    .minSpeedKmh = 5.0f,          // Active above 5 km/h
    .cycleMs = 100,               // 10 Hz ABS pulse
    .pressureReduction = 0.3f     // 30% pressure reduction per pulse
};

static State state;
static bool initialized = false;

// Helper: clamp float value
static inline float clampf(float v, float lo, float hi) {
    if (v < lo) return lo;
    if (v > hi) return hi;
    return v;
}

// Calculate vehicle reference speed (average of non-slipping wheels)
static float calculateVehicleSpeed(const float wheelSpeeds[4]) {
    float maxSpeed = 0.0f;
    int validCount = 0;
    
    for (int i = 0; i < 4; i++) {
        if (wheelSpeeds[i] > 0.1f && Sensors::isWheelSensorOk(i)) {
            if (wheelSpeeds[i] > maxSpeed) {
                maxSpeed = wheelSpeeds[i];
            }
            validCount++;
        }
    }
    
    // Use highest wheel speed as reference (closest to true vehicle speed)
    return (validCount > 0) ? maxSpeed : 0.0f;
}

// Calculate slip ratio for a wheel
static float calculateSlipRatio(float wheelSpeed, float vehicleSpeed) {
    if (vehicleSpeed < 0.1f) return 0.0f;
    
    float slip = ((vehicleSpeed - wheelSpeed) / vehicleSpeed) * 100.0f;
    return clampf(slip, 0.0f, 100.0f);
}

void init() {
    state = {};
    state.systemActive = false;
    state.activationCount = 0;
    state.avgSlipRatio = 0.0f;
    
    for (int i = 0; i < 4; i++) {
        state.wheels[i].active = false;
        state.wheels[i].slipRatio = 0.0f;
        state.wheels[i].lastCycleMs = 0;
        state.wheels[i].pulsesApplied = 0;
    }
    
    initialized = true;
    Logger::infof("ABS initialized - Threshold: %.1f%%, MinSpeed: %.1f km/h", 
                  config.slipThreshold, config.minSpeedKmh);
}

void update() {
    if (!initialized || !config.enabled) {
        state.systemActive = false;
        return;
    }
    
    // Get wheel speeds from sensors
    float wheelSpeeds[4];
    for (int i = 0; i < 4; i++) {
        wheelSpeeds[i] = Sensors::getWheelSpeed(i);
    }
    
    // Calculate reference vehicle speed
    float vehicleSpeed = calculateVehicleSpeed(wheelSpeeds);
    
    // Check if speed is above minimum threshold
    if (vehicleSpeed < config.minSpeedKmh) {
        state.systemActive = false;
        for (int i = 0; i < 4; i++) {
            state.wheels[i].active = false;
            state.wheels[i].slipRatio = 0.0f;
        }
        return;
    }
    
    // Update slip ratio and ABS state for each wheel
    bool anyWheelActive = false;
    float totalSlip = 0.0f;
    int validWheels = 0;
    
    uint32_t now = millis();
    
    for (int i = 0; i < 4; i++) {
        if (!Sensors::isWheelSensorOk(i)) continue;
        
        float slip = calculateSlipRatio(wheelSpeeds[i], vehicleSpeed);
        state.wheels[i].slipRatio = slip;
        totalSlip += slip;
        validWheels++;
        
        // Check if ABS should activate on this wheel
        if (slip > config.slipThreshold) {
            if (!state.wheels[i].active) {
                // ABS activation
                state.wheels[i].active = true;
                state.wheels[i].lastCycleMs = now;
                state.wheels[i].pulsesApplied = 0;
                state.activationCount++;
                
                Logger::infof("ABS activated on wheel %d (slip: %.1f%%)", i, slip);
                Alerts::play(Audio::AUDIO_MODULO_OK); // Alert sound
            }
            
            anyWheelActive = true;
            
            // Apply ABS pulse cycle
            if (now - state.wheels[i].lastCycleMs >= config.cycleMs) {
                state.wheels[i].lastCycleMs = now;
                state.wheels[i].pulsesApplied++;
            }
        } else {
            // Deactivate ABS if slip is back under control
            if (state.wheels[i].active && slip < (config.slipThreshold * 0.7f)) {
                state.wheels[i].active = false;
                Logger::infof("ABS deactivated on wheel %d", i);
            }
        }
    }
    
    state.systemActive = anyWheelActive;
    state.avgSlipRatio = (validWheels > 0) ? (totalSlip / validWheels) : 0.0f;
}

void setEnabled(bool enabled) {
    config.enabled = enabled;
    Logger::infof("ABS %s", enabled ? "enabled" : "disabled");
}

const State& getState() {
    return state;
}

bool isActive() {
    return state.systemActive;
}

Config& getConfig() {
    return config;
}

float modulateBrake(int wheel, float brakeDemand) {
    if (!initialized || !config.enabled || wheel < 0 || wheel >= 4) {
        return brakeDemand;
    }
    
    // If ABS is active on this wheel, reduce brake pressure
    if (state.wheels[wheel].active) {
        return brakeDemand * (1.0f - config.pressureReduction);
    }
    
    return brakeDemand;
}

} // namespace ABSSystem

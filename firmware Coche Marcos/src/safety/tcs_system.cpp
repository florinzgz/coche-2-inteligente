#include "tcs_system.h"
#include "wheels.h"
#include "logger.h"
#include "alerts.h"
#include <Arduino.h>

namespace TCSSystem {
    
    // Default configuration
    static Config config = {
        .enabled = true,
        .slipThreshold = 20.0f,       // Trigger TCS at 20% slip
        .minSpeedKmh = 5.0f,          // Only activate above 5 km/h
        .reductionRate = 5.0f,        // Reduce power by 5% per cycle
        .recoveryRate = 2.0f,         // Recover power by 2% per cycle
        .updateIntervalMs = 50        // Update at 20 Hz
    };
    
    // Current state
    static State state = {0};
    
    // Internal state
    static bool initialized = false;
    static float vehicleSpeedKmh = 0.0f;
    static uint32_t lastUpdateMs = 0;
    
    // Calculate vehicle reference speed (slowest wheel speed)
    // For TCS, we use the slowest wheel as reference to detect wheel spin
    static float calculateVehicleSpeed() {
        float minSpeed = 999.0f;
        bool foundValid = false;
        
        for(int i = 0; i < 4; i++) {
            if(Sensors::isWheelSensorOk(i)) {
                float wheelSpeed = Sensors::getWheelSpeed(i);
                if(!state.wheels[i].active) {  // Don't use slipping wheels as reference
                    if(wheelSpeed < minSpeed) {
                        minSpeed = wheelSpeed;
                        foundValid = true;
                    }
                }
            }
        }
        
        return foundValid ? minSpeed : 0.0f;
    }
    
    // Calculate slip ratio for a wheel (positive slip = wheel spinning faster)
    // Slip ratio = (wheel_speed - vehicle_speed) / vehicle_speed * 100
    static float calculateSlipRatio(int wheel, float vehSpeed) {
        if(vehSpeed < 0.1f) return 0.0f;
        
        float wheelSpeed = Sensors::getWheelSpeed(wheel);
        if(!Sensors::isWheelSensorOk(wheel)) return 0.0f;
        
        float slip = ((wheelSpeed - vehSpeed) / vehSpeed) * 100.0f;
        return constrain(slip, -100.0f, 100.0f);
    }
    
    void init() {
        Logger::info("Initializing TCS system...");
        
        memset(&state, 0, sizeof(State));
        
        for(int i = 0; i < 4; i++) {
            state.wheels[i].active = false;
            state.wheels[i].slipRatio = 0.0f;
            state.wheels[i].powerReduction = 0.0f;
            state.wheels[i].lastActiveMs = 0;
        }
        
        state.systemActive = false;
        state.activationCount = 0;
        state.avgSlipRatio = 0.0f;
        state.totalPowerReduction = 0.0f;
        
        initialized = true;
        Logger::info("TCS system initialized");
    }
    
    void update() {
        if(!initialized || !config.enabled) {
            state.systemActive = false;
            return;
        }
        
        uint32_t now = millis();
        
        // Throttle updates
        if(now - lastUpdateMs < config.updateIntervalMs) {
            return;
        }
        
        // Update vehicle reference speed
        vehicleSpeedKmh = calculateVehicleSpeed();
        
        // Don't activate TCS below minimum speed
        if(vehicleSpeedKmh < config.minSpeedKmh) {
            state.systemActive = false;
            for(int i = 0; i < 4; i++) {
                state.wheels[i].active = false;
                // Gradually recover power reduction
                if(state.wheels[i].powerReduction > 0) {
                    state.wheels[i].powerReduction -= config.recoveryRate;
                    if(state.wheels[i].powerReduction < 0) {
                        state.wheels[i].powerReduction = 0;
                    }
                }
            }
            return;
        }
        
        // Check each wheel for excessive slip
        bool anyActive = false;
        float totalSlip = 0.0f;
        float totalReduction = 0.0f;
        int activeCount = 0;
        
        for(int i = 0; i < 4; i++) {
            if(!Sensors::isWheelSensorOk(i)) {
                state.wheels[i].active = false;
                continue;
            }
            
            // Calculate current slip ratio
            float slip = calculateSlipRatio(i, vehicleSpeedKmh);
            state.wheels[i].slipRatio = slip;
            
            // Check if slip exceeds threshold (positive slip = wheel spinning)
            if(slip > config.slipThreshold) {
                if(!state.wheels[i].active) {
                    // TCS activation on this wheel
                    state.wheels[i].active = true;
                    state.wheels[i].lastActiveMs = now;
                    state.activationCount++;
                    
                    Logger::infof("TCS activated on wheel %d (slip: %.1f%%)", i, slip);
                }
                
                // Increase power reduction
                state.wheels[i].powerReduction += config.reductionRate;
                if(state.wheels[i].powerReduction > 100.0f) {
                    state.wheels[i].powerReduction = 100.0f;
                }
                
                anyActive = true;
                activeCount++;
                totalSlip += slip;
                totalReduction += state.wheels[i].powerReduction;
                
            } else if(slip < config.slipThreshold * 0.6f) {
                // Slip under control, start recovering power
                if(state.wheels[i].active) {
                    Logger::infof("TCS deactivating on wheel %d", i);
                    state.wheels[i].active = false;
                }
                
                // Gradually recover power
                if(state.wheels[i].powerReduction > 0) {
                    state.wheels[i].powerReduction -= config.recoveryRate;
                    if(state.wheels[i].powerReduction < 0) {
                        state.wheels[i].powerReduction = 0;
                    }
                }
            }
        }
        
        // Update system state
        bool wasActive = state.systemActive;
        state.systemActive = anyActive;
        state.avgSlipRatio = (activeCount > 0) ? (totalSlip / activeCount) : 0.0f;
        state.totalPowerReduction = totalReduction;
        
        // Audio feedback on activation/deactivation
        if(state.systemActive && !wasActive) {
            Alerts::play(Audio::AUDIO_ALERTA_TRACCION);
        }
        
        lastUpdateMs = now;
    }
    
    void setEnabled(bool enabled) {
        config.enabled = enabled;
        if(!enabled) {
            state.systemActive = false;
            for(int i = 0; i < 4; i++) {
                state.wheels[i].active = false;
                state.wheels[i].powerReduction = 0.0f;
            }
        }
        Logger::infof("TCS %s", enabled ? "enabled" : "disabled");
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
    
    float modulatePower(int wheel, float powerDemand) {
        if(!initialized || !config.enabled || wheel < 0 || wheel >= 4) {
            return powerDemand;
        }
        
        // Apply power reduction if TCS is active or recovering
        if(state.wheels[wheel].powerReduction > 0) {
            float reduction = state.wheels[wheel].powerReduction / 100.0f;
            return powerDemand * (1.0f - reduction);
        }
        
        return powerDemand;
    }
}

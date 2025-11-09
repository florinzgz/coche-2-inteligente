#include "bluetooth_controller.h"
#include "config_storage.h"
#include "power_mgmt.h"
#include "audio.h"
#include <Preferences.h>

// Static member initialization
BluetoothSerial BluetoothController::btSerial;
ControllerState BluetoothController::state = {};
unsigned long BluetoothController::last_update_ms = 0;
String BluetoothController::paired_device_mac = "";

// Connection timeout (2 seconds)
#define BT_TIMEOUT_MS 2000
#define BT_UPDATE_INTERVAL_MS 100  // 10Hz

void BluetoothController::init() {
    // Initialize Bluetooth Serial
    btSerial.begin("Smart Car ESP32");
    
    // Load paired device from EEPROM
    loadPairing();
    
    // Initialize state
    state.connected = false;
    state.override_active = false;
    state.mode = OverrideMode::NONE;
    state.last_packet_ms = 0;
    state.override_start_ms = 0;
    
    // Zero out joystick values
    state.left_x = 0;
    state.left_y = 0;
    state.right_x = 0;
    state.right_y = 0;
    
    // Clear button states
    state.button_a = false;
    state.button_b = false;
    state.button_x = false;
    state.button_y = false;
    state.button_start = false;
    state.button_select = false;
    
    Serial.println("Bluetooth Controller initialized - waiting for 8BitDo Zero 2");
}

void BluetoothController::update() {
    unsigned long now = millis();
    
    // Rate limit updates to 10Hz
    if (now - last_update_ms < BT_UPDATE_INTERVAL_MS) {
        return;
    }
    last_update_ms = now;
    
    // Check for incoming Bluetooth data
    if (btSerial.available()) {
        uint8_t buffer[32];
        size_t len = 0;
        
        while (btSerial.available() && len < sizeof(buffer)) {
            buffer[len++] = btSerial.read();
        }
        
        if (len > 0) {
            processPacket(buffer, len);
            state.last_packet_ms = now;
            state.connected = true;
        }
    }
    
    // Check connection timeout
    checkConnectionTimeout();
}

bool BluetoothController::isConnected() {
    return state.connected;
}

bool BluetoothController::startPairing() {
    Serial.println("Starting Bluetooth pairing mode...");
    // ESP32 Bluetooth is already discoverable
    return true;
}

void BluetoothController::disconnect() {
    btSerial.disconnect();
    state.connected = false;
    clearOverride();
    Serial.println("Bluetooth controller disconnected");
}

bool BluetoothController::isOverrideActive() {
    return state.override_active;
}

OverrideMode BluetoothController::getOverrideMode() {
    return state.mode;
}

void BluetoothController::clearOverride() {
    state.override_active = false;
    state.mode = OverrideMode::NONE;
    state.override_start_ms = 0;
}

int8_t BluetoothController::getSteeringOverride() {
    if (!state.override_active || state.mode == OverrideMode::EMERGENCY_STOP) {
        return 0;
    }
    return applyDeadzone(state.left_x);
}

int8_t BluetoothController::getThrottleOverride() {
    if (!state.override_active || state.mode == OverrideMode::EMERGENCY_STOP) {
        return 0;
    }
    return applyDeadzone(state.right_y);
}

bool BluetoothController::isEmergencyStop() {
    return state.mode == OverrideMode::EMERGENCY_STOP;
}

int BluetoothController::getGearOverride() {
    if (!state.override_active) {
        return 0; // No override
    }
    
    // X button forces Park
    if (state.button_x) {
        return 1; // Park
    }
    
    return 0; // No gear override
}

ControllerState& BluetoothController::getState() {
    return state;
}

void BluetoothController::savePairing() {
    Preferences prefs;
    prefs.begin("bt_ctrl", false);
    prefs.putString("mac", paired_device_mac);
    prefs.end();
}

void BluetoothController::loadPairing() {
    Preferences prefs;
    prefs.begin("bt_ctrl", true);
    paired_device_mac = prefs.getString("mac", "");
    prefs.end();
    
    if (paired_device_mac.length() > 0) {
        Serial.print("Loaded paired device MAC: ");
        Serial.println(paired_device_mac);
    }
}

void BluetoothController::processPacket(uint8_t* data, size_t len) {
    // 8BitDo Zero 2 sends HID gamepad reports
    // This is a simplified parser - actual implementation depends on controller mode
    
    if (len < 8) return;
    
    // Parse joystick values (bytes 2-5, assuming standard HID report)
    state.left_x = map(data[2], 0, 255, -100, 100);
    state.left_y = map(data[3], 0, 255, -100, 100);
    state.right_x = map(data[4], 0, 255, -100, 100);
    state.right_y = map(data[5], 0, 255, -100, 100);
    
    // Parse button states (byte 6-7, bit flags)
    uint16_t buttons = (data[7] << 8) | data[6];
    state.button_a = buttons & 0x0001;
    state.button_b = buttons & 0x0002;
    state.button_x = buttons & 0x0004;
    state.button_y = buttons & 0x0008;
    state.button_start = buttons & 0x0010;
    state.button_select = buttons & 0x0020;
    
    // Handle emergency stop button (A)
    if (state.button_a && !state.override_active) {
        handleEmergencyStop();
    }
    
    // Handle resume button (B)
    if (state.button_b && state.override_active) {
        handleResume();
    }
    
    // Activate override if joysticks moved significantly
    if (!state.override_active) {
        if (abs(state.left_x) > 30 || abs(state.right_y) > 30) {
            state.override_active = true;
            state.mode = OverrideMode::FULL;
            state.override_start_ms = millis();
            Audio::playAlert(Audio::Alert::WARNING);
            Serial.println("Bluetooth override ACTIVE");
        }
    }
}

void BluetoothController::handleEmergencyStop() {
    state.override_active = true;
    state.mode = OverrideMode::EMERGENCY_STOP;
    state.override_start_ms = millis();
    
    // Cut all motors immediately
    PowerMgmt::emergencyStop();
    
    // Play emergency alert
    Audio::playAlert(Audio::Alert::CRITICAL);
    
    Serial.println("EMERGENCY STOP activated via Bluetooth!");
}

void BluetoothController::handleResume() {
    clearOverride();
    Audio::playAlert(Audio::Alert::INFO);
    Serial.println("Bluetooth override cleared - normal operation resumed");
}

int8_t BluetoothController::applyDeadzone(int8_t value, int8_t deadzone) {
    if (abs(value) < deadzone) {
        return 0;
    }
    return value;
}

void BluetoothController::checkConnectionTimeout() {
    if (!state.connected) return;
    
    unsigned long now = millis();
    
    // Check if we haven't received data in a while
    if (now - state.last_packet_ms > BT_TIMEOUT_MS) {
        Serial.println("Bluetooth connection timeout");
        state.connected = false;
        
        // Dead man's switch: if override was active, emergency stop
        if (state.override_active) {
            handleEmergencyStop();
            Serial.println("Dead man's switch activated - connection lost during override!");
        }
    }
}

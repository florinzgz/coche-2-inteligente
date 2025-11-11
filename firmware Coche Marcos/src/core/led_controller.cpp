#include "led_controller.h"
#include "pins.h"

// LED arrays
static CRGB frontLeds[LED_FRONT_COUNT];
static CRGB rearLeds[LED_REAR_COUNT];

// Internal state
namespace LEDController {
    static FrontMode currentFrontMode = FRONT_OFF;
    static RearMode currentRearMode = REAR_OFF;
    static TurnSignal currentTurnSignal = TURN_OFF;
    static Config config = {128, 20, true};
    static bool enabled = true;
    static unsigned long lastUpdate = 0;
    static unsigned long lastTurnBlink = 0;
    static bool turnBlinkState = false;
    static uint16_t animationStep = 0;
    
    void init() {
        FastLED.addLeds<WS2812B, LED_FRONT_PIN, GRB>(frontLeds, LED_FRONT_COUNT);
        FastLED.addLeds<WS2812B, LED_REAR_PIN, GRB>(rearLeds, LED_REAR_COUNT);
        FastLED.setBrightness(config.brightness);
        FastLED.clear(true);
    }
    
    void update() {
        if (!enabled) return;
        
        unsigned long now = millis();
        if (now - lastUpdate < config.updateRateMs) return;
        lastUpdate = now;
        
        // Update turn signal blink (500ms period)
        if (now - lastTurnBlink >= 500) {
            lastTurnBlink = now;
            turnBlinkState = !turnBlinkState;
        }
        
        animationStep++;
        
        // Update front LEDs based on mode
        updateFrontLEDs();
        
        // Update rear LEDs based on mode
        updateRearLEDs();
        
        // Apply turn signals
        updateTurnSignals();
        
        FastLED.show();
    }
    
    void updateFrontLEDs() {
        switch (currentFrontMode) {
            case FRONT_OFF:
                fill_solid(frontLeds, LED_FRONT_COUNT, CRGB::Black);
                break;
                
            case FRONT_KITT_IDLE: {
                // KITT scanner effect
                uint8_t pos = beatsin8(30, 0, LED_FRONT_COUNT - 1);
                fill_solid(frontLeds, LED_FRONT_COUNT, CRGB::Black);
                frontLeds[pos] = CRGB::Red;
                if (pos > 0) frontLeds[pos-1] = CRGB(64, 0, 0);
                if (pos < LED_FRONT_COUNT - 1) frontLeds[pos+1] = CRGB(64, 0, 0);
                break;
            }
            
            case FRONT_ACCEL_LOW:
                fill_solid(frontLeds, LED_FRONT_COUNT, CRGB::OrangeRed);
                break;
                
            case FRONT_ACCEL_MED:
                fill_solid(frontLeds, LED_FRONT_COUNT, CRGB::Orange);
                break;
                
            case FRONT_ACCEL_HIGH:
                fill_solid(frontLeds, LED_FRONT_COUNT, CRGB::Yellow);
                break;
                
            case FRONT_ACCEL_MAX:
                fill_rainbow(frontLeds, LED_FRONT_COUNT, animationStep * 2);
                break;
                
            case FRONT_REVERSE: {
                // White scanner
                uint8_t pos = beatsin8(40, 0, LED_FRONT_COUNT - 1);
                fill_solid(frontLeds, LED_FRONT_COUNT, CRGB::Black);
                frontLeds[pos] = CRGB::White;
                if (pos > 0) frontLeds[pos-1] = CRGB(64, 64, 64);
                if (pos < LED_FRONT_COUNT - 1) frontLeds[pos+1] = CRGB(64, 64, 64);
                break;
            }
            
            case FRONT_ABS_ALERT:
                // Red/white flash
                fill_solid(frontLeds, LED_FRONT_COUNT, (animationStep & 0x08) ? CRGB::Red : CRGB::White);
                break;
                
            case FRONT_TCS_ALERT:
                // Amber flash
                fill_solid(frontLeds, LED_FRONT_COUNT, (animationStep & 0x08) ? CRGB::Orange : CRGB::Black);
                break;
        }
    }
    
    void updateRearLEDs() {
        switch (currentRearMode) {
            case REAR_OFF:
                fill_solid(rearLeds, LED_REAR_COUNT, CRGB::Black);
                break;
                
            case REAR_POSITION:
                // Center red at 20%
                for (int i = LED_REAR_CENTER_START; i <= LED_REAR_CENTER_END; i++) {
                    rearLeds[i] = CRGB(51, 0, 0);  // ~20% of 255
                }
                break;
                
            case REAR_BRAKE:
                // Center red at 100%
                for (int i = LED_REAR_CENTER_START; i <= LED_REAR_CENTER_END; i++) {
                    rearLeds[i] = CRGB::Red;
                }
                break;
                
            case REAR_BRAKE_EMERGENCY:
                // Flashing red
                for (int i = LED_REAR_CENTER_START; i <= LED_REAR_CENTER_END; i++) {
                    rearLeds[i] = (animationStep & 0x04) ? CRGB::Red : CRGB::Black;
                }
                break;
                
            case REAR_REVERSE:
                // Center white
                for (int i = LED_REAR_CENTER_START; i <= LED_REAR_CENTER_END; i++) {
                    rearLeds[i] = CRGB::White;
                }
                break;
                
            case REAR_REGEN_ACTIVE: {
                // Blue pulse
                uint8_t brightness = beatsin8(60, 50, 255);
                for (int i = LED_REAR_CENTER_START; i <= LED_REAR_CENTER_END; i++) {
                    rearLeds[i] = CRGB(0, 0, brightness);
                }
                break;
            }
        }
    }
    
    void updateTurnSignals() {
        bool showLeft = (currentTurnSignal == TURN_LEFT || currentTurnSignal == TURN_HAZARD) && turnBlinkState;
        bool showRight = (currentTurnSignal == TURN_RIGHT || currentTurnSignal == TURN_HAZARD) && turnBlinkState;
        
        // Left turn signal
        for (int i = LED_REAR_LEFT_START; i <= LED_REAR_LEFT_END; i++) {
            rearLeds[i] = showLeft ? CRGB::Orange : CRGB::Black;
        }
        
        // Right turn signal
        for (int i = LED_REAR_RIGHT_START; i <= LED_REAR_RIGHT_END; i++) {
            rearLeds[i] = showRight ? CRGB::Orange : CRGB::Black;
        }
    }
    
    void setFrontMode(FrontMode mode) {
        currentFrontMode = mode;
    }
    
    void setFrontFromThrottle(float throttlePercent) {
        if (throttlePercent < 0) {
            setFrontMode(FRONT_REVERSE);
        } else if (throttlePercent == 0) {
            setFrontMode(FRONT_KITT_IDLE);
        } else if (throttlePercent < 25.0f) {
            setFrontMode(FRONT_ACCEL_LOW);
        } else if (throttlePercent < 50.0f) {
            setFrontMode(FRONT_ACCEL_MED);
        } else if (throttlePercent < 75.0f) {
            setFrontMode(FRONT_ACCEL_HIGH);
        } else {
            setFrontMode(FRONT_ACCEL_MAX);
        }
    }
    
    void setRearMode(RearMode mode) {
        currentRearMode = mode;
    }
    
    void setTurnSignal(TurnSignal signal) {
        currentTurnSignal = signal;
    }
    
    void setBrightness(uint8_t brightness) {
        config.brightness = brightness;
        FastLED.setBrightness(brightness);
    }
    
    void setEnabled(bool enable) {
        enabled = enable;
        if (!enabled) {
            FastLED.clear(true);
        }
    }
    
    Config& getConfig() {
        return config;
    }
    
    void emergencyFlash(uint8_t count) {
        for (uint8_t i = 0; i < count; i++) {
            fill_solid(frontLeds, LED_FRONT_COUNT, CRGB::Red);
            fill_solid(rearLeds, LED_REAR_COUNT, CRGB::Red);
            FastLED.show();
            delay(100);
            FastLED.clear(true);
            delay(100);
        }
    }
}

#include "led_controller.h"
#include "pins.h"

#define NUM_LEDS 30  // Adjust based on actual LED count
#define LED_TYPE WS2812B
#define COLOR_ORDER GRB

// Static member initialization
LEDPattern LEDController::currentPattern = PATTERN_RAINBOW;
uint32_t LEDController::currentColor = 0xFF0000; // Default red
uint8_t LEDController::currentBrightness = 128;
uint8_t LEDController::currentSpeed = 128;
bool LEDController::enabled = true;
unsigned long LEDController::lastUpdate = 0;
uint16_t LEDController::animationStep = 0;

static CRGB leds[NUM_LEDS];

void LEDController::init() {
    FastLED.addLeds<LED_TYPE, PIN_LEDS, COLOR_ORDER>(leds, NUM_LEDS);
    FastLED.setBrightness(currentBrightness);
    FastLED.clear();
    FastLED.show();
}

void LEDController::update() {
    if (!enabled) {
        return;
    }
    
    unsigned long currentTime = millis();
    uint8_t delay = calculateDelay();
    
    if (currentTime - lastUpdate < delay) {
        return;
    }
    
    lastUpdate = currentTime;
    animationStep++;
    
    // Call appropriate pattern update function
    switch (currentPattern) {
        case PATTERN_OFF:
            updateOff();
            break;
        case PATTERN_SOLID:
            updateSolid();
            break;
        case PATTERN_RAINBOW:
            updateRainbow();
            break;
        case PATTERN_FADE:
            updateFade();
            break;
        case PATTERN_STROBE:
            updateStrobe();
            break;
        case PATTERN_CHASE:
            updateChase();
            break;
        case PATTERN_SCANNER:
            updateScanner();
            break;
        case PATTERN_TWINKLE:
            updateTwinkle();
            break;
    }
    
    FastLED.show();
}

void LEDController::setPattern(LEDPattern pattern) {
    if (pattern < PATTERN_COUNT) {
        currentPattern = pattern;
        animationStep = 0;
        FastLED.clear();
    }
}

LEDPattern LEDController::getPattern() {
    return currentPattern;
}

const char* LEDController::getPatternName(LEDPattern pattern) {
    switch (pattern) {
        case PATTERN_OFF: return "Off";
        case PATTERN_SOLID: return "Solid";
        case PATTERN_RAINBOW: return "Rainbow";
        case PATTERN_FADE: return "Fade";
        case PATTERN_STROBE: return "Strobe";
        case PATTERN_CHASE: return "Chase";
        case PATTERN_SCANNER: return "Scanner";
        case PATTERN_TWINKLE: return "Twinkle";
        default: return "Unknown";
    }
}

void LEDController::setColor(uint32_t rgb) {
    currentColor = rgb;
}

uint32_t LEDController::getColor() {
    return currentColor;
}

void LEDController::setColorRGB(uint8_t r, uint8_t g, uint8_t b) {
    currentColor = ((uint32_t)r << 16) | ((uint32_t)g << 8) | b;
}

void LEDController::setBrightness(uint8_t brightness) {
    currentBrightness = brightness;
    FastLED.setBrightness(brightness);
}

uint8_t LEDController::getBrightness() {
    return currentBrightness;
}

void LEDController::setSpeed(uint8_t speed) {
    currentSpeed = speed;
}

uint8_t LEDController::getSpeed() {
    return currentSpeed;
}

void LEDController::enable() {
    enabled = true;
}

void LEDController::disable() {
    enabled = false;
    FastLED.clear();
    FastLED.show();
}

bool LEDController::isEnabled() {
    return enabled;
}

uint8_t LEDController::calculateDelay() {
    // Convert speed (0-255) to delay (200ms-10ms)
    // Higher speed = lower delay = faster animation
    return map(currentSpeed, 0, 255, 200, 10);
}

CRGB LEDController::wheel(uint8_t pos) {
    // Color wheel for rainbow effects
    if (pos < 85) {
        return CRGB(pos * 3, 255 - pos * 3, 0);
    } else if (pos < 170) {
        pos -= 85;
        return CRGB(255 - pos * 3, 0, pos * 3);
    } else {
        pos -= 170;
        return CRGB(0, pos * 3, 255 - pos * 3);
    }
}

// Pattern implementations
void LEDController::updateOff() {
    FastLED.clear();
}

void LEDController::updateSolid() {
    uint8_t r = (currentColor >> 16) & 0xFF;
    uint8_t g = (currentColor >> 8) & 0xFF;
    uint8_t b = currentColor & 0xFF;
    
    fill_solid(leds, NUM_LEDS, CRGB(r, g, b));
}

void LEDController::updateRainbow() {
    for (int i = 0; i < NUM_LEDS; i++) {
        leds[i] = wheel(((i * 256 / NUM_LEDS) + animationStep) & 255);
    }
}

void LEDController::updateFade() {
    uint8_t r = (currentColor >> 16) & 0xFF;
    uint8_t g = (currentColor >> 8) & 0xFF;
    uint8_t b = currentColor & 0xFF;
    
    uint8_t brightness = beatsin8(map(currentSpeed, 0, 255, 10, 60), 0, 255);
    
    CRGB color = CRGB(r, g, b);
    color.nscale8(brightness);
    
    fill_solid(leds, NUM_LEDS, color);
}

void LEDController::updateStrobe() {
    uint8_t r = (currentColor >> 16) & 0xFF;
    uint8_t g = (currentColor >> 8) & 0xFF;
    uint8_t b = currentColor & 0xFF;
    
    if (animationStep % 2 == 0) {
        fill_solid(leds, NUM_LEDS, CRGB(r, g, b));
    } else {
        FastLED.clear();
    }
}

void LEDController::updateChase() {
    FastLED.clear();
    
    uint8_t r = (currentColor >> 16) & 0xFF;
    uint8_t g = (currentColor >> 8) & 0xFF;
    uint8_t b = currentColor & 0xFF;
    
    int pos = animationStep % NUM_LEDS;
    leds[pos] = CRGB(r, g, b);
    
    // Tail
    for (int i = 1; i < 5; i++) {
        int tailPos = (pos - i + NUM_LEDS) % NUM_LEDS;
        leds[tailPos] = CRGB(r >> i, g >> i, b >> i);
    }
}

void LEDController::updateScanner() {
    FastLED.clear();
    
    uint8_t r = (currentColor >> 16) & 0xFF;
    uint8_t g = (currentColor >> 8) & 0xFF;
    uint8_t b = currentColor & 0xFF;
    
    int pos = beatsin16(map(currentSpeed, 0, 255, 10, 60), 0, NUM_LEDS - 1);
    leds[pos] = CRGB(r, g, b);
    
    // Fade all LEDs
    fadeToBlackBy(leds, NUM_LEDS, 20);
}

void LEDController::updateTwinkle() {
    // Fade all LEDs slightly
    fadeToBlackBy(leds, NUM_LEDS, 10);
    
    // Randomly light up LEDs
    if (random8() < map(currentSpeed, 0, 255, 5, 50)) {
        int pos = random16(NUM_LEDS);
        uint8_t r = (currentColor >> 16) & 0xFF;
        uint8_t g = (currentColor >> 8) & 0xFF;
        uint8_t b = currentColor & 0xFF;
        leds[pos] = CRGB(r, g, b);
    }
}

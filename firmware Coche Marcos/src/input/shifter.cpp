#include "shifter.h"
#include "pins.h"
#include "logger.h"
#include "dfplayer.h"
#include "alerts.h"

static Shifter::State s = {Shifter::P, false};

// Ejemplo: 5 entradas discretas con pull-up (LOW activo)
static bool readPin(uint8_t pin) { return digitalRead(pin) == 0; }

static void announce(Shifter::Gear g) {
    // Use generic AUDIO_MODULO_OK for now - specific gear tracks need to be defined
    (void)g; // Suppress unused warning
    Alerts::play(Audio::AUDIO_MODULO_OK);
    // TODO: Add specific gear audio tracks to alerts.h:
    // AUDIO_MARCHA_P, AUDIO_MARCHA_D2, AUDIO_MARCHA_D1, AUDIO_MARCHA_N, AUDIO_MARCHA_R
}

void Shifter::init() {
    // Define pines reales en pins.h (aquí ejemplos)
    pinMode(PIN_TFT_CS, INPUT_PULLUP);  // P
    pinMode(PIN_TFT_DC, INPUT_PULLUP);  // D2
    pinMode(PIN_TFT_RST, INPUT_PULLUP); // D1
    pinMode(PIN_DFPLAYER_RX, INPUT_PULLUP); // N
    pinMode(PIN_DFPLAYER_TX, INPUT_PULLUP); // R
    Logger::info("Shifter init");
}

void Shifter::update() {
    Shifter::Gear newGear = s.gear;

    if(readPin(PIN_TFT_CS))       newGear = Shifter::P;
    else if(readPin(PIN_TFT_DC))  newGear = Shifter::D2;
    else if(readPin(PIN_TFT_RST)) newGear = Shifter::D1;
    else if(readPin(PIN_DFPLAYER_RX)) newGear = Shifter::N;
    else if(readPin(PIN_DFPLAYER_TX)) newGear = Shifter::R;

    if(newGear != s.gear) {
        s.gear = newGear;
        s.changed = true;
        announce(newGear);
        Logger::info("Gear changed");
    } else {
        s.changed = false;
    }
}

Shifter::State Shifter::get() { return s; }
void Shifter::setGear(Shifter::Gear g) { s.gear = g; s.changed = true; announce(g); }
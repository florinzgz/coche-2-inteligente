#pragma once
#include <Arduino.h>

namespace Relays {
    struct State {
        bool mainOn;
        bool tractionOn;
        bool steeringOn;
        bool lightsOn;
        bool mediaOn;
    };

    void init();
    void enablePower();  // principal + tracción + dirección
    void disablePower();

    void setLights(bool on);
    void setMedia(bool on);

    void update();
    const State& get();

    // 🔎 Nuevo: estado de inicialización
    bool initOK();
}
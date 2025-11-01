#include "sensors.h"
#include "logger.h"

namespace Sensors {

    static bool ok = false;

    void init() {
        // Aquí inicializarías tus sensores reales
        ok = true;
        Logger::info("Sensors init OK");
    }

    void update() {
        // Aquí refrescarías lecturas reales
    }

    bool initOK() {
        return ok;
    }

    float getWheelSpeed(int idx) {
        // Stub: devuelve 0.0 km/h
        // Sustituir por lectura real de rueda idx
        return 0.0f;
    }

    float getVoltage(int idx) {
        // Stub: devuelve 12.5 V
        // Sustituir por lectura real de batería idx
        return 12.5f;
    }
}
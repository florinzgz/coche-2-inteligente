#include "touch_map.h"
#include "icons.h"   // para las coordenadas de cada icono
#include "logger.h"

TouchAction getTouchedZone(int x, int y) {
    if (x >= BATTERY_X1 && x <= BATTERY_X2 && y >= BATTERY_Y1 && y <= BATTERY_Y2)
        return TouchAction::Battery;
    if (x >= LIGHTS_X1 && x <= LIGHTS_X2 && y >= LIGHTS_Y1 && y <= LIGHTS_Y2)
        return TouchAction::Lights;
    if (x >= MEDIA_X1 && x <= MEDIA_X2 && y >= MEDIA_Y1 && y <= MEDIA_Y2)
        return TouchAction::Multimedia;
    if (x >= MODE4X4_X1 && x <= MODE4X4_X2 && y >= MODE4X4_Y1 && y <= MODE4X4_Y2)
        return TouchAction::Mode4x4;
    if (x >= WARNING_X1 && x <= WARNING_X2 && y >= WARNING_Y1 && y <= WARNING_Y2)
        return TouchAction::Warning;

    return TouchAction::None;
}

// Ejemplo de uso
void checkTouch(int x, int y) {
    TouchAction action = getTouchedZone(x, y);
    switch(action) {
        case TouchAction::Battery:
            Logger::info("Tocado: Battery");
            break;
        case TouchAction::Lights:
            Logger::info("Tocado: Lights");
            break;
        case TouchAction::Multimedia:
            Logger::info("Tocado: Multimedia");
            break;
        case TouchAction::Mode4x4:
            Logger::info("Tocado: 4x4");
            break;
        case TouchAction::Warning:
            Logger::info("Tocado: Warning");
            break;
        default:
            // Nada
            break;
    }
}
#pragma once
#include "queue.h"

namespace Audio {
    // Definición de los tracks de audio disponibles
    enum Track : uint16_t {
        AUDIO_INICIO = 1,
        AUDIO_MODULO_OK = 2,
        AUDIO_ERROR_GENERAL = 3,
        AUDIO_MENU_OCULTO = 4,
        // Agregar más tracks según sea necesario
    };
}

class Alerts {
public:
    static void init();
    static void play(const Audio::Item &item);
    static void play(Audio::Track t);

    // 🔎 Declaración añadida para que coincida con alerts.cpp
    static bool initOK();
};
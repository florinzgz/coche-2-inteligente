#pragma once
#include "queue.h"

namespace Audio {
    enum Track : uint16_t; // forward-declaration si Track está definido en otro sitio
}

class Alerts {
public:
    static void init();
    static void play(const Audio::Item &item);
    static void play(Audio::Track t);

    // 🔎 Declaración añadida para que coincida con alerts.cpp
    static bool initOK();
};
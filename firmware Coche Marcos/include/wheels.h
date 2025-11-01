#pragma once
namespace Sensors {
    // Número de ruedas
    constexpr int NUM_WHEELS = 4;

    // Inicialización y actualización
    void initWheels();
    void updateWheels();

    // 🔎 Nuevo: estado de inicialización global de sensores de rueda
    bool wheelsInitOK();

    // Velocidad de rueda en km/h
    // Índices:
    // 0 = FL (Front Left)
    // 1 = FR (Front Right)
    // 2 = RL (Rear Left)
    // 3 = RR (Rear Right)
    float getWheelSpeed(int wheel);

    // Distancia acumulada en milímetros
    unsigned long getWheelDistance(int wheel);
}
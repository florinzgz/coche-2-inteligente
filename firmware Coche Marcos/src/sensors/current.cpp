#include "current.h"
#include <Wire.h>
#include <INA226.h>
#include "logger.h"
#include "storage.h"
#include "settings.h"
#include "system.h"   // para logError()

#define TCA_ADDR 0x70   // Dirección I2C del TCA9548A

extern Storage::Config cfg;

// INA226 sensors - will be initialized in initCurrent()
// Using pointers to avoid constructor issues with array initialization
static INA226* ina[Sensors::NUM_CURRENTS] = {nullptr, nullptr, nullptr, nullptr, nullptr};
static bool sensorOk[Sensors::NUM_CURRENTS];
static float lastCurrent[Sensors::NUM_CURRENTS];
static float lastVoltage[Sensors::NUM_CURRENTS];
static float lastPower[Sensors::NUM_CURRENTS];
static float lastShunt[Sensors::NUM_CURRENTS];

static uint32_t lastUpdateMs = 0;

// Flag de inicialización global
static bool initialized = false;

// Selecciona canal del TCA9548A
static void tcaSelect(uint8_t channel) {
    if(channel > 7) return;
    Wire.beginTransmission(TCA_ADDR);
    Wire.write(1 << channel);
    Wire.endTransmission();
}

void Sensors::initCurrent() {
    Wire.begin();

    bool allOk = true;

    for(int i=0; i<NUM_CURRENTS; i++) {
        // Create INA226 object for this channel
        ina[i] = new INA226(0x40);  // Address will be selected via TCA9548A multiplexer
        
        tcaSelect(i);
        if(!ina[i]->begin()) {
            Logger::errorf("INA226 init fail ch %d", i);
            System::logError(300+i);   // registrar fallo persistente
            sensorOk[i] = false;
            allOk = false;
        } else {
            // Note: Some INA226 library methods may not be available
            // Uncomment and adjust these if your library supports them:
            // ina[i]->setShunt(cfg.shuntCoeff[i]);
            // ina[i]->setAverage(INA226_AVERAGES_16);
            // ina[i]->setConversionTime(INA226_CONV_TIME_1100US, INA226_CONV_TIME_1100US);
            // ina[i]->setMode(INA226_MODE_CONTINUOUS);
            sensorOk[i] = true;
            Logger::infof("INA226 init OK ch %d", i);
        }
        lastCurrent[i] = 0.0f;
        lastVoltage[i] = 0.0f;
        lastPower[i]   = 0.0f;
        lastShunt[i]   = 0.0f;
    }
    // Desactivar todos los canales
    Wire.beginTransmission(TCA_ADDR);
    Wire.write(0x00);
    Wire.endTransmission();

    initialized = allOk;
}

void Sensors::updateCurrent() {
    uint32_t now = millis();
    if(now - lastUpdateMs < 50) return; // ~20 Hz
    lastUpdateMs = now;

    if(!cfg.currentSensorsEnabled) {
        for(int i=0; i<NUM_CURRENTS; i++) {
            lastCurrent[i] = 0.0f;
            lastVoltage[i] = 0.0f;
            lastPower[i]   = 0.0f;
            lastShunt[i]   = 0.0f;
            sensorOk[i]    = false;
        }
        return;
    }

    for(int i=0; i<NUM_CURRENTS; i++) {
        if(!sensorOk[i] || !ina[i]) continue; // saltar si falló init o no está inicializado

        tcaSelect(i);

        float c = ina[i]->getCurrent();
        float v = ina[i]->getBusVoltage();
        float p = ina[i]->getPower();
        float s = ina[i]->getShuntVoltage();

        // Validación y fallback
        if(!isfinite(c)) { 
            c = 0.0f; 
            System::logError(310+i); 
            Logger::errorf("INA226 ch %d: corriente inválida", i);
        }
        if(!isfinite(v)) { 
            v = 0.0f; 
            System::logError(320+i); 
            Logger::errorf("INA226 ch %d: voltaje inválido", i);
        }
        if(!isfinite(p)) { 
            p = 0.0f; 
            System::logError(330+i); 
            Logger::errorf("INA226 ch %d: potencia inválida", i);
        }
        if(!isfinite(s)) { 
            s = 0.0f; 
            System::logError(340+i); 
            Logger::errorf("INA226 ch %d: shunt inválido", i);
        }

        // Clamps
        c = constrain(c, -CURR_MAX_BATT, CURR_MAX_BATT);
        v = constrain(v, 0.0f, 80.0f);
        p = constrain(p, -4000.0f, 4000.0f); // ejemplo
        s = constrain(s, -100.0f, 100.0f);

        // Filtro EMA para suavizar
        lastCurrent[i] = lastCurrent[i] + 0.2f * (c - lastCurrent[i]);
        lastVoltage[i] = lastVoltage[i] + 0.2f * (v - lastVoltage[i]);
        lastPower[i]   = lastPower[i]   + 0.2f * (p - lastPower[i]);
        lastShunt[i]   = lastShunt[i]   + 0.2f * (s - lastShunt[i]);
    }
}

float Sensors::getCurrent(int channel) {
    if(channel < NUM_CURRENTS) return lastCurrent[channel];
    return 0.0f;
}

float Sensors::getVoltage(int channel) {
    if(channel < NUM_CURRENTS) return lastVoltage[channel];
    return 0.0f;
}

float Sensors::getPower(int channel) {
    if(channel < NUM_CURRENTS) return lastPower[channel];
    return 0.0f;
}

float Sensors::getShuntVoltage(int channel) {
    if(channel < NUM_CURRENTS) return lastShunt[channel];
    return 0.0f;
}

bool Sensors::isCurrentSensorOk(int channel) {
    if(channel < NUM_CURRENTS) return sensorOk[channel];
    return false;
}

bool Sensors::currentInitOK() {
    return initialized;
}
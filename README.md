# Coche 2 Inteligente

Sistema de control inteligente para coche eléctrico basado en ESP32-S3 con funcionalidades avanzadas de automatización, monitoreo y control.

## Descripción

Coche 2 Inteligente es un proyecto de mejoras y optimización para un vehículo eléctrico infantil/educativo, incorporando un sistema completo de control basado en microcontrolador ESP32-S3 que gestiona:

- Control de motores de tracción (RS775 con reductora 1:75)
- Sistema de dirección asistida con motor RS390 y encoder de alta resolución (1200 PPR)
- Monitoreo en tiempo real de parámetros eléctricos
- Interfaz HUD con pantalla táctil TFT
- Sistema de audio con alertas y notificaciones

## Características Técnicas

### Hardware
- **Microcontrolador**: ESP32-S3 DevKitC-1 (240MHz, 16MB Flash)
- **Pantalla**: TFT con touch screen (XPT2046)
- **Motores de tracción**: RS775 (15000 RPM nominal) con reductora 1:75
- **Motor de dirección**: RS390 (6000 RPM nominal) con reductora 1:50
- **Encoder dirección**: 1200 PPR (Pulsos Por Revolución)
- **Sensores de rueda**: Inductivos, 6 pulsos por revolución

### Sensores y Monitoreo
- **Sensor de corriente**: INA226 (monitoreo de voltaje, corriente y potencia)
- **Sensores de temperatura**: Dallas DS18B20 (múltiples puntos de medición)
- **Sensores de rueda**: Para cálculo de velocidad y odometría
- **Pedal acelerador**: Analógico con filtrado EMA
- **Volante**: Encoder óptico de alta resolución

### Sistema de Control
- Control de tracción con diferencial electrónico (geometría Ackermann)
- Dirección asistida con control PID
- Gestión inteligente de energía
- Sistema de relés para control de potencia
- Watchdog para seguridad del sistema

### Interfaz de Usuario
- HUD gráfico con información en tiempo real:
  - Velocidad actual y máxima
  - Indicadores de batería, temperatura y corriente
  - Menú de configuración oculto
- Sistema de audio con DFPlayer Mini
- Alertas visuales y sonoras configurables

## Estructura del Proyecto

```
firmware Coche Marcos/
├── src/
│   ├── main.cpp              # Programa principal
│   ├── audio/                # Gestión de audio y alertas
│   ├── control/              # Control de motores y tracción
│   ├── core/                 # Sistema, almacenamiento, logger
│   ├── hud/                  # Interfaz gráfica
│   ├── input/                # Gestión de entradas (pedal, volante, botones)
│   ├── sensors/              # Lectura de sensores
│   └── utils/                # Utilidades (filtros, matemáticas, debug)
├── include/                  # Headers de configuración
│   ├── constants.h           # Constantes del vehículo
│   ├── pins.h               # Mapeo de pines
│   ├── settings.h           # Configuración del sistema
│   └── ...
├── lib/                      # Librerías personalizadas
├── data/                     # Recursos (imágenes, iconos)
├── audio/                    # Archivos de audio MP3
└── platformio.ini           # Configuración de PlatformIO
```

## Especificaciones del Vehículo

- **Distancia entre ejes**: 0.95 m
- **Ancho de vía**: 0.70 m
- **Ángulo máximo de dirección**: 54°
- **Circunferencia de rueda**: 1100 mm
- **Velocidad máxima teórica**: ~26 km/h
  - Cálculo: RPM motor (15000) / Reductora (75) = 200 RPM rueda
  - 200 RPM × 1.1m circunferencia × 60 = 13200 m/h = ~22 km/h (motores RS775)
  - Con margen de seguridad y eficiencia del sistema: ~26 km/h máximo

## Requisitos de Desarrollo

- **PlatformIO**: IDE recomendado (extensión para VS Code)
- **Framework**: Arduino para ESP32
- **Librerías principales**:
  - TFT_eSPI (pantalla TFT)
  - DFRobotDFPlayerMini (reproductor MP3)
  - DallasTemperature (sensores de temperatura)
  - INA226 (sensor de corriente)
  - Adafruit PWM Servo Driver
  - XPT2046_Touchscreen (pantalla táctil)

## Compilación y Carga

```bash
# Instalar PlatformIO Core
pip install platformio

# Compilar el proyecto
cd "firmware Coche Marcos"
pio run

# Cargar al ESP32-S3
pio run --target upload

# Monitor serial
pio device monitor
```

## Configuración

La configuración principal se encuentra en los archivos:
- `include/pins.h`: Asignación de pines GPIO
- `include/settings.h`: Parámetros de control y calibración
- `include/constants.h`: Constantes físicas del vehículo
- `platformio.ini`: Configuración de compilación y librerías

## Estado del Proyecto

✅ **Funcional** - El sistema está operativo con las siguientes características implementadas:

- [x] Control de motores de tracción con diferencial electrónico
- [x] Sistema de dirección asistida con encoder
- [x] Monitoreo de corriente, voltaje y temperatura
- [x] HUD con pantalla táctil
- [x] Sistema de audio y alertas
- [x] Gestión de seguridad con watchdog
- [x] Sistema de almacenamiento de configuración
- [x] Logger de eventos

## Autor

Proyecto desarrollado por Marcos para mejora de coche eléctrico infantil.

## Contribuir

Las contribuciones son bienvenidas. Por favor, abre un issue o pull request para sugerir mejoras.

### Áreas de Mejora Potencial

- Implementación de telemetría inalámbrica (WiFi/Bluetooth)
- Dashboard web para monitoreo remoto
- Optimización del consumo energético
- Sistema de navegación autónoma básico
- Mejoras en la interfaz HUD

## Licencia

Pendiente de definir.

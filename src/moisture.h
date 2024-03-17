#include <Arduino.h>
#include <ArduinoJson.h>
#define MOISTURE_SENSOR 14

enum MoistureMinMax
{
    dry = 2650,
    wet = 900
};

int GetMoisture();


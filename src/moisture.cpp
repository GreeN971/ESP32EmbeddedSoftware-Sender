#include "moisture.h"

int GetMoisture()
{
    int sensorVal = analogRead(MOISTURE_SENSOR);
    int percentageHumidity = map(sensorVal, wet, dry, 100, 0); //wet -200, dry + 90

    return percentageHumidity; 
} 

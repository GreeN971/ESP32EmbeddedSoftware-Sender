#include <ErriezDHT22.h>

#define DHT22_PIN 4

struct DhtSensor
{
    int16_t humidity;
    int16_t tempeture;
    int16_t pin;
};

DhtSensor &GetDHTSensor();
DHT22 &GetDHTObject();

void CheckHMObject();
void ConfigureHMSensor();
int ReadTemp();
int ReadHum();
void RefreshHMData();


    

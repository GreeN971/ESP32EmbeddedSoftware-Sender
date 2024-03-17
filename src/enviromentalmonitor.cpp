#include "enviromentalmonitor.h"

void CheckHMObject()
{
    if(!GetDHTObject().available())
       Serial.println("DHT22 is not avaible");
}

void ConfigureHMSensor()
{
    GetDHTSensor().pin = DHT22_PIN;
    GetDHTSensor().humidity = ReadHum();
    GetDHTSensor().tempeture = ReadTemp();
}

int ReadTemp()
{
    return GetDHTObject().readTemperature() / 10;
}

int ReadHum()
{
    return GetDHTObject().readHumidity() / 10;
}

void RefreshHMData()
{
    GetDHTSensor().tempeture = ReadTemp();
    GetDHTSensor().humidity = ReadHum();
}

DhtSensor &GetDHTSensor()
{
    static DhtSensor dht;
    return dht;
}

DHT22 &GetDHTObject()
{
    static DHT22 dht22 = DHT22(DHT22_PIN);
    return dht22;
}

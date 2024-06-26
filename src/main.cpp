#include "ArduinoJson/Array/JsonArray.hpp"
#include "ArduinoJson/Document/StaticJsonDocument.hpp"
#include "ArduinoJson/Json/JsonSerializer.hpp"
#include "HardwareSerial.h"
#include "Print.h"
#include "esp32-hal-adc.h"
#include "esp32-hal-gpio.h"
#include "esp32-hal.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "esp_log.h"
#include "driver/spi_slave.h"
#include "driver/gpio.h"
#include <Arduino.h>
#include <ArduinoJson.h>
#include <WebServer.h>
#include <array>
#include <cstddef>
#include <cstdio>
#include <sys/types.h>
#include <thread>
#include <type_traits>
#include <chrono>
#include <stdio.h>
#include <stdint.h>
#include <stddef.h>
#include <string.h>
#include "enviromentalmonitor.h"
#include "sender.h"
#include "gardenbeds.h"

#define DHT22_PIN 4
#define PUMP_RELAY_PIN 32

#define MOISTURE_SENSOR_PIN_A 33
#define MOISTURE_SENSOR_PIN_B 26
#define MOISTURE_SENSOR_PIN_C 27

#define PUMP_VALVE_A_PIN 33
#define PUMP_VALVE_B_PIN 13
#define PUMP_VALVE_C_PIN 17


void setup() 
{
    Serial.begin(9600);

    auto &beds = GetGardenBeds();

    pinMode(beds[0].pumpValvePin, OUTPUT);
    pinMode(beds[1].pumpValvePin, OUTPUT);

    //Moisture sensor incialization
    for(size_t i = 0; i < GetBedSize(); i++)
        beds[i].moistureSensorValue = ReadMoisture()[i];   
    delay(50);

    InitGardenbeds(); //inicialize beds

    //Initialize DHT22
    pinMode(DHT22_PIN, OUTPUT);
    pinMode(PUMP_RELAY_PIN, OUTPUT);
    GetDHTObject().begin();
    CheckHMObject();
    ConfigureHMSensor();
    
    ConfigureSPIbus();
    delay(2000);
}


void loop() 
{
    while(true)
    {
        delay(200);
        ReadMoisture();
        if(GetTime() <= 7 && GetTime() >= 22)
            IrrigationLogic();
        else
        {
            /*
                Just in case it was watering close to 7 am and it did not 
                make in time. If this was not here, this would result in 
                pump pumping while it is not needed. 
            */
            TurnOffPump(); 
        }

        sender();
    }
}

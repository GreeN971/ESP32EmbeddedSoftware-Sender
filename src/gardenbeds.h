#pragma once
#include "moisture.h"
#include "sys/_stdint.h"
#include <array>
#include <string>
#include <type_traits>

#define MOISTURE_SENSOR_PIN_A 33
#define MOISTURE_SENSOR_PIN_B 26
#define MOISTURE_SENSOR_PIN_C 27

#define PUMP_VALVE_A_PIN 33
#define PUMP_VALVE_B_PIN 13
#define PUMP_VALVE_C_PIN 17

#define PUMP_RELAY_PIN 32

struct GardenBeds
{
    int16_t moistureSensorValue;
    int16_t moistureSensorPin;
    bool pumpValveState;
    int16_t pumpValvePin;
};

struct SoilMoistureSensors
{
    std::vector<std::string> name;
    std::vector<int> value;
};

struct ValveStatuses 
{
    std::vector<int8_t> id;
    std::vector<int8_t> status;
};


GardenBeds CreateBed(int16_t pinMoist, int16_t moistVal, int16_t pinValve, bool valveVal); //TO DO PREDELAT VALVE NA BOOL LATER 
std::array<GardenBeds, 2> &GetGardenBeds();
constexpr size_t GetBedSize()
{
    return typename std::remove_reference<decltype(GetGardenBeds())>::type{}.size();
}

std::array<int16_t, GetBedSize()> ReadMoisture();
std::array<int16_t, GetBedSize()> ChangePumpValeState();

void CheckSoilMoisture(int pin);
void TurnOnPump();
void TurnOffPump();
bool CheckStateOfAllValves();
void EditValveState(bool state, const int &element);
void OpenCloseValve(const int &pinValve, bool state, const int &element);
void OpenValve(const int &pinValve, bool state, const int &element);
void CloseValve(const int &pinValve, bool state, const int &element);
void IrrigationLogic();

void InitGardenbeds();

void UpdateMoistureValues();
void UpdateValveStatuses();

SoilMoistureSensors &GetSensorsStruct();
ValveStatuses &GetValveStatusesStruct();
#include "gardenbeds.h"
#include "sys/_stdint.h"
#include <array>
#include <cstring>

GardenBeds CreateBed(int16_t pinMoist, int16_t moistVal, int16_t pinValve, bool valveVal) //TO DO PREDELAT VALVE NA BOOL LATER 
{
    GardenBeds bed; 
    bed.moistureSensorPin = pinMoist;
    bed.moistureSensorValue = moistVal;
    bed.pumpValvePin = pinValve;
    bed.pumpValveState = valveVal;
    return bed;
}

std::array<GardenBeds, 2> &GetGardenBeds()
{
    
    static std::array<GardenBeds,2>beds{
    CreateBed(MOISTURE_SENSOR_PIN_A,1,PUMP_VALVE_A_PIN,1), //TO DO make a list of used pins
    CreateBed(MOISTURE_SENSOR_PIN_B,1,PUMP_VALVE_B_PIN,1),
    };
    return beds;
}

std::array<int16_t, GetBedSize()> ReadMoisture()
{
    std::array<int16_t, GetBedSize()> moistureResultArr;

    for(size_t i = 0; i < moistureResultArr.size(); i++)
    {
        int16_t sensorVal = analogRead(GetGardenBeds()[i].moistureSensorPin);
        int16_t percentageHumidity = map(sensorVal, dry, wet, 0, 100); ;
        if(percentageHumidity > 100)
            percentageHumidity = 100;
        moistureResultArr[i] = percentageHumidity;
    }   
    return moistureResultArr;
}

std::array<int16_t, GetBedSize()> ChangePumpValeState()
{
    auto &beds = GetGardenBeds();
    return {};  //TODO
}

void CheckSoilMoisture(int pin)
{
    int16_t moistVal = analogRead(12);
}

void TurnOnPump()
{
    digitalWrite(PUMP_RELAY_PIN, HIGH);
}

void TurnOffPump()
{
    digitalWrite(PUMP_RELAY_PIN, LOW);
}

bool CheckStateOfAllValves()
{
    auto &beds = GetGardenBeds();
    int16_t counter = 0;
    for(size_t i = 0; i < GetBedSize(); i++)
    {
        if(beds[i].pumpValveState == true)
            counter++;
    }

    return (counter == GetBedSize()) ? true : false;
}

void EditValveState(bool state, const int &element)
{
    auto &beds = GetGardenBeds();
    beds[element].pumpValveState = state;
    //Serial.print(beds[element].pumpValveState);
}

void OpenCloseValve(const int &pinValve, bool state, const int &element)
{
    bool passState = state;
    auto &beds = GetGardenBeds();
    if(beds[element].pumpValveState == false)
    {
        state = true;
        digitalWrite(pinValve, HIGH);
        EditValveState(state, element);
    }
    else if(beds[element].pumpValveState == true) 
    {
        state = false;
        digitalWrite(pinValve, LOW);
        EditValveState(state, element);
    }
}

void OpenValve(const int &pinValve, bool state, const int &element)
{
    bool passState = state;
    auto &beds = GetGardenBeds();
    state = true;
    digitalWrite(pinValve, HIGH);
    EditValveState(state, element);
}

void CloseValve(const int &pinValve, bool state, const int &element)
{
    bool passState = state;
    auto &beds = GetGardenBeds();
    state = false;
    digitalWrite(pinValve, LOW);
    EditValveState(state, element);
}

void IrrigationLogic()
{
    // dry = 2650,
    // wet = 900,
    // bed.moistureSensorPin = pinMoist;
    // bed.moistureSensorValue = moistVal;
    // bed.pumpValvePin = pinValve;
    // bed.pumpValveState = valveVal;
    int16_t moistureValue; //TO DO MAYBE LATER MOVE DECLARATION INTO FOR
    auto &beds = GetGardenBeds();
    
    for(size_t k = 0; k < GetBedSize(); k++)
        beds[k].moistureSensorValue = ReadMoisture()[k];   
    delay(50);

    for(size_t i = 0; i < GetBedSize(); i++)
    {
        //Serial.println(beds[i].moistureSensorValue); 
        delay(50);

        if(beds[i].moistureSensorValue > 90) 
        {   
            //Serial.println("PUMP IS OFF");
            CloseValve(beds[i].pumpValvePin, beds[i].pumpValveState, i); //Closes Valve
        }
        else if (beds[i].moistureSensorValue < 10)  
        {
            //Serial.println("PUMP IS ON");
            OpenValve(beds[i].pumpValvePin, beds[i].pumpValveState, i); //ADD SMALL DELAY SO IT HAS TIME TO OPEN BEFORE 
            TurnOnPump();
        }
        else if(CheckStateOfAllValves() == true)
            TurnOffPump();
    }
}

void UpdateMoistureValues()
{
    auto &beds = GetGardenBeds();
    std::string tmp[2] = {"soilMoistureOne", "soilMoistureTwo"};

    for(size_t k = 0; k < GetBedSize(); k++)
    {
        beds[k].moistureSensorValue = ReadMoisture()[k];
        GetSensorsStruct().name[k] = tmp[k];
        GetSensorsStruct().value[k] = beds[k].moistureSensorValue;
    } 
}

void UpdateValveStatuses()
{
    auto &beds = GetGardenBeds();
    int8_t tmpID = 1;

    for(int8_t k = 0; k < GetBedSize(); k++)
    {
        GetValveStatusesStruct().id[k] = tmpID;
        tmpID++;
        GetValveStatusesStruct().status[k] = beds[k].pumpValveState;
    } 
}

SoilMoistureSensors &GetSensorsStruct()
{
    static SoilMoistureSensors soilSensors;
    return soilSensors;
}

ValveStatuses &GetValveStatusesStruct()
{
    static ValveStatuses valveStatuses;
    return valveStatuses;
}

void InitGardenbeds()
{
    GetSensorsStruct().name.resize(2);
    GetSensorsStruct().value.resize(2);
    GetValveStatusesStruct().id.resize(2);
    GetValveStatusesStruct().status.resize(2);
}
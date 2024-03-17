#include "serializejson.h"
#include "gardenbeds.h"
#include "enviromentalmonitor.h"
#include "ArduinoJson/Document/StaticJsonDocument.hpp"
#include "ArduinoJson/Object/JsonObject.hpp"
#include <cstddef>

void ConstructJsonDocTemplate()
{
    espData = doc.createNestedObject("ESPData");
    valveStatusesArr = espData.createNestedArray("valveStatuses");
}

void SerializeDHT()
{
    RefreshHMData();
    espData["tempeture"] = GetDHTSensor().tempeture;
    espData["airhumidity"] = GetDHTSensor().humidity;
}

void SerializeMoistures()
{
    UpdateMoistureValues();
    auto &soilSensorsRef = GetSensorsStruct();
    for(int i = 0; i < 2; i++)
    {
        espData[soilSensorsRef.name[i]] = soilSensorsRef.value[i];
    }
    
}

void SerializeValveStatuses()
{
    UpdateValveStatuses();
    auto &valveStatusesRef = GetValveStatusesStruct();
    JsonObject valveOne = valveStatusesArr.createNestedObject();
    JsonObject valveTwo = valveStatusesArr.createNestedObject();

    valveOne["id"] = valveStatusesRef.id[0];
    valveOne["status"] = valveStatusesRef.status[0];
    
    valveTwo["id"] = valveStatusesRef.id[1];
    valveTwo["status"] = valveStatusesRef.status[1];

}

StaticJsonDocument<CAPACITYx> &GetJsonDoc()
{
    ConstructJsonDocTemplate();
    SerializeDHT();
    SerializeMoistures();
    SerializeValveStatuses();
    return doc;
}
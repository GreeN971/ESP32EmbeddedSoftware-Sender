#pragma once
#include "gardenbeds.h"
#include <ArduinoJson.h>


static inline constexpr size_t CAPACITYx = JSON_OBJECT_SIZE(200);
inline StaticJsonDocument<CAPACITYx> doc;
static inline JsonObject espData;
static inline JsonArray valveStatusesArr;

void ConstructJsonDocTemplate();
void SerializeDHT();
void SerializeMoistures();
void SerializeValveStatuses();
StaticJsonDocument<CAPACITYx> &GetJsonDoc();

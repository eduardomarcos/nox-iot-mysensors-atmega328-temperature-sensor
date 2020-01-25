#define MY_DEBUG
#define MY_RADIO_RF24

#define MY_RF24_PA_LEVEL RF24_PA_MIN
#define MY_RF24_CHANNEL (72)
#define MY_SECURITY_SIMPLE_PASSWD "dell-nox-simple-password"

#define MY_NODE_ID 1

#include <Wire.h>
#include <SPI.h>
#include <MySensors.h>
#include "AHT10.h"

static const uint64_t UPDATE_INTERVAL = 5000;

#define CHILD_ID_TEMP 1
#define CHILD_ID_HUM 2

MyMessage msgTemp(CHILD_ID_TEMP, V_TEMP);
MyMessage msgHum(CHILD_ID_HUM, V_HUM);

AHT10 aht10;

float lastTemperatureRead = 0;
float lastHumidityRead = 0;

void readTemperatureAndHumidity();
void processHumidity(float humidity);
void processTemperature(float temperature);

void presentation()
{
  sendSketchInfo("TemperatureSensor", "1.0");
  present(CHILD_ID_TEMP, S_TEMP);
  present(CHILD_ID_HUM, S_HUM);
}

void setup()
{
  if (aht10.begin())
  {
    Serial.println("AHT10 Connected!");
  }
}

void loop()
{
  readTemperatureAndHumidity();
  sleep(UPDATE_INTERVAL);
}

void readTemperatureAndHumidity()
{
  wait(2000);
  // float temperature = aht10.readTemperature();
  float temperature = 29.9;
  Serial.println("--->Temp:");
  Serial.println(temperature);
  // float humidity = aht10.GetHumidity();
  float humidity = 0;

  processTemperature(temperature);
  processHumidity(humidity);
}

void processTemperature(float temperature)
{
  if (isnan(temperature))
  {
    Serial.println("Failed reading temperature from DHT!");
  }
  else
  {
    if (temperature != lastTemperatureRead)
    {
      send(msgTemp.set(temperature, 1));
      lastTemperatureRead = temperature;
    }
  }
}

void processHumidity(float humidity)
{
  if (isnan(humidity))
  {
    Serial.println("Failed reading humidity from DHT!");
  }
  else
  {
    if (humidity != lastHumidityRead)
    {
      send(msgHum.set(humidity, 1));
      lastHumidityRead = humidity;
    }
  }
}
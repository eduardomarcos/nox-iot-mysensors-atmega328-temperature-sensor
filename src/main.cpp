#define MY_DEBUG
#define MY_RADIO_RF24

#define MY_RF24_PA_LEVEL RF24_PA_MIN
#define MY_RF24_CHANNEL (72)

#define MY_NODE_ID 1

#include <MySensors.h>
#include "AHT10.h"
#include <Battery.h>

static const uint64_t UPDATE_INTERVAL = 5000;

#define CHILD_ID_TEMP 1
#define CHILD_ID_HUM 2

MyMessage msgTemp(CHILD_ID_TEMP, V_TEMP);
MyMessage msgHum(CHILD_ID_HUM, V_HUM);
Battery battery(3000, 4200, A0);

AHT10 aht10;

float lastTemperatureRead = 0;
float lastHumidityRead = 0;
int oldBatteryPcnt = 0;

void readBatteryLevel();
void readTemperatureAndHumidity();
float roundMeasurement(float val);
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
  battery.begin(5000, 1.0);
  if (aht10.begin())
  {
    Serial.println("AHT10 Connected!");
  }
}

void loop()
{
  readBatteryLevel();
  readTemperatureAndHumidity();
  sleep(UPDATE_INTERVAL);
}

void readTemperatureAndHumidity()
{
  float temperature = aht10.readTemperature();
  float humidity = aht10.readHumidity();

  processTemperature(temperature);
  processHumidity(humidity);
}

void processTemperature(float temperature)
{
  if (isnan(temperature))
  {
    Serial.println("Failed reading temperature!");
  }
  else
  {
    temperature = roundMeasurement(temperature);
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
    Serial.println("Failed reading humidity!");
  }
  else
  {
    humidity = roundMeasurement(humidity);
    if (humidity != lastHumidityRead)
    {
      send(msgHum.set(humidity, 1));
      lastHumidityRead = humidity;
    }
  }
}

float roundMeasurement(float val)
{
  val = val * 10.0f;
  val = (val > (floor(val) + 0.5f)) ? ceil(val) : floor(val);
  val = val / 10.0f;
  return val;
}

void readBatteryLevel()
{
  int batteryLevel = battery.level();
  Serial.print("Battery voltage is ");
  Serial.print(battery.voltage());
  Serial.print(" (");
  Serial.print(batteryLevel);
  Serial.println("%)");
  if (oldBatteryPcnt != batteryLevel)
  {
    sendBatteryLevel(batteryLevel);
    oldBatteryPcnt = batteryLevel;
  }
}
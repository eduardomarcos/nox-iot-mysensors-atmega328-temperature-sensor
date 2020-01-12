#define MY_DEBUG
#define MY_RADIO_RF24

#define MY_RF24_PA_LEVEL RF24_PA_MIN
#define MY_RF24_CHANNEL (72)
#define MY_SECURITY_SIMPLE_PASSWD "dell-nox-simple-password"

#define MY_NODE_ID 1

#define DHT_DATA_PIN 3
#define DHT_TYPE DHT11

#include <SPI.h>
#include <MySensors.h>
#include <DHT.h>

static const uint64_t UPDATE_INTERVAL = 5000;

#define CHILD_ID_TEMP 1
#define CHILD_ID_HUM 2

MyMessage msgTemp(CHILD_ID_TEMP, V_TEMP);
MyMessage msgHum(CHILD_ID_HUM, V_HUM);
DHT dht(DHT_DATA_PIN, DHT_TYPE);

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
  dht.begin();
}

void loop()
{
  readTemperatureAndHumidity();
  sleep(UPDATE_INTERVAL);
}

void readTemperatureAndHumidity()
{
  wait(2000);
  float temperature = dht.readTemperature();
  float humidity = dht.readHumidity();

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
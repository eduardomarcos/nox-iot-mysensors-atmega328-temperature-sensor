#define MY_DEBUG
#define MY_RADIO_RF24

#define MY_RF24_CHANNEL (72)
#define MY_SECURITY_SIMPLE_PASSWD "dell-nox-simple-password"

#define MY_NODE_ID 1

#define DHT_DATA_PIN 3
#define DHT_TYPE DHT11

#include <SPI.h>
#include <MySensors.h>
#include <DHT.h>

static const uint64_t UPDATE_INTERVAL = 3000;

#define CHILD_ID_HUM 0
#define CHILD_ID_TEMP 1

MyMessage msgHum(CHILD_ID_HUM, V_HUM);
MyMessage msgTemp(CHILD_ID_TEMP, V_TEMP);
DHT dht(DHT_DATA_PIN, DHT_TYPE);

void presentation()
{
  sendSketchInfo("TemperatureAndHumidity", "1.0");

  present(CHILD_ID_HUM, S_HUM);
  present(CHILD_ID_TEMP, S_TEMP);
}

void setup()
{
  dht.begin();
}

void loop()
{
  delay(2000);

  float temperature = dht.readTemperature();
  Serial.print("RAW: ");
  Serial.println(temperature);
  if (isnan(temperature))
  {
    Serial.println("Failed reading temperature from DHT!");
  }

  send(msgTemp.set(temperature, 1));

  sleep(UPDATE_INTERVAL);
}

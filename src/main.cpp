#define MY_DEBUG
#define MY_RADIO_RF24

#define MY_RF24_PA_LEVEL RF24_PA_MIN
#define MY_RF24_CHANNEL (72)

#define MY_NODE_ID 1

#define MIN_V 2800
#define MAX_V 4130

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
int oldBatteryPcnt = 0;

long readVcc();
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
  analogReference(INTERNAL);
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

long readVcc()
{
// Read 1.1V reference against AVcc
// set the reference to Vcc and the measurement to the internal 1.1V reference
#if defined(__AVR_ATmega32U4__) || defined(__AVR_ATmega1280__) || defined(__AVR_ATmega2560__)
  ADMUX = _BV(REFS0) | _BV(MUX4) | _BV(MUX3) | _BV(MUX2) | _BV(MUX1);
#elif defined(__AVR_ATtiny24__) || defined(__AVR_ATtiny44__) || defined(__AVR_ATtiny84__)
  ADMUX = _BV(MUX5) | _BV(MUX0);
#elif defined(__AVR_ATtiny25__) || defined(__AVR_ATtiny45__) || defined(__AVR_ATtiny85__)
  ADMUX = _BV(MUX3) | _BV(MUX2);
#else
  ADMUX = _BV(REFS0) | _BV(MUX3) | _BV(MUX2) | _BV(MUX1);
#endif

  delay(2);            // Wait for Vref to settle
  ADCSRA |= _BV(ADSC); // Start conversion
  while (bit_is_set(ADCSRA, ADSC))
    ; // measuring

  uint8_t low = ADCL;  // must read ADCL first - it then locks ADCH
  uint8_t high = ADCH; // unlocks both

  long result = (high << 8) | low;

  result = 1125300L / result; // Calculate Vcc (in mV); 1125300 = 1.1*1023*1000
  return result;              // Vcc in millivolts
}

void readBatteryLevel()
{
  long currentVcc = readVcc();
  int batteryLevel = min(map(currentVcc, MIN_V, MAX_V, 0, 100), 100);
  Serial.print("Battery level (");
  Serial.print(batteryLevel);
  Serial.println("%)");
  if (oldBatteryPcnt != batteryLevel)
  {
    sendBatteryLevel(batteryLevel);
    oldBatteryPcnt = batteryLevel;
  }
}
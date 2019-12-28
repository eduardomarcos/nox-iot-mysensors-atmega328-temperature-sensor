#include <Arduino.h>
#include <Adafruit_BMP280.h>

Adafruit_BMP280 bmp; // I2C

long readVcc();

void setup()
{
  Serial.begin(9600);
  while (!Serial)
  {
    delay(10);
  }

  Serial.write("Loading...\n");

  if (!bmp.begin(0x76))
  {
    Serial.println("BMP280 sensor not found");
  }

  pinMode(LED_BUILTIN, OUTPUT);
  Serial.write("Loaded.\n");
}

// the loop function runs over and over again forever
void loop()
{
  float presion = bmp.readPressure() / 100;
  float temperatura = bmp.readTemperature();

  Serial.print(F("Presion: "));
  Serial.print(presion);
  Serial.print(" hPa");
  Serial.print("\t");
  Serial.print(("Temp: "));
  Serial.print(temperatura);
  Serial.print(" *C");
  Serial.print("\t");

  digitalWrite(LED_BUILTIN, HIGH); // turn the LED on (HIGH is the voltage level)
  delay(500);                      // wait for a second
  digitalWrite(LED_BUILTIN, LOW);  // turn the LED off by making the voltage LOW
  delay(500);                      // wait for a second

  Serial.print("Battery Vcc: ");
  Serial.println(readVcc());
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
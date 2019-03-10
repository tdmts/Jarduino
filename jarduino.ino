#include <LiquidCrystal.h>

const int rs = 12, en = 11, d4 = 5, d5 = 4, d6 = 3, d7 = 2;
LiquidCrystal lcd(rs, en, d4, d5, d6, d7);

float waterLevel;
float moistureLevel;

unsigned long offsetHours, offsetMinutes, offsetSeconds;
unsigned long offsetInput, offsetNow;

int days, hours, minutes, seconds;

void setup() {
  Serial.begin(9600);
  lcd.begin(16, 2);
  lcd.setCursor(0, 0);
  lcd.print("1234567890ABCDEF");
  lcd.setCursor(0, 1);
  lcd.print("1234567890ABCDEF");
}

void loop() {
  listenCommand();
  updateClock();
  measureWaterLevel();
  measureMoistureLevel();
  updateDisplay();
}

void listenCommand()
{
  if(Serial.available())
  {
    String command = Serial.readStringUntil('\r\n');
    //Set like this: Time=19:45:15
    if(command.indexOf("Time=")>=0)
    {
      offsetHours = (int) command.substring(5,7).toInt();
      offsetMinutes = (int) command.substring(8,10).toInt();
      offsetSeconds = (int) command.substring(11,13).toInt();
      offsetInput = (unsigned long)((offsetHours * 3600) + (offsetMinutes * 60) + (offsetSeconds)) * 1000;
      offsetNow = millis();
    }
  }
}

void updateClock()
{
  unsigned long now = millis();
  unsigned long offset = (unsigned long) (now - offsetNow);
  now = (unsigned long) (offsetInput + offset);

  float inputDays = (float) now / 1000.0;
  float fDays = (float) inputDays / 86400.0; //4102.626
  days = floor(fDays); //4102

  float inputHours = (float) inputDays - (days * 86400.0);
  float fHours = (float) inputHours / 3600.0;
  hours = floor(fHours);
  
  float inputMinutes = (float) inputHours - (hours * 3600.0);
  float fMinutes = (float) inputMinutes / 60.0;
  minutes = floor(fMinutes);

  float inputSeconds = (float) inputMinutes - (minutes * 60.0);
  seconds = round(inputSeconds);

  Serial.print("DateTime:");
  Serial.print(days);
  Serial.print("|");
  Serial.print(hours);
  Serial.print(":");
  Serial.print(minutes);
  Serial.print(":");
  Serial.print(seconds);
  Serial.println(".");
}

void updateDisplay()
{
  lcd.begin(16, 2);
  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print("Water:");
  lcd.print(" ");
  lcd.print(waterLevel);
  lcd.setCursor(0, 1);
  lcd.print("Moisture:");
  lcd.print(" ");
  lcd.print(moistureLevel);
}

void measureMoistureLevel()
{
  int pinMoisture1 = A0;
  int pinMoisture2 = A1;
  int pinMoisture3 = A2;

  int moistureValue1 = analogRead(pinMoisture1);
  int moistureValue2 = analogRead(pinMoisture2);
  int moistureValue3 = analogRead(pinMoisture3);

  float moistureLevel1 = ((float) moistureValue1 - 1023) / ( (float) 0 - 1023);
  float moistureLevel2 = ((float) moistureValue2 - 1023) / ( (float) 0 - 1023);
  float moistureLevel3 = ((float) moistureValue3 - 1023) / ( (float) 0 - 1023);

  moistureLevel = (moistureLevel1 + moistureLevel2 + moistureLevel3) / (float) 3;
  /*
  Serial.print("Moisture level1: ");
  Serial.println(moistureLevel1);
  Serial.print("Moisture level2: ");
  Serial.println(moistureLevel2);
  Serial.print("Moisture level3: ");
  Serial.println(moistureLevel3);

  Serial.print("Moisture level: ");
  Serial.println(moistureLevel);
  */
}

void measureWaterLevel()
{
  int pinTrig = 9;
  int pinEcho = 10;

  pinMode(pinTrig, OUTPUT); // Sets the pinTrig as an Output
  pinMode(pinEcho, INPUT); // Sets the pinEcho as an Input

  // Clears the pinTrig
  digitalWrite(pinTrig, LOW);
  delayMicroseconds(2);
  
  // Sets the pinTrig on HIGH state for 10 micro seconds
  digitalWrite(pinTrig, HIGH);
  delayMicroseconds(10);
  digitalWrite(pinTrig, LOW);
  
  long duration;
  int distance;
  
  // Reads the pinEcho, returns the sound wave travel time in microseconds
  duration = pulseIn(pinEcho, HIGH);
  
  // Calculating the distance
  distance= duration*0.034/2;
  
  const int minDistance = 3;
  const int maxDistance = 17;

  waterLevel = 1 - ((float) (distance - minDistance)) / ((float) (maxDistance - minDistance));

  //Cap the values
  if(waterLevel < 0)waterLevel = 0;
  if(waterLevel > 1)waterLevel = 1;
  /*
  Serial.print("Water level: ");
  Serial.println(waterLevel);
  */
}

/*
  dhtArduino

  On interval read DHT sensor and display to serial and LCD : 
  anthropomorphized action gliph responding to isComfortable() function, 
  current temp and humidity, 
  and indicator if readings are up +,down -, or same = over the time perid 10 minutes, given a saved average of collected events
  and a rotating action gliph. 
  16x2 LCD Example:
  ^_^ 75.00F + <|>
  >|> 49.70% = > <

*/
#include "DHT.h"
#include <LiquidCrystal.h>

#define DHTPIN 2     // Digital pin connected to the DHT sensor
// Feather HUZZAH ESP8266 note: use pins 3, 4, 5, 12, 13 or 14 --
// Pin 15 can work but DHT must be disconnected during program upload.

// Uncomment whatever type you're using!
//#define DHTTYPE DHT11   // DHT 11
#define DHTTYPE DHT22   // DHT 22  (AM2302), AM2321
//#define DHTTYPE DHT21   // D

// as the current DHT reading algorithm adjusts itself to work on faster procs.
DHT dht(DHTPIN, DHTTYPE);

//LCD pin to Arduino
const int pin_RS = 8; 
const int pin_EN = 9; 
const int pin_d4 = 4; 
const int pin_d5 = 5; 
const int pin_d6 = 6; 
const int pin_d7 = 7; 
const int pin_BL = 10; 

LiquidCrystal lcd( pin_RS,  pin_EN,  pin_d4,  pin_d5,  pin_d6,  pin_d7);

bool autonomous; 
bool heuristic; 
bool topLeftGliph; // position 0,0  : *_* ^_^
bool bottomLeftGliph; // position 0,1 .|. -|-

void setup() {
  Serial.begin(9600);
  Serial.println("Yass!");

  dht.begin();

  lcd.begin(16, 2);
  lcd.setCursor(0,0);
  lcd.print("EnviroSphere");
  lcd.setCursor(0,1);
  lcd.print("Press Key:");
}

bool isComfortable(float f, float h) {
  bool comfortable = false;
  if(f > 70 && f < 80)
  {
       if(h > 30 && h < 70)
      {
        Serial.print("All good!");
        //lcd.setCursor(4, 0);
        //lcd.print("All good!");
        comfortable = true;
      }
  }
  else {
    Serial.print("Check temperature");
    //lcd.setCursor(4, 0);
    //lcd.print("Temp!");
    comfortable = false;
  }
   
   if(h < 30 && h > 70)
    {
      Serial.print("Check water vapor");
      //lcd.setCursor(9, 0);
      //lcd.print("Vapor");
      comfortable = false;
    }
    return comfortable;
}

bool blinkGliph(String threeChars1, String threeChars2, int posx, int posy, bool isTrue)
{
  String gliph;
  if(isTrue) {
      gliph = threeChars1; //"*_*";  
    }
    else
    {
      gliph = threeChars2; //"^_^";
    }
  lcd.setCursor(posx, posy);
  lcd.print(gliph);
  return !(isTrue); ///return opposite of input always
}

/*
float lastRisingArray[2][9]; 
long lastRisingCheck[9]; // millis
String isRising(float f)
{
  int betweenTime = 60000; // interval time to saves entries in array 
  String toReturn;
  float lastTemp = 0.00;

  int count = 0;
  for (int i = 0; i < (sizeof(lastRisingArray) / sizeof(lastRisingArray[0])); i++)
  {
   if(lastRisingArray[i] > 0)
   {
     count = count + 1;
     lastTemp += lastRisingArray[0][i];
   }
  }
  if(count != 0)
  {
    lastTemp = lastTemp / count; // average over time
  }
  else
  {
    if(lastRisingArray[0][0] == 0.00)
    {
      lastTemp = f; // if this is the very first then now is the best last
    }
    else
    {
      lastTemp = lastRisingArray[0][0]; 
    }
  }

  Serial.print(" avg Temp: ");
  Serial.print(lastTemp);

  if(lastTemp < f)
  {
    toReturn = "+";
  }
  else if(lastTemp > f)
  {
    toReturn = "-";
  }
  else
  {
    toReturn = "=";
  }
  if((millis() > lastRisingCheck[0] + betweenTime) || (lastRisingCheck[0] == 0)) // ten minutes = 600000, 
  {
    for (int b = 0; b < (sizeof(lastRisingArray) / sizeof(lastRisingArray[0])); b++)
    {
      if(lastRisingArray[0][b] <= 0.00) // if no entry
      {
        lastRisingArray[0][b] = f;
        lastRisingCheck[0][b] = millis();
        break;
      }
    }
  }
  if(count == ((sizeof(lastRisingArray[0]) / sizeof(lastRisingArray[0][0]))))
  {
    //Serial.println("Clearing array");
    // skip first element 0, it's always the last temp/average
    for (int a = 1; a < (sizeof(lastRisingArray[0]) / sizeof(lastRisingArray[0][0])); a++)
    {
      lastRisingArray[0][a] = 0;
    }
    lastRisingArray[0][0] = lastTemp; // Add the running average back to the array
  }

  Serial.print(toReturn);
  return toReturn;
}*/

float lastTempArray[9]; // 0 index holds running average
long lastTempCheck = 0; // millis
String isTempRising(float f)
{
  int betweenTime = 30000; // interval time to saves entries in array 
  String toReturn;
  float lastTemp = 0.00;

  int count = 0;
  for (int i = 0; i < (sizeof(lastTempArray) / sizeof(lastTempArray[0])); i++)
  {
   if(lastTempArray[i] > 0)
   {
     count = count + 1;
     lastTemp += lastTempArray[i];
   }
  }
  if(count != 0)
  {
    lastTemp = lastTemp / count; // average over time
  }
  else
  {
    if(lastTempArray[0] == 0.00)
    {
      lastTemp = f; // if this is the very first then now is the best last
    }
    else
    {
      lastTemp = lastTempArray[0]; 
    }
  }

  Serial.print(" avg Temp: ");
  Serial.print(lastTemp);

  if(lastTemp < f)
  {
    toReturn = "+";
  }
  else if(lastTemp > f)
  {
    toReturn = "-";
  }
  else
  {
    toReturn = "=";
  }
  if((millis() > lastTempCheck + betweenTime) || (lastTempCheck == 0)) // ten minutes = 600000, 
  {
    for (int b = 0; b < (sizeof(lastTempArray) / sizeof(lastTempArray[0])); b++)
    {
      if(lastTempArray[b] <= 0.00) // if no entry
      {
        lastTempArray[b] = f;
        lastTempCheck = millis();
        break;
      }
    }
  }
  if(count == ((sizeof(lastTempArray) / sizeof(lastTempArray[0]))))
  {
    //Serial.println("Clearing array");
    // skip first element 0, it's always the last temp/average
    for (int a = 1; a < (sizeof(lastTempArray) / sizeof(lastTempArray[0])); a++)
    {
      lastTempArray[a] = 0;
    }
    lastTempArray[0] = lastTemp; // Add the running average back to the array
  }

  Serial.print(toReturn);
  return toReturn;
}

float lastHumidArray[9]; //
long lastHumidCheck = 0; // millis
String isHumidRising(float f)
{
  int betweenTime = 30000; // interval time to saves entries in array 
  String toReturn;
  float lastHumid = 0.00;

  int count = 0;
  for (int i = 0; i < (sizeof(lastHumidArray) / sizeof(lastHumidArray[0])); i++)
  {
   if(lastHumidArray[i] > 0)
   {
     count = count + 1;
     lastHumid += lastHumidArray[i];
   }
  }
  if(count != 0)
  {
    lastHumid = lastHumid / count; // average over time
  }
  else
  {
    if(lastHumidArray[0] == 0.00)
    {
      lastHumid = f; // if this is the very first then now is the best last
    }
    else
    {
      lastHumid = lastHumidArray[0]; 
    }
  }

  Serial.print(" avg Humid: ");
  Serial.print(lastHumid);

  if(lastHumid < f)
  {
    toReturn = "+";
  }
  else if(lastHumid > f)
  {
    toReturn = "-";
  }
  else
  {
    toReturn = "=";
  }
  if((millis() > lastHumidCheck + betweenTime) || (lastHumidCheck == 0)) // ten minutes = 600000, 
  {
    for (int b = 0; b < (sizeof(lastHumidArray) / sizeof(lastHumidArray[0])); b++)
    {
      if(lastHumidArray[b] <= 0.00) // if no entry
      {
        lastHumidArray[b] = f;
        lastHumidCheck = millis();
        break;
      }
    }
  }
  if(count == ((sizeof(lastHumidArray) / sizeof(lastHumidArray[0]))))
  {
    //Serial.println("Clearing array");
    for (int a = 0; a < (sizeof(lastHumidArray) / sizeof(lastHumidArray[0])); a++)
    {
      lastHumidArray[a] = 0;
    }
    lastHumidArray[0] = lastHumid; // Add the running average back to the array
  }

  Serial.print(toReturn);
  return toReturn;
}

int lineGliphCount = 0;
void ShowStatusToLCD(float f, float h)
{
  lcd.clear();

  if(isComfortable(f,h)) {
    topLeftGliph = blinkGliph("O_O","^_^",0,0,topLeftGliph);
  }
  else
  {
    topLeftGliph = blinkGliph("o_O","#_#",0,0,topLeftGliph);
  }

  bool topRight1 = blinkGliph(">","<",13,0,bottomLeftGliph);
  //blink two on, two off
  if(lineGliphCount >= 2)
  {
    bool topRight2 = blinkGliph("|"," ",14,0,true);
  }
  else
  {
    bool topRight2 = blinkGliph("|"," ",14,0,false);
  }
  bool topRight = blinkGliph("<",">",15,0,bottomLeftGliph);
  bottomLeftGliph = blinkGliph("<|<",">|>",0,1,bottomLeftGliph);
  bool bottomRight1 = blinkGliph(">","<",13,1,bottomLeftGliph);

  if(lineGliphCount >= 2)
  { 
    bool bottomRight1 = blinkGliph("|"," ",14,1,false);
  }
  else
  {
    bool bottomRight1 = blinkGliph("|"," ",14,1,true);
  }

  bool bottomRight = blinkGliph("<",">",15,1,bottomLeftGliph);

  if(lineGliphCount == 3) { lineGliphCount = 0;}

  lineGliphCount = lineGliphCount + 1;

  lcd.setCursor(4,0);
  lcd.print(f);
  lcd.print(F("F "));
  lcd.print(isTempRising(f));

  lcd.setCursor(4, 1);
  lcd.print(h); 
  lcd.print(F("% "));
  lcd.print(isHumidRising(h));
}

long intervalToRun = 4000;
long lastRun;

void loop() {
  float h = dht.readHumidity();
  // Read temperature as Celsius (the default)
  float t = dht.readTemperature();
  // Read temperature as Fahrenheit (isFahrenheit = true)
  float f = dht.readTemperature(true);
  // Check if any reads failed and exit early (to try again).
  if (isnan(h) || isnan(t) || isnan(f)) {
    Serial.println(F("Failed to read from DHT sensor!"));
    return;
  }
  // Compute heat index in Fahrenheit (the default)
  float hif = dht.computeHeatIndex(f, h);
  // Compute heat index in Celsius (isFahreheit = false)
  //float hic = dht.computeHeatIndex(t, h, false);
  Serial.print(millis());
  Serial.print(F(" | Humidity: "));
  Serial.print(h);
  Serial.print(F("% | Temp: "));
  Serial.print(f);
  Serial.print(F("°F | Index: "));
  Serial.print(hif);
  Serial.print(F("°F | "));
 
  ShowStatusToLCD(f,h);
  
  Serial.println(" ");
  delay(intervalToRun);
}

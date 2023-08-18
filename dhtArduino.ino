/*
  dhtArduino

  On interval read DHT sensor, write debug lines to serial, and display to LCD : 
  anthropomorphized action gliph responding to isComfortable() function, true between 73-80F temp and 30-70% humid
  current temp and humidity, 
  indicator if readings are up +, down -, or same = given the average of the running average and a saved collection of events over an interval
  rotating action gliph.

  Serial: 
  88906 | Humidity: 42.10% | Temp: 79.34°F | Index: 78.95°F | All good! avg : 79.38- avg : 42.18-

  16x2 LCD :
  ----------------
  ^_^ 75.00F + <|>
  >|> 49.70% = > <
  
  O_O 75.00F = <|>
  <|< 49.70% = > <
  ---------------
  #_# 85.00F - > <
  >|> 90.50% + <|>

  o_O 85.00F = > <
  <|< 90.50% + <|>
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

bool isComfortable(float f, float h, float lowF=73, float highF=80, float lowh=30, float highH=70) {
  bool comfortable = false;
  if(f >= lowF && f <= highF)
  {
       if(h >= lowh && h <= highH)
      {
        Serial.print("All good!");
        comfortable = true;
      }
  }
  else {
    Serial.print("Check temperature");
    comfortable = false;
  }
   
   if(h < lowh && h > highH)
    {
      Serial.print("Check water vapor");
      comfortable = false;
    }
    return comfortable;
}

bool blinkGliph(String openGliph, String closeGliph, int posx, int posy, bool blinked)
{
  String gliph;
  if(blinked) {
      gliph = openGliph; //"O_O";  
    }
    else
    {
      gliph = closeGliph; //"^_^";
    }
  lcd.setCursor(posx, posy);
  lcd.print(gliph);
  return !(blinked); ///return opposite of input always
}

float lastRisingArray[2][9];  // 2 by 9 element array of floats, 1 each for temp and humidity and with 9 data points per given time period 
long lastRisingCheck[9]; // millis // 1 by element array for 9 elements, 1st is temp, 2nd is humidit 
String isRising(int dataPoint, float f)
{
  int betweenTime = 15000; //15000; // interval time to saves entries in array 
  String toReturn;
  float lastTemp = 0.00;
  int count = 0;
  for (int i = 0; i < (sizeof(lastRisingArray[dataPoint]) / sizeof(lastRisingArray[dataPoint][0])); i++)
  {
   if(lastRisingArray[dataPoint][i] > 0)
   {
     count = count + 1;
     lastTemp += lastRisingArray[dataPoint][i];
   }
  }
  if(count != 0)
  {
    lastTemp = lastTemp / count; // average over time
  }
  else
  {
    if(lastRisingArray[dataPoint][0] == 0.00)
    {
      lastTemp = f; // if this is the very first then now is the best last
    }
    else
    {
      lastTemp = lastRisingArray[dataPoint][0]; 
    }
  }
  Serial.print(" avg : ");
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
  if((millis() > lastRisingCheck[dataPoint] + betweenTime) || (lastRisingCheck[dataPoint] == 0)) // ten minutes = 600000, 
  {
    for (int b = 0; b < (sizeof(lastRisingArray[dataPoint]) / sizeof(lastRisingArray[dataPoint][0])); b++)
    {
      //Serial.println("");
      //Serial.println(lastRisingCheck[dataPoint] + betweenTime);
      //Serial.println(millis());
      //Serial.println("dataPoint : " + (String)dataPoint + " b : " + (String)b + " f : " + (String)f);
      if(lastRisingArray[dataPoint][b] <= 1.00) // if no entry
      {
        //Serial.println(millis());
        lastRisingArray[dataPoint][b] = f;
        lastRisingCheck[dataPoint] = millis();
        break;
      }
      else 
      {
        //Serial.println("lastRisingArray[dataPoint][b] <= 0.00");
      }
    }
  }
  else {
  //Serial.println("millis < lastRun");
  }
  if(count == ((sizeof(lastRisingArray[dataPoint]) / sizeof(lastRisingArray[dataPoint][0]))))
  {
    //Serial.println("Clearing array");
    // skip first element 0, it's always the last temp/average
    for (int a = 1; a < (sizeof(lastRisingArray[dataPoint]) / sizeof(lastRisingArray[dataPoint][0])); a++)
    {
      lastRisingArray[dataPoint][a] = 0;
    }
    lastRisingArray[dataPoint][0] = lastTemp; // Add the running average back to the array
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
  lcd.print(isRising(0,f));

  lcd.setCursor(4, 1);
  lcd.print(h); 
  lcd.print(F("% "));
  lcd.print(isRising(1,h));
}

long intervalToRun = 4000;
long lastRun;
bool flip;
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
  /*if(flip == true)
  { 
    Serial.print("o_O ");
   } 
  else {
    Serial.print("O_o ");
  }
  flip = !flip;*/
  
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

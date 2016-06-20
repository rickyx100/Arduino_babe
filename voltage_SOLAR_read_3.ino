// include the library code:
#include <LiquidCrystal.h>
#include <math.h>

LiquidCrystal lcd(12, 11, 5, 4, 3, 2);

int vBatSensePin = 1;
int vSolSensePin = 0;

int lcdLightPin = 7;
int lcdLightSwitch = 6;
int lampPin = 8;
int solarPin = 9;
int chargingStatus = 13;
int lowBatStatus = 10;

float vChrgMax = 14.5;
//float vChrgMax = 33;

float vBatMean = 1;
float analogVBatMax = 1023;
//float vBatMax = 22.83;
float vBatMax = 33.7;
int counter = 1;

float vSolMean = 1;
float analogVSolMax = 1030;
//float vSolMax = 22.83;
float vSolMax = 33.5;

bool isFullyCharged = false;

bool isWaitingToRestore = false;
bool isRecharging = false;

int lcdLightCounter = 0;
bool lcdButtonPressed = false;

bool isFirstRunning = true;

void setup() {
  // set up the LCD's number of columns and rows:
  lcd.begin(16, 2);
  // initialize the serial communications:
  Serial.begin(9600);
  pinMode(lampPin, OUTPUT);
  pinMode(solarPin, OUTPUT);
  pinMode(lcdLightPin, OUTPUT);
  pinMode(lcdLightSwitch, INPUT);
  pinMode(chargingStatus, OUTPUT);
  pinMode(lowBatStatus, OUTPUT);
  digitalWrite(lcdLightPin, HIGH);
}

void loop() {
  int batValue = analogRead(vBatSensePin);
  int solValue = analogRead(vSolSensePin);
  if(!lcdButtonPressed)
  {
    lcdButtonPressed = digitalRead(lcdLightSwitch);
    if(isFirstRunning)
    {
      lcdButtonPressed = true;
      isFirstRunning = false;
    }
    if(lcdButtonPressed)
      lcdLightCounter = 0;
  }
//
//  Serial.println("Bat Val:" + (String)batValue);
//  Serial.println("Solar Val:" + (String)solValue);
  
  if(counter <= 10)
  {
    delay(100);
    //Serial.println("CALCULATING");
    float tempBat = (batValue * vBatMax) / analogVBatMax;
    vBatMean = (vBatMean + tempBat) / 2;

    float tempSol = (solValue * vSolMax) / analogVSolMax;
    vSolMean = (vSolMean + tempSol) / 2;
    lcdLightCounter++;
  }
  else
  {      
    // clear the screen
    lcd.clear();
    lcd.setCursor(0, 0);
    //float currentBattery = (batValue * vBatMax) / analogVBatMax;
    Serial.println("Bat:" + (String)vBatMean + "v");
    Serial.println("Solar:" + (String)vSolMean + "v");
    lcd.print("Bat:" + (String)vBatMean + "v");

    lcd.setCursor(0,1);
    lcd.print("Sun:" + (String)vSolMean + "v");
    
    counter = 1;

    if(lcdLightCounter >= 100 && lcdButtonPressed)
    {
      Serial.println("LCD DONE");
      lcdButtonPressed = false; 
      lcdLightCounter = 0;      
    }
    else if(lcdLightCounter < 100 && lcdButtonPressed)
    {
      digitalWrite(lcdLightPin, HIGH);
      Serial.println("LCD RUNNING");
    }
    else if(!lcdButtonPressed)
    {
      digitalWrite(lcdLightPin, LOW);
      Serial.println("LCD OFF");
      lcdLightCounter = 0;
    }

      
    
    //if(vSolMean >= vBatMean && vBatMean <= vChrgMax)
    if(vSolMean >= 5 && vBatMean <= vChrgMax)
    {
      if(isFullyCharged && vBatMean + 0.05 > vChrgMax)
      {
        Serial.println("Charging Batt");
        lcd.setCursor(12,0);
        lcd.print("Batt");
        lcd.setCursor(12,1);
        lcd.print("Full");
        return;
      }
      else if(vBatMean < vChrgMax - 0.05)
      {
        isFullyCharged = false;
      }
      digitalWrite(lampPin, HIGH);
      digitalWrite(solarPin, LOW);
      Serial.println("Charging Batt");
      lcd.setCursor(12,0);
      lcd.print("Chrg");
      lcd.setCursor(14,1);
      lcd.print("On");
      return;
    }
    else if(vBatMean > vChrgMax && vSolMean >= 5)
    //else
    {
      digitalWrite(lampPin, HIGH);
      digitalWrite(solarPin, HIGH);
      Serial.println("Charging Batt OFF");
      lcd.setCursor(12,0);
      lcd.print("Chrg");
      lcd.setCursor(13,1);
      lcd.print("Off");
      isFullyCharged = true;
      return;
    }
//    else if((vBatMean > vChrgMax && vSolMean < 5)
//    {
//      digitalWrite(lampPin, LOW);
//      digitalWrite(solarPin, HIGH);
//      Serial.println("LED ON to dump overdischarge");
//      lcd.setCursor(12,0);
//      lcd.print("LED");
//      lcd.setCursor(13,1);
//      lcd.print("ON");
//      isFullyCharged = true;
//      return;
//    }
    
    if(!isWaitingToRestore && vSolMean <= 5)
    {
      // no sun
      digitalWrite(solarPin, HIGH);
      if(vBatMean >= 10.00)
      {
        lcd.setCursor(13,0);
        digitalWrite(lampPin, LOW);
        lcd.print("LED");     
        lcd.setCursor(14,1);
        lcd.print("On");
      }
      else
      {
        lcd.setCursor(12,0);
        digitalWrite(lampPin, HIGH);
        lcd.print("Batt");     
        lcd.setCursor(13,1);
        lcd.print("Low");
        isWaitingToRestore = true;
        Serial.println("lessthan10");
        digitalWrite(lowBatStatus, LOW);
      }
    }
    else
    {
      digitalWrite(lampPin, HIGH);
      if(vSolMean >= 5 && vBatMean < vBatMax)
      {
        digitalWrite(solarPin, LOW);
        Serial.println("waiting");
        lcd.setCursor(12,0);
        lcd.print("Chrg");
        lcd.setCursor(14,1);
        lcd.print("On");
      }
      else if(vSolMean < 5 && vBatMean < 10.00)
      {
        digitalWrite(solarPin, HIGH);
        digitalWrite(lampPin, HIGH);
        lcd.setCursor(12,0);
        lcd.print("Batt");     
        lcd.setCursor(13,1);
        lcd.print("Low");
        digitalWrite(lowBatStatus, LOW);
      }
      else
      {
        digitalWrite(solarPin, HIGH);
        lcd.setCursor(13,0);
        lcd.print("LED");    
        lcd.setCursor(13,1);
        lcd.print("Off");
      }

      if(vBatMean >= 12.50)
      {
          isWaitingToRestore = false;
      }
    }

    vBatMean = 1;
    vSolMean = 1;
  }
  counter++;
}

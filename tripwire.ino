//Laser Tripwire sketch

#include <TM1637Display.h>

#define LED 2 //LED is connected to pin 2

#define Buzzer 3 //Buzzer is connected to pin 3

#define Sensor 0 //Photoresistor is connected to pin 0

int val = 0; //val is used to store state of input pin

// initialize the Display
int DIO = 7;
int CLK = 6;
boolean colon = true ;
TM1637Display display(CLK, DIO);

int counter = 0;

void setup() {
 
  pinMode(LED, OUTPUT); //sets digital pin as output
 
  pinMode(Buzzer, OUTPUT); //sets digital pin as output
  display.setBrightness(0xF);
  pinMode(Sensor, INPUT); //sets analog pin as input
  Serial.begin(9600);
}

void loop() {
 
  val = analogRead(Sensor); //read input value of sensor and store it
  Serial.println(val);
    if (val<400) // if the sensor goes below 100...
    {
    digitalWrite(LED, HIGH); //turn on LED
    digitalWrite(Buzzer, HIGH); //turn on buzzer
    counter += 1;
    display.showNumberDec(counter, false);
    delay(500);
    digitalWrite(LED, LOW); //else, keep LED off
    digitalWrite(Buzzer, LOW); //else, keep Buzzer off
    delay(500);
    }
else if (val>400)
    {
      digitalWrite(LED, LOW); //else, keep LED off
      digitalWrite(Buzzer, LOW); //else, keep Buzzer off
    }
    display.showNumberDec(counter, false);
}

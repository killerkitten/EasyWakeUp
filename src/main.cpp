/* 
Name: EasyWakeUp 
Description: Protoype for bluetooth add-on to alarmclock. Uses the sunlight as a wake up light if it's
bright enough outside, otherwise it falls back to turning on the lights (emulated with a LED). 
If turning the blinds is not enough it will turn on the light to reach the target brightness.

Dummy Lightlevels: 

Bright morning: 700;
Cloudy morning: 500;
*/

#include <Arduino.h>
#include <Servo.h>

// Setup pins
const int outdoorPin = A0;
const int indoorPin = A1;
const int lightBulbPin = A2;
const int servoPin = 9;

// Dummy indoor target brightness
const int targetIndoorLightlevel = 800;
// Measured indoor brightness
int insideLightReading = 0;
int outsideLightReading = 0;
// Measured outdoorB
// Create Servo object
Servo blindTurnServo;
// Integer for storing servo position
int servoPosition = 0;
// Variable to store bluetooth data
char data = 0;            
// varable to store 
int setLightLevel = 0;

// boolean to check if the blinds are turned 
bool blindsTurned = false;


/* Turn on the lights */

void turnOnLight() {
    while(insideLightReading < targetIndoorLightlevel)  {
      
      insideLightReading = analogRead(indoorPin);  
      insideLightReading = 1023 - insideLightReading;
      setLightLevel = map(insideLightReading, 0, 1023, 0, 255);
      analogWrite(lightBulbPin, setLightLevel);
      Serial.print("setLightLevel: ");
      Serial.println(setLightLevel);
      Serial.print("\n");
    }

}

/* Turn the blinds until target light level is reached, or blinds fully open */

void turnBlinds() {

  while(insideLightReading < targetIndoorLightlevel && servoPosition <= 180) {
        servoPosition += 1;
        blindTurnServo.write(servoPosition);
        insideLightReading = analogRead(indoorPin);
        Serial.print("Turning blinds");
        Serial.println(servoPosition); 
        Serial.print("\n");
        Serial.print("indoorBrightness: ");
        Serial.println(insideLightReading);
        Serial.print("\n");
        delay(15);
     }
     blindsTurned = true;
}

void setup() {
    Serial.begin(9600);   
    pinMode(lightBulbPin, OUTPUT);  
    pinMode(outdoorPin, INPUT);
    pinMode(indoorPin, INPUT);
    blindTurnServo.attach(9); 
}

void loop() {

// Read data from photocells
  outsideLightReading = analogRead(outdoorPin);
  insideLightReading = analogRead(indoorPin);
  /*
   If there is not enough sun outside, turn on the lights. Otherwise turn the blinds.
   if the blinds is not enough for targetIndoorLightLevel, also turn on the lights. */
  
   if(Serial.available() > 0)  
  {
    data = Serial.read();      
    Serial.print(data);        
    Serial.print("\n");
    // Only work if alarm is on.        
    if(data == '1')             
      Serial.println("On");
      if (outsideLightReading < 500 && insideLightReading < targetIndoorLightlevel) {
        turnOnLight();
      }
      else if (outsideLightReading > 700) {
        turnBlinds();
      }
        else if(blindsTurned == true && insideLightReading < targetIndoorLightlevel){
        turnOnLight();
      }

    else if(data == '0')       
      Serial.println("Off");
    }                      
}
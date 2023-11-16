#line 1 "C:\\Users\\Aaseb\\Prosjekter\\ZumoProsjekt\\speedometer\\speedometer.ino"
// Code to calculate and display the average speed and distance traveled for Pololu Zumo 32u4 OLED
#include <Arduino.h>
#include <Zumo32U4.h>
#include <Wire.h>

Zumo32U4Motors motors;
Zumo32U4Encoders encoders;
Zumo32U4OLED display;
Zumo32U4ButtonA buttonA;
Zumo32U4ButtonB buttonB;

unsigned long previousMillis = 0;
const unsigned long secondInterval = 1000; // 1 Second intervals
const float wheelCircumference = 12.25;        // Wheel circumferance with belt
const int countsPerRevolution = 900;           // Tics for a full wheel rotation
float previousLeftCounts = 0;                  // To calculate amount of tics the last second
float previousRightCounts = 0;
float totalDistance = 0;                       // Total distance traveled
float averageSpeed = 0;                        // Average speed per second
int motionDirection = 1;                       // 1 for forward, -1 for backwards

const unsigned long minuteInterval = 2000; // 1 minute
unsigned long lastMinuteStart = 0;
unsigned long minuteSpeeds[60] ={0};  // Stores speeds for the last 60 seconds
int currentSecond = 0;

void setup(){
buttonA.waitForButton();   // Wait for button before running the program
encoders.init();           // Initiate encoders
Serial.begin(9600);        // Initiate serial communication
motors.setSpeeds(200,200); // Start speed
}

void loop(){
  directionChange();
  speedometer();
  speedometerDispay();
  minuteAverageSpeed();
}//Stop loop


void speedometer (){ // Updates the speed and distance driven every second
  unsigned long currentMillis = millis();
  if (currentMillis - previousMillis >= secondInterval){ 
    previousMillis = currentMillis;
  
    // Read encoders
    int currentLeftCounts = encoders.getCountsLeft();
    int currentRightCounts = encoders.getCountsRight();
    // Calculate changes in tics last second
    int leftCountChange = currentLeftCounts - previousLeftCounts;
    int rightCountChange = currentRightCounts - previousRightCounts;

    // Determine direction of motion
    int direction = (leftCountChange + rightCountChange) >= 0 ? 1 : -1;
    // Calculate distance last second
    float leftDistance = (leftCountChange / float(countsPerRevolution)) * wheelCircumference;
    float rightDistance = (rightCountChange / float(countsPerRevolution)) * wheelCircumference;

    // Calculate speed last second 
    float leftSpeed = leftDistance / (secondInterval / 1000.0);
    float rightSpeed = rightDistance / (secondInterval / 1000.0);
    // Positive speed values
    float positiveLeftSpeed = abs(leftSpeed);
    float positiveRightSpeed = abs(rightSpeed);
    /*
    Serial.print ("Left Speed: ");
    Serial.print(positiveLeftSpeed);
    Serial.println(" cm/s");
    Serial.print ("Right Speed: ");
    Serial.print(positiveRightSpeed);
    Serial.println(" cm/s");
    */ 

    // calculate average speed
    averageSpeed = abs(leftSpeed + rightSpeed) / 2;
    /*
    Serial.print ("Average Speed: ");
    Serial.print(averageSpeed);
    Serial.println(" cm/s");
    */
    // Store the speed in minuteSpeed array
    minuteSpeeds[currentSecond] = round(averageSpeed);
    currentSecond = (currentSecond + 1) % 60;

    // Calculate total distance traveled
    float distance = ((leftDistance + rightDistance)/ 2) * direction;

    totalDistance += distance;
    /*
    Serial.print("Total Distance traveled: ");
    Serial.print(totalDistance);
    Serial.println(" cm");
    */
    previousLeftCounts = currentLeftCounts;
    previousRightCounts = currentRightCounts;
    motionDirection = direction;
  }
}

void directionChange(){ // Change direction if button B is pressed, to test the distance when backing up
  if (buttonB.isPressed()){
    motionDirection *=-1;
 
    if (motionDirection == 1){
      motors.setSpeeds(0,0);
      motors.setSpeeds(200,200);
    } else {
      motors.setSpeeds(0,0);
      motors.setSpeeds(-200,-200);
    }
  }
}

void speedometerDispay(){
  display.clear();
  display.print(round(averageSpeed));
  display.print(" cm/s");
  display.gotoXY(0,1);
  display.print(round(totalDistance));
  display.print(" cm");
}

//For hver 60 sekundersperiode at motorene på bilen kjøres, skal det beregnes: gjennomsnittshastighet, makshastighet,
// og antall sekunder bilen har kjørt raskere enn 70% av absolutt makshastighet
void minuteAverageSpeed(){
  unsigned long currentMillis = millis();
  if (currentMillis - lastMinuteStart >= minuteInterval){
    lastMinuteStart = currentMillis;
    int sumSpeeds = 0;
    int numSpeeds = 0;

    for(int i = 0; i < 60; i++){
      sumSpeeds += minuteSpeeds[i];
      if (minuteSpeeds[i] > 0){
        numSpeeds++;
      }
    }

    if (numSpeeds > 0) {
      averageSpeed = sumSpeeds / numSpeeds;
    } else {
      averageSpeed = 0;
    }
    Serial.print("Average Speed (60s): ");
    Serial.print(round(averageSpeed));
    Serial.println(" cm/s"); 
    // Clear the minuteSpeeds array after calculating the average
    for (int i = 0; i < 60; i++) {
      minuteSpeeds[i] = 0;
    }
  }
}

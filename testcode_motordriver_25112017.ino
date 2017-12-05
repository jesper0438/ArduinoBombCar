#include <SparkFunMiniMoto.h>

//Quintijn en Marlon

// Attributes
// Create two MiniMoto instances, with different address settings.
MiniMoto motor0(0xC4); // A1 = 1, A0 = clear
MiniMoto motor1(0xC0); // A1 = 1, A0 = 1 (default)

#define FAULTN  16     // Pin used for fault detection.
#define LS 5
#define CS 4
#define RS 3
//Assign sensorpins to ports
int echoPin[3] = {7, 11, 9, }; // Echo Pin
int trigPin[3] = {6, 10, 8, }; // Trigger Pin


int maximumRange = 200; // Maximum range needed
int minimumRange = 0; // Minimum range needed

long durationF, distanceF;
long durationL, distanceL;
long durationR, distanceR;

void setup()
{
  Serial.begin(9600);
  Serial.println("Hello, world!");
  pinMode(FAULTN, INPUT);
  //Voorzijde
  pinMode(trigPin[0], OUTPUT);
  pinMode(echoPin[0], INPUT);
  //Linkerzijde
  pinMode(trigPin[1], OUTPUT);
  pinMode(echoPin[1], INPUT);
  //Rechterzijde
  pinMode(trigPin[2], OUTPUT);
  pinMode(echoPin[2], INPUT);

}

void cruiseControl() {
  motor0.drive(25);
  motor1.drive(25);
}

void slowCruiseControl() {
  motor0.drive(15);
  motor1.drive(15);
}

void brake()
{
  motor0.brake();
  motor1.brake();
}

void turnRight()
{
  motor0.drive(20);
  motor1.drive(-20);
}

void turnLeft()
{
  motor0.drive(-20);
  motor1.drive(20);
}
void turnLittleRight()
{
  motor0.drive(15);
  motor1.drive(0);
}

void turnLittleLeft()
{
  motor0.drive(0);
  motor1.drive(15);
}

void hardReverse()
{
  motor0.drive(-40);
  motor1.drive(-40);
  //delay(100);
}

void softReverse() {
  motor0.drive(-10);
  motor1.drive(-10);
}

void calcDistance() {
  if (distanceF > maximumRange || distanceF < minimumRange) {
    /* Send a negative number to computer and Turn LED ON
      // to indicate "out of range" */
    Serial.println("Forward: OoR");
  }
  if (distanceL > maximumRange || distanceL < minimumRange) {
    Serial.println("Left: OoR");
  }
  if (distanceR > maximumRange || distanceR < minimumRange) {
    Serial.println("Right: OoR");
  }
}

/* Send the distance to the computer using Serial protocol, and
    turn LED OFF to indicate successful reading. */
void printReadings() {

  Serial.println("Voorzijde: ");
  Serial.println(distanceF);
  Serial.println("Links: ");
  Serial.println(distanceL);
  Serial.println("Rechts: ");
  Serial.println(distanceR);
}

boolean checkForwardSensor() {
  /* Hard reverses if the forward sensor expects collision from a short distance. */
  //FORWARD
  if (distanceF < 15) {
    brake();
    hardReverse();
    return false;
  }

  /* Randomly generates a direction to turn if the forward sensor expects collision from a long distance */
  else if (distanceF > 14 && distanceF < 30) {
    long randomLong = random(1, 10);
    if (randomLong <= 5) {
      turnLeft();
    }
    else {
      turnRight();
    }
    return false;
  }
  return true;
}

boolean checkRightSensor() {
  //RIGHT
  if (distanceR > 10) {
    brake();
    hardReverse();
    delay(90);
    turnLeft();
    return false;
  }
  return true;
}

boolean checkLeftSensor() {
  //LEFT
  if (distanceL > 10) {
    brake();
    hardReverse();
    delay(90);
    turnRight();
    return false;
  }
  return true;
}

void checkAllSensors() {
  boolean allOk = true;
  if (checkForwardSensor() == false) {
    delay(250);
    allOk = false;
  }
  if (checkLeftSensor() == false) {
    delay(500);
    allOk = false;
  }
  if (checkRightSensor() == false) {
    delay(500);
    allOk = false;
  }
  if (allOk == true) {
  }

}

void trackLine() {
  if (digitalRead(CS) == false) {
    slowCruiseControl();
  }
  else if (digitalRead(RS) == false) {
      brake();
      turnLittleLeft();
    }
    else if (digitalRead(LS) == false) {
      brake();
      turnLittleRight();
    }
  else {
    slowCruiseControl();
  }
}


void loop()
{
  /* The following trigPin/echoPin cycle is used to determine the
    distance of the nearest object by bouncing soundwaves off of it. */

  //SENSOR Voorzijde
  digitalWrite(trigPin[0], LOW);
  delayMicroseconds(2);

  digitalWrite(trigPin[0], HIGH);
  delayMicroseconds(10);

  digitalWrite(trigPin[0], LOW);
  durationF = pulseIn(echoPin[0], HIGH);
  delay(16);

  //SENSOR Links
  digitalWrite(trigPin[1], LOW);
  delayMicroseconds(2);

  digitalWrite(trigPin[1], HIGH);
  delayMicroseconds(10);

  digitalWrite(trigPin[1], LOW);
  durationL = pulseIn(echoPin[1], HIGH);
  delay(16);

  //SENSOR Rechts
  digitalWrite(trigPin[2], LOW);
  delayMicroseconds(2);

  digitalWrite(trigPin[2], HIGH);
  delayMicroseconds(10);

  digitalWrite(trigPin[2], LOW);
  durationR = pulseIn(echoPin[2], HIGH);

  delay(16);


  //Calculate the distance (in cm) based on the speed of sound.
  distanceF = durationF / 58.2;
  distanceL = durationL / 58.2;
  distanceR = durationR / 58.2;
  calcDistance();

  printReadings();



  checkAllSensors();
  trackLine();
  


}

//delayUntil() is a little function to run the motor either for
//a designated time OR until a fault occurs. Note that this is
//a very simple demonstration; ideally, an interrupt would be
//used to service faults rather than blocking the application
//during motion and polling for faults.
//void delayUntil(unsigned long elapsedTime)
//{}
// See the "BlinkWithoutDelay" example for more details on how
//  and why this loop works the way it does.
//unsigned long startTime = millis();
//while (startTime + elapsedTime > millis())
//{
//  // If FAULTN goes low, a fault condition *may* exist. To be
//  //  sure, we'll need to check the FAULT bit.
//  if (digitalRead(FAULTN) == LOW)
//  {
//    // We're going to check both motors; the logic is the same
//    //  for each...
//    byte result = motor0.getFault();
//    // If result masked by FAULT is non-zero, we've got a fault
//    //  condition, and we should report it.
//    if (result & FAULT)
//    {
//      Serial.print("Motor 0 fault: ");
//      if (result & OCP) Serial.println("Chip overcurrent!");
//      if (result & ILIMIT) Serial.println("Load current limit!");
//      if (result & UVLO) Serial.println("Undervoltage!");
//      if (result & OTS) Serial.println("Over temp!");
//      break; // We want to break out of the motion immediately,
//      //  so we can stop motion in response to our fault.
//    }
//    result = motor1.getFault();
//    if (result & FAULT)
//    {
//      Serial.print("Motor 1 fault: ");
//      if (result & OCP) Serial.println("Chip overcurrent!");
//      if (result & ILIMIT) Serial.println("Load current limit!");
//      if (result & UVLO) Serial.println("Undervoltage!");
//      if (result & OTS) Serial.println("Over temp!");
//      break;
//    }
//  }
//}
//}



// This is code is actived for servo ! 

#include <Servo.h>  // Include the Servo library

Servo servo_0; // Declaration of object to control the first servo
Servo servo_1; // Declaration of object to control the second servo
Servo servo_2; // Declaration of object to control the third servo
Servo servo_3; // Declaration of object to control the fourth servo
Servo servo_4; // Declaration of object to control the fifth servo
Servo servo_5; // Declaration of object to control the sixth servo
Servo servo_6; // Declaration of object to control the seventh servo (not used in this project)

void setup() {
  Serial.begin(9600); // Initialize serial communication
  servo_0.attach(2);  // Associate servo_0 to pin 2
  servo_1.attach(3);  // Associate servo_1 to pin 3
  servo_2.attach(4);  // Associate servo_2 to pin 4
  servo_3.attach(5);  // Associate servo_3 to pin 5
  servo_4.attach(6);  // Associate servo_4 to pin 6
  servo_5.attach(7);  // Associate servo_5 to pin 7
  servo_6.attach(8);  // Associate servo_6 to pin 8
}

void loop() {
  if (Serial.available() > 0) { // If there is data available to read
    String input = Serial.readStringUntil('\n'); // Read the data string until newline
    int servoIndex = input.substring(0, 1).toInt(); // Get the servo index
    int servoValue = input.substring(2).toInt();    // Get the servo value
    
    switch (servoIndex) {
      case 1:
        servo_0.write(servoValue);
        break;
      case 2:
        servo_1.write(servoValue);
        break;
      case 3:
        servo_2.write(servoValue);
        break;
      case 4:
        servo_3.write(servoValue);
        break;
      case 5:
        servo_4.write(servoValue);
        servo_6.write(180 - servoValue);
        break;
      case 6:
        servo_5.write(servoValue);
        break;
      default:
        // Invalid servo index
        break;
    }
  }
}

#include <Servo.h>

Servo myservo;

// ==========================
// IR SENSOR PINS
// ==========================
#define leftIR 2
#define rightIR 3

// ==========================
// MOTOR DRIVER PINS
// ==========================
#define IN1 4
#define IN2 5
#define IN3 7
#define IN4 8

// ==========================
// ULTRASONIC SENSOR PINS
// ==========================
#define trigPin 9
#define echoPin 10

long duration;
int distance;

void setup() {

  Serial.begin(9600);

  // IR Sensors
  pinMode(leftIR, INPUT);
  pinMode(rightIR, INPUT);

  // Motor Driver
  pinMode(IN1, OUTPUT);
  pinMode(IN2, OUTPUT);
  pinMode(IN3, OUTPUT);
  pinMode(IN4, OUTPUT);

  // Ultrasonic Sensor
  pinMode(trigPin, OUTPUT);
  pinMode(echoPin, INPUT);

  // Servo Motor
  myservo.attach(6);

  // Servo Center Position
  myservo.write(90);

  stopRobot();
}

void loop() {

  // ==========================
  // ULTRASONIC DISTANCE
  // ==========================

  digitalWrite(trigPin, LOW);
  delayMicroseconds(2);

  digitalWrite(trigPin, HIGH);
  delayMicroseconds(10);

  digitalWrite(trigPin, LOW);

  duration = pulseIn(echoPin, HIGH);

  distance = duration * 0.034 / 2;

  // ==========================
  // IR SENSOR VALUES
  // ==========================

  int leftValue = digitalRead(leftIR);
  int rightValue = digitalRead(rightIR);

  // ==========================
  // MAIN ROBOT LOGIC
  // ==========================

  // Object detected within range
  if (distance > 0 && distance < 50) {

    // LEFT DETECTED
    if (leftValue == 0 && rightValue == 1) {

      myservo.write(150);

      turnLeft();
    }

    // RIGHT DETECTED
    else if (rightValue == 0 && leftValue == 1) {

      myservo.write(30);

      turnRight();
    }

    // CENTER DETECTED
    else if (leftValue == 0 && rightValue == 0) {

      myservo.write(90);

      moveForward();
    }

    // NO DIRECTION
    else {

      myservo.write(90);

      stopRobot();
    }

  }

  // NO OBJECT
  else {

    myservo.write(90);

    stopRobot();
  }

  delay(200);
}

// ==========================
// MOTOR FUNCTIONS
// ==========================

void moveForward() {

  digitalWrite(IN1, HIGH);
  digitalWrite(IN2, LOW);

  digitalWrite(IN3, HIGH);
  digitalWrite(IN4, LOW);
}

void turnLeft() {

  digitalWrite(IN1, LOW);
  digitalWrite(IN2, HIGH);

  digitalWrite(IN3, HIGH);
  digitalWrite(IN4, LOW);
}

void turnRight() {

  digitalWrite(IN1, HIGH);
  digitalWrite(IN2, LOW);

  digitalWrite(IN3, LOW);
  digitalWrite(IN4, HIGH);
}

void stopRobot() {

  digitalWrite(IN1, LOW);
  digitalWrite(IN2, LOW);

  digitalWrite(IN3, LOW);
  digitalWrite(IN4, LOW);
}
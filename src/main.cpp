#include <Arduino.h>
#include <Servo.h>

// ==========================
// DRIVE MOTOR PINS
// ==========================
#define LEFT_IN1   22
#define LEFT_IN2   23
#define LEFT_PWM   5

#define RIGHT_IN1  24
#define RIGHT_IN2  25
#define RIGHT_PWM  6

// ==========================
// ARM MOTOR PINS
// ==========================
#define ARM_IN1    26
#define ARM_IN2    27
#define ARM_PWM    7

// ==========================
// SERVO PINS
// ==========================
#define SERVO1_PIN 9
#define SERVO2_PIN 10
#define CLAW_PIN   11

Servo servo1;
Servo servo2;
Servo claw;

// ==========================
// Servo positions
// Adjust these after testing
// ==========================
int servo1Pos = 90;
int servo2Pos = 180;
int clawPos   = 90;

const int SERVO1_MIN = 20;
const int SERVO1_MAX = 160;

const int SERVO2_MIN = 0;
const int SERVO2_MAX = 180;

const int CLAW_MIN = 40;
const int CLAW_MAX = 160;

// ==========================
// Optional motor inversion
// Flip true if motion is backwards
// ==========================
bool invertLeftDrive  = true;
bool invertRightDrive = false;
bool invertArm        = false;

// ==========================
void setMotor(int in1, int in2, int pwmPin, int speed, bool invertDirection = false) {
  speed = constrain(speed, -255, 255);

  if (invertDirection) {
    speed = -speed;
  }

  if (speed > 0) {
    digitalWrite(in1, HIGH);
    digitalWrite(in2, LOW);
    analogWrite(pwmPin, speed);
  }
  else if (speed < 0) {
    digitalWrite(in1, LOW);
    digitalWrite(in2, HIGH);
    analogWrite(pwmPin, -speed);
  }
  else {
    digitalWrite(in1, LOW);
    digitalWrite(in2, LOW);
    analogWrite(pwmPin, 0);
  }
}

void stopAll() {
  setMotor(LEFT_IN1, LEFT_IN2, LEFT_PWM, 0, invertLeftDrive);
  setMotor(RIGHT_IN1, RIGHT_IN2, RIGHT_PWM, 0, invertRightDrive);
  setMotor(ARM_IN1, ARM_IN2, ARM_PWM, 0, invertArm);
}

void setup() {
  pinMode(LEFT_IN1, OUTPUT);
  pinMode(LEFT_IN2, OUTPUT);
  pinMode(LEFT_PWM, OUTPUT);

  pinMode(RIGHT_IN1, OUTPUT);
  pinMode(RIGHT_IN2, OUTPUT);
  pinMode(RIGHT_PWM, OUTPUT);

  pinMode(ARM_IN1, OUTPUT);
  pinMode(ARM_IN2, OUTPUT);
  pinMode(ARM_PWM, OUTPUT);

  servo1.attach(SERVO1_PIN);
  servo2.attach(SERVO2_PIN);
  claw.attach(CLAW_PIN);

  servo1.write(servo1Pos);
  servo2.write(servo2Pos);
  claw.write(clawPos);

  Serial.begin(9600);
  stopAll();
}

void loop() {
  static String input = "";

  while (Serial.available()) {
    char c = Serial.read();

    if (c == '\n') {
      input.trim();

      // --------------------------
      // DRIVE,left,right
      // --------------------------
      if (input.startsWith("DRIVE,")) {
        int firstComma = input.indexOf(',');
        int secondComma = input.indexOf(',', firstComma + 1);

        if (firstComma > 0 && secondComma > firstComma) {
          int leftSpeed = input.substring(firstComma + 1, secondComma).toInt();
          int rightSpeed = input.substring(secondComma + 1).toInt();

          leftSpeed = constrain(leftSpeed, -255, 255);
          rightSpeed = constrain(rightSpeed, -255, 255);

          setMotor(LEFT_IN1, LEFT_IN2, LEFT_PWM, leftSpeed, invertLeftDrive);
          setMotor(RIGHT_IN1, RIGHT_IN2, RIGHT_PWM, rightSpeed, invertRightDrive);
        }
      }

      // --------------------------
      // ARM,speed
      // --------------------------
      else if (input.startsWith("ARM,")) {
        int comma = input.indexOf(',');
        if (comma > 0) {
          int armSpeed = input.substring(comma + 1).toInt();
          armSpeed = constrain(armSpeed, -255, 255);
          setMotor(ARM_IN1, ARM_IN2, ARM_PWM, armSpeed, invertArm);
        }
      }

      // --------------------------
      // SERVO1,dir
      // dir = -1, 0, 1
      // --------------------------
      else if (input.startsWith("SERVO1,")) {
        int comma = input.indexOf(',');
        if (comma > 0) {
          int dir = input.substring(comma + 1).toInt();
          if (dir == -1) servo1Pos -= 30;
          else if (dir == 1) servo1Pos += 30;

          servo1Pos = constrain(servo1Pos, SERVO1_MIN, SERVO1_MAX);
          servo1.write(servo1Pos);
        }
      }

      // --------------------------
      // SERVO2,dir
      // --------------------------
      else if (input.startsWith("SERVO2,")) {
        int comma = input.indexOf(',');
        if (comma > 0) {
          int dir = input.substring(comma + 1).toInt();
          if (dir == -1) servo2Pos -= 30;
          else if (dir == 1) servo2Pos += 30;

          servo2Pos = constrain(servo2Pos, SERVO2_MIN, SERVO2_MAX);
          servo2.write(servo2Pos);
        }
      }

      // --------------------------
      // CLAW,dir
      // --------------------------
      else if (input.startsWith("CLAW,")) {
        int comma = input.indexOf(',');
        if (comma > 0) {
          int dir = input.substring(comma + 1).toInt();
          if (dir == -1) clawPos -= 30;
          else if (dir == 1) clawPos += 30;

          clawPos = constrain(clawPos, CLAW_MIN, CLAW_MAX);
          claw.write(clawPos);
        }
      }

      else if (input == "STOP_ALL") {
        stopAll();
      }

      input = "";
    }
    else {
      input += c;
    }
  }
}
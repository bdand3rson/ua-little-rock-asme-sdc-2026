#include <Arduino.h>
#include <Servo.h>

// UA Little Rock ASME SDC 2026 Algorithm

// ==========================
// DRIVE MOTOR PINS - VEX 393 Motors
// ==========================
#define LEFT_IN1   22
#define LEFT_IN2   23
#define LEFT_PWM   5

#define RIGHT_IN1  24
#define RIGHT_IN2  25
#define RIGHT_PWM  6

// ==========================
// SERVO PINS
// ==========================
#define ARM_PIN     9    // continuous-rotation servo
#define ARM2_PIN    12   // standard positional servo
#define SERVO2_PIN  10   // standard positional servo
#define CLAW_PIN    11   // standard positional servo

Servo arm;      // continuous servo
Servo arm2;     // positional servo
Servo servo2;   // positional servo
Servo claw;     // positional servo

// ==========================
// Positional servo positions
// ==========================
int arm2Pos   = 90;
int servo2Pos = 180;
int clawPos   = 90;

// ==========================
// Positional servo limits
// Tune after testing
// ==========================
const int ARM2_MIN   = 20;
const int ARM2_MAX   = 160;

const int SERVO2_MIN = 0;
const int SERVO2_MAX = 180;

const int CLAW_MIN   = 40;
const int CLAW_MAX   = 160;

// Continuous servo commands
const int ARM_STOP    = 90;
const int ARM_FORWARD = 180;
const int ARM_REVERSE = 0;

// ==========================
// Optional drive inversion
// ==========================
bool invertLeftDrive  = true;
bool invertRightDrive = false;

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

void stopDrive() {
  setMotor(LEFT_IN1, LEFT_IN2, LEFT_PWM, 0, invertLeftDrive);
  setMotor(RIGHT_IN1, RIGHT_IN2, RIGHT_PWM, 0, invertRightDrive);
}

void setup() {
  pinMode(LEFT_IN1, OUTPUT);
  pinMode(LEFT_IN2, OUTPUT);
  pinMode(LEFT_PWM, OUTPUT);

  pinMode(RIGHT_IN1, OUTPUT);
  pinMode(RIGHT_IN2, OUTPUT);
  pinMode(RIGHT_PWM, OUTPUT);

  arm.attach(ARM_PIN);
  arm2.attach(ARM2_PIN);
  servo2.attach(SERVO2_PIN);
  claw.attach(CLAW_PIN);

  // Initialize servo positions
  arm.write(ARM_STOP);
  arm2.write(arm2Pos);
  servo2.write(servo2Pos);
  claw.write(clawPos);

  Serial.begin(9600);
  stopDrive();
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
      // ARM,dir  (continuous servo)
      // dir = -1, 0, 1
      // --------------------------
      else if (input.startsWith("ARM,")) {
        int comma = input.indexOf(',');
        if (comma > 0) {
          int dir = input.substring(comma + 1).toInt();

          if (dir == -1) {
            arm.write(ARM_REVERSE);
          }
          else if (dir == 1) {
            arm.write(ARM_FORWARD);
          }
          else {
            arm.write(ARM_STOP);
          }
        }
      }

      // --------------------------
      // ARM2,dir  (positional servo)
      // dir = -1, 0, 1
      // --------------------------
      else if (input.startsWith("ARM2,")) {
        int comma = input.indexOf(',');
        if (comma > 0) {
          int dir = input.substring(comma + 1).toInt();

          if (dir == -1) arm2Pos -= 5;
          else if (dir == 1) arm2Pos += 5;

          arm2Pos = constrain(arm2Pos, ARM2_MIN, ARM2_MAX);
          arm2.write(arm2Pos);
        }
      }

      // --------------------------
      // SERVO2,dir
      // dir = -1, 0, 1
      // --------------------------
      else if (input.startsWith("SERVO2,")) {
        int comma = input.indexOf(',');
        if (comma > 0) {
          int dir = input.substring(comma + 1).toInt();

          if (dir == -1) servo2Pos -= 5;
          else if (dir == 1) servo2Pos += 5;

          servo2Pos = constrain(servo2Pos, SERVO2_MIN, SERVO2_MAX);
          servo2.write(servo2Pos);
        }
      }

      // --------------------------
      // CLAW,dir
      // dir = -1, 0, 1
      // --------------------------
      else if (input.startsWith("CLAW,")) {
        int comma = input.indexOf(',');
        if (comma > 0) {
          int dir = input.substring(comma + 1).toInt();

          if (dir == -1) clawPos -= 5;
          else if (dir == 1) clawPos += 5;

          clawPos = constrain(clawPos, CLAW_MIN, CLAW_MAX);
          claw.write(clawPos);
        }
      }

      else if (input == "STOP_ALL") {
        stopDrive();
        arm.write(ARM_STOP);
      }

      input = "";
    }
    else {
      input += c;
    }
  }
}
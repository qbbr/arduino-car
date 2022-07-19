/*
   Arduino Robot Car controlled via Bluetooth (HC-06).

   @author qbbr
*/

#include <Servo.h>

// HC-06 via RX/TX 0pin/1pin
#define QBBR_CAR_INFO "QBBR Car 19.07.2022"
#define DEBUG false

// L293N
#define ENA_PIN 3 // right motor
#define IN1_PIN 2
#define IN2_PIN 4
#define ENB_PIN 5 // left motor
#define IN3_PIN 6
#define IN4_PIN 7
#define CAR_SPEED_INIT 50
//#define CAR_SPEED_FACTOR 3
int carSpeed = CAR_SPEED_INIT; // 50-255

// serial commands
#define CMD_FORWARD 'f'
#define CMD_FORWARD_LEFT 'l'
#define CMD_FORWARD_RIGHT 'r'
#define CMD_BACKWARD 'b'
#define CMD_BACKWARD_LEFT 'h'
#define CMD_BACKWARD_RIGHT 'j'
#define CMD_ROTATE_LEFT 'n'
#define CMD_ROTATE_RIGHT 'm'
#define CMD_STOP 's'
#define CMD_SPEED_1 '0'
#define CMD_SPEED_2 '1'
#define CMD_SPEED_3 '2'
#define CMD_SPEED_4 '3'
#define CMD_SPEED_5 '4'
#define CMD_SPEED_6 '5'
#define CMD_SPEED_7 '6'
#define CMD_SPEED_8 '7'
#define CMD_SPEED_9 '8'
#define CMD_SPEED_10 '9'
#define CMD_GET_SPEED 'e'
#define CMD_SERVO_MID 'w'
#define CMD_SERVO_LEFT 'a'
#define CMD_SERVO_RIGHT 'd'
#define CMD_GET_DISTANCE 'g'
char cmd;

// SG-90
#define SERVO_PIN 10 // 8
#define SERVO_ANGLE_LEFT 180
#define SERVO_ANGLE_MID 90
#define SERVO_ANGLE_RIGHT 5
Servo servo;

// HC-SR04
#define TRIGGER_PIN 8 // 9
#define ECHO_PIN 9 // 8
//#define MAX_DISTANCE 200
#define DISTANCE_SAFE 10 // CM
#define DISTANCE_SAFE_ENABLED false

void setup() {
  Serial.begin(9600);
  Serial.println(QBBR_CAR_INFO);

  pinMode(TRIGGER_PIN, OUTPUT);
  pinMode(ECHO_PIN, INPUT);

  pinMode(ENA_PIN, OUTPUT);
  pinMode(IN1_PIN, OUTPUT);
  pinMode(IN2_PIN, OUTPUT);
  pinMode(ENB_PIN, OUTPUT);
  pinMode(IN3_PIN, OUTPUT);
  pinMode(IN4_PIN, OUTPUT);

  setMotorSpeed(CAR_SPEED_INIT);
  carStop();

  servo.attach(SERVO_PIN);
  servo.write(SERVO_ANGLE_MID);
}

void loop() {
  if (Serial.available() > 0) {
    cmd = Serial.read();

    if (cmd != '\r' && cmd != '\n') {
      runCmd(cmd);
    }
  }
}

void runCmd(char c) {
  if (DEBUG) {
    Serial.print("[D] run cmd: ");
    Serial.println(cmd);
  }

  switch (c) {
    case CMD_FORWARD:
      carForward();
      break;
    case CMD_FORWARD_LEFT:
      carForwardLeft();
      break;
    case CMD_FORWARD_RIGHT:
      carForwardRight();
      break;
    case CMD_BACKWARD:
      carBackward();
      break;
    case CMD_BACKWARD_LEFT:
      carBackwardLeft();
      break;
    case CMD_BACKWARD_RIGHT:
      carBackwardRight();
      break;
    case CMD_ROTATE_LEFT:
      carRotateLeft();
      break;
    case CMD_ROTATE_RIGHT:
      carRotateRight();
      break;
    case CMD_STOP:
      carStop();
      break;
    case CMD_SPEED_1:
      setMotorSpeed(50);
      break;
    case CMD_SPEED_2:
      setMotorSpeed(72);
      break;
    case CMD_SPEED_3:
      setMotorSpeed(94);
      break;
    case CMD_SPEED_4:
      setMotorSpeed(116);
      break;
    case CMD_SPEED_5:
      setMotorSpeed(138);
      break;
    case CMD_SPEED_6:
      setMotorSpeed(160);
      break;
    case CMD_SPEED_7:
      setMotorSpeed(182);
      break;
    case CMD_SPEED_8:
      setMotorSpeed(204);
      break;
    case CMD_SPEED_9:
      setMotorSpeed(226);
      break;
    case CMD_SPEED_10:
      setMotorSpeed(255);
      break;
    case CMD_GET_SPEED:
      Serial.print("$speed: ");
      Serial.println(carSpeed);
      break;
    case CMD_SERVO_LEFT:
      servo.write(SERVO_ANGLE_LEFT);
      break;
    case CMD_SERVO_MID:
      servo.write(SERVO_ANGLE_MID);
      break;
    case CMD_SERVO_RIGHT:
      servo.write(SERVO_ANGLE_RIGHT);
      break;
    case CMD_GET_DISTANCE:
      Serial.print("$distance: ");
      Serial.println(getDistance());
      break;
    default:
      if (DEBUG) {
        Serial.print("[D] unknown cmd: ");
        Serial.println(c);
      }
      break;
  }
}


/* Motor */

void motorLeftForward() {
  digitalWrite(IN3_PIN, HIGH);
  digitalWrite(IN4_PIN, LOW);
}

void motorLeftBackward() {
  digitalWrite(IN3_PIN, LOW);
  digitalWrite(IN4_PIN, HIGH);
}

void motorLeftStop() {
  digitalWrite(IN3_PIN, LOW);
  digitalWrite(IN4_PIN, LOW);
}

void motorRightForward() {
  digitalWrite(IN1_PIN, HIGH);
  digitalWrite(IN2_PIN, LOW);
}

void motorRightBackward() {
  digitalWrite(IN1_PIN, LOW);
  digitalWrite(IN2_PIN, HIGH);
}

void motorRightStop() {
  digitalWrite(IN1_PIN, LOW);
  digitalWrite(IN2_PIN, LOW);
}

void setMotorLeftSpeed(int speed) {
  analogWrite(ENB_PIN, speed);
}

void setMotorRightSpeed(int speed) {
  analogWrite(ENA_PIN, speed);
}

void setMotorSpeed(int speed) {
  carSpeed = speed;
  setMotorLeftSpeed(speed);
  setMotorRightSpeed(speed);
}


/* Car */

int getDistance() {
  digitalWrite(TRIGGER_PIN, LOW);
  delayMicroseconds(5);
  digitalWrite(TRIGGER_PIN, HIGH);
  delayMicroseconds(10);
  digitalWrite(TRIGGER_PIN, LOW);

  long duration = pulseIn(ECHO_PIN, HIGH);

  return (duration / 2) / 29.1;
  //return duration * 0.034 / 2;
}

boolean isDistanceSafe() {
  if (DISTANCE_SAFE_ENABLED) {
    if (getDistance() < DISTANCE_SAFE) {
      return false;
    }
  }

  return true;
}

void carForward() {
  if (isDistanceSafe()) {
    motorLeftForward();
    motorRightForward();
  } else {
    carStop();
  }
}

void carForwardLeft() {
  if (isDistanceSafe()) {
    motorLeftStop();
    motorRightForward();
  } else {
    carStop();
  }
}

void carForwardRight() {
  if (isDistanceSafe()) {
    motorLeftForward();
    motorRightStop();
  } else {
    carStop();
  }
}

void carBackward() {
  motorLeftBackward();
  motorRightBackward();
}

void carBackwardLeft() {
  motorLeftStop();
  motorRightBackward();
}

void carBackwardRight() {
  motorLeftBackward();
  motorRightStop();
}

void carRotateLeft() {
  motorLeftBackward();
  motorRightForward();
}

void carRotateRight() {
  motorLeftForward();
  motorRightBackward();
}

void carStop() {
  motorLeftStop();
  motorRightStop();
}

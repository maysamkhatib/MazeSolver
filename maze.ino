// The robot's IR sensors
#define rSensor 14 // Right Sensor
#define fSensor 15 // Front Sensor
#define lSensor 16 // Left Sensor

// The robot's ultrasonic sensors 
#define leftEchoPin 9
#define leftTriggerPin 13
int leftUltrasonicDistance;
int leftUltrasonicDuration;
#define rightEchoPin 20
#define rightTriggerPin 21
int rightUltrasonicDistance;
int rightUltrasonicDuration;

// The robot's motors
#define leftMotor 6
#define leftMotorReverse 7
#define leftMotorForward 8
int leftMotorSpeed;
#define rightMotorReverse 11
#define rightMotorForward 10
#define rightMotor 12
int rightMotorSpeed;

// The robot's encoder
#define leftEncoder 18
#define rightEncoder 19
volatile int rightEncoderCount = 0;
volatile int leftEncoderCount = 0;


void setup() {
  Serial.begin(9600);

  pinMode(rSensor, INPUT);
  pinMode(fSensor, INPUT);
  pinMode(lSensor, INPUT);

  pinMode(leftEchoPin, INPUT);
  pinMode(leftTriggerPin, OUTPUT);
  pinMode(rightEchoPin, INPUT);
  pinMode(rightTriggerPin, OUTPUT);

  pinMode(leftMotor, OUTPUT);
  pinMode(leftMotorReverse, OUTPUT);
  pinMode(leftMotorForward, OUTPUT);
  pinMode(rightMotor, OUTPUT);
  pinMode(rightMotorReverse, OUTPUT);
  pinMode(rightMotorForward, OUTPUT);

  pinMode(rightEncoder, INPUT_PULLUP);
  pinMode(leftEncoder, INPUT_PULLUP);
  digitalWrite(rightEncoder, HIGH);
  digitalWrite(leftEncoder, HIGH);

  attachInterrupt(digitalPinToInterrupt(rightEncoder), updateRightEncoder, CHANGE);
  attachInterrupt(digitalPinToInterrupt(leftEncoder), updateLeftEncoder, CHANGE);
}

int rightCounter = 0;
int reachedGoalForFirstTime = 1;

void loop() {
  delay(750);
  if(rightCounter==3){
    if(reachedGoalForFirstTime){
      turnRight();
      turnRight();
      turnRight();
      reachedGoalForFirstTime = 0;      
    }
    leftMotorSpeed = 0;
    rightMotorSpeed = 0;    
  }else if(!obstacleLeft()){
    leftMotorSpeed = 0;
    rightMotorSpeed = 0;
    turnLeft();
    moveForward();
    rightCounter = 0;
  }else if (!obstacleFront()){
    moveForward();
    rightCounter = 0;
  }else if (!obstacleRight()){
    leftMotorSpeed = 0;
    rightMotorSpeed = 0;
    turnRight();
    rightCounter++;
    moveForward();
  }else{
    leftMotorSpeed = 0;
    rightMotorSpeed = 0;
    turnRight();
    turnRight();
    rightCounter = 0;
  }
}


void runMotors() {
  analogWrite(leftMotor, leftMotorSpeed);
  analogWrite(rightMotor, rightMotorSpeed);
}

void moveForward(){
  digitalWrite(rightMotorForward, HIGH);
  digitalWrite(rightMotorReverse, LOW);
  rightMotorSpeed = 45;
  digitalWrite(leftMotorReverse, LOW);
  digitalWrite(leftMotorForward, HIGH);
  leftMotorSpeed = 45;
  runMotors();
  rightEncoderCount = 0;
  while (rightEncoderCount < 41) {
    if (obstacleFront()) {
      break;
    }
    leftUltrasonicDurationToAdjust(41);
    if (obstacleFront()) {
      break;
    }
    rightUltrasonicDurationToAdjust(41);
  }
  leftMotorSpeed = 0;
  rightMotorSpeed = 0;
  runMotors();
  delay(300);
}

void turnLeft() {
  digitalWrite(leftMotorReverse, HIGH);
  digitalWrite(leftMotorForward, LOW);
  leftMotorSpeed = 45;
  digitalWrite(rightMotorForward, HIGH);
  digitalWrite(rightMotorReverse, LOW);
  rightMotorSpeed = 45;
  runMotors();
  rightEncoderCount = 0;
  while (rightEncoderCount < 10);
  leftMotorSpeed = 0;
  rightMotorSpeed = 0;
  runMotors();
  delay(500);
}

void turnRight() {
  digitalWrite(leftMotorReverse, LOW);
  digitalWrite(leftMotorForward, HIGH);
  leftMotorSpeed = 45;
  digitalWrite(rightMotorForward, LOW);
  digitalWrite(rightMotorReverse, HIGH);
  rightMotorSpeed = 45;
  runMotors();
  rightEncoderCount = 0;
  while (rightEncoderCount < 12);
  leftMotorSpeed = 0;
  rightMotorSpeed = 0;
  runMotors();
  delay(500);
}

bool obstacleFront() {
  if (digitalRead(fSensor) == HIGH)
    return 0;
  else
    return 1;
}

bool obstacleLeft() {
  if (digitalRead(lSensor) == HIGH)
    return 0;
  else
    return 1;
}

bool obstacleRight() {
  if (digitalRead(rSensor) == HIGH)
    return 0;
  else
    return 1;
}

void updateRightEncoder() {
  rightEncoderCount++;
}

void updateLeftEncoder() {
  leftEncoderCount++;
}

void rightUltrasonicDurationToAdjust(int duration) {
  digitalWrite(rightTriggerPin, LOW);
  delayMicroseconds(2);
  digitalWrite(rightTriggerPin, HIGH);
  delayMicroseconds(10);
  digitalWrite(rightTriggerPin, LOW);
  rightUltrasonicDuration = pulseIn(rightEchoPin, HIGH);
  rightUltrasonicDistance = (rightUltrasonicDuration * 0.034) / 2;
  if (rightUltrasonicDistance < 9 || rightUltrasonicDistance > 200) {
    rightMotorSpeed = 55;
    runMotors();
    int tmp = rightEncoderCount;
    while (rightEncoderCount < tmp + 5) {
      if (rightEncoderCount >= duration) {
        break;
      }
      if (obstacleFront()){
          break;
        }
    }
    rightMotorSpeed = 45;
    runMotors();
  }
}

void leftUltrasonicDurationToAdjust(int duration) {
  digitalWrite(leftTriggerPin, LOW);
  delayMicroseconds(2);
  digitalWrite(leftTriggerPin, HIGH);
  delayMicroseconds(10);
  digitalWrite(leftTriggerPin, LOW);
  leftUltrasonicDuration = pulseIn(leftEchoPin, HIGH);
  leftUltrasonicDistance = (leftUltrasonicDuration * 0.034) / 2;
  
  if (leftUltrasonicDistance < 9 || leftUltrasonicDistance > 200) {
    leftMotorSpeed = 55;
    runMotors();
    int tmp = rightEncoderCount;
    while (rightEncoderCount  < tmp + 5) {
      if (rightEncoderCount >= duration) {
        break;
      }
      if (obstacleFront()){
          break;
        }
    }
    leftMotorSpeed = 45;
    runMotors();
  }
}
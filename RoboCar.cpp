/* ===== PIN DEFINITIONS ===== */

int ENA = 5;
int IN1 = 4;
int IN2 = 7;

int ENB = 6;
int IN3 = 8;
int IN4 = 12;

int redPin = 3;
int greenPin = 9;
int bluePin = 2;

int buzzer = 13;

/* Line sensors */
int leftSensor = A0;
int centerSensor = A1;
int rightSensor = A2;

/* Threshold */
int darkThreshold = 450;

/* Motor speeds */
int baseSpeed = 160;
int turnSpeed = 110;

/* Engine timing */
unsigned long lastBuzz = 0;
int buzzState = 0;

void setup(){

  Serial.begin(9600);

  pinMode(IN1,OUTPUT);
  pinMode(IN2,OUTPUT);
  pinMode(IN3,OUTPUT);
  pinMode(IN4,OUTPUT);

  pinMode(ENA,OUTPUT);
  pinMode(ENB,OUTPUT);

  pinMode(redPin,OUTPUT);
  pinMode(greenPin,OUTPUT);
  pinMode(bluePin,OUTPUT);

  pinMode(buzzer,OUTPUT);

  Serial.println("Line follower test starting");

}

void loop(){

  int leftVal = analogRead(leftSensor);
  int centerVal = analogRead(centerSensor);
  int rightVal = analogRead(rightSensor);

  Serial.print("L:");
  Serial.print(leftVal);
  Serial.print(" C:");
  Serial.print(centerVal);
  Serial.print(" R:");
  Serial.println(rightVal);

  /* NEW: Check if left sensor < 15 - drive both motors if true */
  if(leftVal < 15){
    driveForward();
    setColor(0, 255, 0); // Green - indicates special drive mode
  }
  else {
    /* Determine dark detection */
    bool leftDark = leftVal < darkThreshold;
    bool centerDark = centerVal < darkThreshold;
    bool rightDark = rightVal < darkThreshold;

    /* Steering decisions */
    if(centerDark){
      driveForward();
      setColor(0,0,255); // blue
    }
    else if(leftDark){
      turnLeft();
      setColor(255,0,255); // purple
    }
    else if(rightDark){
      turnRight();
      setColor(0,255,0); // green
    }
    else{
      stopMotors();
      setColor(255,0,0); // red
    }
  }

  engineSound();

  delay(50);

}

/* ===== MOTOR FUNCTIONS ===== */

void driveForward(){

  digitalWrite(IN1,HIGH);
  digitalWrite(IN2,LOW);

  digitalWrite(IN3,HIGH);
  digitalWrite(IN4,LOW);

  analogWrite(ENA,baseSpeed);
  analogWrite(ENB,baseSpeed);

}

void turnLeft(){

  digitalWrite(IN1,HIGH);
  digitalWrite(IN2,LOW);

  digitalWrite(IN3,HIGH);
  digitalWrite(IN4,LOW);

  analogWrite(ENA,turnSpeed);
  analogWrite(ENB,baseSpeed);

}

void turnRight(){

  digitalWrite(IN1,HIGH);
  digitalWrite(IN2,LOW);

  digitalWrite(IN3,HIGH);
  digitalWrite(IN4,LOW);

  analogWrite(ENA,baseSpeed);
  analogWrite(ENB,turnSpeed);

}

void stopMotors(){

  analogWrite(ENA,0);
  analogWrite(ENB,0);

}

/* ===== RGB FUNCTION ===== */

void setColor(int r,int g,int b){

  analogWrite(redPin,r);
  analogWrite(greenPin,g);
  analogWrite(bluePin,b);

}

/* ===== ENGINE SOUND ===== */

void engineSound(){

  unsigned long now = millis();

  if(now - lastBuzz > 30){

    lastBuzz = now;

    buzzState = !buzzState;

    digitalWrite(buzzer,buzzState);

  }

}

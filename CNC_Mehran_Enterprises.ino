#define limitX1 53
#define limitX2 47
#define limitY1 41
#define limitY2 51
#define limitZ1 36
#define limitZ2 49

#define motorX1X2 A0
#define motorX2X1 A1
#define motorY1Y2 A2
#define motorY2Y1 A3
#define motorZ1Z2 A5
#define motorZ2Z1 A6

#define resetButton A10
#define startButton A9

#define motorPWM 100
#define sprayPWM 250
#define Ydisplacement 400

#define On 1
#define Off 0
#define TurningOn 3
#define TurningOff 4

const int X12=1;
const int X21=2;
const int Y12=3;
const int Y21=4;

int motorMoving,displacedY,Spray;
long displacedYtime,displacedResetTime;
bool directionReversed,resettingPosition,sprayTurningOn,firstDisplacement;

void setup() {
  
  pinMode(limitX1,INPUT_PULLUP);
  pinMode(limitX2,INPUT_PULLUP);
  pinMode(limitY1,INPUT_PULLUP);
  pinMode(limitY2,INPUT_PULLUP);
  pinMode(limitZ1,INPUT_PULLUP);
  pinMode(limitZ2,INPUT_PULLUP);
  
  pinMode(resetButton,INPUT_PULLUP);
  pinMode(startButton,INPUT_PULLUP);

  pinMode(motorX1X2,OUTPUT);
  pinMode(motorX2X1,OUTPUT);
  pinMode(motorY1Y2,OUTPUT);
  pinMode(motorY2Y1,OUTPUT);
  pinMode(motorZ1Z2,OUTPUT);
  pinMode(motorZ2Z1,OUTPUT);

  Serial.begin(9600);
  motorMoving = 0;
  displacedY=0;
  displacedYtime=0;
  displacedResetTime=0;
  directionReversed = false;
  resettingPosition = false;
  Spray = Off;
  firstDisplacement = true;
  //stopTheMachine = false;

  digitalWrite(motorZ1Z2,LOW);
  digitalWrite(motorZ2Z1,LOW);
}

void loop() {

  if(digitalRead(resetButton)==LOW){
      Serial.println("Reset Button Triggered");
      resettingPosition = true;
      TurnOffSpray();
  }

  if(digitalRead(limitX1)==LOW && digitalRead(limitY1)==LOW && (motorMoving==Y21 || motorMoving==X21)){
    Serial.println("Stopped the machine at first end");
      motorMoving = 0;
      TurnOffSpray();
    }
  if(digitalRead(limitX2)==LOW && digitalRead(limitY1)==LOW && (motorMoving==Y21 || motorMoving==X21)){
    Serial.println("Stopped the machine at first end");
      motorMoving = 0;
      TurnOffSpray();
    }
  if(digitalRead(limitX2)==LOW && digitalRead(limitY2)==LOW && (motorMoving==Y12 || motorMoving==X21)){
    Serial.println("Stopped the machine at second end");
      motorMoving = 0;
      TurnOffSpray();
    }
  if(digitalRead(limitX1)==LOW && digitalRead(limitY2)==LOW && (motorMoving==Y12 || motorMoving==X12)){
    Serial.println("Stopped the machine at second end");
      motorMoving = 0;
      TurnOffSpray();
    }
    

    if(Spray==TurningOn){
    //Serial.println("Turning On Spray");
    TurnOnSpray();
    
  }else if(Spray==TurningOff){
    //Serial.println("Turning Off Spray");
    TurnOffSpray();
  }else if(resettingPosition){
   

    //Serial.println("Resetting the Position in Actual");
    
    if(digitalRead(limitX1)!=LOW){
      moveMotor(X21);
    }else{
      if(digitalRead(limitY1)!=LOW){
        moveMotor(Y21);
      }
    }
    
    

    if(digitalRead(limitX1)==LOW && digitalRead(limitY1)==LOW){
      Serial.println("Reset is already Done");
      resettingPosition = false;
      motorMoving = 0;
    }
    
  }else {

  if(digitalRead(limitY2)==LOW && motorMoving!=Y21){
    directionReversed=true;
  }
  if(digitalRead(limitY1)==LOW && motorMoving!=Y12){
    directionReversed=false;
  }

  if((digitalRead(limitY1)==LOW && motorMoving==Y21) || (digitalRead(limitY2)==LOW && motorMoving==Y12)){
    Serial.println(motorMoving);
    motorMoving = 0;
  }

  if(motorMoving!=0){

  if((digitalRead(limitX1)==LOW || motorMoving==X12) && digitalRead(limitX2)!=LOW){
    
    if((displacedY!=1 || ((motorMoving==Y12 || motorMoving==Y21) && (millis()-displacedYtime)<Ydisplacement)) && digitalRead(limitX1)==LOW){
      performYdisplacement();

      if(firstDisplacement){
      firstDisplacement = false;
      displacedYtime = millis();
      }
      
      if(millis()-displacedYtime>Ydisplacement && firstDisplacement==false){
        displacedY = 1;
        displacedYtime = millis();
        Serial.println("Displaced Y after reaching X1");
        //motorMoving = X12;
        firstDisplacement = true;
      }
      
    }else{
      moveMotor(X12);
    }
    
    
  }
  if((digitalRead(limitX2)==LOW || motorMoving==X21) && digitalRead(limitX1)!=LOW){
    
    if((displacedY!=1 || ((motorMoving==Y12 || motorMoving==Y21) && (millis()-displacedYtime)<Ydisplacement)) && digitalRead(limitX2)==LOW){
      performYdisplacement();
      
      if(firstDisplacement){
      firstDisplacement = false;
      displacedYtime = millis();
      }
      
      if(millis()-displacedYtime>Ydisplacement && firstDisplacement==false){
        displacedY = 1;
        displacedYtime = millis();
        Serial.println("Displaced Y after reaching X2");
        //motorMoving = X12;
        firstDisplacement = true;
      }
      
    }else{
      moveMotor(X21);
    }
    
  }

  }else{
    // Machine Stopped, Waiting for Reset Input
    
    // Add Start Button to Continue work if it is at Limits
    if(digitalRead(startButton)==LOW){
      Serial.println("Start Button Triggered");
      if(digitalRead(limitY2)==LOW){
        Serial.println("Starting with X21");
        moveMotor(X21);
      }else{
        Serial.println("Starting with X12");
        motorMoving = X12;
        moveMotor(X12);
      }
      TurnOnSpray();
    }


    
  }

  }

}

void performYdisplacement(){
  if(directionReversed){
    moveMotor(Y21);
  }else{
    moveMotor(Y12);
  }
}

void moveMotor(int XY12){
  if(motorMoving!=XY12 && XY12!=Y12 && XY12!=Y21){
    if(millis()-displacedResetTime>(Ydisplacement*2)){
      displacedY=0;
      Serial.println("Reset the Y Displacement");
      displacedResetTime = millis();
    }
    Serial.println("Trying Y Displacement Reset");
  }
  motorMoving = XY12;
  switch(XY12){
    case X12:
    digitalWrite(motorX1X2,HIGH);
    delayMicroseconds(motorPWM);
    digitalWrite(motorX1X2,LOW);
    delayMicroseconds(motorPWM);
    break;
    case X21:
    digitalWrite(motorX2X1,HIGH);
    delayMicroseconds(motorPWM);
    digitalWrite(motorX2X1,LOW);
    delayMicroseconds(motorPWM);
    break;
    case Y12:
    digitalWrite(motorY1Y2,HIGH);
    delayMicroseconds(motorPWM);
    digitalWrite(motorY1Y2,LOW);
    delayMicroseconds(motorPWM);
    break;
    case Y21:
    digitalWrite(motorY2Y1,HIGH);
    delayMicroseconds(motorPWM);
    digitalWrite(motorY2Y1,LOW);
    delayMicroseconds(motorPWM);
    break;
  }
  
}

void TurnOnSpray(){
  digitalWrite(motorZ1Z2,HIGH);
  digitalWrite(motorZ2Z1,HIGH);
  
  if(digitalRead(limitZ2)!=LOW){
    Spray = TurningOn;
    digitalWrite(motorZ1Z2,HIGH);
    delayMicroseconds(sprayPWM);
    digitalWrite(motorZ1Z2,LOW);
    delayMicroseconds(sprayPWM);
  }else{
    Serial.println("Spray is already On");
    Spray = On;
  }
}
void TurnOffSpray(){
  digitalWrite(motorZ1Z2,HIGH);
  digitalWrite(motorZ2Z1,HIGH);
  
  if(digitalRead(limitZ1)!=LOW){
    Spray = TurningOff;
    digitalWrite(motorZ2Z1,HIGH);
    delayMicroseconds(sprayPWM);
    digitalWrite(motorZ2Z1,LOW);
    delayMicroseconds(sprayPWM);
  }else{
    Serial.println("Spray is already Off");
    Spray = Off;
  }
}


#include<stdio.h>
#include<stdlib.h>
#include <wiringPi.h> // Include WiringPi library!
#include <softPwm.h>

#define FOLLOWING_LINE 1
#define STOPPED 2
#define NO_LINE 3
#define rightMaxSpeed 30
#define leftMaxSpeed 30
#define adjTurn 500

const int M1_softPWM1 = 7;
const int M1_softPWM2 = 0;
const int M2_softPWM1 = 2;
const int M2_softPWM2 = 3;
float Kp=30;
float Ki=0;
float Kd=0;

float error=0, P=0, I=0, D=0, PIDvalue=0;
float previousError=0, previousI=0;

const int LF0 = 27;
const int LF1 = 26;
const int LF2 = 25;
const int LF3 = 23;
const int LF4 = 4;
int mode;

int lineSensor[5]={0, 0, 0, 0, 0};

void readLineSensors()
{
    lineSensor[0] = digitalRead(LF0);
    lineSensor[1] = digitalRead(LF1);
    lineSensor[2] = digitalRead(LF2);
    lineSensor[3] = digitalRead(LF3);
    lineSensor[4] = digitalRead(LF4);
    
    if((   lineSensor[0]== 0 )&&(lineSensor[1]== 0 )&&(lineSensor[2]== 0 )&&(lineSensor[3]== 0 )&&(lineSensor[4]== 1 ))  {mode = FOLLOWING_LINE; error = 4;}
  else if((lineSensor[0]== 0 )&&(lineSensor[1]== 0 )&&(lineSensor[2]== 0 )&&(lineSensor[3]== 1 )&&(lineSensor[4]== 1 ))  {mode = FOLLOWING_LINE; error = 3;}
  else if((lineSensor[0]== 0 )&&(lineSensor[1]== 0 )&&(lineSensor[2]== 0 )&&(lineSensor[3]== 1 )&&(lineSensor[4]== 0 ))  {mode = FOLLOWING_LINE; error = 2;}
  else if((lineSensor[0]== 0 )&&(lineSensor[1]== 0 )&&(lineSensor[2]== 1 )&&(lineSensor[3]== 1 )&&(lineSensor[4]== 0 ))  {mode = FOLLOWING_LINE; error = 1;}
  else if((lineSensor[0]== 0 )&&(lineSensor[1]== 0 )&&(lineSensor[2]== 1 )&&(lineSensor[3]== 0 )&&(lineSensor[4]== 0 ))  {mode = FOLLOWING_LINE; error = 0;}
  else if((lineSensor[0]== 0 )&&(lineSensor[1]== 1 )&&(lineSensor[2]== 1 )&&(lineSensor[3]== 0 )&&(lineSensor[4]== 0 ))  {mode = FOLLOWING_LINE; error =- 1;}
  else if((lineSensor[0]== 0 )&&(lineSensor[1]== 1 )&&(lineSensor[2]== 0 )&&(lineSensor[3]== 0 )&&(lineSensor[4]== 0 ))  {mode = FOLLOWING_LINE; error = -2;}
  else if((lineSensor[0]== 1 )&&(lineSensor[1]== 1 )&&(lineSensor[2]== 0 )&&(lineSensor[3]== 0 )&&(lineSensor[4]== 0 ))  {mode = FOLLOWING_LINE; error = -3;}
  else if((lineSensor[0]== 1 )&&(lineSensor[1]== 0 )&&(lineSensor[2]== 0 )&&(lineSensor[3]== 0 )&&(lineSensor[4]== 0 ))  {mode = FOLLOWING_LINE; error = -4;}
  else if((lineSensor[0]== 1 )&&(lineSensor[1]== 1 )&&(lineSensor[2]== 1 )&&(lineSensor[3]== 1 )&&(lineSensor[4]== 1 ))  {mode = STOPPED; error = 0;}
  else if((lineSensor[0]== 0 )&&(lineSensor[1]== 0 )&&(lineSensor[2]== 0 )&&(lineSensor[3]== 0 )&&(lineSensor[4]== 0 ))  {mode = NO_LINE; error = 0;}
  else if((lineSensor[0]== 1 )&&(lineSensor[1]== 1 )&&(lineSensor[2]== 1 )&&(lineSensor[3]== 1 )&&(lineSensor[4]== 0 ))  {mode = STOPPED; error = 0;}
  else if((lineSensor[0]== 0 )&&(lineSensor[1]== 1 )&&(lineSensor[2]== 1 )&&(lineSensor[3]== 1 )&&(lineSensor[4]== 1 ))  {mode = STOPPED; error = 0;} // MULTIPLE CAZURI DE STOP -- DACA MASINA ARE O DEVIERE DE LA POZITIA NORMALA 
  else if((lineSensor[0]== 0 )&&(lineSensor[1]== 1 )&&(lineSensor[2]== 1 )&&(lineSensor[3]== 1 )&&(lineSensor[4]== 0 ))  {mode = STOPPED; error = 0;}   //AR PUTEA SA GENEREZE ERORI DACA ESTE DIAGONAL PE LINIA DE CONCURS
  else if((lineSensor[0]== 1 )&&(lineSensor[1]== 1 )&&(lineSensor[2]== 1 )&&(lineSensor[3]== 0 )&&(lineSensor[4]== 0 ))  {mode = STOPPED; error = 0;}
  else if((lineSensor[0]== 0 )&&(lineSensor[1]== 0 )&&(lineSensor[2]== 1 )&&(lineSensor[3]== 1 )&&(lineSensor[4]== 1 ))  {mode = STOPPED; error = 0;}   

}

void calculatePID()
{
  P = error;
  I = I + error;
  D = error-previousError;
  PIDvalue = (Kp*P) + (Ki*I) + (Kd*D);
  previousError = error;
}

void printValues(){
    printf("  Citire :  %d   %d %d   %d   %d   %.2f  ERROR : %d\n",lineSensor[0] ,lineSensor[1], lineSensor[2], lineSensor[3], lineSensor[4]  , error ,mode );
	usleep(100000);
}

void go(int speedLeft, int speedRight) {  // parameters will be positive or negative duty cycles

	if (speedLeft > 0) {
		softPwmWrite (M1_softPWM1, speedLeft) ;
		softPwmWrite (M1_softPWM2, 0) ;
	}

	else {
		softPwmWrite (M1_softPWM1, 0) ;
		softPwmWrite (M1_softPWM2, -speedLeft) ;
	}



	if (speedRight > 0) {
		softPwmWrite (M2_softPWM1, speedRight) ;
		softPwmWrite (M2_softPWM2, 0) ;
	}

	else {
		softPwmWrite (M2_softPWM1, 0) ;
		softPwmWrite (M2_softPWM2, -speedRight) ;
	}
	}
	
	
	void motorPIDcontrol(int dutyC)
{
    int rightMotorSpeed = dutyC + PIDvalue;
    int leftMotorSpeed = dutyC - PIDvalue;

    if (rightMotorSpeed > rightMaxSpeed ) rightMotorSpeed = rightMaxSpeed; // prevent the motor from going beyond max speed
    if (leftMotorSpeed > leftMaxSpeed ) leftMotorSpeed = leftMaxSpeed; // prevent the motor from going beyond max speed
    if (rightMotorSpeed < 0) rightMotorSpeed = 0; // keep the motor speed positive
    if (leftMotorSpeed < 0) leftMotorSpeed = 0; // keep the motor speed positive

	go(leftMotorSpeed, rightMotorSpeed);
}

void drive_s(){
	go(0,0);
		}
void mainLF()
{
    
        
        switch (mode)
    {
       
        case NO_LINE:
            drive_s();
            previousError = 0;
        break;

        case FOLLOWING_LINE:
            calculatePID();
            motorPIDcontrol(20);
        break;
    }
    
}



int main()
{
	//int sensor;
	wiringPiSetup();
softPwmCreate (M1_softPWM1, 0, 100);
    softPwmCreate (M1_softPWM2, 0, 100);
    softPwmCreate (M2_softPWM1, 0, 100);
    softPwmCreate (M2_softPWM2, 0, 100);
	while(1){

	    calculatePID();
	    readLineSensors();
		//sensor = digitalRead(4);
	    mainLF();
		printValues();
		
    }

return 0;
}


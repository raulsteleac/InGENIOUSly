#include <wiringPi.h> // Include WiringPi library!
#include <softPwm.h>

const int M1_softPWM1 = 7;
const int M1_softPWM2 = 0;
const int M2_softPWM1 = 2;
const int M2_softPWM2 = 3;
float Kp=30;
float Ki=0;
float Kd=0;

float error=0, P=0, I=0, D=0, PIDvalue=0;
float previousError=0, previousI=0;


void go(int speedLeft, int speedRight);
void drive_s();
void drive_f(int dutyC);
void drive_l(int dutyC);
void drive_r(int dutyC);
void drive_b(int dutyC);
//-------------------STOP--------------------------
void drive_s(){
	go(0,0);
		}

//-------------------FORWARD--------------------------
void drive_f(int dutyC){
	printf("Forward!!!!!!");
	calculatePID();
	go(dutyC - PIDvalue, dutyC + PIDvalue);       //decrease/increase speed to correct
		}

//-------------------BACKWARD--------------------------
void drive_b(int dutyC){
	printf("Back!!!!!!");
	calculatePID();
	go(-dutyC + PIDvalue, -dutyC - PIDvalue);
		}


//---------------------------------------------
void motorFwTime (int dutyC, unsigned int time)
{
  drive_f(int dutyC);
  delay (time);
  drive_s();
}

//---------------------------------------------
void motorBwTime (int dutyC, unsigned int time)
{
  drive_b(int dutyC);
  delay (time);
  drive_s();
}

// -----------------PID value--------------------------
void calculatePID()
{
  P = error;
  I = I + error;
  D = error-previousError;
  PIDvalue = (Kp*P) + (Ki*I) + (Kd*D);
  previousError = error;
}

void motorTurn(int direction, int degrees)
{
  switch(direction)
	{
		case 'S': printf("Stop!\n");
				  drive_s();
				  break;
		case 'F': printf("Drive Forward!\n");
				  drive_f(init->vt.viteza);
				  break;
		case 'L': printf("Drive Left!\n");
				  drive_l(init->vt.viteza);
				  break;
		case 'R': printf("Drive Right!\n");
				  drive_r(init->vt.viteza);
				  break;
		case 'B': printf("Drive Back!\n");
				  drive_b(init->vt.viteza);
				  break;

		}
  delay (round(adjTurn*degrees+1));
  drive_s();
}

void motorPIDcontrol(int dutyC)
{
	go(dutyC - PIDvalue, dutyC + PIDvalue);
}

void checkPIDvalues()
{

  printf("PID: ");
  printf(Kp);
  printf(" - ");
  printf(Ki);
  printf(" - ");
  printf(Kd);

}

void drive_l(int dutyC){
	printf("Left!!!!!!");
	go(-dutyC, dutyC);
		}

void drive_r(int dutyC){
	printf("Right!!!!!!");
	go(dutyC, -dutyC);
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

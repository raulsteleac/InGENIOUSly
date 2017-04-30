#include <wiringPi.h> // Include WiringPi library!
#include <softPwm.h>

const int M1_softPWM1 = 7;
const int M1_softPWM2 = 0;
const int M2_softPWM1 = 2;
const int M2_softPWM2 = 3;

void go(int speedLeft, int speedRight);
void drive_s();
void drive_f(int dutyC);
void drive_l(int dutyC);
void drive_r(int dutyC);
void drive_b(int dutyC);

void drive_s(){
	go(0,0);
		}


void drive_f(int dutyC){
	printf("Forward!!!!!!");
	go(dutyC, dutyC);
		}

void drive_b(int dutyC){
	printf("Back!!!!!!");
	go(-dutyC, -dutyC);
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

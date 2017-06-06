#include <wiringPi.h> // Include WiringPi library!
#include <softPwm.h>


#define FOLLOWING_LINE 1
#define STOPPED 2
#define NO_LINE 3
#define LEFT 4
#define RIGHT 5
#define FWD 6
#define LEFT_TURN 10
#define RIGHT_TURN 11
#define rightMaxSpeed 60
#define leftMaxSpeed 60
#define globalSpeed 40
#define adjTurn 500
#define PIDSPD 40
#define TURNCALIBTIME 1300

const int M1_softPWM1 = 29;
const int M1_softPWM2 = 0;
const int M2_softPWM1 = 2;
const int M2_softPWM2 = 3;
const int STOP_softPWM = 7;
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
int map;
int counter = 0;
int mapPointer = 0;
int moveDecision;
//---------------------------
const int adj = 0;
//float adjTurn = 8;
int adjGoAndTurn = 20;
int THRESHOLD = 150;
const int power = 250;
const int iniMotorPower = 250;
int extraInch = 250;
int myPos,  nextPos, in_state, out_state;
//--------------------------------

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
  //else if((lineSensor[0]== 1 )&&(lineSensor[1]== 1 )&&(lineSensor[2]== 1 )&&(lineSensor[3]== 1 )&&(lineSensor[4]== 0 ))  {mode = STOPPED; error = 0;}
  //else if((lineSensor[0]== 0 )&&(lineSensor[1]== 1 )&&(lineSensor[2]== 1 )&&(lineSensor[3]== 1 )&&(lineSensor[4]== 1 ))  {mode = STOPPED; error = 0;} // MULTIPLE CAZURI DE STOP -- DACA MASINA ARE O DEVIERE DE LA POZITIA NORMALA
  //else if((lineSensor[0]== 0 )&&(lineSensor[1]== 1 )&&(lineSensor[2]== 1 )&&(lineSensor[3]== 1 )&&(lineSensor[4]== 0 ))  {mode = STOPPED; error = 0;}   //AR PUTEA SA GENEREZE ERORI DACA ESTE DIAGONAL PE LINIA DE CONCURS
  //else if((lineSensor[0]== 1 )&&(lineSensor[1]== 1 )&&(lineSensor[2]== 1 )&&(lineSensor[3]== 0 )&&(lineSensor[4]== 0 ))  {mode = STOPPED; error = 0;}
  //else if((lineSensor[0]== 0 )&&(lineSensor[1]== 0 )&&(lineSensor[2]== 1 )&&(lineSensor[3]== 1 )&&(lineSensor[4]== 1 ))  {mode = STOPPED; error = 0;}

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
 //   printf("  Citire :  %d   %d %d   %d   %d   %.2f  ERROR : %d\n",lineSensor[0] ,lineSensor[1], lineSensor[2], lineSensor[3], lineSensor[4]  , error ,mode );
//	usleep(100000);
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

void drive_l(int dutyC){
	printf("LEFT!");
	go(-globalSpeed ,globalSpeed);
		}

void drive_r(int dutyC){
	printf("RIGHT!");
	go(globalSpeed ,-globalSpeed);
		}

void motorTurn(int direction, int degrees)
{
  switch(direction)
	{

		case LEFT: printf("Drive Left!\n");
				  drive_l(globalSpeed);
				  break;
		case RIGHT: printf("Drive Right!\n");
				  drive_r(globalSpeed);
				  break;

		}
  delay ((int)(adjTurn*degrees+1));
  drive_s();
}

void goAndTurn(int direction, int degrees)
{
  motorPIDcontrol(PIDSPD);
  delay(adjGoAndTurn);
  motorTurn(direction, degrees);
}

void movementSignaling(int signal)
{

}
void runExtraInch()
{
 drive_s();
printf(" INCH \n");
  motorPIDcontrol(PIDSPD);
  delay(extraInch);
  drive_s();
}

int getMapDecision(int i,int *traseu)
{
	// traseul din milestone1header : traseu[1024]
	// va contine 34 , 53, 11 etc...
	// 1. Citire mesaj moving in pentru a afla : currentPosition
	// 2. Comparare currentPosition cu nextMapDecision
	// 3. decodare => LEFT RIGHT FWD
	//
	//  mapPointer se va incrementa din 2 in 2 la fiecare moving in
	// IMPLEMENTAREA SEMNALIZARII LUMINOASE TOT AICI
	myPos = in_state;
    nextPos = out_state;
    //printf("Curr: %d\n", myPos);
    //printf("Next: %d\n", myPos);

    if((myPos+1)%4==nextPos) {moveDecision = LEFT_TURN; printf("Decision: Left\n");movementSignaling(moveDecision);}
    if((myPos+2)%4==nextPos) {moveDecision = FWD;printf("Decision: Forward\n");movementSignaling(moveDecision);}
    if((myPos+3)%4==nextPos) {moveDecision = RIGHT_TURN;printf("Decision: Right\n");movementSignaling(moveDecision);}
    return moveDecision;
}

int stopSignaling(int moveDecision)
{
    digitalWrite (STOP_softPWM, HIGH) ;
    return 0;
}


void controlLF(int *traseu)
{
          switch (mode)
      {

         case STOPPED:
                  drive_s();
        printf(" LINE , %d \n", counter);
        delay(1000);
        if((counter % 3 )==1)
        {
          printf(" Middle -> Decision \n");
          map = getMapDecision(mapPointer,traseu);
          switch (map)
     				 {
        				case FWD:
    						readLineSensors();
    						calculatePID();
                motorPIDcontrol(PIDSPD);
           					break;

    					case RIGHT_TURN:
    					printf(" right \n");
    						runExtraInch();

    						go(-40, 40);
    						delay(TURNCALIBTIME);
    						readLineSensors();
    						calculatePID();
                motorPIDcontrol(PIDSPD);
           					break;

    					case LEFT_TURN:
    					  printf(" rleft \n");
    						runExtraInch();
    						go(40, -40);
    						delay(TURNCALIBTIME);
    						readLineSensors();
    						calculatePID();
              	motorPIDcontrol(PIDSPD);
           					break;

    				} }

    			    previousError = 0;
                		counter += 1;
    	    		runExtraInch();
    	    		readLineSensors();
    				break;

            case NO_LINE:
                drive_s();
                previousError = 0;
            break;

            case FOLLOWING_LINE:
    	          readLineSensors();
                calculatePID();
                motorPIDcontrol(PIDSPD);
            break;
        }

    }

#include "mileston1header.h"
#include "sensorsalgorithms.h"
#include <unistd.h>
#include <bcm2835.h>
#include "rc522.h"
#include "rfid.h"
#include "motorControl.h"

void algLF()
{
    
        
        switch (mode)
    { 

       case STOPPED:
            		drive_s();
			printf(" LINE , %d \n", counter);
			//digitalWrite (STOP_softPWM, HIGH) ;
			//digitalWrite (STOP_softPWM, LOW) ;
			if((counter % 3 )==1)
			{
				printf(" Middle -> Decision \n");
				
				//map = getMapDecision(mapPointer,traseu);
				map = FWD;
				if (counter > 3) map = RIGHT_TURN;
				if (counter > 5) map = LEFT_TURN;
				if (counter > 8) map = FOLLOWING_LINE;
				switch (map)
 				 {
    				case FWD:
						readLineSensors();
						calculatePID();
                    				motorPIDcontrol(PIDSPD);
       					break;

					case RIGHT_TURN:
					printf(" right \n");
						//goAndTurn (RIGHT, 90);
						//drive_r(40);
						runExtraInch();
					/*	lineSensor[4] = digitalRead(LF4);
						while(lineSensor[4])
						{	
							lineSensor[4] = digitalRead(LF4);
							go(-40, 40);
						} */
						go(-40, 40);
						delay(TURNCALIBTIME);
						readLineSensors();
						calculatePID();
                    				motorPIDcontrol(PIDSPD);
       					break;

					case LEFT_TURN:
					     printf(" rleft \n");
						//goAndTurn (LEFT, 90);
						//drive_l(40);
						runExtraInch();
						/* lineSensor[0] = digitalRead(LF0);
						while(lineSensor[0])
						{	
							lineSensor[0] = digitalRead(LF0);
							go(40, -40);
						} */
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


int main(){
	wiringPiSetup();
softPwmCreate (M1_softPWM1, 0, 100);
    softPwmCreate (M1_softPWM2, 0, 100);
    softPwmCreate (M2_softPWM1, 0, 100);
    softPwmCreate (M2_softPWM2, 0, 100);
    //pinMode (STOP_softPWM, OUTPUT) ;
    counter = 0;
    //digitalWrite (STOP_softPWM, LOW) ;
readLineSensors();readLineSensors();readLineSensors();readLineSensors();readLineSensors();readLineSensors();readLineSensors();readLineSensors();readLineSensors();readLineSensors();readLineSensors();readLineSensors();
    while(1){
	readLineSensors();
	calculatePID();
	motorPIDcontrol(PIDSPD);
	algLF();
	}
return 0;
}

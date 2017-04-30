
#include <wiringPi.h> // Include WiringPi library!
#include <softPwm.h>
#include "lineSensor.h"
#include "lineMoto.h"

# define STOPPED 0
# define FOLLOWING_LINE 1
# define NO_LINE 2
string command;
string device;
//---------------------------------------------
void sensorInit()
{
  // line follow sensors
  pinMode(LF0, INPUT);
  pinMode(LF1, INPUT);
  pinMode(LF2, INPUT);
  pinMode(LF3, INPUT);
  pinMode(LF4, INPUT);
  checkPIDvalues();

}

void mainLF()
{
    while(1){
        readLineSensors();
        switch (mode)
    {
        case STOPPED:
            drive_s();
            printf("The End");
            previousError = error;
        break;

        case NO_LINE:
            drive_s();
            motorTurn(L, 180);
            previousError = 0;
        break;

        case FOLLOWING_LINE:
            calculatePID();
            motorPIDcontrol();
        break;
    }
    }
}

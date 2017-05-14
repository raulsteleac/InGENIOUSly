
#include <wiringPi.h> // Include WiringPi library!
#include <softPwm.h>
#include "lineSensor.h"
#include "lineMoto.h"

# define STOPPED 0
# define FOLLOWING_LINE 1
# define NO_LINE 2
string command;
string device;
int counter = 0;  // pentru a contoriza de cate ori intra in intersectie 

const int adj = 0; 
float adjTurn = 8;
int adjGoAndTurn = 800;
THRESHOLD = 150
const int power = 250; 
const int iniMotorPower = 250; 
int extraInch = 200;

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

void runExtraInch(void)
{
  motorPIDcontrol();
  delay(extraInch);
  motorStop();
}

void goAndTurn(int direction, int degrees)
{
  motorPIDcontrol();
  delay(adjGoAndTurn);
  motorTurn(direction, degrees);
}

void lineDecision()
{
  readLFSsensors();
  switch (mode)
  {
    case NO_LINE:
       motorStop();
       goAndTurn (LEFT, 180);
       break;
    case CONT_LINE:
       runExtraInch();
       readLFSsensors();
       if (mode == CONT_LINE) mazeEnd();
       else goAndTurn (LEFT, 90); // or it is a "T" or "Cross"). In both cases, goes to LEFT
       break;
    case RIGHT_TURN:
       runExtraInch();
       readLFSsensors();
       if (mode == NO_LINE) goAndTurn (RIGHT, 90);
       break;
    case LEFT_TURN:
       goAndTurn (LEFT, 90);
       break;
    case FOLLOWING_LINE:
       followingLine();
       break;
  }
}


void mainLF()
{
    while(1){
        
        switch (mode)
    {
        case STOPPED:
            counter += 1;
            if(counter % 3 == 0)
            {
                //ar putea sa trimita si mesajul de MOVING IN
                printf("MOVING IN!");
                drive_s();           //se opreste 3 secunde
                delay(3000);
                motorFwTime(50, 100);  //merge inainte pentru a iesi de pe linia perpendiculara
                drive_s();             // se opreste 
                calculatePID();         //recalculeaza eroarea
                previousError = error;
            }
            else if(counter % 3 == 1)
            {   
                printf("MIDDLE!");     // se afla in centrul intersectiei
                motorFwTime(50, 100);  // ignora linia perpendiculara
                drive_s();              
                calculatePID();
                previousError = error;
            }
            else
            {
                printf("MOVING OUT!"); //iese din intersectie
                motorFwTime(50, 100);  // ignora linia perpendiculara
                drive_s();
                calculatePID();
                previousError = error;

            }
            
        break;

        case NO_LINE:
            drive_s();
            motorTurn(L, 180);
            previousError = 0;
        break;

        case FOLLOWING_LINE:
            calculatePID();
            motorPIDcontrol(50);
        break;
    }
    }
}


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

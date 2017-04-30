#include <wiringPi.h> // Include WiringPi library!
#include <softPwm.h>

//-------------------------------------------------------------
/* read line sensors values

Sensor Array 	Error Value
0 0 0 0 1	    4
0 0 0 1 1	    3
0 0 0 1 0	    2
0 0 1 1 0	    1
0 0 1 0 0	    0
0 1 1 0 0	   -1
0 1 0 0 0	   -2
1 1 0 0 0	   -3
1 0 0 0 0	   -4

1 1 1 1 1        0 Robot found continuous line :STOPPED => DECISION => CROSSING
0 0 0 0 0        0 Robot found no line: NO_LINE turn 180o

*/

const int LF0 = 12;
const int LF0 = 18;
const int LF0 = 17;
const int LF0 = 16;
const int LF0 = 19;

int lineSensor[5]={0, 0, 0, 0, 0};

//-----------------Line SENSOR INIT----------------------------


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
}

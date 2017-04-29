#include <stdio.h>    // Used for printf() statements
#include <wiringPi.h> // Include WiringPi library!
#include <softPwm.h>

const int M1_softPWM1 = 4;
const int M1_softPWM2 = 5;
const int M2_softPWM1 = 6;
const int M2_softPWM2 = 10;



const int pwmValue = 75; // Use this to set an LED brightness
int delayOn;

int startPWM(int pin, int dutyC)     //dutyC at freq of 50Hz
{
    delayOn  = (dutyC * 20) / 100;
    digitalWrite(pin, HIGH); // Turn LED ON
    delay(delayOn); // Wait 75ms
    digitalWrite(pin, LOW); // Turn LED OFF
    delay(20-delayOn);

return 0;
}

void motor(int speedLeft, int speedRight) {  // parameters will be positive or negative duty cycles

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
 return 0;
 }

int main(void)
{
    // Setup stuff:
    wiringPiSetupGpio();

    //softPwmCreate (int pin, int initialValue, int pwmRange) ;
    softPwmCreate (M1_softPWM1, 0, 100);
    softPwmCreate (M1_softPWM2, 0, 100);
    softPwmCreate (M2_softPWM1, 0, 100);
    softPwmCreate (M2_softPWM2, 0, 100);
    printf("PWM  is running! Press CTRL+C to quit.\n");

    motor(50, 50);
    delay(1000);
    motor(-50, -50);
    delay(1000);
    motor(50, -50);
    delay(1000);
    motor(-50, 50);
    delay(1000);

    return 0;
}

#include <stdio.h>
#include <stdlib.h>
#include "mileston1header.h"
//#include "motorDrive.h"
// TRATAREA ERRORILOR

//DECLARAREA VARIABILELOR GLOBALE
pthread_mutex_t mutex = PTHREAD_MUTEX_INITIALIZER;
int flag;
pthread_mutex_t mutex;
pthread_cond_t flag_cv;
struct container *init;
int soc;
socklen_t receiverlen;
char buffer[255];
struct sockaddr_in receiver;
//INITIALIZAREA VARIABILELOR GLOBALE

void initial()
{
      init=(struct container *)malloc(sizeof(struct container));
      init->directie='B';
      init->vt.viteza=30;
      pthread_mutex_init(&mutex,NULL);
      pthread_cond_init(&flag_cv,NULL);
      flag=0;
}
//O AFISARE
void* afis()
{

     printf("%c %d\n",init->directie,init->vt.viteza);
     return 0;
}

// CLIENTUL UDP , AICI SE FACE COMUNICAREA SI STOCAREA DATELOR
void* udpclient(struct container *init)
{
initializesocket(&soc,receiverlen,receiver);
broadcastpermissioner(&soc);
letssend(&soc,receiverlen);
		while(1)
		{
      letsreceive(&soc,receiverlen,buffer,receiver);
      setbufferul(buffer,init);

//AICI TRANSMIT SEMNALUL CU SCOPA DE A ANUNTA CA S-A MODIFICAT MEMORIA , FLAGUL CV

	     pthread_mutex_lock(&mutex);
	     flag=1;
	     pthread_cond_signal(&flag_cv);
	     pthread_mutex_unlock(&mutex);
		}
	close(soc);
	return 0;

}
// AICI V-A FI INTRODUSA FUNCTIA DE MISCARE A MOTOARELOR
void *motors(struct container *init)
{
	char miscareAnterioara;
	int vitezaAnterioara;
	while(1)
	{
	
		if(init->directie!='S')
		{
			 miscareAnterioara=init->directie;
			 vitezaAnterioara=init->vt.viteza;
		}
	pthread_mutex_lock(&mutex);
//AICI SE ASTEAPTA SEMNALUL
	while(flag==0)
	{
		pthread_cond_wait(&flag_cv,&mutex);
	}
	pthread_mutex_unlock(&mutex);
//......AICI VA FI APELATA FUNCTIA DE MISCARE A MOTOARELOR , threadul asteapta ca memoria sa fi schimbata si apoi va anunta functia
// DACA MEMORIA NU S-A SCHIMBAT FUNCTIA VA CONTINUA CU ACEEASI DEIRECTIE
	
	if(flag)
	{/*
	switch(init->directie)
	{
		case 'S': printf("Stop!\n");
				  drive_s();
				  
				  delay((init->vt.timp)*1000);
				  if(miscareAnterioara=='F')
				  	drive_f(vitezaAnterioara);
				  else
				  	drive_b(vitezaAnterioara);
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
		*/
	printf("%c %d\n",init->directie,init->vt.viteza);
	flag=0;
	}
	}
	return 0;
}

void threadcreator()
{
	 // Setup stuff:
   /* wiringPiSetup();

    softPwmCreate (M1_softPWM1, 0, 100);
    softPwmCreate (M1_softPWM2, 0, 100);
    softPwmCreate (M2_softPWM1, 0, 100);
    softPwmCreate (M2_softPWM2, 0, 100);
    printf("PWM  is running! Press CTRL+C to quit.\n");
    */
    //initializare thread-uri
    
	initial();
	pthread_t t1,t2;
	pthread_create(&t1,NULL,(void*)udpclient,init);
	pthread_create(&t2,NULL,(void*)motors,init);
	pthread_join(t1,NULL);
	pthread_join(t2,NULL);

}
int main()
{
threadcreator();

return 0;
}

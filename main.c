#include <stdio.h>
#include <stdlib.h>
#include "mileston1header.h"
//#include "motorDrive.h"
// TRATAREA ERRORILOR

//DECLARAREA VARIABILELOR GLOBALE
pthread_mutex_t mutex = PTHREAD_MUTEX_INITIALIZER;
pthread_mutex_t mutex2 = PTHREAD_MUTEX_INITIALIZER;
int flag1,flag2;
pthread_cond_t flag1_cv,flag2_cv;
struct container * init;
int socr,soct;
socklen_t receiverlen;
socklen_t transmitterlen;
char buffer[255];
struct sockaddr_in receiver;
struct sockaddr_in transmitter;
//INITIALIZAREA VARIABILELOR GLOBALE

void initial()
{
      init=(struct container *)malloc(sizeof(struct container));
      init->directie='F';
      init->viteza=70;
      pthread_mutex_init(&mutex,NULL);
      pthread_mutex_init(&mutex2,NULL);
      pthread_cond_init(&flag1_cv,NULL);
      pthread_cond_init(&flag2_cv,NULL);
      flag1=0;
      flag2=0;
      initializesocket(&socr,receiverlen,receiver);
      broadcastpermissioner(&socr);
      initializesocket(&soct,transmitterlen,transmitter);
      broadcastpermissioner(&soct);
}
//O AFISARE
void* afis()
{

     printf("%c %d %d\n",init->directie,init->viteza,init->timp);
     return 0;
}

// CLIENTUL UDP , AICI SE FACE COMUNICAREA SI STOCAREA DATELOR
void* udpclientreader(struct container *init)
{
gethostip(socr);
		while(1)
		{

      letsreceive(&socr,receiverlen,buffer,receiver);
      setcontainer(buffer,init);

//AICI TRANSMIT SEMNALUL CU SCOPA DE A ANUNTA CA S-A MODIFICAT MEMORIA , FLAGUL CV

	     pthread_mutex_lock(&mutex);
	     flag1=1;
	     pthread_cond_signal(&flag1_cv);
	     pthread_mutex_unlock(&mutex);
  printf("Clinetul asteapta o comanda :\n" );
		}
	close(socr);
	return 0;

}
void* udpclienttransmitter()
{
  letssend(&socr,receiverlen);
  memset((char*)&transmitter,0,sizeof(transmitter));
  transmitter.sin_family=AF_INET;
  transmitter.sin_port=htons(5000);
  transmitterlen=sizeof(transmitter);
  inet_pton(AF_INET, "255.255.255.255", &transmitter.sin_addr);
  int i=1;
  char numar ;
      while(1)
      {

        if(sendto(soct,"111",strlen("111"),0,(struct sockaddr*)&transmitter,transmitterlen)==-1)
            err("Nu merge sendto");
            i++;
         sleep(2);
      }
}
// AICI V-A FI INTRODUSA FUNCTIA DE MISCARE A MOTOARELOR
void *motors(struct container *init)
{
	//char miscareAnterioara;
	//int vitezaAnterioara;
	while(1)
	{

	/*	if(init->directie!='S')
		{
			 miscareAnterioara=init->directie;
			 vitezaAnterioara=init->viteza;
		}
    */
	pthread_mutex_lock(&mutex);
//AICI SE ASTEAPTA SEMNALUL
	while(flag1==0)
	{
		pthread_cond_wait(&flag1_cv,&mutex);
	}
	pthread_mutex_unlock(&mutex);
//......AICI VA FI APELATA FUNCTIA DE MISCARE A MOTOARELOR , threadul asteapta ca memoria sa fi schimbata si apoi va anunta functia
// DACA MEMORIA NU S-A SCHIMBAT FUNCTIA VA CONTINUA CU ACEEASI DEIRECTIE

	if(flag1)
	{/*
	switch(init->directie)
	{
		case 'S': printf("Stop!\n");
				  drive_s();
				  delay((init->timp)*1000);
				  if(miscareAnterioara=='F')
				  	drive_f(vitezaAnterioara);
				  else
				  	drive_b(vitezaAnterioara);
				  break;
		case 'F': printf("Drive Forward!\n");
				  drive_f(init->viteza);
          sleep(init->timp);
          drive_s();
				  break;
		case 'L': printf("Drive Left!\n");
				  drive_l(init->viteza);
          sleep(init->timp);
          drive_s();
				  break;
		case 'R': printf("Drive Right!\n");
				  drive_r(init->viteza);
          sleep(init->timp);
          drive_s();
				  break;
		case 'B': printf("Drive Back!\n");
				  drive_b(init->viteza);
          sleep(init->timp);
          drive_s();
				  break;
		}
		*/
    if(init->directie=='B' || init->directie=='F')
	     printf("%c %d %d\n",init->directie,init->viteza,init->timp);
    else
      printf("%c %d\n",init->directie,init->timp );
	flag1=0;
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
	pthread_t t1,t2,t3;
	pthread_create(&t1,NULL,(void*)udpclientreader,init);
  pthread_create(&t2,NULL,(void*)udpclienttransmitter,NULL);
  pthread_create(&t3,NULL,(void*)motors,init);
	pthread_join(t1,NULL);
	pthread_join(t2,NULL);
  pthread_join(t3,NULL);

}
int main()
{
threadcreator();

return 0;
}

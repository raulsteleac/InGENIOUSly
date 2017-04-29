
#include "mileston1header.h"
#include "sensorsalgorithms.h"
//#include "motorDrive.h"
// TRATAREA ERRORILOR

//DECLARAREA VARIABILELOR GLOBALE
pthread_mutex_t mutex = PTHREAD_MUTEX_INITIALIZER;
pthread_mutex_t mutex2 = PTHREAD_MUTEX_INITIALIZER;
int flag1,flag2,flag3;
pthread_cond_t flag1_cv,flag2_cv,flag3_cv;
struct container * conti;
int socr,soct;
socklen_t receiverlen;
socklen_t transmitterlen;
char buffer[8192];
char state[5];
struct sockaddr_in receiver;
struct sockaddr_in transmitter;
//INITIALIZAREA VARIABILELOR GLOBALE

void initial()
{
      conti=(struct container *)malloc(sizeof(struct container));
      conti->directie='F';
      conti->viteza=70;
      pthread_mutex_init(&mutex,NULL);
      pthread_mutex_init(&mutex2,NULL);
      pthread_cond_init(&flag1_cv,NULL);
      pthread_cond_init(&flag2_cv,NULL);
      pthread_cond_init(&flag3_cv,NULL);
      flag1=0;
      flag2=0;
      flag3=0;
}
// CLIENTUL UDP , AICI SE FACE COMUNICAREA SI STOCAREA DATELOR
void* udpclientreceiver(struct container *conti)
{
	char buffer2[255];
	int soc;
	struct sockaddr_in receiver;
	socklen_t receiverlen;
	if((soc=socket(AF_INET,SOCK_DGRAM,IPPROTO_UDP))==-1)
				err("Nu merge socket");
	//setare conexiune , de unde primim datele
	memset((char*)&receiver,0,sizeof(receiver));
	receiver.sin_family=AF_INET;
	receiver.sin_port=htons(5000);
	receiverlen=sizeof(receiver);
	inet_pton(AF_INET, "255.255.255.255", &receiver.sin_addr);
	if(bind(soc,(struct sockaddr*)&receiver,sizeof(receiver))==-1)
			err("Nu merge bind");
printf("\n Clientul s-a conectat la : %s si portul %d\n",inet_ntoa(receiver.sin_addr),ntohs(receiver.sin_port));

    conti->rfidwt=conti->rfidwt|3;
    conti->rfidwt=conti->rfidwt<<4|1;

char mine;
mine=mine&0;
mine=mine|3;

gethostip(soc);
//Receptionam primele instructiuni de la acces point
do{
    letsreceive(&soc,receiverlen,buffer,receiver);
  }
    while(strlen(buffer)<5);
    spargeremesajinitial(buffer,conti);
      int i;
      printf("\n###########################TRASEU\n");
      for(i=0;i<strlen(conti->traseu);i++)
                printf("%d %d \n",conti->traseu[i]>>4&15,conti->traseu[i]&15);
      printf("\n###########################\n");
//trimitem semnalul ca instructiunile au fost receptionate
      pthread_mutex_lock(&mutex2);
      flag2=1;

;
      pthread_cond_signal(&flag2_cv);
      pthread_mutex_unlock(&mutex2);
    //  pthread_mutex_lock(&mutex2);

      //
    //  pthread_mutex_unlock(&mutex2);
//receptionarea celorlate mesaje
    memset(buffer,0,sizeof(buffer));
		while(1)
		{
    letsreceive(&soc,receiverlen,buffer,receiver);
		if(strcmp(buffer,buffer2)!=0 && buffer[1]!=mine)
		 {
		   setcontainer(buffer,conti);
	     pthread_mutex_lock(&mutex);
	     flag1=1;
       flag3=1;
       pthread_cond_signal(&flag3_cv);
	     pthread_cond_signal(&flag1_cv);
	     pthread_mutex_unlock(&mutex);


	     printf("Clinetul asteapta o comanda :\n" );
  strcpy(buffer2,buffer);
    }

    }
	close(socr);
	return 0;

}
void* udpclienttransmitter()
{

  memset((char*)&transmitter,0,sizeof(transmitter));
  transmitter.sin_family=AF_INET;
  transmitter.sin_port=htons(5000);
  transmitterlen=sizeof(transmitter);
  inet_pton(AF_INET, "255.255.255.255", &transmitter.sin_addr);
 if((soct=socket(AF_INET,SOCK_DGRAM,IPPROTO_UDP))==-1)
          err("Nu merge socket");
 broadcastpermissioner(&soct);

 	pthread_mutex_lock(&mutex2);
 while(flag2==0)
 {
   pthread_cond_wait(&flag2_cv,&mutex2);
 }
 pthread_mutex_unlock(&mutex2);

 state[0]=state[0]|1;
 state[1]=state[1]|3;
 state[2]=state[2]|(conti->tipmasina>>4&15);
 state[2]=state[2]<<4|conti->tipmasina&15;
 state[3]=state[3]|(conti->rfidwt>>4&15);
 state[3]=state[3]<<4|conti->rfidwt&15;
 state[4]=state[4]|3;
sleep(0.1);
  int i=1;
  char numar ;
      while(1)
      {

        if(sendto(soct,state,strlen(state),0,(struct sockaddr*)&transmitter,transmitterlen)==-1)
            err("Nu merge sendto");
            i++;
         sleep(2);
      }
}
// AICI V-A FI INTRODUSA FUNCTIA DE MISCARE A MOTOARELOR
/*
void *motors(struct container *conti)
{
	char miscareAnterioara;
	int vitezaAnterioara;
	while(1)
	{

		if(conti->directie!='S')
		{
			 miscareAnterioara=conti->directie;
			 vitezaAnterioara=conti->viteza;
		}

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
	{
	switch(conti->directie)
	{
		case 'S': printf("Stop!\n");
				  drive_s();
				  delay((conti->timp)*1000);
				  if(miscareAnterioara=='F')
				  	drive_f(vitezaAnterioara);
				  else
				  	drive_b(vitezaAnterioara);
				  break;
		case 'F': printf("Drive Forward!\n");
				  drive_f(conti->viteza);
          sleep(conti->timp);
          drive_s();
				  break;
		case 'L': printf("Drive Left!\n");
				  drive_l(conti->viteza);
          sleep(conti->timp);
          drive_s();
				  break;
		case 'R': printf("Drive Right!\n");
				  drive_r(conti->viteza);
          sleep(conti->timp);
          drive_s();
				  break;
		case 'B': printf("Drive Back!\n");
				  drive_b(conti->viteza);
          sleep(conti->timp);
          drive_s();
				  break;
		}

    if(conti->directie=='B' || conti->directie=='F')
	     printf("%c %d %d\n",conti->directie,conti->viteza,conti->timp);
    else
      printf("%c %d\n",conti->directie,conti->timp );
	flag1=0;
	}
	}
	return 0;
}
*/
void *rfiddriver(struct container *conti)
{
  while (1)
  {
  pthread_mutex_lock(&mutex);
  while(flag1==0)
  {
    pthread_cond_wait(&flag1_cv,&mutex);
  }
  pthread_mutex_unlock(&mutex);
  printf("###RFIDRFIDRFIDRFIDRFIDRFIDRFIDRFIDRFID####\n");
  sleep(1);
}
}
void *lfdriver(struct container *conti)
{
  while (1)
  {
  pthread_mutex_lock(&mutex);
  while(flag3==0)
  {
    pthread_cond_wait(&flag3_cv,&mutex);
  }
  pthread_mutex_unlock(&mutex);
   printf("###LFLFLFLLFLFLFLFLFLLFLFLFLF####\n");
   sleep(1);
   }
}
void threadcreator()
{
	 // Setup stuff:
  /*  wiringPiSetup();
    softPwmCreate (M1_softPWM1, 0, 100);
    softPwmCreate (M1_softPWM2, 0, 100);
    softPwmCreate (M2_softPWM1, 0, 100);
    softPwmCreate (M2_softPWM2, 0, 100);
    printf("PWM  is running! Press CTRL+C to quit.\n");
*/
    //initializare thread-uri

	initial();
	pthread_t t1,t2,t3,t4,t5;
	pthread_create(&t1,NULL,(void*)udpclientreceiver,conti);
  pthread_create(&t2,NULL,(void*)udpclienttransmitter,NULL);
  //pthread_create(&t3,NULL,(void*)motors,conti);
  pthread_create(&t4,NULL,(void*)rfiddriver,NULL);
  pthread_create(&t5,NULL,(void*)lfdriver,NULL);
	pthread_join(t1,NULL);
	pthread_join(t2,NULL);
  //pthread_join(t3,NULL);
  pthread_join(t4,NULL);
  pthread_join(t5,NULL);
}
int main()
{
threadcreator();

return 0;
}

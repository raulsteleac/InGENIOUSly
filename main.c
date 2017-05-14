
#include "mileston1header.h"
#include "sensorsalgorithms.h"
#include <unistd.h>
#include <bcm2835.h>
#include "rc522.h"
#include "rfid.h"
#include "motorControl.h"
// TRATAREA ERRORILOR

//DECLARAREA VARIABILELOR GLOBALE
pthread_mutex_t mutex = PTHREAD_MUTEX_INITIALIZER;
pthread_mutex_t mutex2 = PTHREAD_MUTEX_INITIALIZER;

int flag1,flag2,flag3,flag4;
pthread_cond_t flag1_cv,flag2_cv,flag3_cv,flag4_cv;

struct container * conti;
int soct;
socklen_t transmitterlen;

char buffer[8192];
char state[5];
struct sockaddr_in transmitter;

uint8_t data[6];
uint32_t rfids[3][6];
uint32_t rfx=0;

int z=0;
void initial()
{
      conti=(struct container *)malloc(sizeof(struct container));

      pthread_mutex_init(&mutex,NULL);
      pthread_mutex_init(&mutex2,NULL);
      pthread_cond_init(&flag1_cv,NULL);
      pthread_cond_init(&flag2_cv,NULL);
      pthread_cond_init(&flag3_cv,NULL);
      pthread_cond_init(&flag4_cv,NULL);

      flag1=0;
      flag2=0;
      flag3=0;
      flag4=0;

	rfids[1][3]=0xE07C85D9;
	rfids[1][2]=0x587C18FC;
	rfids[1][1]=0x337E55B8;
	rfids[1][4]=0x00AB92CF;
	rfids[2][3]=0X24ABB2E8;
	rfids[2][2]=0xB3EDBA5C;
       rfids[2][1]=0x4979FB4B;
 	rfids[2][4]=0xC5ABDE3B;
}

char rfiddecoder(uint32_t data)
{

	int i,j,ok=0;
	char rf=rf&0;
	for(i=1;i<=2;i++)
		{
		for(j=1;j<=4;j++)
			{
			if(data==rfids[i][j])
				{
				rf=rf|i;
				rf=rf<<4|j;
				ok=1;
		    }
      }
    }
return rf;
}
void* udpclienttransmitter()
{

  /*     pthread_mutex_lock(&mutex2);

       flag3=1;
       flag4=1;

       pthread_cond_signal(&flag3_cv);
       pthread_cond_signal(&flag4_cv);

       pthread_mutex_unlock(&mutex2);
*/

  memset((char*)&transmitter,0,sizeof(transmitter));
  transmitter.sin_family=AF_INET;
  transmitter.sin_port=htons(5000);
  transmitterlen=sizeof(transmitter);
  inet_pton(AF_INET, "255.255.255.255", &transmitter.sin_addr);

  if((soct=socket(AF_INET,SOCK_DGRAM,IPPROTO_UDP))==-1)
          err("Nu merge socket");
  broadcastpermissioner(&soct);

 state[0]=state[0]|1;
 state[1]=state[1]|3;
 state[2]=0;
 state[3]=0;

  int i=0,sec=0;
      while(1)
  {
	if(rfx!=0)
	   conti->rfidwt=rfiddecoder(rfx);
	printf(" DATA RECEIVED : %d%d   ",conti->rfidwt>>4&15,conti->rfidwt&15);
	printf("  RFX : %X \n",rfx);
     if( conti->rfidwt!=state[2])
    {
          state[2]=0;
          state[2]=state[2]|(conti->rfidwt>>4&15);
          state[2]=state[2]<<4|(conti->rfidwt&15);
	        if(state[2]!=0)
	            i++;
    }
        if(i==1)
      		{
            flag1=1;
            state[3]=1;
            sec++;
          }
        if(sec==30)
           		{
                 state[3]=2;
  		flag1=0;
                 sec=0;
		 i++;
               }
				if(i==3)
          			 {
					state[3]=3;
					i=0;
				}
	//if(state[3]!=0)
        if(sendto(soct,state,strlen(state),0,(struct sockaddr*)&transmitter,transmitterlen)==-1)
            err("Nu merge sendto");

         usleep(100000);

      }
}
// AICI V-A FI INTRODUSA FUNCTIA DE MISCARE A MOTOARELOR

void *motors(struct container *conti)
{
}

void *rfiddriver(struct container *conti)
{

    if (!bcm2835_init())
    {
      printf("bcm2835_init failed. Are you running as root??\n");
      return 1;
    }

    if (!bcm2835_spi_begin())
    {
      printf("bcm2835_spi_begin failedg. Are you running as root??\n");
      return 1;
    }

    InitRc522();
    uint8_t pTagType,d2[6];
    char status;
    while(1)
    {
      status=PcdRequest(PICC_REQIDL,&pTagType);
      if(status==TAG_OK)
        status=PcdAnticoll(PICC_ANTICOLL1 , data);
      int i;
      if(d2!=data)
	      {
			       rfx=0;
			       rfx=data[1]|(data[2]<<8)|(data[3]<<16)|(data[4]<<24);

             for (i=0; i<5; i++)
		           {
			              d2[i]=data[i];
		                }
             printf("\n");
           }
}

    bcm2835_spi_end();
    bcm2835_close();
    return 0;

}
void *lfdriver(struct container *conti)
{
	//int sensor;
	wiringPiSetup();
softPwmCreate (M1_softPWM1, 0, 100);
    softPwmCreate (M1_softPWM2, 0, 100);
    softPwmCreate (M2_softPWM1, 0, 100);
    softPwmCreate (M2_softPWM2, 0, 100);
	while(1){
	if(flag1==1)
		drive_s();
else
{
	    calculatePID();
	    readLineSensors();
		//sensor = digitalRead(4);
	    mainLF();
		printValues();
}		
    }

return 0;
}
void threadcreator()
{

    //initializare thread-uri

	initial();
	pthread_t t2,t3,t4,t5;
  pthread_create(&t2,NULL,(void*)udpclienttransmitter,NULL);
pthread_create(&t3,NULL,(void*)motors,conti);
  pthread_create(&t4,NULL,(void*)rfiddriver,NULL);
  pthread_create(&t5,NULL,(void*)lfdriver,NULL);
	pthread_join(t2,NULL);
 pthread_join(t3,NULL);
  pthread_join(t4,NULL);
  pthread_join(t5,NULL);
}
int main()
{
threadcreator();

return 0;
}

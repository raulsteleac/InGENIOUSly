

#include "mileston1header.h"
//#include "sensorsalgorithms.h"
#include <unistd.h>
//#include <bcm2835.h>
//#include "rc522.h"
//#include "rfid.h"
//#include "motorControl.h"
// TRATAREA ERRORILOR

//DECLARAREA VARIABILELOR GLOBALE
pthread_mutex_t mutex1 = PTHREAD_MUTEX_INITIALIZER;
pthread_mutex_t mutex2 = PTHREAD_MUTEX_INITIALIZER;

int flag1,rfwififlag,lfwififlag,flag4;
pthread_cond_t flag1_cv,rfwififlag_cv,lfwififlag_cv,flag4_cv;

struct container * conti;
int socr,soct;
socklen_t receiverlen;
socklen_t transmitterlen;

char buffer[8192];
char state[5];
struct sockaddr_in receiver;
struct sockaddr_in transmitter;

uint8_t data[6];
uint32_t rfids[3][6];
uint32_t rfx=0;

int z=0,oprire=0;
void initial()
{
      conti=(struct container *)malloc(sizeof(struct container));

      pthread_mutex_init(&mutex1,NULL);
      pthread_mutex_init(&mutex2,NULL);
      pthread_cond_init(&flag1_cv,NULL);
      pthread_cond_init(&rfwififlag_cv,NULL);
      pthread_cond_init(&lfwififlag_cv,NULL);
      pthread_cond_init(&flag4_cv,NULL);

      flag1=0;
      rfwififlag=0;
      lfwififlag=0;
      flag4=0;
      memset( conti, 0, sizeof(*conti) );
	rfids[1][3]=0xE07C85D9;
	rfids[1][2]=0x587C18FC;
	rfids[1][1]=0x337E55B8;
	rfids[1][4]=0x00AB92CF;
	rfids[2][3]=0X24ABB2E8;
	rfids[2][2]=0xB3EDBA5C;
  rfids[2][1]=0x4979FB4B;
 	rfids[2][4]=0xC5ABDE3B;
}
/////////////////////////////////////
/////////////////////////////////////
/////////////////////////////////////
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
gethostip(soc);

do{
    letsreceive(&soc,receiverlen,buffer,receiver);
  }
    while(strlen(buffer)<5);
      spargeremesajinitial(buffer,conti);
      printf("am iesit");
      int i;
      printf("\n###########################TRASEU\n");
      for(i=0;i<strlen(conti->traseu);i++)
                printf("%d %d \n",conti->traseu[i]>>4&15,conti->traseu[i]&15);
      printf("\n###########################\n");
    memset(buffer,0,sizeof(buffer));
    // trimit semnal la  LFdriver si RFIDdriver ca am primit prima instructiune
    pthread_mutex_lock(&mutex1);

      rfwififlag=1;
      lfwififlag=1;
      pthread_cond_signal(&rfwififlag_cv);
      pthread_cond_signal(&lfwififlag_cv);

    pthread_mutex_unlock(&mutex1);
    ////
		while(1)
		{
    letsreceive(&soc,receiverlen,buffer,receiver);
		if(strcmp(buffer,buffer2)!=0)
		 {
		   setcontainer(buffer,conti);


	     printf("XXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXX\n" );
  strcpy(buffer2,buffer);
    }

    }
	close(socr);
	return 0;

}

//|||||||||||||||||||||||||||||||||
//|||||||||||||||||||||||||||||||||
//|||||||||||||||||||||||||||||||||
char rfiddecoder(uint32_t data)
{

	int i,j;
	char rf=rf&0;
	for(i=1;i<=2;i++)
		{
		for(j=1;j<=4;j++)
			{
			if(data==rfids[i][j])
				{
				rf=rf|i;
				rf=rf<<4|j;

		    }
      }
    }
return rf;
}
//0000000000000000000000000000000
//0000000000000000000000000000000
//0000000000000000000000000000000
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

 state[0]=state[0]|1;
 state[1]=state[1]|3;
 state[2]=0;
 state[3]=0;

  int i=0,sec=0;
      while(1)
  {

	//printf(" DATA RECEIVED : %d%d   ",conti->rfidwt>>4&15,conti->rfidwt&15);
	//printf("  RFX : %X \n",rfx);
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
            oprire=1;
            state[3]=1;
            sec++;
          }
        if(sec==30)
           		{
                 state[3]=2;
  		oprire=0;
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
///ZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZ
//ZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZ
//ZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZ
/*void *rfiddriver(struct container *conti)
{
  //astept semnal de la WIFIreader cum ca masina isi incepe traseul
  pthread_mutex_lock(&mutex1);
 while(rfwififlag==0)
 {
   pthread_cond_wait(&rfwififlag_cv,&mutex1);
 }
 pthread_mutex_unlock(&mutex1);
 ////
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
        {
          status=PcdAnticoll(PICC_ANTICOLL1 , data);

        }
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
           if(rfx!=0)
              {
                conti->rfidwt=rfiddecoder(rfx);
                conti->rfidtolf=conti->rfidwt;
              }
}

    bcm2835_spi_end();
    bcm2835_close();
    return 0;

}
//VVVVVVVVVVVVVVVVVVVVVVVVVVVVV
//VVVVVVVVVVVVVVVVVVVVVVVVVVVVV
//VVVVVVVVVVVVVVVVVVVVVVVVVVVVV
void *lfdriver(struct container *conti)
{
  //astept semnal de la WIFIreader cum ca masina isi incepe traseul
pthread_mutex_lock(&mutex1);
while(lfwififlag==0)
{
 pthread_cond_wait(&lfwififlag_cv,&mutex1);
}
pthread_mutex_unlock(&mutex1);

	//int sensor;
	wiringPiSetup();
softPwmCreate (M1_softPWM1, 0, 100);
    softPwmCreate (M1_softPWM2, 0, 100);
    softPwmCreate (M2_softPWM1, 0, 100);
    softPwmCreate (M2_softPWM2, 0, 100);
	while(1){
	if(oprire==1)
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
//#####################################
//#####################################
//#####################################
*/
void threadcreator()
{
	 // Setup stuff:

	initial();
  pthread_t t1,t2;
	pthread_create(&t1,NULL,(void*)udpclientreceiver,conti);
  pthread_create(&t2,NULL,(void*)udpclienttransmitter,NULL);
  //pthread_create(&t3,NULL,(void*)motors,conti);
	pthread_join(t1,NULL);
	pthread_join(t2,NULL);


}
int main()
{
threadcreator();

return 0;
}

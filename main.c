
#include "mileston1header.h"
#include "sensorsalgorithms.h"
#include <unistd.h>
#include <bcm2835.h>
#include "rc522.h"
#include "rfid.h"
#include "motorControl.h"
// TRATAREA ERRORILOR

//DECLARAREA VARIABILELOR GLOBALE
pthread_mutex_t mutex1 = PTHREAD_MUTEX_INITIALIZER;
pthread_mutex_t mutex2 = PTHREAD_MUTEX_INITIALIZER;

int flag1,rfwififlag,lfwififlag,transmitterflag,capat,stopprio;
pthread_cond_t flag1_cv,rfwififlag_cv,lfwififlag_cv,transmitterflag_cv;

struct container * conti;
int socr,soct;
socklen_t receiverlen;
socklen_t transmitterlen;

char buffer[8192];
char state[7];
struct sockaddr_in receiver;
struct sockaddr_in transmitter;

uint8_t data[6];
uint32_t rfids[17][6];
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
      pthread_cond_init(&transmitterflag_cv,NULL);

      flag1=0;
      rfwififlag=0;
      lfwififlag=0;
      transmitterflag=0;
      memset( conti, 0, sizeof(*conti) );
      rfids[1][1]=0xA0B8557E;
      rfids[1][2]=0xC0FC187C ;
	  rfids[1][3]=0xC0D9857C;
	  rfids[1][4]=0x9DCF92AB;
	  rfids[2][1]=0x804BFB79;
	  rfids[2][2]=0xB0F4157C;
	  rfids[2][3]=0X70D08A7C;
      rfids[2][4]=0xB0DE0D7C;
 	  rfids[3][1]=0xE0D9817C;
	  rfids[3][2]=0x00F28D7C;
	  rfids[3][3]=0X7012897C;
     // rfids[3][4]=0x2066FB79;     
 	  rfids[4][1]=0x2066FB79;
	  rfids[4][2]=0x2057187C;
	  rfids[4][3]=0X2035887C;
      rfids[5][4]=0xA0388d7C;     
      rfids[5][4]=0xC0CE1B7C;      
      rfids[14][0]=0x2B5063D0;
      rfids[14][1]=0x2B5A64D0;
      rfids[14][2]=0xD0524600;
      rfids[15][3]=0x8B3BDEAB;
      rfids[15][4]=0xD5E8B2AB;
}

/////////////////////////////////////
/////////////////////////////////////
/////////////////////////////////////
void* udpclientreceiver(struct container *conti)
{
//creare socket receiver
  initializesocketrecv(&socr,&receiverlen,&receiver);
  gethostip(socr);

do{
    letsreceive(&socr,receiverlen,buffer,receiver);
  }
    while(strlen(buffer)<5);
//introducerea traseului in container
      spargeremesajinitial(buffer,conti);
      printf("am iesit");
      afisaretraseupeecran(conti);


    // trimitere semnal la  lfdriver , rfiddriver si udpclienttransmitter:primrea traseului
      pthread_mutex_lock(&mutex1);
      rfwififlag=1;
      lfwififlag=1;
      transmitterflag=1;
      pthread_cond_signal(&rfwififlag_cv);
      pthread_cond_signal(&lfwififlag_cv);
	    pthread_cond_signal(&transmitterflag_cv);
      pthread_mutex_unlock(&mutex1);
    ////
//receptionarea celorlalte mesaje
		while(1)
		{
        memset(buffer,0,sizeof(buffer));
        letsreceive(&socr,receiverlen,buffer,receiver);
        if((buffer[1]&15)!=3)
          if(flag1)
          { 
                   printf("\n\n\n STOP !!!!!!!!!!! \n\n\n");
           if ( (((buffer[3]>>4&15)==(conti->rfidwt>>4&15))&&((buffer[3]&15)==((conti->rfidwt&15)+1)%4) ) || (((buffer[3]>>4&15)==(conti->rfidwt>>4&15)+1)%4 && ((buffer[3]&15)==4) && ((conti->rfidwt&15)==3)  ) )
                    {
                    stopprio=1;
                    printf("\n\n\n STOP !!!!!!!!!!! \n\n\n");
                    }
              else
                    stopprio=0;
                  }
          if(capat)
        {
           
          if( ( ((buffer[3]>>4&15)==(conti->rfidwt>>4&15)) &&((buffer[3]&15)==((conti->rfidwt&15)+2)%4) ) || ( ((buffer[3]>>4&15)==(conti->rfidwt>>4&15))%4 && ((buffer[3]&15)==4) && ((conti->rfidwt&15)==2) ) )
                 {
                    stopprio=1;
                    printf("\n\n\n STOP  @@@@@@@\n\n\n");
                    }
              else
                    stopprio=0;
                  }

    }
	close(socr);
	return 0;

}
//0000000000000000000000000000000
//0000000000000000000000000000000
//0000000000000000000000000000000
void* udpclienttransmitter()
{
  //asteptare semnalul de confirmare a primirii traseului
  pthread_mutex_lock(&mutex1);
 while(transmitterflag==0)
 {
   pthread_cond_wait(&transmitterflag_cv,&mutex1);
 }
 pthread_mutex_unlock(&mutex1);

  //creare socket transmitter
  initializesockettran(&soct,&transmitterlen,&transmitter);
  broadcastpermissioner(&soct);
  setaredatemasina(state,conti);

//transmiterea ciclica pozitiei curente si a starii
  while(1)
  {
    letssend(&soct,&transmitterlen,&transmitter,conti,state,data,rfx,rfids,&flag1,&capat,&rfiddecoder);
    
    usleep(100000);
    }
}

void *rfiddriver(struct container *conti)
{
  pthread_mutex_lock(&mutex1);
 while(rfwififlag==0)
 {
   pthread_cond_wait(&rfwififlag_cv,&mutex1);
 }
 pthread_mutex_unlock(&mutex1);
    if (!bcm2835_init())
    {
      printf("bcm2835_init failed. Are you running as root??\n");
		exit(1);
    }

    if (!bcm2835_spi_begin())
    {
      printf("bcm2835_spi_begin failedg. Are you running as root??\n");
    	exit(1);
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
			       rfx=data[3]|(data[2]<<8)|(data[1]<<16)|(data[0]<<24);

             for (i=0; i<5; i++)
		           {
			              d2[i]=data[i];
		                }

          //   printf("\n");
           }
}

    bcm2835_spi_end();
    bcm2835_close();
    return 0;

}

void *lfdriver()
{
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
	if(flag1==1)
	{
		drive_s();
		if(mapPointer == 0 )
		{
			in_state = ((conti->traseu[0])%10)-1;
			out_state = ((conti->traseu[1])%10)-1;
			mapPointer += 1;
		}
		else
		{
			in_state = (out_state +2)%4;
			out_state = ((conti->traseu[mapPointer])%10)-1;
		}
		controlLF(conti->traseu);
		mapPointer += 1;
		while(flag1);
	}
		
else
{
	    calculatePID();
	    readLineSensors();
		//sensor = digitalRead(4);
	    controlLF(conti->traseu);
		printValues();
}		
    }

return 0;
}
//#####################################
//#####################################
//#####################################

void threadcreator()
{
	 // Setup stuff:

	initial();
  pthread_t t1,t2,t3,t4;
	pthread_create(&t1,NULL,(void*)udpclientreceiver,conti);
    pthread_create(&t2,NULL,(void*)udpclienttransmitter,NULL);
    pthread_create(&t3,NULL,(void*)rfiddriver,NULL);
    pthread_create(&t4,NULL,(void*)lfdriver,NULL);
	pthread_join(t1,NULL);
	pthread_join(t2,NULL);
	pthread_join(t3,NULL);
	pthread_join(t4,NULL);

}
int main()
{
threadcreator();

return 0;
}

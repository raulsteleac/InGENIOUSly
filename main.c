
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

int flag1,rfwififlag,lfwififlag,transmitterflag,capat,stopprio,flagMiddle,ledLeft,ledRight,stopap,wfrf;
pthread_cond_t flag1_cv,rfwififlag_cv,lfwififlag_cv,transmitterflag_cv;
int echipa;
struct container * conti;
int socr,soct;
socklen_t receiverlen;
socklen_t transmitterlen;
int traseu[100];
char buffer[8192];
char state[7];
struct sockaddr_in receiver;
struct sockaddr_in transmitter;
int poz;
uint8_t data[6];
uint32_t rfids[17][6];
uint32_t rfx=0;
int reset=0;
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

		conti->semnatura[0]=conti->semnatura[0]|'A';
		conti->semnatura[0]=conti->semnatura[0]<<4|'A';
		conti->semnatura[1]=conti->semnatura[1]|'B';
		conti->semnatura[1]=conti->semnatura[1]<<4|'B';
		conti->semnatura[2]=conti->semnatura[2]|'C';
		conti->semnatura[2]=conti->semnatura[2]<<4|'C';
		conti->semnatura[3]=conti->semnatura[3]|'D';
		conti->semnatura[3]=conti->semnatura[3]<<4|'D';


      flag1=0;
      rfwififlag=0;
      lfwififlag=0;
      transmitterflag=0;
      memset( conti, 0, sizeof(*conti) );
  rfids[1][1]=0x2066FB79;
      //rfids[1][2]=0xC0D9857C ;
	  rfids[1][3]=0xC0D9857C;
	  rfids[1][4]=0x70D08A7C;
	  rfids[2][1]=0XB0F4157C;
	  rfids[2][2]=0xB0DE0D7C;
	  rfids[2][3]=0x804BFB79;
      rfids[2][4]=0xC0CE1B7C;
 	  rfids[3][1]=0xE0D9817C;
	  rfids[3][2]=0x2035887C;
	  rfids[3][3]=0x9DCF92AB;
      rfids[3][4]=0x2057187C;     
 	  rfids[4][1]=0xA0B8557E;
	  rfids[4][2]=0xA0388D7C;
	  rfids[4][3]=0xC0FC187C;
      rfids[5][4]=0x7012897C;     
      rfids[6][2]=0x00F28D7C;      
      rfids[14][0]=0x2b5063d0 ;
      rfids[14][1]=0x2b5a64d0;
      rfids[14][2]=0xd0524600;
      rfids[14][3]=0x6b3364d0;
      rfids[14][4]=0xdb3e64d0;
      //rfids[13][3]=0x8B3BDEAB;
      //rfids[13][2]=0xDB3E64D0;
      

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

      afisaretraseupeecran(conti);
      printf("am iesit");

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
   
 if((buffer[1]&15)!=3 && (buffer[0]&15)==1)
          {
          if(flag1)
          { 
          reset=1;
           if ( (((buffer[3]>>4&15)==(conti->rfidwt>>4&15))&&((buffer[3]&15)==((conti->rfidwt&15)+1)%4) && (buffer[4]&15)!=3) || (((buffer[3]>>4&15)==(conti->rfidwt>>4&15)) && ((buffer[3]&15)==4) && ((conti->rfidwt&15)==3)  ) )
                    {
                     stopprio=1;
                    printf("\n\n\n STOP  %d  !!!!!!!!!!! \n\n\n",(((conti->rfidwt&15)+3)%4));
                    echipa=buffer[1];
                    wfrf=1;
                    }
               else
       if(echipa==buffer[1] && ((buffer[3]>>4&15)==(conti->rfidwt>>4&15))&&((buffer[3]&15)==((conti->rfidwt&15)+3)%4))
                 		
  				    {
                 						stopprio=0;
                 						echipa=0;
                 						    printf("\n\n\n MERGEM !!!!!!!!!!! \n\n\n");
                 						    wfrf=0;
                 				}
                    }
           if(capat)
         {
         		
         if(( (buffer[3]>>4&15)==(poz/10) && (buffer[3]&15)==(poz%10+2)%4 ))
                  {
                     stopprio=1;
                     printf("\n\n\n STOP  @@@@@@@\n\n\n");
                     }
               else
                     stopprio=0;
                   }
                   }
     if((buffer[0]&15)==2)
     	{
     	//	if(buffer[1]==conti->semnatura[0] && buffer[2]==conti->semnatura[1]  && buffer[3]==conti->semnatura[2] && buffer[4]==conti->semnatura[3])
     					{
     					stopap=1;
     					printf("M-AM OPRIT");
     	}
     	}
     	else
     	  if((buffer[0]&15)==3)
     		stopap=0;
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
    letssend(&soct,&transmitterlen,&transmitter,conti,state,data,rfx,rfids,&flag1,&capat,&flagMiddle,&rfiddecoder,wfrf);
    
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


void *leduri()
{

 // INIT BECURI 
    digitalWrite (LEFT_softPWM, LOW) ;
   digitalWrite (RIGHT_softPWM, LOW) ;         // INIT BECURI
   digitalWrite (STOP_softPWM, LOW) ;
   delay (500);
   
  // ------------------ BLINKING ----------------
  
   digitalWrite (LEFT_softPWM, LOW) ;
   digitalWrite (RIGHT_softPWM, LOW) ;        
   digitalWrite (STOP_softPWM, LOW) ;
   delay (500);
   digitalWrite (LEFT_softPWM, HIGH) ;
   digitalWrite (RIGHT_softPWM, HIGH) ;         
   digitalWrite (STOP_softPWM, HIGH) ;
   delay (500);
     digitalWrite (LEFT_softPWM, LOW) ;
   digitalWrite (RIGHT_softPWM, LOW) ;        
   digitalWrite (STOP_softPWM, LOW) ;
   delay (500);
   digitalWrite (LEFT_softPWM, HIGH) ;
   digitalWrite (RIGHT_softPWM, HIGH) ;         
   digitalWrite (STOP_softPWM, HIGH) ;
   delay (500);
      digitalWrite (LEFT_softPWM, LOW) ;
   digitalWrite (RIGHT_softPWM, LOW) ;         
   digitalWrite (STOP_softPWM, LOW) ;
   printf("\n\nLEDURI");
   printf("\n...\n");
   
   while(1)
   {
   		  if(ledLeft){
   		  printf("\n\nLEFT");
   		  digitalWrite (LEFT_softPWM, HIGH) ;
   		  delay (500);
   		  digitalWrite (LEFT_softPWM, LOW) ;
   		   delay (500);
   		  }
   
   		if(ledRight){
   		  printf("\n\nRIGHT");
   		   digitalWrite (RIGHT_softPWM, HIGH) ;  
   		   delay (500);
   		   digitalWrite (RIGHT_softPWM, LOW) ;  
   		   delay (500);   
   		}

   }
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
	//wiringPiSetup();
//softPwmCreate (M1_softPWM1, 0, 100);
   // softPwmCreate (M1_softPWM2, 0, 100);
    //softPwmCreate (M2_softPWM1, 0, 100);
    //softPwmCreate (M2_softPWM2, 0, 100);
    /*pinMode (STOP_softPWM, OUTPUT) ;
    pinMode (LEFT_softPWM, OUTPUT) ;
   pinMode (RIGHT_softPWM, OUTPUT) ;   // INIT BECURI
    digitalWrite (STOP_softPWM, LOW) ;
    digitalWrite (LEFT_softPWM, LOW) ;
   digitalWrite (RIGHT_softPWM, LOW) ;         // INIT BECURI*/
   //pinMode (STOP_softPWM, OUTPUT) ;
  // digitalWrite (STOP_softPWM, LOW) ;
           wiringPiSetup();
    softPwmCreate (M1_softPWM1, 0, 100);
    softPwmCreate (M1_softPWM2, 0, 100);
    softPwmCreate (M2_softPWM1, 0, 100);
    softPwmCreate (M2_softPWM2, 0, 100);
   pinMode (STOP_softPWM, OUTPUT) ;
   pinMode (LEFT_softPWM, OUTPUT) ;
   pinMode (RIGHT_softPWM, OUTPUT) ;  
   
   digitalWrite (STOP_softPWM, LOW );   //mod
	while(1){
	if(flag1 || stopap || stopprio)
	{
		drive_s();
   		digitalWrite (STOP_softPWM, HIGH) ;
		while(flag1|| stopap||stopprio);
	digitalWrite (STOP_softPWM, LOW) ;
	onlyLF(conti,&ledRight,&ledLeft, flagMiddle,&reset);
	}
		
else
{	
	 readLineSensors();
	    calculatePID();
	    readLineSensors();
		onlyLF(conti,&ledRight,&ledLeft, flagMiddle,&reset);

}		
    }

return 0;

}
void threadcreator()
{
	 // Setup stuff:

	initial();
  pthread_t t1,t2,t3,t4,t5;
	pthread_create(&t1,NULL,(void*)udpclientreceiver,conti);
    pthread_create(&t2,NULL,(void*)udpclienttransmitter,NULL);
    pthread_create(&t3,NULL,(void*)rfiddriver,NULL);
    pthread_create(&t4,NULL,(void*)lfdriver,NULL);
        pthread_create(&t5,NULL,(void*)leduri,NULL);
    
	pthread_join(t1,NULL);
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

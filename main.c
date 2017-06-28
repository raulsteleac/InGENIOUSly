
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
uint32_t rfids[25][18];
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


      flag1=0;
      rfwififlag=0;
      lfwififlag=0;
      transmitterflag=0;
      memset( conti, 0, sizeof(*conti) );

          
		conti->semnatura[0]=conti->semnatura[0]|10;
		conti->semnatura[0]=conti->semnatura[0]<<4|10;
		conti->semnatura[1]=conti->semnatura[1]|11;
		conti->semnatura[1]=conti->semnatura[1]<<4|11;
		conti->semnatura[2]=conti->semnatura[2]|12;
		conti->semnatura[2]=conti->semnatura[2]<<4|12;
		conti->semnatura[3]=conti->semnatura[3]|13;
		conti->semnatura[3]=conti->semnatura[3]<<4|13;

/*
 rfids[0][2]=0xf0c57a7a;
rfids[0][3]=0xe0b7807a;
rfids[1][1]=0xb007797a;
rfids[1][2]=0xa0ab837c;
rfids[1][3]=0x8083897c;
rfids[1][4]=0xb0b0847c; 
rfids[2][1]=0x3068907c;
rfids[2][2]=0x4096117c;
rfids[2][3]=0x90a0797a;
rfids[2][4]=0x50ba7b7a;
rfids[3][3]=0x10eb817a;
rfids[3][4]=0x30a1797a;
rfids[4][1]=0xf028827a;
rfids[4][2]=0x3032817a;
rfids[4][3]=0xc0d3827a;
rfids[4][4]=0xa0eb817a;
rfids[5][1]=0x801a797a;
rfids[5][2]=0xb0eb817a;
rfids[5][3]=0x8057797a;
rfids[5][4]=0xd06a797a;
rfids[6][1]=0x202efd73;
rfids[6][2]=0xa053917c;
rfids[6][3]=0xc02a8d7c; 
rfids[6][4]=0xf083197c;
rfids[7][1]=0x507c827c;
rfids[7][2]=0xb069857c;
rfids[7][3]=0x106b897c;
rfids[7][4]=0xc056907c;
rfids[8][1]=0x90da7a7a;
rfids[8][2]=0xd0bf7f7a;
rfids[8][3]=0x9079777a;
rfids[8][4]=0xf016887c;
rfids[9][1]=0x30628f7c;
rfids[9][2]=0x901f827c;
rfids[9][3]=0x401b8d7c;
rfids[9][4]=0x805b867c;
rfids[10][1]=0x4094867c;
rfids[10][2]=0x30c58d7c;
rfids[10][3]=0x4028887c;
rfids[10][4]=0xb0921b7c;
rfids[11][1]=0x0091867c;
rfids[11][2]=0x70ef877c;
rfids[11][3]=0xa0ec8a7c;
rfids[11][4]=0x60c48a7c;
rfids[12][1]=0x3087857c;
rfids[12][2]=0x003e8d7c;
rfids[13][1]=0x90a08d7c;
rfids[13][2]=0x4064847c;
rfids[13][3]=0xe065867c;
rfids[13][4]=0xb09a8e7c;
rfids[14][1]=0xa046617e;
rfids[14][2]=0x505b897c;
rfids[14][3]=0x50e3967c;
rfids[14][4]=0x1027f179;
rfids[15][1]=0xb0080f7c;
rfids[15][4]=0x80b6f779;
rfids[16][1]=0xe425616f;
rfids[16][2]=0x84ed6a6f;
rfids[16][3]=0xe6941a7e;
rfids[16][4]=0xa434666f;
rfids[16][5]=0x247b5f6f;
rfids[16][5]=0x84a6636f;
rfids[16][6]=0x14f6576f;
rfids[16][7]=0xa4ca626f;
rfids[16][8]=0x84ed676f;
rfids[16][9]=0xd4dd686f;
rfids[16][10]=0x44ab6f6f;
rfids[16][11]=0xa465656f;
rfids[16][12]=0x647f5a6f;
rfids[16][13]=0x94dd716f;
rfids[16][14]=0x24f06b6f;
rfids[16][15]=0xe4fd5e6f;
*/


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
      rfids[0][0]=0x2b5063d0 ;
      rfids[0][1]=0x2b5a64d0;
      rfids[0][2]=0xd0524600;
      rfids[0][3]=0x6b3364d0;
      rfids[0][4]=0xdb3e64d0;
      //rfids[13][3]=0x8B3BDEAB;
      //rfids[13][2]=0xDB3E64D0;
      

}
int rfo=0;
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
    while((buffer[0]&15)!=3);
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
     
           if ( (((buffer[3]>>4&15)==(conti->rfidwt>>4&15))&&((buffer[3]&15)==((conti->rfidwt&15)+3)%4) && (buffer[4]&15)!=3) || (((buffer[3]>>4&15)==(conti->rfidwt>>4&15)) && ((buffer[3]&15)==4) && ((conti->rfidwt&15)==1)  ) )
                    {
                     stopprio=1;
                    printf("\n\n\n STOP  %d  !!!!!!!!!!! \n\n\n",(((conti->rfidwt&15)+3)%4));
                    echipa=buffer[1];
                    wfrf=1;
                    }
               else
       if(echipa==buffer[1] && ((buffer[3]>>4&15)==((conti->rfidwt>>4&15)))&&( ((buffer[3]&15)==((conti->rfidwt&15)+1)%4) || ( ((buffer[3]&15)==4) && ((conti->rfidwt&15)==3)) || ( ((buffer[3]&15)==4) && ((conti->rfidwt&15)==2)) || ((buffer[3]&15)==((conti->rfidwt&15)+2)%4)))             		
  				    {
                 						stopprio=0;
                 						echipa=0;
                 						    printf("\n\n\n MERGEM !!!!!!!!!!! \n\n\n");
                 						    wfrf=0;
                 				}
                    }

                   }
     if((buffer[0]&15)==2)
     	{
     		if(buffer[1]==conti->semnatura[0] && buffer[2]==conti->semnatura[1]  && buffer[3]==conti->semnatura[2] && buffer[4]==conti->semnatura[3])
     					{
     		
     					stopap=1;
              rfwififlag=1;
              lfwififlag=1;
              transmitterflag=1;
              reset=1;
              memset( conti, 0, sizeof(*conti) );
       //       memset( state, 0, sizeof(*state) );
          //    memset( traseu, 0, sizeof(*traseu) );
			 //     memset( data, 0, sizeof(*data) );
			                         memset(&rfx,0,sizeof(rfx));
     					printf("M-AM OPRIT");
                conti->rfidwt=1;
		conti->semnatura[0]=conti->semnatura[0]|10;
		conti->semnatura[0]=conti->semnatura[0]<<4|10;
		conti->semnatura[1]=conti->semnatura[1]|11;
		conti->semnatura[1]=conti->semnatura[1]<<4|11;
		conti->semnatura[2]=conti->semnatura[2]|12;
		conti->semnatura[2]=conti->semnatura[2]<<4|12;
		conti->semnatura[3]=conti->semnatura[3]|13;
		conti->semnatura[3]=conti->semnatura[3]<<4|13;
              do{
                  letsreceive(&socr,receiverlen,buffer,receiver);
                }
                
              //introducerea traseului in container
                  while((buffer[0]&15)!=3);
                    spargeremesajinitial(buffer,conti);

                    afisaretraseupeecran(conti);
                    printf("am iesit");

                  // trimitere semnal la  lfdriver , rfiddriver si udpclienttransmitter:primrea traseului

                    rfwififlag=0;
                    lfwififlag=0;
                    transmitterflag=0;
                    stopap=0;
                    stopprio=0;
                    flagMiddle=0;
                    flag1=0;
                    echipa=0;
                    oprire=0;
					rfo=0;
     	}
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
transmitterflag=0;
//transmiterea ciclica pozitiei curente si a starii
  while(1)
  {
  while(transmitterflag);
    letssend(&soct,&transmitterlen,&transmitter,conti,state,data,rfx,rfids,&flag1,&capat,&flagMiddle,&rfiddecoder,wfrf);
    if((conti->rfidwt>>4&15)==(conti->traseu[conti->lungimetraseu-1]/10) && ((conti->rfidwt&15)==(conti->traseu[conti->lungimetraseu-1]%10)))
    			oprire=1;
    usleep(100000);
    }

}

void *rfiddriver(struct container *conti)
{
    uint8_t pTagType,d2[6];
    char status;
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
	if(flag1 || stopap || stopprio || oprire )
	{
		drive_s();
   		digitalWrite (STOP_softPWM, HIGH) ;
		while(flag1|| stopap||stopprio || oprire);
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

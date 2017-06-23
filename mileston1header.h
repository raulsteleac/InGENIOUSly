#ifndef MILESONE1HEARED_H
#define MILESONE1HEARED_H
#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <errno.h>
#include <string.h>
#include <arpa/inet.h>
#include <sys/ioctl.h>
#include <net/if.h>

struct container
{
// wifi reader / modor direction area : traseu
// aici stochez traseul de la wifi -reader
 unsigned char tipmasina ;
 unsigned char semnatura [5];
int traseu [1024];
int lungimetraseu;
//wifi transmitter / rfid
// aici e campul impartasit de wifi transmitter si rfid de unde wifi tr va trimite pozitia curenta ciclic
 int rfidwt;
// Miscare :rfid+lf+wifireader
// aici vor pune algoritmii in urma prelucrarii informatiei de la drivere
 int miscare[4];
 // aici va pune algoritmul de prelucrare a miscarii de mai sus ^^^^^
// PWM decoder area -> trimite la algoritmul de Miscare
 unsigned char directie;
 int viteza;
 int timp;
 //aici vor pune driverele de la sensori informatia
 // line follower driver output area
 int lfdrout;
 // rfid driver output area
 int rfidtolf;
};
void err(char *msg)
 {
	perror(msg);
	exit(0);
 }

 //CREARE SOCKET
void initializesocketrecv(int* soc,socklen_t * receiverlen,struct sockaddr_in * receiver)
{
  memset((char*)receiver,0,sizeof(*receiver));
  //creare socket
    if((*soc=socket(AF_INET,SOCK_DGRAM,IPPROTO_UDP))==-1)
          err("Nu merge socket");
    //setare conexiune , unde trimitem datele
  receiver->sin_family=AF_INET;
  receiver->sin_port=htons(5000);
  *receiverlen=sizeof(*receiver);
   inet_pton(AF_INET, "255.255.255.255",&receiver->sin_addr);
   if(bind(*soc,(struct sockaddr*)receiver,sizeof(*receiver))==-1)
       err("Nu merge bind");
    printf("\n Clientul s-a conectat la : %s si portul %d\n",inet_ntoa(receiver->sin_addr),ntohs(receiver->sin_port));
  }
  void initializesockettran(int* soc,socklen_t * receiverlen,struct sockaddr_in * receiver)
  {
  memset((char*)receiver,0,sizeof(*receiver));
    //creare socket
      if((*soc=socket(AF_INET,SOCK_DGRAM,IPPROTO_UDP))==-1)
            err("Nu merge socket");
      //setare conexiune , unde trimitem datele
    receiver->sin_family=AF_INET;
    receiver->sin_port=htons(5000);
    *receiverlen=sizeof(*receiver);
     inet_pton(AF_INET, "255.255.255.255",&receiver->sin_addr);
  }

void broadcastpermissioner(int *soc)
{
  // Broadcast
  int broadcastPermission = 1;
  if (setsockopt(*soc, SOL_SOCKET, SO_BROADCAST, (void *) &broadcastPermission,sizeof(broadcastPermission)) < 0)
      err("Eroare la broadcast");

}
 int sec=0;
void letssend(int *soct,socklen_t * transmitterlen,struct sockaddr_in * transmitter,struct container *conti,char *state,uint8_t *data,uint32_t rfx,uint32_t rfids[17][6],int *flag1,int *flag2,int *flag3,char (*rfiddecoder)(uint32_t ,uint32_t [17][6] ),int ok)
{
 if(!ok)
 {
if(rfx!=0)
 conti->rfidwt=rfiddecoder(rfx,rfids);
printf(" DATA RECEIVED : %d%d   ",conti->rfidwt>>4&15,conti->rfidwt&15);
printf("  RFX : %X \n",rfx);
     if(conti->rfidwt!=state[3] )//&&( (conti->rfidwt>>4&15)!=14))
      {
          if((conti->rfidwt>>4&15)==(state[3]>>4&15))
                       { 
      
              if(state[4]!=1)
                if(state[3]!=0)
                  {
                    state[3]=0;
                    state[3]=state[3]|(conti->rfidwt>>4&15);
                    state[3]=state[3]<<4|(conti->rfidwt&15);
                   
                 		   state[4]=(state[4])%4+1;
                  
                  }	
               if(((state[4]))==4)
                  state[4]=1;    
                }
                else
            {
	      state[3]=0;
              state[3]=state[3]|(conti->rfidwt>>4&15);
              state[3]=state[3]<<4|(conti->rfidwt&15);
          if( (conti->rfidwt>>4&15)!=14)
              state[4]=1;
              }
            }
   if((conti->rfidwt>>4&15)==14 )
   							*flag3=1;
   			else
   							*flag3=0;	
   		
 if(state[4]==1)
{
                 sec++;
              *flag1=1;
   }
if(sec==30)
                {
                   state[4]=2;
                   *flag1=0;
                    sec=0;
                }
if(state[4]==3)
    	*flag2=1;
    	else
    	*flag2=0;
}    
if(state[4]!=0  && state[3]!=0  )
      if(sendto(*soct,state,strlen(state),0,(struct sockaddr*)transmitter,*transmitterlen)==-1)
          err("Nu merge sendto");


}
void letsreceive(int *soc,socklen_t receiverlen,char *buffer,struct sockaddr_in receiver)
{

  if(recvfrom(*soc,buffer,8192,0,(struct sockaddr*)&receiver,&receiverlen)==-1)
  err("Nu merge recvfrom");
}
void gethostip(int soc)
{
  struct ifreq ifr;
  char iface[] = "wlp3s0";
  ifr.ifr_addr.sa_family = AF_INET;
  strncpy(ifr.ifr_name , iface , IFNAMSIZ-1);
  ioctl(soc, SIOCGIFADDR, &ifr);
  printf("\n Raspberry pi-ul are ip-adressul : %s \n",inet_ntoa(((struct sockaddr_in *)&ifr.ifr_addr)->sin_addr));
}

void spargeremesajinitial(char *buffer,struct container * conti)
{
  int i=1,ok=1,j,k=0;
  int ln=0;
  printf("am intrat");
  while(buffer[i]!='\0' && k==0 && strlen(buffer+3)>1)
  {
    if((buffer[i]>>4&15)==0)
        if((buffer[i]&15)==3)
          {
            ok=0;
          (conti)->tipmasina=conti->tipmasina|(buffer[i+1]>>4&15);
          (conti)->tipmasina=conti->tipmasina<<4|(buffer[i+1]&15);
	}
    i=i+2;
    ln=(buffer[i]>>4&15*10)+(buffer[i]&15);
    i++;
    if(!ok)
    {
          for(j=0;j<ln;j++)
         conti->traseu[j]=(buffer[i+j]>>4&15)*10+(buffer[i+j]&15);
         conti->lungimetraseu=ln;
          break;
	k=1;
            }
    else
   i+=ln;
}

}

void setaredatemasina(char *state, struct container *conti)
{
  state[0]=state[0]|1;
  state[1]=state[1]|3;
  state[2]=state[2]|(conti->tipmasina>>4&15);
  state[2]=state[2]<<4|(conti->tipmasina&15);
  state[3]=0;
  state[4]=0;
}

void afisaretraseupeecran(struct container *conti)
{
  int i;
  printf("\nTRASEUL CU LUNGIMEA :   %d\n",conti->lungimetraseu);
  for(i=0;i<conti->lungimetraseu;i++)
            printf("%d\n",conti->traseu[i]);
  printf("\n###########################\n");
}



#endif






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
 unsigned char traseu [1024];
 unsigned char lungimetraseu;
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
 int rfiddrout;
};
void err(char *msg)
 {
	perror(msg);
	exit(0);
 }

 //CREARE SOCKET
void initializesocket (int* soc,socklen_t receiverlen,struct sockaddr_in receiver)
{
  //creare socket
    if((*soc=socket(AF_INET,SOCK_DGRAM,IPPROTO_UDP))==-1)
          err("Nu merge socket");
    //setare conexiune , unde trimitem datele
    memset((char*)&receiver,0,sizeof(receiver));
    receiver.sin_family=AF_INET;
    receiver.sin_port=htons(5000);
    receiverlen=sizeof(receiver);
    inet_pton(AF_INET, "255.255.255.255", &receiver.sin_addr);
  //afisare detailii
    printf("\n Clientul s-a conectat la : %s si portul %d\n",inet_ntoa(receiver.sin_addr),ntohs(receiver.sin_port));
  }
void broadcastpermissioner(int *soc)
{
  // Broadcast
  int broadcastPermission = 1;
  if (setsockopt(*soc, SOL_SOCKET, SO_BROADCAST, (void *) &broadcastPermission,sizeof(broadcastPermission)) < 0)
      err("Eroare la broadcast");

}
void letssend(int *soc,socklen_t receiverlen)
{
  //trimitere primei date ca semn de conectare

  struct sockaddr_in receiver;
  memset((char*)&receiver,0,sizeof(receiver));
  receiver.sin_family=AF_INET;
  receiver.sin_port=htons(5000);
  receiverlen=sizeof(receiver);
  inet_pton(AF_INET, "255.255.255.255", &receiver.sin_addr);

 // if(sendto(*soc,"Hello Server",strlen("Hello Server"),0,(struct sockaddr*)&receiver,receiverlen)==-1)
  //    err("Nu merge sendto");
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

void spargeremesajinitial(char *buffer,struct container *conti)
{
  int i=1,ok=1,ex=1,j=0;
  while(buffer[i]!='\0')
  {
    if((buffer[i]>>4&15)==0)
        if((buffer[i]&15)==3)
          {
            ok=0;
            i++;
            conti->tipmasina=buffer[i];
            i++;
            conti->lungimetraseu=buffer[i];
            i++;
          }

    if(!ok)
    {
      if((buffer[i]>>4&15)==0)
        ex=0;
      else
        {
          conti->traseu[j]=buffer[i];
          j++;
        }
    }
    if(!ex)
      break;
        i++;
  }
}

void setcontainer(char *buffer,struct container* init)
{
  int c,i=2;
  init->directie=buffer[0];
  init->viteza=0;
  if(buffer[0]=='B' || buffer[0]=='F')
  {
    while((c=buffer[i])!=' ')
      {
          init->viteza=init->viteza*10+(c-'0');
          i++;
      }
    init->timp=atoi(buffer+i);
    }
      else
    init->timp=atoi(buffer+i);

    if(init->directie=='F')
      printf("Masina se va misca in fata cu viteza : %d in timp de %d \n",init->viteza,init->timp);
    else
      if(init->directie=='B')
      printf("Masina se va misca in spate cu viteza : %d  in timp de %d \n",init->viteza,init->timp);
        else if(init->directie=='S')
          printf("Masina va sta pe loc  in timp de : %d \n",init->timp);
}




#endif
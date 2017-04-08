#ifndef MILESONE1HEARED_H
#define MILESONE1HEARED_H

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
// AICI INITIALIZEZ STRUCTURA BUFFERULI PENTRU MILESTONE I

struct container
{
 char directie;
 int viteza;
 int timp;


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

  if(sendto(*soc,"Hello Server",strlen("Hello Server"),0,(struct sockaddr*)&receiver,receiverlen)==-1)
      err("Nu merge sendto");
}
void letsreceive(int *soc,socklen_t receiverlen,char *buffer,struct sockaddr_in receiver)
{

  if(recvfrom(*soc,buffer,255,0,(struct sockaddr*)&receiver,&receiverlen)==-1)
  err("Nu merge recvfrom");
}
void gethostip(int soc)
{
  struct ifreq ifr;
  char iface[] = "wlx00160a2401d4";
  ifr.ifr_addr.sa_family = AF_INET;
  strncpy(ifr.ifr_name , iface , IFNAMSIZ-1);
  ioctl(soc, SIOCGIFADDR, &ifr);
  printf("\n Placuta are ip-adressul : %s \n",inet_ntoa(((struct sockaddr_in *)&ifr.ifr_addr)->sin_addr));
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
    }
      else
    init->timp=atoi(buffer+i);
    
    if(init->directie=='F')
      printf("Masina se va misca in fata cu viteza : %d in timp de \n",init->viteza);
    else
      if(init->directie=='B')
      printf("Masina se va misca in spate cu viteza : %d \n",init->viteza);
        else if(init->directie=='S')
          printf("Masina va sta pe loc  in timp de : %d \n",init->timp);
}
#endif

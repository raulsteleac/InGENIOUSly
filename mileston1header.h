#ifndef MILESONE1HEARED_H
#define MILESONE1HEARED_H

#include <pthread.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netdb.h>
#include <errno.h>
#include <string.h>
#include <sys/ioctl.h>
#include <net/if.h>
#include <arpa/inet.h>
// AICI INITIALIZEZ STRUCTURA BUFFERULI PENTRU MILESTONE I

struct bufferul
{
 char directie;
 union
   {
     int viteza;
     int timp;
   }vt;

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
void setbufferul(char *buffer,struct bufferul* init)
{
  init->directie=buffer[0];
  if(buffer[0]=='B' || buffer[0]=='F')
    init->vt.viteza=atoi(buffer+2);
    else
      init->vt.timp=atoi(buffer+2);
    if(init->directie=='F')
      printf("Masina se va misca in fata cu viteza : %d \n",init->vt.viteza);
    else
      if(init->directie=='B')
      printf("Masina se va misca in spate cu viteza : %d \n",init->vt.viteza);
        else if(init->directie=='S')
          printf("Masina va sta pe loc  in timp de : %d \n",init->vt.timp);
}
#endif

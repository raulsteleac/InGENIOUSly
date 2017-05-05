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


#endif

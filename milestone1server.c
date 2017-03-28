#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netdb.h>
#include <unistd.h>
#include <errno.h>
#include <arpa/inet.h>
#include <sys/ioctl.h>
#include <netinet/in.h>
#include <net/if.h>
#include <arpa/inet.h>

void err(char *msg)
{
	perror(msg);
	exit(1);
}	
int main (void)
{
int soc;
char buffer[3],mesajclient[10];
struct sockaddr_in server,client;
   socklen_t slen=sizeof(client);	
	
if((soc=socket(AF_INET,SOCK_DGRAM,IPPROTO_UDP))==-1)
		err("Nu merge socket");
		//creare socket server
memset((char *) &server,0,sizeof(server));
server.sin_family=AF_INET;
server.sin_port=htons(5000);
inet_aton("255.255.255.255",&server.sin_addr.s_addr);
		// afisare ip server
struct ifreq ifr;
char iface[] = "wlx00160a2401d4";
ifr.ifr_addr.sa_family = AF_INET;
strncpy(ifr.ifr_name , iface , IFNAMSIZ-1);
ioctl(soc, SIOCGIFADDR, &ifr);
printf("\n Serverul cu ip-adressul : %s si portul %d\n",inet_ntoa(((struct sockaddr_in *)&ifr.ifr_addr)->sin_addr),ntohs(server.sin_port));
		//binding	
if(bind(soc,(struct sockaddr*)&server,sizeof(server))==-1)
		err("Nu merge bind");
		//broadcast
int broadcastPermission=1;
if (setsockopt(soc, SOL_SOCKET, SO_BROADCAST, (void *) &broadcastPermission, 
          sizeof(broadcastPermission)) < 0)
          err("Eroare la broadcast");
		//waiting for a client
printf("\nAstept client:\n");
if(recvfrom(soc,mesajclient,10,0,(struct sockaddr*)&client,&slen)==-1)
	err("Recv from error");
printf("Received packet from %s : %d \n",inet_ntoa(client.sin_addr),ntohs(client.sin_port));
printf("Data received : %s\n",mesajclient);
while(1)
	{
	printf("\nAlegeti comanda dorita : \n");
	fgets(buffer,100,stdin);printf("%s",buffer);
			if(sendto(soc,buffer,strlen(buffer),0,(struct sockaddr*)&client,slen)==-1)
				err("Sendto error");
		
	

}
close(soc);
return 0;
}


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
#include <pthread.h>
void err(char *msg)
{
	perror(msg);
	exit(1);
}

pthread_mutex_t mutex = PTHREAD_MUTEX_INITIALIZER;
int flag1,flag2;
pthread_cond_t flag1_cv,flag2_cv;


void *udpserver()
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

	client.sin_family=AF_INET;
	client.sin_port=htons(5000);
	inet_aton("255.255.255.255",&client.sin_addr.s_addr);
			// afisare ip server


	struct ifreq ifr;
	char iface[] = "wlx00160a2401d4";
	ifr.ifr_addr.sa_family = AF_INET;
	strncpy(ifr.ifr_name , iface , IFNAMSIZ-1);
	ioctl(soc, SIOCGIFADDR, &ifr);
	printf("\n Serverul cu ip-adressul : %s si portul %d\n",inet_ntoa(((struct sockaddr_in *)&ifr.ifr_addr)->sin_addr),ntohs(server.sin_port));

	int broadcastPermission=1;
	if (setsockopt(soc, SOL_SOCKET, SO_BROADCAST, (void *) &broadcastPermission,
	          sizeof(broadcastPermission)) < 0)
	          err("Eroare la broadcast");
			//waiting for a client
			  memset(buffer,0,sizeof(buffer)*14);
	printf("\nAstept client:\n");
		buffer[0]=buffer[0]&0;
		buffer[0]=buffer[0]<<4|3;
		//
		buffer[1]=buffer[1]&0;
		buffer[1]=buffer[1]<<4|4;

		buffer[2]=buffer[2]&0;
		buffer[2]=buffer[2]<<4|1;

		buffer[3]=buffer[3]&0|0;
	  buffer[3]=buffer[3]<<4|4;

		buffer[4]=buffer[4]|1;
		buffer[4]=buffer[4]<<4|2;
		buffer[5]=buffer[5]|2;
		buffer[5]=buffer[5]<<4|3;
		buffer[6]=buffer[6]|2;
		buffer[7]=buffer[7]|3;
		buffer[7]=buffer[7]<<4|4;
		//
		buffer[8]=buffer[8]|0;
		buffer[8]=buffer[8]<<4|3;

		buffer[9]=buffer[9]|0;
		buffer[9]=buffer[9]<<4|1;

		buffer[10]=buffer[10]&0;
		buffer[10]=buffer[10]<<4|4;

		buffer[11]=buffer[11]|2;
		buffer[11]=buffer[11]<<4|4;
		buffer[12]=buffer[12]|4;
		buffer[12]=buffer[12]<<4|2;
	  buffer[13]=buffer[13]|5;
		buffer[13]=buffer[13]<<4|1;
		buffer[14]=buffer[14]|6;
		buffer[14]=buffer[14]<<4|3;
		buffer[15]='\0';

	if(sendto(soc,buffer,strlen(buffer),0,(struct sockaddr*)&client,slen)==-1)
		err("Sendto error");

		while(1)
		{

		printf("\nAlegeti comanda dorita : \n");
		fgets(buffer,100,stdin);printf("%s",buffer);
				if(sendto(soc,buffer,strlen(buffer),0,(struct sockaddr*)&client,slen)==-1)
					err("Sendto error");



	}
	close(soc);

}
int main (void)
{
  pthread_mutex_init(&mutex,NULL);
	pthread_cond_init(&flag1_cv,NULL);

	pthread_t t2;
	pthread_create(&t2,NULL,(void*)udpserver,NULL);
	pthread_join(t2,NULL);

return 0;
}

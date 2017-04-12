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
	printf("\nAstept client:\n");


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

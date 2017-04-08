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
int flag1=0,flag2=0;
pthread_cond_t flag1_cv,flag2_cv;

void* udpclientreader()
{
	int soc;
	struct sockaddr_in receiver;
	socklen_t receiverlen;
	if((soc=socket(AF_INET,SOCK_DGRAM,IPPROTO_UDP))==-1)
				err("Nu merge socket");
	//setare conexiune , de unde primim datele
	memset((char*)&receiver,0,sizeof(receiver));
	receiver.sin_family=AF_INET;
	receiver.sin_port=htons(5000);
	receiverlen=sizeof(receiver);
	inet_pton(AF_INET, "255.255.255.255", &receiver.sin_addr);
	if(bind(soc,(struct sockaddr*)&receiver,sizeof(receiver))==-1)
			err("Nu merge bind");

				char buffer[255];

	if(recvfrom(soc,buffer,strlen("hello server"),0,(struct sockaddr*)&receiver,&receiverlen)==-1)
			err("Nu merge recvfrom");

			pthread_mutex_lock(&mutex);
			flag1=1;
			pthread_cond_signal(&flag1_cv);
			pthread_mutex_unlock(&mutex);

	int i=1;
	while(1)
	{
		if(recvfrom(soc,buffer,strlen("hello server"),0,(struct sockaddr*)&receiver,&receiverlen)==-1)
			err("Nu merge recvfrom");
		printf("%s \n",buffer);


	}
}
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
			//binding

	//if(bind(soc,(struct sockaddr*)&server,sizeof(server))==-1)
	//	err("Nu merge bind");

			//broadcast
	int broadcastPermission=1;
	if (setsockopt(soc, SOL_SOCKET, SO_BROADCAST, (void *) &broadcastPermission,
	          sizeof(broadcastPermission)) < 0)
	          err("Eroare la broadcast");
		//waiting for a client
	printf("\nAstept client:\n");

	/*if(recvfrom(soc,mesajclient,strlen("Hello Server"),0,(struct sockaddr*)&client,&slen)==-1)
		err("Recv from error");
	printf("Received packet from %s : %d \n",inet_ntoa(client.sin_addr),ntohs(client.sin_port));
	printf("Data received : %s\n",mesajclient);
*/
	pthread_mutex_lock(&mutex);
	while(flag1==0)
	{
		pthread_cond_wait(&flag1_cv,&mutex);
	}
	pthread_mutex_unlock(&mutex);
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
	pthread_t t1;
	pthread_create(&t1,NULL,(void*)udpclientreader,NULL);
	pthread_join(t1,NULL);

	pthread_t t2;
	pthread_create(&t2,NULL,(void*)udpserver,NULL);
	pthread_join(t2,NULL);

return 0;
}

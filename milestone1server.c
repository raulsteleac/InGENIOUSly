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
int reuse = 1;
   	 if (setsockopt(soc, SOL_SOCKET, SO_REUSEPORT, &reuse, sizeof(reuse)) == -1)
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
				memset(buffer,0,sizeof(char )*255);

	while(1)
	{
		memset(buffer,0,sizeof(char)*strlen(buffer));
		if(recvfrom(soc,buffer,strlen("B 999999 99999"),0,(struct sockaddr*)&receiver,&receiverlen)==-1)
			err("Nu merge recvfrom");
		printf("%s \n",buffer);



	}
}
int main (void)
{

	pthread_t t1;
	pthread_create(&t1,NULL,(void*)udpclientreader,NULL);
	pthread_join(t1,NULL);
return 0;
}

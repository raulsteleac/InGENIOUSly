#include <stdio.h>
#include <stdlib.h>
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
// TRATAREA ERRORILOR
void err(char *msg)
 {
	perror(msg);
	exit(0);
 } 
//DECLARAREA VARIABILELOR GLOBALE
pthread_mutex_t mutex = PTHREAD_MUTEX_INITIALIZER;
int flag;
pthread_mutex_t mutex;
pthread_cond_t flag_cv;
struct bufferul *init;
//INITIALIZAREA VARIABILELOR GLOBALE
void initial()
{
	init=(struct bufferul *)malloc(sizeof(struct bufferul));
	init->directie='B';
	init->vt.viteza=30;
	pthread_mutex_init(&mutex,NULL);
	pthread_cond_init(&flag_cv,NULL);
	flag=0;
}
//O AFISARE 
void* afis()
{	
	
	printf("%c %d\n",init->directie,init->vt.viteza);
	return 0;
}
// CLIENTUL UDP , AICI SE FACE COMUNICAREA SI STOCAREA DATELOR
void* udpclient(struct bufferul *init)
{
	int soc;
	socklen_t receiverlen;
	char buffer[255];
	struct sockaddr_in receiver;
				//creare socket
	if((soc=socket(AF_INET,SOCK_DGRAM,IPPROTO_UDP))==-1)
		err("Nu merge socket");
			    //setare conexiune , unde trimitem datele
	memset((char*)&receiver,0,sizeof(receiver));
	receiver.sin_family=AF_INET;
	receiver.sin_port=htons(5000);
	receiverlen=sizeof(receiver);
	inet_pton(AF_INET, "255.255.255.255", &receiver.sin_addr);	
				//afisare detailii
    printf("\n Clientul s-a conectat la : %s si portul %d\n",inet_ntoa(receiver.sin_addr),ntohs(receiver.sin_port));
				// Broadcast
	int broadcastPermission = 1;
    if (setsockopt(soc, SOL_SOCKET, SO_BROADCAST, (void *) &broadcastPermission,sizeof(broadcastPermission)) < 0)
          err("Eroare la broadcast");
    			//trimitere primei date ca semn de conectare      	
	if(sendto(soc,"Hello Server",strlen("Hello Server"),0,(struct sockaddr*)&receiver,receiverlen)==-1)
		err("Nu merge sendto");
	
		while(1)
		{	
			if(recvfrom(soc,buffer,255,0,(struct sockaddr*)&receiver,&receiverlen)==-1)
			err("Nu merge recvfrom");
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
			
		
//AICI TRANSMIT SEMNALUL CU SCOPA DE A ANUNTA CA S-A MODIFICAT MEMORIA , FLAGUL CV
	pthread_mutex_lock(&mutex);
	flag=1;
	pthread_cond_signal(&flag_cv);
	pthread_mutex_unlock(&mutex);
		}
		
	  				
	close(soc);
	return 0;

}
// AICI V-A FI INTRODUSA FUNCTIA DE MISCARE A MOTOARELOR
void *motors(struct bufferul *init)
{

	while(1)
	{
	pthread_mutex_lock(&mutex);
//AICI SE ASTEAPTA SEMNALUL
	while(flag==0)
	{
		pthread_cond_wait(&flag_cv,&mutex);
	}
	pthread_mutex_unlock(&mutex);
//......AICI VA FI APELATA FUNCTIA DE MISCARE A MOTOARELOR , threadul asteapta ca memoria sa fi schimbata si apoi va anunta functia
// DACA MEMORIA NU S-A SCHIMBAT FUNCTIA VA CONTINUA CU ACEEASI DEIRECTIE
	if(flag)
	printf("%c %d\n",init->directie,init->vt.viteza);
	flag=0;
	
	}
	return 0;
}
void threadcreator()
{
	initial();
	pthread_t t1,t2;
	pthread_create(&t1,NULL,(void*)udpclient,init);
	pthread_create(&t2,NULL,(void*)motors,init);
	pthread_join(t1,NULL);
	pthread_join(t2,NULL);

}
int main()
{	
threadcreator();
	
return 0;
}

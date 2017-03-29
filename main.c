#include <stdio.h>
#include <stdlib.h>
#include "mileston1header.h"

// TRATAREA ERRORILOR

//DECLARAREA VARIABILELOR GLOBALE
pthread_mutex_t mutex = PTHREAD_MUTEX_INITIALIZER;
int flag;
pthread_mutex_t mutex;
pthread_cond_t flag_cv;
struct bufferul *init;
int soc;
socklen_t receiverlen;
char buffer[255];
struct sockaddr_in receiver;
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
initializesocket(&soc,receiverlen,receiver);
broadcastpermissioner(&soc);
letssend(&soc,receiverlen);
		while(1)
		{
      letsreceive(&soc,receiverlen,buffer,receiver);
      setbufferul(buffer,init);

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


#include "mileston1header.h"
#include "sensorsalgorithms.h"
//#include "motorDrive.h"
// TRATAREA ERRORILOR

//DECLARAREA VARIABILELOR GLOBALE
pthread_mutex_t mutex = PTHREAD_MUTEX_INITIALIZER;
pthread_mutex_t mutex2 = PTHREAD_MUTEX_INITIALIZER;
int flag1,flag2,flag3,flag4;
pthread_cond_t flag1_cv,flag3_cv,flag4_cv;
struct container * conti;
int soct;
socklen_t transmitterlen;
char buffer[8192];
char state[5];
struct sockaddr_in transmitter;
//INITIALIZAREA VARIABILELOR GLOBALE

void initial()
{
      conti=(struct container *)malloc(sizeof(struct container));
      pthread_mutex_init(&mutex,NULL);
      pthread_mutex_init(&mutex2,NULL);
      pthread_cond_init(&flag1_cv,NULL);
      pthread_cond_init(&flag3_cv,NULL);
      pthread_cond_init(&flag4_cv,NULL);
      flag1=0;
      flag3=0;
      flag4=0;
      conti->rfidwt=conti->rfidwt|3;
      conti->rfidwt=conti->rfidwt<<4|1;

}
// CLIENTUL UDP , AICI SE FACE COMUNICAREA SI STOCAREA DATELOR

void* udpclienttransmitter()
{
//1
    /*  pthread_mutex_lock(&mutex);
	   while(flag1==0)
	     {
		       pthread_cond_wait(&flag1_cv,&mutex);
	      }
     pthread_mutex_unlock(&mutex);
     */
//1

// 2
       pthread_mutex_lock(&mutex2);

       flag3=1;
       flag4=1;

       pthread_cond_signal(&flag3_cv);
       pthread_cond_signal(&flag4_cv);

       pthread_mutex_unlock(&mutex2);
//2
  memset((char*)&transmitter,0,sizeof(transmitter));
  transmitter.sin_family=AF_INET;
  transmitter.sin_port=htons(5000);
  transmitterlen=sizeof(transmitter);
  inet_pton(AF_INET, "255.255.255.255", &transmitter.sin_addr);
 if((soct=socket(AF_INET,SOCK_DGRAM,IPPROTO_UDP))==-1)
          err("Nu merge socket");

 broadcastpermissioner(&soct);


 state[0]=state[0]|1;
 state[1]=state[1]|3;

sleep(0.1);
  int i=0,z=0,q=0;
      while(1)
      {
        if(((state[3]>>4&15)!=(conti->rfidwt>>4&15)) && (state[3]&15)!=(conti->rfidwt&15))
        {
        state[2]=state[2]|(conti->rfidwt>>4&15);
        state[2]=state[2]<<4|(conti->rfidwt&15);
        i++;
        }
        if(i==1)
        {
          state[3]=1;
          z++;
          if(z==30)
          {
            state[3]=2;
            z=0;i++;
          }
        }
        if(i==2)
         q++;

        //if(i==3)
        if(q==20)
          state[3]=3;
        if(i==4)
            i=0;

        if(i!=0);
        if(sendto(soct,state,strlen(state),0,(struct sockaddr*)&transmitter,transmitterlen)==-1)
            err("Nu merge sendto");

         usleep(100000);

      }
}
// AICI V-A FI INTRODUSA FUNCTIA DE MISCARE A MOTOARELOR

void *motors(struct container *conti)
{

  pthread_mutex_lock(&mutex);

  flag1=1;;

  pthread_cond_signal(&flag1_cv);

  pthread_mutex_unlock(&mutex);
  while(1)
  {
    ;
  }

	return 0;
}

void *rfiddriver(struct container *conti)
{
  while (1)
  {
  pthread_mutex_lock(&mutex2);
  while(flag3==0)
  {
    pthread_cond_wait(&flag3_cv,&mutex2);
  }

  pthread_mutex_unlock(&mutex2);
  printf("###RFIDRFIDRFIDRFIDRFIDRFIDRFIDRFIDRFID####\n");
  sleep(1);
}
}
void *lfdriver(struct container *conti)
{
  while (1)
  {
  pthread_mutex_lock(&mutex2);
  while(flag4==0)
  {
    pthread_cond_wait(&flag4_cv,&mutex2);
  }
  pthread_mutex_unlock(&mutex2);
   printf("###LFLFLFLLFLFLFLFLFLLFLFLFLF####\n");
   sleep(1);
   }
}
void threadcreator()
{

    //initializare thread-uri

	initial();
	pthread_t t2,t3,t4,t5;
  pthread_create(&t2,NULL,(void*)udpclienttransmitter,NULL);
  //pthread_create(&t3,NULL,(void*)motors,conti);
  pthread_create(&t4,NULL,(void*)rfiddriver,NULL);
  pthread_create(&t5,NULL,(void*)lfdriver,NULL);
	pthread_join(t2,NULL);
  //pthread_join(t3,NULL);
  pthread_join(t4,NULL);
  pthread_join(t5,NULL);
}
int main()
{
threadcreator();

return 0;
}

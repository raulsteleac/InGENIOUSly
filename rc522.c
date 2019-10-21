
#include <string.h>
#include <stdio.h>
#include <unistd.h>
#include <bcm2835.h>
#include "rc522.h"
#include "rfid.h"



int main(int argc, char **argv)
{
   
    if (!bcm2835_init())
    {
      printf("bcm2835_init failed. Are you running as root??\n");
      return 1;
    }

    if (!bcm2835_spi_begin())
    {
      printf("bcm2835_spi_begin failedg. Are you running as root??\n");
      return 1;
    }
InitRc522();
uint8_t pTagType,data[6],d2[6];
char status;
while(1)
{
status=PcdRequest(PICC_REQIDL,&pTagType);
if(status==TAG_OK)

status=PcdAnticoll(PICC_ANTICOLL1 , data);
int i;
if(d2!=data)
	{
printf("Card detected\n");
	printf("Data recevied : ");
for (i=0; i<5; i++)
		{
			printf("%X",data[i]);
			d2[i]=data[i];
		}
printf("\n\n");

}
}

    bcm2835_spi_end();

    bcm2835_close();
    return 0;
}


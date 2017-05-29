#ifndef RFID_H
#define RFID_H

char rfiddecoder(uint32_t data,uint32_t rfids[3][6])
{

	int i,j;
	char rf=rf&0;
	for(i=1;i<=2;i++)
		{
		for(j=1;j<=4;j++)
			{
			if(data==rfids[i][j])
				{
				rf=rf|i;
				rf=rf<<4|j;
		    }
      }
    }
return rf;
}



#endif

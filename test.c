#include <lpc214x.h>
#include <stdio.h>

#define bit(x) (1<<x)
#define RS bit(24) 
#define RW bit(23) 
#define EN bit(22) 
#define CNTL (RS|RW|EN)
#define DATA (bit(10)|bit(11)|bit(12)|bit(13))
void delay(int x)
{int i;
for(i=0;i<x;i++);
}

void cmd(char a)
{
char b;
IOCLR1=CNTL;
IOCLR0=DATA;
IOCLR1=RS;
b=a&0XF0;
IOSET0=b<<6;
IOSET1=EN;
delay(1000);
IOCLR1=EN;
IOCLR0=DATA;
b=a&0X0F;
IOSET0=b<<10;
IOSET1=EN;
delay(1000);
IOCLR1=EN;
}
void data(char a)
{
char b;

IOCLR0=DATA;
IOSET1=RS;
b=a&0XF0;
IOSET0=b<<6;
IOSET1=EN;
delay(1000);
IOCLR1=EN;
IOCLR0=DATA;
b=a&0X0F;
IOSET0=b<<10;
IOSET1=EN;
delay(1000);
IOCLR1=EN;
}
void string(char *s)
{  int j;
for(j=0;s[j]!='\0';j++)
{
data(s[j]);
}
}




void timer()__irq
{
	IOCLR1=(1<<28);
	T0TCR=0X00;
	T0IR=1;
	VICVectAddr=0;

}

void init(void)
{
	PINSEL0 =0X00008005;
	PINSEL1=0X05000400;//(PINSEL0 & ~(1 << 16)) | (1 << 17);  Select PWM4 output for Pin0.8
	PINSEL2=0X00000000;

    PWMPCR = 0x0; //Select Single Edge PWM - by default its single Edged so this line can be removed
    //PWMPR = 60-1; // 1 micro-second resolution
    PWMMR0 = 1000; // 10ms period duration
    PWMMR2 = 50;
    PWMMR5 = 50; //0.5ms - pulse duration i.e width (Brigtness level)
    PWMMCR = (1<<1); // Reset PWMTC on PWMMR0 match
    PWMLER = 0X25;//(1<<0)|(1<<2)|(1<<5); // update MR0 and MR4
    PWMPCR = 0X2400;//(1<<12); // enable PWM output
    PWMTCR = (1<<1) ; //Reset PWM TC & PR
 
    PWMTCR = (1<<0) | (1<<10); // enable counters and PWM Mode
   
        IODIR0=0X00000001;
        IODIR1=0X10000000;
	IODIR1|=CNTL;
	IODIR0|=DATA;
	cmd(0x02);
        cmd(0x28);
	cmd(0x01);
	cmd(0x0C);
    
        U0LCR=0X83;
	U0DLL=97;
	U0LCR=0X03;
	


	T0CTCR=0X00;
	T0PR=60000;                             //100000000;
	T0TCR=0X02;
	T0MCR=3;
	T0MR0=1000;
	T0IR=(1<<0);

	VICVectCntl0=(1<<5)|4;
	VICIntEnable=1<<4;
	VICVectAddr0=(unsigned)timer;

	
}




int main()
{
	char a[10],b[10];
        int i,ldr,gas;

 
	init();

	//wifi_connect();

	while(1)
	{

	 AD0CR=0X00200302;
	  delay(10000);
	  AD0CR=0X01200302;
	  while(!(AD0GDR)&(80000000));
	  ldr=((AD0GDR>>6)&(0X03FF));

	   AD0CR=0X00200304;
	   delay(10000); 
	   AD0CR=0X01200304;
	   while(!(AD0GDR)&(80000000));
	   gas=((AD0GDR>>6)&(0X03FF));
	   transfer(gas);
	   /*sprintf(a,"%d %d\n\r",gas,ldr);
	  delay(4000);
	  transmit(a);*/
	 if (gas>200)
	 {
		cmd(0X01);
		cmd(0x80);
 		string("GAS LEAKAGE");
 		cmd(0xC0);
 		string("STOP HERE");
 		delay(100000);
	}
	 else
	 	{
		cmd(0X01);
	 	cmd(0x80);
	 	string("HAPPY JOURNEY")  ;
	 	delay(100000);
	 	}


		if(ldr<256)
		{
			PWMMR2 = 300;
			PWMMR5 = 300;			//T-ON=30% , Hence ON
			PWMLER =0X25; 

				
				
				if( !((IO1PIN) & (1<<25)) ) // Check P1.25
				{

					PWMMR2 = ~1000; //100%

					PWMLER =(1<<2); //Update Latch Enable bit for PWMMR2
					
				}
			
				if( !((IO1PIN) & (1<<26)) ) // Check P1.26
				{

					PWMMR5 = ~1000; //100%
					PWMLER =(1<<5)|(1<<2); //Update Latch Enable bit for PWMMR2
						
				}
				if( ((IO1PIN) & (1<<26)) ) // Check P1.26
				{

					PWMMR5 = 300; //30%
					PWMLER =(1<<5)|(1<<2); 
				}
					

			
				}
		else
		{
		PWMMR2 = 0000;
		PWMMR5 = 0000;			//T-ON=00% , Hence OFF
		PWMLER =0X25; // ((1<<2)|(1<<5)); //Update Latch Enable bit for PWMMR4
		}
		if( !((IO1PIN) & (1<<27)) ) // Check P1.27
		{

			IOSET1=(1<<28);
			T0TCR=0X02;
			T0TCR=0X01;
			
	
			
		}
		



}
}

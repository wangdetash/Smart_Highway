#include <lpc214x.h>
#include <stdio.h>
#include <string.h>



void initClocks(void) // Setup PLL and Clock Frequency
{
 PLL0CON = 0x01;   //Enable PLL
 PLL0CFG = 0x24;   //Multiplier and divider setup
 PLL0FEED = 0xAA;  //Feed sequence
 PLL0FEED = 0x55;
 
 while(!(PLL0STAT & 0x00000400)); //is locked?
 
 PLL0CON = 0x03;   //Connect PLL after PLL is locked
 PLL0FEED = 0xAA;  //Feed sequence
 PLL0FEED = 0x55;
 VPBDIV = 0x01;    // PCLK is same as CCLK i.e.60 MHz
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
	PINSEL1=0X00000400;//(PINSEL0 & ~(1 << 16)) | (1 << 17);  Select PWM4 output for Pin0.8
	PINSEL2=0X00000000;

    PWMPCR = 0x0; //Select Single Edge PWM - by default its single Edged so this line can be removed
    PWMPR = 60-1; // 1 micro-second resolution
    PWMMR0 = 10000; // 10ms period duration
    PWMMR2 = 500;
    PWMMR5 = 500; //0.5ms - pulse duration i.e width (Brigtness level)
    PWMMCR = (1<<1); // Reset PWMTC on PWMMR0 match
    PWMLER = 0X25;//(1<<0)|(1<<2)|(1<<5); // update MR0 and MR4
    PWMPCR = 0X2400;//(1<<12); // enable PWM output
    PWMTCR = (1<<1) ; //Reset PWM TC & PR
 
    PWMTCR = (1<<0) | (1<<10); // enable counters and PWM Mode
   
    IODIR0=0X00000001;
    IODIR1=0X10000000;
    
    U0LCR=0X83;
	U0DLL=97;
	U0LCR=0X03;
	
	AD0CR=0X00200006;

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

   	initClocks(); //Initialize CPU and Peripheral Clocks @ 60Mhz
	init();

	wifi_connect();

	while(1)
	{

	  AD0CR=0X01200002;
	  while(!(AD0GDR)&(80000000));
	  ldr=((AD0GDR>>6)&(0X03FF));
	  sprintf(a,"%d\n\r",ldr);
	  transmit(a);

	  AD0CR=0X01200004;
	  while(!(AD0GDR)&(80000000));
	  gas=((AD0GDR>>6)&(0X03FF));
	  transfer(gas);

		if(ldr<256)
		{
			PWMMR2 = 3000;
			PWMMR5 = 3000;			//T-ON=30% , Hence ON
			PWMLER =0X25; 

				
				
					if( !((IO1PIN) & (1<<25)) ) // Check P1.25
					{

						PWMMR2 = ~10000; //100%

						PWMLER =(1<<2); //Update Latch Enable bit for PWMMR2
					
					}
			
					if( !((IO1PIN) & (1<<26)) ) // Check P1.26
					{

					  PWMMR5 = ~10000; //100%
						PWMLER =(1<<5)|(1<<2); //Update Latch Enable bit for PWMMR2
						
					}
					if( ((IO1PIN) & (1<<26)) ) // Check P1.26
					{

						PWMMR5 = 3000; //30%
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

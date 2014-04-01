/*
 * main implementation: use this 'C' sample to create your own application
 *
 */


#include "derivative.h" /* include peripheral declarations */

#define SSR (0XF3) //Set Sample Rate
#define RE  (0xE8)//Set Resolution
#define EN (0xF4)//Enable
#define GPIOB_EN_MASK (1<<2)
#define GPIOB_RS_MASK (1)
#define GPIOB_RW_MASK (1<<1)
#define GPIOB_MASK 7
#define GPIOD_DATA_MASK (0xFF0)
#define CLEAR             (0x01)
#define TURN_ON           (0X0C)
#define LINE_FEED         (0xC0)
#define SECOND_LINE_EN    (0x38)
#define INCREASE_CURSOR   (0x06)
#define DECREASE_CURSOR   (0x04)
#define BACKSPACE         (0x10)
#define DDRAM_ADDRESS_0   (0x80)
#define DDRAM_ADDRESS_1   (0x81)
#define DDRAM_ADDRESS_15  (0x8F)
#define DDRAM_ADDRESS_64  (0xC0)
#define DDRAM_ADDRESS_67  (0xC3)
#define DDRAM_ADDRESS_71  (0xC7)
#define DDRAM_ADDRESS_74  (0xCA)
#define DDRAM_ADDRESS_79  (0xCF)
#define CGRAM_ADDRESS_0   (0x40)
#define SPACE             (0x20)
#define UP                (0x80)
#define DOWN              (0xC0)
#define HOME              (0x02)
#define CURSOR_ON 		  (0xD)
#define CURSOR_OFF        (0xC)
#define WAIT_TIME (0xFFF)
#define INS (1)
#define CHAR (0)


int i, Byte=0;
char Convert[3],SData,Flag,Receive=1,Data[5],Send,Data_Count, Data_;

void LCD_Init(void);
void Set_RS(void);
void Clear_RS(void);
void Enable(void);
void LCD_Write(char Command, char Data);
void Conv(char Data);
void Interrupt(void);
void Mouse_Send(char Data);
void wait(int time);
void Converter(char Data);
void Mouse_Write(char SData);
int main(void)
{
	LCD_Init();
	

	LCD_Write(INS,UP);
	LCD_Write(CHAR,'E');
	LCD_Write(CHAR,'J');
	LCD_Write(CHAR,'E');
	LCD_Write(CHAR,' ');
	LCD_Write(CHAR,'X');
	LCD_Write(INS,UP+10);
	LCD_Write(CHAR,'E');
	LCD_Write(CHAR,'J');
	LCD_Write(CHAR,'E');
	LCD_Write(CHAR,' ');
	LCD_Write(CHAR,'Y');
	
	
	
	for(;;)
	{
		Mouse_Write(SData);
	}
	
	return 0;
}

void Conv (char Data)
{
Data_= (((Data & 0xF0) >> 4) * 10) + (Data & 0x0F);
}

void Mouse_Send(char Data)
{
	Receive = 0;
	Send =  Data;
	NVIC_ICER =  1<<31;
	GPIOD_PDDR = 1<<5;
	wait(10000);
	GPIOD_PDDR|=  1;
	GPIOD_PDDR &= ~(1<<5);
	PORTD_ISFR = 1<<5;
	NVIC_ICPR = 1<<31;
	NVIC_ISER = 1<<31;
	while(Byte<1);
	Byte = 0;
}

void Mouse_Write(char Sdata)
{

if(SData)
		{
			LCD_Write(INS,DOWN+10);
			if(Data[0] & 0x20)
			{
				Conv(~Data[2]);
				LCD_Write(CHAR,0X76);
			}
			else
			{
				Conv(Data[2]);
				LCD_Write(CHAR,0X5E);
				
			}			
			
			LCD_Write(CHAR,Data_+0x30);
			
			
			LCD_Write(INS,DOWN);
			if(Data[0] & 0x10)
			{
				Conv(~Data[1]);
				LCD_Write(CHAR,0X3C);
			}
			else
			{
				Conv(Data[1]);
				LCD_Write(CHAR,0x3E);
			}			
			LCD_Write(CHAR,Data_+0x30);
			SData = 0;
		}

}
void Interrupt(void)
{
	if(Receive)
	{
		if(Data_Count==0)
		{
			Data[Byte] = 0;
		}
		else if (Data_Count<9)
		{
			Data[Byte] |= (GPIOD_PDIR&1) << (Data_Count-1);
		}
		
		Data_Count++;
		if(Data_Count>10)
		{
			SData = 1;
			Byte++;
			
			if(Byte>2)
			{
				Byte = 0;
				Flag = 1;
			}
			
			
			Data_Count = 0;
		}
	}
	
	else 
	{
		
		if(Data_Count == 8)
		{
			GPIOD_PDOR = ~((1&(Send))
		                  ^(1&(Send>>1))
		                  ^(1&(Send>>2))
		                  ^(1&(Send>>3))
		                  ^(1&(Send>>4))
		                  ^(1&(Send>>5))
		                  ^(1&(Send>>6))
		                  ^(1&(Send>>7)));
		}
		else if(Data_Count == 9)
		{
			GPIOD_PDDR &= ~1;
		}

		
		Data_Count++;
		if(Data_Count>10)
		{
			Data_Count = 0;
			Receive = 1;
			GPIOD_PDOR = 0;
		}
		
		else if (Data_Count<=7)
		{
			GPIOD_PDOR = (Send>>(Data_Count))&1;
		}
		
	}

	PORTD_ISFR = 1<<5;
}

void wait(int time)
{
while(time==0)
{
time--;	
}
}

void LCD_Init(void)
{
	SIM_SCGC5 |= SIM_SCGC5_PORTB_MASK | SIM_SCGC5_PORTC_MASK;
	PORTB_PCR0= PORT_PCR_MUX(1);
	PORTB_PCR1= PORT_PCR_MUX(1);
	PORTB_PCR2= PORT_PCR_MUX(1);
	PORTC_PCR4= PORT_PCR_MUX(1);
	PORTC_PCR5= PORT_PCR_MUX(1);
	PORTC_PCR6= PORT_PCR_MUX(1);      
	PORTC_PCR7= PORT_PCR_MUX(1);
	PORTC_PCR8= PORT_PCR_MUX(1);
	PORTC_PCR9= PORT_PCR_MUX(1);
	PORTC_PCR10= PORT_PCR_MUX(1);
	PORTC_PCR11= PORT_PCR_MUX(1);
	GPIOB_PDDR=GPIOB_EN_MASK | GPIOB_RS_MASK | GPIOB_RW_MASK; 
	GPIOC_PDDR=GPIOD_DATA_MASK; 
		
	LCD_Write(INS,CLEAR); 
	LCD_Write(INS,TURN_ON);
	LCD_Write(INS,SECOND_LINE_EN);
	//Mouse Init
	SIM_SCGC5 |= SIM_SCGC5_PORTD_MASK;
	PORTD_PCR5 = PORT_PCR_MUX(1) | PORT_PCR_PE_MASK | PORT_PCR_IRQC(10);
	PORTD_PCR0 = PORT_PCR_MUX(1) | PORT_PCR_PE_MASK;
	NVIC_ISER = 1<<31;
	Mouse_Send(SSR);
	Mouse_Send(0x0A);
	Mouse_Send(RE);
	Mouse_Send(0x03);
	Mouse_Send(EN);

}

void Set_RS(void)
{
	GPIOB_PSOR=GPIOB_RS_MASK; 
}

void Clear_RS(void)
{
	GPIOB_PCOR=GPIOB_RS_MASK; 
}

void Enable(void)
{
	short hwDelay=WAIT_TIME;
	
	GPIOB_PSOR=GPIOB_EN_MASK;  
	while(hwDelay--); 
	GPIOB_PCOR=GPIOB_EN_MASK; 
}

void LCD_Write(char bINS, char bData)
{
	if(bINS) 
	{
		Clear_RS();  
		GPIOC_PDOR=bData<<4;
		Enable();    
		Set_RS(); 
	}
	else 
	{
		GPIOC_PDOR=bData<<4; 
		Enable();
	}
}

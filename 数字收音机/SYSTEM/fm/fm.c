#include "fm.h"
#include "tea5767.h"
#include "gui.h"
#include "delay.h"
#include "save_flash.h"

int freqset=8800; //88M

int freqget_buf[1000]={0};
int freqget_buf_i=0;
//int freq_demo[16]={8970,9580,9810,9890,9970,10050,10110,10170,10200,10240,10350,10430,10490,10580,10690,10750,0};
int cur_statue;
int cur_station=0;

void value_add_click(void);
void value_dec_click(void);

void value_set_click(void);

void FM_Last()
{
	if(cur_station>0)
	{
		LCD_ShowString(60,40,"Standby");
		cur_station--;
		freqset=freqget_buf[cur_station]*10;
		Set_Frequency(freqset);
		show_freq();
	}
	else
		LCD_ShowString(60,40,"lowest band limit");

	delay_ms(1000);
}

void FM_Next()
{
	if(cur_station<(freqget_buf_i-1))
	{
		LCD_ShowString(60,40,"Standby");
		cur_station++;
		freqset=freqget_buf[cur_station]*10;
		Set_Frequency(freqset);
		show_freq();
	}
	else
		LCD_ShowString(60,40,"highest band limit");
		delay_ms(1000);
}
void freq_scandec(void){
		while(1){
			int i=0;
			for(i=0;i<freqget_buf_i;i++){
				if(freqget_buf[i]==freqset){
					if(freqset>8800) freqset-=10; 
					else freqset=10800;				//????
					i=0; 
					break;
				}							
			}
			if(i>=freqget_buf_i) break;
		}	
}
void freq_scan(void){
		while(1){
			int i=0;
			for(i=0;i<freqget_buf_i;i++){
				if(freqget_buf[i]==freqset){
					if(freqset<10800) freqset+=10; 
					else freqset=8800;				//????
					i=0; 
					break;
				}							
			}
			if(i>=freqget_buf_i) break;
		}	
}

 
void value_add_click() {
				if(freqset<10800)freqset+=10;  //????100Khz
	freq_scan();
				 	Set_Frequency(freqset*10); //???????101.8MHz
		show_freq();
}
void value_dec_click() {
	 			if(freqset>8800)freqset-=10;   //????100Khz
				else freqset=10800;			//???? 
	      freq_scandec();	
				 	Set_Frequency(freqset*10); //???????101.8MHz
		show_freq();
}

static u8 mode=0;

void value_set_click() {
	
	//save(&freq_demo);
	//load(&freq_demo);
int i=0;
	LCD_ShowString(60,40,"Searching");
	
	for(i=0;i<freqget_buf_i;i++){
		 freqget_buf[i] = 0;
	}							

	freqset=8800;
	while(1)
	{
		if(freqset<10800)freqset+=10;   //????100Khz
		else break;				//????

		if( 	Set_Frequency(freqset*10))//???????. 
		{
			freqget_buf[freqget_buf_i]=freqset;
			freqget_buf_i+=1;

		}		    		

 
	}
 //delay_ms(2500);
	freqget_buf_i=16;
	cur_station=0;
	freqset=freqget_buf[0];	
	Set_Frequency(freqget_buf[cur_station]*10);
	save(freqget_buf);
	//show_freq();
	GUI_draw();
}

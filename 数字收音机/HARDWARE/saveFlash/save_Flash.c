#include "save_flash.h"



/*
function: save data to flash
data: a array with unsigned 32bit data
			the array must end with 0 as a flag
*/
void save(uint32_t * data)
{
	uint32_t* address_data=data;
	uint32_t address_Flash=Base_Address;
	FLASH_Unlock();
	FLASH_ErasePage(Base_Address);//clear first then save
	while((*address_data)!=0)//the last number must be 0
	{
		FLASH_ProgramWord(address_Flash,*address_data);
		address_Flash+=4;//four byte foreward
		address_data++;
	}
	FLASH_ProgramWord(address_Flash,0);//as a flag
	FLASH_Lock();
}



/*
function: load data from flash
data: a array with unsigned 32bit data
			the length must be properal
*/
void load(uint32_t* data)
{
	uint32_t* address_data=data;
	uint32_t* address_Flash=(uint32_t*)Base_Address;
	while((*address_Flash)!=0)
	{
		(*address_data)=(*address_Flash);
		address_Flash+=4;
		address_data++;
	}
	(*address_data)=0;
}

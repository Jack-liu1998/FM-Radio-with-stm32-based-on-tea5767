#ifndef _GUI_H_
#define _GUI_H_
#include "stm32f10x.h"
#include "LCD.h"
#include "Touch.h"
#include "fm.h"
#include "TEA5767.h"

enum
{
	SEARCH =1,
	LAST =2,
	NEXT =3
};
enum
{
	standby=0,
	loading=1,
	saving=2,
	searching=3
};
void show_freq(void);
void GUI_draw(void);
void Judge_loc(void);

#endif


#include "stm32f10x.h"                  // Device header
#include "define.h"

u32 compensate(u32 Vtarget_other)
{
	u32 reverse;
	switch(Vtarget_other)
	{
		case 300:reverse = Vtarget_other + 9;break;
		case 350:reverse = Vtarget_other + 9;break;
		case 400:reverse = Vtarget_other + 9;break;
		case 450:reverse = Vtarget_other + 10;break;
		case 500:reverse = Vtarget_other + 10;break;
		case 550:reverse = Vtarget_other + 10;break;
		case 600:reverse = Vtarget_other + 11;break;
		case 650:reverse = Vtarget_other + 12;break;
		case 700:reverse = Vtarget_other + 12;break;
		case 750:reverse = Vtarget_other + 12;break;
		case 800:reverse = Vtarget_other + 10;break;
		case 850:reverse = Vtarget_other + 6;break;
		case 900:reverse = Vtarget_other + 8;break;
		case 950:reverse = Vtarget_other + 12;break;
		case 1000:reverse = Vtarget_other + 15;break;
		case 1050:reverse = Vtarget_other + 14;break;
		case 1100:reverse = Vtarget_other + 17;break;
		case 1150:reverse = Vtarget_other + 18;break;
		case 1200:reverse = Vtarget_other + 22;break;
		case 1250:reverse = Vtarget_other + 25;break;
		case 1300:reverse = Vtarget_other + 28;break;
		case 1350:reverse = Vtarget_other + 29;break;
		case 1400:reverse = Vtarget_other + 30;break;
		case 1450:reverse = Vtarget_other + 32;break;
		case 1500:reverse = Vtarget_other + 34;break;
		case 1550:reverse = Vtarget_other + 38;break;
		case 1600:reverse = Vtarget_other + 46;break;
		case 1650:reverse = Vtarget_other + 50;break;
		case 1700:reverse = Vtarget_other + 56;break;
		case 1750:reverse = Vtarget_other + 74;break;
		case 1800:reverse = Vtarget_other + 91;break;
		case 1850:reverse = Vtarget_other + 105;break;
		case 1900:reverse = Vtarget_other + 150;break;
		case 1950:reverse = Vtarget_other + 184;break;
		case 2000:reverse = Vtarget_other + 200;break;
		default:break;
	}return reverse;
}



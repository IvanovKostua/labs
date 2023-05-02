#include "address_map_nios2.h"

#define SCREEN_WIDTH 320
#define SCREEN_HEIGHT 240
#define LINE_NUMBER 4

int main()
{
    volatile short int *sw_switch = (short int *)SW_BASE;               //инициализация портов
    volatile short int *START_PIXEL = (short int *)FPGA_PIXEL_BUF_BASE;
    volatile short int *ptr = START_PIXEL;
    volatile char *keys = (char *)KEY_BASE;
    volatile char *SW_CODE_LINE = sw_switch + 1;          //переход к переключателям 17, 18
    *SW_CODE_LINE &= 0x03;                               //маска
    char line_fickness = SCREEN_HEIGHT / LINE_NUMBER;   //толщина линии
    int start_line;
	int end_line;
	
    for (int y = 0; y < SCREEN_HEIGHT; ++y)         //цикл закрашивает экран черным цветом
    {
        for (int x = 0; x < SCREEN_WIDTH; ++x)
        {
            ptr[(y << 9) + x] = 0;                  //задание адреса (y << 9 - смещение на 9 битов для задания координаты y)
        }
    }
    
    while (1) 
	{
		if (*keys)                      // при нажатии любой кнопки
		{
			start_line = line_fickness * (*SW_CODE_LINE);       //получение координаты начальной строки 
   			end_line = line_fickness * (*SW_CODE_LINE + 1);     //получение координаты конца строки
   			for (int y = start_line; y < end_line; ++y)
    		{
	        	for (int x = 0; x < SCREEN_WIDTH; ++x)
	       	 	{
	            	ptr[(y << 9) + x] = *sw_switch;             //задание цвета с переключателей
	        	}
   			}
		}
	}
    return 0;
}

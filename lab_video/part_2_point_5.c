#include "address_map_nios2.h"
#define SCREEN_WIDTH 320
#define SCREEN_HEIGHT 240

void draw_text(int x_start, int y_start, char* text){           //функция пишущая текст слева-направо
    char* ptr = (char*)FPGA_CHAR_BASE;                          
    ptr += (y_start << 7);                                      //задание координаты y (y_start << 7) - координата y
    for(int x = x_start; *text != 0; ++x){
        ptr[x] = *text;                                         //Запись символа 
        ++text;
    }
}


int main(){
    char name[] = "Name";
	int x_start = 40;               
	int y_start = 30;
	draw_text(x_start, y_start, name);
    return 0;
}

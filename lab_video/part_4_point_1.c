#include <math.h>
#include "address_map_nios2.h"
#define SCREEN_WIDTH 320
#define SCREEN_HEIGHT 240
volatile int *reg_buffer = (int *)PIXEL_BUF_CTRL_BASE;
volatile int *reg_back_buffer = (int *)(PIXEL_BUF_CTRL_BASE + 4);
volatile int *reg_resolution = (int *)(PIXEL_BUF_CTRL_BASE + 8);
volatile int *reg_status = (int *)(PIXEL_BUF_CTRL_BASE + 12);
volatile short int *FRONT_FRAME = (short int *)FPGA_PIXEL_BUF_BASE;
volatile short int *BACK_FRAME = (short int *)0x0803BE7E;
volatile int *sw_switch = (int *)SW_BASE;
volatile char *keys = (char *)KEY_BASE;
/*Функция заполнения прямоугольной области на экране.
x_start, y_start - координаты начала области,
width - ширина, height - высота, color - цвет, start_address - адрес начала буфера*/

void draw_rectangle(int x_start, int y_start, int width, int height, short int color, volatile short int *start_address)
{//функция, закрашивающая экран заданным цветом
    short int *ptr = start_address;
    for (int y = y_start; y < y_start + height; ++y)
    {
        for (int x = x_start; x < x_start + width; ++x)
        {
            ptr[(y << 9) + x] = color;
        }
    }
}

/*Функция выведения отрезка на экран.
x_start, y_start - координаты начала отрезка,
x_end, y_end - координаты конца отрезка,
color - цвет линии, FRAME - адрес начала буфера
*/
void draw_line(int x_start, int y_start, int x_end, int y_end, short int color, volatile short int *FRAME)  //функция, рисующая отрезок
{
    short int *ptr = FRAME;
    int dx = abs(x_start - x_end);
    int dy = abs(y_start - y_end);
    int dirX = x_start < x_end ? 1 : -1;
    int dirY = y_start < y_end ? 1 : -1;
    int error = dx - dy;
    ptr[(y_end << 9) + x_end] = color;
    while (x_start != x_end || y_start != y_end)        // рисование линии в цикле
    {
        ptr[(y_start << 9) + x_start] = color;
        int doubled_error = 2 * error;
        if (doubled_error > -dy)
        {
            error -= dy;
            x_start += dirX;
        }
        if (doubled_error < dx)
        {
            error += dx;
            y_start += dirY;
        }
    }
}

void draw_circle(int x0, int y0, int radius, short int color, volatile short int *FRAME)
{
    short int *ptr = FRAME;
    int x = 0;
    int y = radius;
    int delta = 1 - 2 * radius;
    int error = 0;
    while (y >= 0)
    {
        ptr[((y0 + y) << 9) + x0 + x] = color;
        ptr[((y0 - y) << 9) + x0 + x] = color;
        ptr[((y0 + y) << 9) + x0 - x] = color;
        ptr[((y0 - y) << 9) + x0 - x] = color;
        error = 2 * (delta + y) - 1;
        if (delta < 0 && error <= 0)
        {
            ++x;
            delta += 2 * x + 1;
            continue;
        }
        error = 2 * (delta - x) - 1;
        if (delta > 0 && error > 0)
        {
            --y;
            delta += 1 - 2 * y;
            continue;
        }
        ++x;
        delta += 2 * (x - y);
        --y;
    }
}

int main()
{
    draw_rectangle(0, 0, SCREEN_WIDTH, SCREEN_HEIGHT, 0, FRONT_FRAME);
    //draw_circle(120,120,100,0x3ffff,FRONT_FRAME);
    while (((*keys) & 0x02) != 0x02)        // ожидание нажатия кнопки KEY1
    {
   	};
    int color = (*sw_switch); 
	while (1)
	{                                           // установите значение цвета на переключателях
              //задание цвета с переключателей
        while (((*keys) & 0x08) != 0x08)        // одилание нажатия кнопки KEY3
        {
        };
   		int x_start = (*sw_switch) & 0x01ff;            //задание начальных координат
    	int y_start = ((*sw_switch) & 0x03fe00) >> 9;
    	while (((*keys) & 0x04) != 0x04)                // ожидание нажатия кнопки KEY2
    	{
    	};
    	int x_end = (*sw_switch) & 0x01ff;              //задание конечных коордиинат ортезка
    	int y_end = ((*sw_switch) & 0x03fe00) >> 9;
    	draw_line(x_start, y_start, x_end, y_end, color, FRONT_FRAME);        //вызов функции, рисующей отрезок
	}
    return 0;
}

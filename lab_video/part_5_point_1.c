#include "address_map_nios2.h"
#define SCREEN_WIDTH 320
#define SCREEN_HEIGHT 240

volatile short int *FRONT_FRAME = (short int *)FPGA_PIXEL_BUF_BASE;
volatile short int *BACK_FRAME = (short int *)0x0803BE7E;

volatile int *sw_switch = (int *)SW_BASE;
volatile char *keys = (char *)KEY_BASE;

volatile int *reg_buffer = (int *)PIXEL_BUF_CTRL_BASE;
volatile int *reg_back_buffer = (int *)(PIXEL_BUF_CTRL_BASE + 4);
volatile int *reg_resolution = (int *)(PIXEL_BUF_CTRL_BASE + 8);
volatile int *reg_status = (int *)(PIXEL_BUF_CTRL_BASE + 12);

/*Функция заполнения прямоугольной области на экране.
x_start, y_start - координаты начала области,
width - ширина, height - высота, color - цвет, FRAME - адрес начала буфера*/
void draw_filled_rectangle(int x_start, int y_start, int width, int height, short int color, volatile short int *FRAME)
{
    short int *ptr = FRAME;
    for (int y = y_start; y < y_start + height; ++y)
    {
        for (int x = x_start; x < x_start + width; ++x)
        {
            ptr[(y << 9) + x] = color;
        }
    }
}

/*Функция, корректирующая координаты таким образом, 
чтобы при достижении границ экрана отрезок продолжал выводиться с другой стороны экрана*/
void correct_current_coordinates(int *x, int *y, int *x_cur, int *y_cur)
{
    if (*x_cur < 0)
    {
        *x = *x_cur + SCREEN_WIDTH;
    }
    else if (*x_cur > SCREEN_WIDTH - 1)
    {
        *x = *x_cur - SCREEN_WIDTH;
    }
    else
        *x = *x_cur;

    if (*y_cur < 0)
    {
        *y = *y_cur + SCREEN_HEIGHT;
    }
    else if (*y_cur > SCREEN_HEIGHT - 1)
    {
        *y = *y_cur - SCREEN_HEIGHT;
    }
    else
        *y = *y_cur;
}

/*Функция, корректирующая начальные координаты прямой, на случай,
 когда начальные и конечные координаты выходят одновременно за рамки экрана*/
void correct_start_coordinates(int *x_start, int *y_start, int *x_end, int *y_end)
{
    if ((*x_start) < 0 && (*x_end) < 0)
    {
        *x_start += SCREEN_WIDTH;
        *x_end += SCREEN_WIDTH;
    }
    else if ((*x_start) > SCREEN_WIDTH - 1 && (*x_end) > SCREEN_WIDTH - 1)
    {
        *x_start -= SCREEN_WIDTH;
        *x_end -= SCREEN_WIDTH;
    }

    if ((*y_start) < 0 && (*y_end) < 0)
    {
        *y_start += SCREEN_WIDTH;
        *y_end += SCREEN_WIDTH;
    }
    else if ((*y_start) > SCREEN_WIDTH - 1 && (*y_end) > SCREEN_WIDTH - 1)
    {
        *y_start -= SCREEN_WIDTH;
        *y_end -= SCREEN_WIDTH;
    }
}


/*Функция выведения отрезка на экран.
x_start, y_start - координаты начала отрезка,
x_end, y_end - координаты конца отрезка,
color - цвет линии, FRAME - адрес начала буфера
*/
void draw_line(int x_start, int y_start, int x_end, int y_end, short int color, volatile short int *FRAME)
{
    short int *ptr = FRAME;
    correct_start_coordinates(&x_start, &y_start, &x_end, &y_end);
    int dx = abs(x_start - x_end);
    int dy = abs(y_start - y_end);
    int dirX = x_start < x_end ? 1 : -1;
    int dirY = y_start < y_end ? 1 : -1;
    int error = dx - dy;
    int x, y;
    while (x_start != x_end || y_start != y_end)
    {
        correct_current_coordinates(&x, &y, &x_start, &y_start);
        ptr[(y << 9) + x] = color;
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
    correct_current_coordinates(&x, &y, &x_end, &y_end);
    ptr[(y << 9) + x] = color;
}

/*Функция ввода данных с переключателей SW17-SW0 
a - указатель на область памяти, куда нужно поместить вводимое значение,
mask - накладываемая маска на вводимое значение,
offset - сдвиг вправо
*/
void input_sw(int *a, int mask, int offset)
{
    *a = ((*sw_switch) & mask) >> offset;
}

/*Функция ввода координат x, y
Запись значений происходит при нажатии любой из клавиш KEY3-KEY0*/
void input_xy(int *x, int *y)
{
    while (*keys == 0)
    {
    };
    while (*keys != 0)
    {
        input_sw(x, 0x01ff, 0);
        input_sw(y, 0x03fe00, 9);
    };
}

/*Функция смены буферов.
FRAME - адрес нового буфера*/
void change_frame(volatile short int *FRAME)
{
    int status = *reg_status;
    *reg_back_buffer = FRAME;
    *reg_buffer = 1;

    while ((status & 0x01) != 0)
    {
        status = *reg_status;
    }
}

/*Функция, выполняющая передвижение отрезка по экрану
x_start, y_start - координаты начала отрезка,
x_end, y_end - координаты конца отрезка,
background_color - цвет фона,
color - цвет линии, FRAME - адрес начала буфера
При нажатии кнопки KEY3 прямая сдвигается на 1 позицию вверх,
кнопки KEY2 – вниз, 
кнопки KEY1 – влево, 
кнопки KEY1 и установленном в верхнее положение переключателе SW0 – вправо.
Завершение передвижения выполняется при возведении всех переключателей SW17-SW0 в единицу.
*/
void moving_line(int x_start, int y_start, int x_end, int y_end, short int background_color, short int color)
{
    volatile short int *FRAME = BACK_FRAME;
    int ks = *keys;
    int sw = 0;

    while (sw != 0x03ffff)
    {
        while (*keys == 0)
        {
        };
        ks = *keys;
        draw_filled_rectangle(0, 0, SCREEN_WIDTH, SCREEN_HEIGHT, background_color, FRAME);

        switch (ks)
        {
        case 0x08: //Движение вправо
            x_start++;
            x_end++;
            draw_line(x_start, y_start, x_end, y_end, color, FRAME);
            break;
        case 0x04: //Движение вправо
            x_start++;
            x_end++;
            draw_line(x_start, y_start, x_end, y_end, color, FRAME);
            break;
        case 0x02: 
            if (((*sw_switch) & 0x01) == 1) //Движение вправо
            {
                x_start++;
                x_end++;
                draw_line(x_start, y_start, x_end, y_end, color, FRAME);
            }
            else //Движение вправо
            {
                x_start++;
                x_end++;
                draw_line(x_start, y_start, x_end, y_end, color, FRAME);
            }
            break;
        }

        change_frame(FRAME);

        if (FRAME == BACK_FRAME)
            FRAME = FRONT_FRAME;
        else
            FRAME = BACK_FRAME;
        /* 
        while (*keys != 0)
        {
        };*/
        input_sw(&sw, 0x03ffff, 0);
    }
}

int main()
{
    draw_filled_rectangle(0, 0, SCREEN_WIDTH, SCREEN_HEIGHT, 0, FRONT_FRAME);
    change_frame(FRONT_FRAME);
    int x1, y1, x2, y2, x3, y3;
    int radius, color;

/*Ввод координат начальной и конечной точек отрезка*/
    input_xy(&x1, &y1);
    input_xy(&x2, &y2);

/*Ввод цвета при нажатии любой из клавиш KEY3-KEY0*/
    while (*keys == 0)
    {
    };
    while (*keys != 0)
    {
        input_sw(&color, 0x0ffff, 0);
    }
    //Вызов функции отрисовки отрезка
    draw_line(x1, y1, x2, y2, color, FRONT_FRAME);
    
    //Вызов функции передвижения отрезка
    moving_line(x1, y1, x2, y2, 0, color);

    return 0;
}
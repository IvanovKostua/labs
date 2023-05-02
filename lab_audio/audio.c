#include "address_map_nios2.h"
#define BUF_SIZE 144000
#define BUF_75 96
#define SCREEN_WIDTH 320
#define SCREEN_HEIGHT 240
void draw_rectangle(int x_start, int y_start, int width, int height, short int color, short int *start_address)
{
    short int *ptr = start_address;
    for (int y = y_start; y < y_start + height; ++y)
    {
        for (int x = x_start; x < x_start + width; ++x)
        {
            ptr[(y << 9) + x] = color;
        }
    }
}

void check_KEYs(int * KEY1, int * KEY2, int * KEY3, int * counter)
{
	volatile int * KEY_ptr = (int *) 0x10000050;		// pushbutton KEY address
	volatile int * audio_ptr = (int *) 0x10003040;	// audio port address
	int KEY_value;

	KEY_value = *(KEY_ptr); 				// read the pushbutton KEY values
	while (*KEY_ptr);							// wait for pushbutton KEY release

	if (KEY_value == 0x2)					// check KEY1
	{
		// reset counter to start recording
		*counter = 0;
		// clear audio-in FIFO
		*(audio_ptr) = 0x4;
		*(audio_ptr) = 0x0;

		*KEY1 = 1;
	}
	else if (KEY_value == 0x4)				// check KEY2
	{
		// reset counter to start playback
		*counter = 0;
		// clear audio-out FIFO
		*(audio_ptr) = 0x8;
		*(audio_ptr) = 0x0;

		*KEY2 = 1;
	}
	else if (KEY_value == 0x8)
	{
		*KEY3 = 1;	
	}
}

void main()
{
short int *FRONT_FRAME = (short int *)FPGA_PIXEL_BUF_BASE;
short int *ptr = FRONT_FRAME;
int * audio_ptr = (int *) 0x10003040;
int * green_LED_ptr = (int *) 0x10000010;
int left_buffer[BUF_SIZE];
int right_buffer[BUF_SIZE];
int fifospace;
int record=0, play = 0, buffer_index = 0, draw=0;
int c = 0;
int t1;
int t2;
int start = 0;
int p1 = 0;
int p2 = 0;
int color = 0xffff;
int scale = 5000000;
int offset = 100;

draw_rectangle(0, 0, SCREEN_WIDTH, SCREEN_HEIGHT, 0, FRONT_FRAME);
while(1)
{
  check_KEYs ( &record, &play, &draw, &buffer_index );
  if (record)
  {
	fifospace = *(audio_ptr + 1);
	*(green_LED_ptr) = 0x1;
	if ((fifospace & 0x000000ff) <BUF_75)
		{
  			while ( (fifospace & 0x000000FF) && (buffer_index < BUF_SIZE) )
				{
					left_buffer[buffer_index] = *(audio_ptr + 2); 		
					right_buffer[buffer_index] = *(audio_ptr + 3); 		
					++buffer_index;

					if (buffer_index == BUF_SIZE)
					{
						// done recording
                        record = 0;
						*(green_LED_ptr) = 0x0;				// turn off LEDG
					}
					fifospace = *(audio_ptr + 1);	// read the audio port fifospace register
				}
		} 
  }
	else if (play)
		{
			fifospace = *(audio_ptr + 1);	 		// read the audio port fifospace register
			*(green_LED_ptr) = 0x2;					// turn on LEDG_1
			if ( (fifospace & 0x00FF0000) > BUF_75 ) 	// check WSRC
			{
					// output data until the buffer is empty or the audio-out FIFO is full
					while ( (fifospace & 0x00FF0000) && (buffer_index < BUF_SIZE) )
					{
							*(audio_ptr + 2) = left_buffer[buffer_index];
							*(audio_ptr + 3) = right_buffer[buffer_index];
							++buffer_index;
							if (buffer_index == BUF_SIZE)
							{
								// done playback
								play = 0;
								*(green_LED_ptr) = 0x0;				// turn off LEDG
							}
							fifospace = *(audio_ptr + 1);	// read the audio port fifospace register
					}
			}
		}
	else if (draw)
		{
		switch (c)
			{
				case 0: color = 0xffff; break;
				case 1: color = 0xf800; break;
				case 2: color = 0x7e0; break;
				case 3: color = 0x1f; break;
				case 4: color = 0xffe0; break;
				case 5: color = 0xffff; c=0; break;
			}
	
	for (int i =0; i <320; i++)
	{
		t1 = left_buffer[i];
		t2 = left_buffer[i+1];

		if ((t1>0) && (t2<0) && (start == 0))
		{
			start = i;
		}
		else if ((t1>0) && (t2<0) && (p1 == 0))
		{
			p1 = i;
		}
		else if ((t1>0) && (t2<0) && (p2 == 0))
		{
			p2 = i;
		}
	
	}	
			for (int i =0; i <320; i++)
			{
				ptr[(left_buffer[start]/scale + 100 << 9) + i] = color;
			}
			for (int i =0; i <240; i++)
			{
				ptr[(i << 9) + p1-start] = color;
			}
			for (int i =0; i <240; i++)
			{
				ptr[(i << 9) + p2-start] = color;
			}

			for (int i =0; i <320; i++)
			{
				ptr[(left_buffer[i+start]/scale + offset << 9) + i] = color;
			}
			c++;
			draw = 0;
		}
	}
}

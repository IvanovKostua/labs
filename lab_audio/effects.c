#define BUF_SIZE 144000
#define BUF_75 96
void check_KEYs(int * KEY1, int * KEY2, int * counter)
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
}

main()
{
	int * audio_ptr = (int *) 0x10003040;
	int * green_LED_ptr = (int *) 0x10000010;
	volatile int * sw_ptr = (int *) 0x10000040;
	int sw=0;
	int fifospace;
	int record=0, play = 0, buffer_index = 0;
	int left_buffer[BUF_SIZE];
	int right_buffer[BUF_SIZE];
	int c=0;
	memset(left_buffer,0,BUF_SIZE*sizeof(int));
	memset(right_buffer,0,BUF_SIZE*sizeof(int));
	while(1)
	{
		check_KEYs ( &record, &play, &buffer_index );
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
			*(green_LED_ptr) = 0x2;					// turn on LEDG_1
			fifospace = *(audio_ptr + 1);	 		// read the audio port fifospace register
			sw=*(sw_ptr);							// read the switches
			if ( (fifospace & 0x00FF0000) > BUF_75 ) 	// check WSRC
			{
				// output data until the buffer is empty or the audio-out FIFO is full
				while ( (fifospace & 0x00FF0000) && (buffer_index < BUF_SIZE) )
				{
					if (((sw)& 0x0000000F) == 1) {
						*(audio_ptr + 2) = left_buffer[buffer_index];
						*(audio_ptr + 3) = right_buffer[buffer_index];
						c++;


						if (c == 2)
						{
							++buffer_index;
							c = 0;
						}

						if (buffer_index == BUF_SIZE)
						{
							// done playback
							play = 0;
							*(green_LED_ptr) = 0x0;				// turn off LEDG
						}
						fifospace = *(audio_ptr + 1);	// read the audio port fifospace register
					}
					else if (((sw)& 0x0000000F) == 2) {
						*(audio_ptr + 2) = left_buffer[buffer_index];
						*(audio_ptr + 3) = right_buffer[buffer_index];
						buffer_index = buffer_index + 2;

						if (buffer_index == BUF_SIZE)
						{
							// done playback
							play = 0;
							*(green_LED_ptr) = 0x0;				// turn off LEDG
						}
						fifospace = *(audio_ptr + 1);	// read the audio port fifospace register
					}
					else if (((sw)& 0x0000000F) == 4) {
						if (buffer_index <= 8000)
						{
							*(audio_ptr + 2) = left_buffer[buffer_index];
							*(audio_ptr + 3) = right_buffer[buffer_index];
						}
						else
						{
							*(audio_ptr + 2) = left_buffer[buffer_index] + left_buffer[buffer_index - 8000];
							*(audio_ptr + 3) = right_buffer[buffer_index] + right_buffer[buffer_index - 8000];
						}
						++buffer_index;

						if (buffer_index == BUF_SIZE)
						{
							// done playback
							play = 0;
							*(green_LED_ptr) = 0x0;				// turn off LEDG
						}
						fifospace = *(audio_ptr + 1);	// read the audio port fifospace register
					}
					else {
						*(audio_ptr + 2) = left_buffer[buffer_index];
						*(audio_ptr + 3) = right_buffer[buffer_index];
						buffer_index = buffer_index + 1;

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
		}
	}
}

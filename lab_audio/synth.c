#include <stdio.h>
#define BUF_SIZE 240000			
#define BUF_THRESHOLD 96		// 75% of 128 word buffer
#include "E:\efremov\audio\tones.h"
void check_KEYs(int * KEY1, int * KEY2,int * KEY3,int * KEY4,int * KEY5,int * KEY6,int * KEY7,int * KEY8,int * KEY9,int * KEY10,int * KEY11,int * KEY12,long int * counter,int *stop)
{
int * PS2_ptr = (int *) 0x10000100;
int work=0,work2=0,work3=0,work4=0,work5=0,work6=0,work7=0,work8=0,work9=0,work10=0,work11=0,work12=0;
        int PS2_data,RVALID=0,Data;          
      PS2_data =*PS2_ptr;
      Data=PS2_data&0x000000ff;
      RVALID=PS2_data&0x00008000;      
if ((RVALID!=0) && (Data==0x1C))   
         if (*stop==0) {work=1;}
          else *stop=0;
if ((RVALID!=0) && (Data==0x1D)) 
        if (*stop==0) {work2=1;}
          else *stop=0;				       
if ((RVALID!=0) && (Data==0x1B)) 
        if (*stop==0) {work3=1;}
          else *stop=0;
if ((RVALID!=0) && (Data==0x24)) 
        if (*stop==0) {work4=1;}
          else *stop=0;	
if ((RVALID!=0) && (Data==0x23)) 
        if (*stop==0) {work5=1;}
          else *stop=0;	
if ((RVALID!=0) && (Data==0x2B)) 
        if (*stop==0) {work6=1;}
          else *stop=0;
if ((RVALID!=0) && (Data==0x2C)) 
        if (*stop==0) {work7=1;}
          else *stop=0;                
if ((RVALID!=0) && (Data==0x34)) 
        if (*stop==0) {work8=1;}
          else *stop=0;                    
if ((RVALID!=0) && (Data==0x35)) 
        if (*stop==0) {work9=1;}
          else *stop=0;
if ((RVALID!=0) && (Data==0x33)) 
        if (*stop==0) {work10=1;}
          else *stop=0;	
if ((RVALID!=0) && (Data==0x3C)) 
        if (*stop==0) {work11=1;}
          else *stop=0;	
if ((RVALID!=0) && (Data==0x3B)) 
        if (*stop==0) {work12=1;}
          else *stop=0;	      				 
if (*KEY1==0)   if (work==1)
        {
		*counter = 0;
		*KEY1 = 1;
	}                   	
if (*KEY2==0)        if (work2==1)
        {
		*counter = 0;
		*KEY2 = 1; 
	}              	
if (*KEY3==0)        if (work3==1)
        {
		*counter = 0;
		*KEY3 = 1; 
	}              
if (*KEY4==0)        if (work4==1)
        {
		*counter = 0;
		*KEY4 = 1; 
	}              
if (*KEY5==0)        if (work5==1)
        {
		*counter = 0;
		*KEY5 = 1; 
	}              	
if (*KEY6==0)        if (work6==1)
        {
		*counter = 0;
		*KEY6 = 1; 
	}              	
if (*KEY7==0)        if (work7==1)
        {
		*counter = 0;
		*KEY7 = 1; 
	}              	
if (*KEY8==0)        if (work8==1)
        {
		*counter = 0;
		*KEY8 = 1; 
	}              	
if (*KEY9==0)        if (work9==1)
        {
		*counter = 0;
		*KEY9 = 1; 
	}              
if (*KEY10==0)        if (work10==1)
        {
		*counter = 0;
		*KEY10 = 1; 
	}                
if (*KEY11==0)        if (work11==1)
        {
		*counter = 0;
		*KEY11 = 1; 
	}              
if (*KEY12==0)        if (work12==1)
        {
		*counter = 0;
		*KEY12 = 1; 
	}  


if ((RVALID!=0) && (Data==0xf0))	 
        {
	//	*counter = 48000;
		*KEY1 = 0;             
                *KEY2=0;
                *KEY3=0;
                *KEY4=0;
                *KEY5=0;
                *KEY6=0;
           	*KEY7=0;
		*KEY8=0;
		*KEY9=0;
		*KEY10=0;
		*KEY11=0;
		*KEY12=0;
                *stop=1; 
		work=0; 
		work2=0; 
		work3=0; 
		work4=0; 
		work5=0; 
		work6=0; 
		work7=0; 
		work8=0; 
		work9=0; 
		work10=0; 
		work11=0; 
		work12=0;
	}       
}
void main()
{
volatile int * audio_ptr = (int *) 0x10003040;	// audio port address
int stop=0;       
	long int fifospace;
	int key1 = 0, key2 = 0,key3=0,key4=0,key5=0,key6=0,key7=0,key8=0,key9=0,key10=0,key11=0,key12=0; 
        long int buffer_index = 0;
	while(1)
	{  
 check_KEYs ( &key1, &key2,&key3,&key4,&key5,&key6,&key7,&key8,&key9,&key10,&key11,&key12, &buffer_index,&stop);    
if (key1==1)            
		{   
			fifospace = *(audio_ptr + 1);	 		// read the audio port fifospace register
			if ( (fifospace & 0x00FF0000) > BUF_THRESHOLD ) 	// check WSRC
			{
				// output data until the buffer is empty or the audio-out FIFO is full
				while ( (fifospace & 0x00FF0000) && (buffer_index < BUF_SIZE) )
				{
					*(audio_ptr + 2) = tone1[buffer_index];
					*(audio_ptr + 3) = tone1[buffer_index];
					++buffer_index;
                                        check_KEYs ( &key1, &key2,&key3,&key4,&key5,&key6,&key7,&key8,&key9,&key10,&key11,&key12, &buffer_index,&stop);

					if (buffer_index == BUF_SIZE)
					{
						// done playback
					         key1 = 0;               				// turn off LEDG    
					}       
					fifospace = *(audio_ptr + 1);	// read the audio port fifospace register
				}
			}         
		}
if (key2==1)
		{					
			fifospace = *(audio_ptr + 1);	 		// read the audio port fifospace register
			if ( (fifospace & 0x00FF0000) > BUF_THRESHOLD ) 	// check WSRC
			{
				// output data until the buffer is empty or the audio-out FIFO is full
				while ( (fifospace & 0x00FF0000) && (buffer_index < BUF_SIZE) )
				{
					*(audio_ptr + 2) = tone2[buffer_index];
					*(audio_ptr + 3) = tone2[buffer_index];
					++buffer_index;
                                        check_KEYs ( &key1, &key2,&key3,&key4,&key5,&key6,&key7,&key8,&key9,&key10,&key11,&key12, &buffer_index,&stop);
					if (buffer_index == BUF_SIZE)
					{
						// done playback
						key2 = 0;
					}   
					fifospace = *(audio_ptr + 1);	// read the audio port fifospace register
				}
			}         
		}
if (key3==1)
		{ 
			fifospace = *(audio_ptr + 1);	 		// read the audio port fifospace register
			if ( (fifospace & 0x00FF0000) > BUF_THRESHOLD ) 	// check WSRC
			{
				// output data until the buffer is empty or the audio-out FIFO is full
				while ( (fifospace & 0x00FF0000) && (buffer_index < BUF_SIZE) )
				{
					*(audio_ptr + 2) = tone3[buffer_index];
					*(audio_ptr + 3) = tone3[buffer_index];
					++buffer_index;
                                        check_KEYs ( &key1, &key2,&key3,&key4,&key5,&key6,&key7,&key8,&key9,&key10,&key11,&key12, &buffer_index,&stop);
					if (buffer_index == BUF_SIZE)
					{
						// done playback
						key3 = 0;
					}   
					fifospace = *(audio_ptr + 1);	// read the audio port fifospace register
				}
			}
		}	
if (key4==1)
		{   
			fifospace = *(audio_ptr + 1);	 		// read the audio port fifospace register
			if ( (fifospace & 0x00FF0000) > BUF_THRESHOLD ) 	// check WSRC
			{
				// output data until the buffer is empty or the audio-out FIFO is full
				while ( (fifospace & 0x00FF0000) && (buffer_index < BUF_SIZE) )
				{
					*(audio_ptr + 2) = tone4[buffer_index];
					*(audio_ptr + 3) = tone4[buffer_index];
					++buffer_index;
                                        check_KEYs ( &key1, &key2,&key3,&key4,&key5,&key6,&key7,&key8,&key9,&key10,&key11,&key12, &buffer_index,&stop);
					if (buffer_index == BUF_SIZE)
					{
						// done playback
						key4 = 0;
					}      
					fifospace = *(audio_ptr + 1);	// read the audio port fifospace register
				}
			}
		}	
if (key5==1)	{  
			fifospace = *(audio_ptr + 1);	 		// read the audio port fifospace register
			if ( (fifospace & 0x00FF0000) > BUF_THRESHOLD ) 	// check WSRC
			{
				// output data until the buffer is empty or the audio-out FIFO is full
				while ( (fifospace & 0x00FF0000) && (buffer_index < BUF_SIZE) )
				{
					*(audio_ptr + 2) = tone5[buffer_index];
					*(audio_ptr + 3) = tone5[buffer_index];
					++buffer_index;
                                        check_KEYs ( &key1, &key2,&key3,&key4,&key5,&key6,&key7,&key8,&key9,&key10,&key11,&key12, &buffer_index,&stop);
					if (buffer_index == BUF_SIZE)
					{
						// done playback
						key5 = 0;
					}     
					fifospace = *(audio_ptr + 1);	// read the audio port fifospace register
				}
			}
		}	
if (key6==1)
		{  
			fifospace = *(audio_ptr + 1);	 		// read the audio port fifospace register
			if ( (fifospace & 0x00FF0000) > BUF_THRESHOLD ) 	// check WSRC
			{
				// output data until the buffer is empty or the audio-out FIFO is full
				while ( (fifospace & 0x00FF0000) && (buffer_index < BUF_SIZE) )
				{
					*(audio_ptr + 2) = tone6[buffer_index];
					*(audio_ptr + 3) = tone6[buffer_index];
					++buffer_index;
                                        check_KEYs ( &key1, &key2,&key3,&key4,&key5,&key6,&key7,&key8,&key9,&key10,&key11,&key12, &buffer_index,&stop);
					if (buffer_index == BUF_SIZE)
					{
						// done playback
						key6 = 0;
					}   
					fifospace = *(audio_ptr + 1);	// read the audio port fifospace register
				}
			}
		}	
if (key7==1)
		{    
			fifospace = *(audio_ptr + 1);	 		// read the audio port fifospace register
			if ( (fifospace & 0x00FF0000) > BUF_THRESHOLD ) 	// check WSRC
			{
				// output data until the buffer is empty or the audio-out FIFO is full
				while ( (fifospace & 0x00FF0000) && (buffer_index < BUF_SIZE) )
				{
					*(audio_ptr + 2) = tone7[buffer_index];
					*(audio_ptr + 3) = tone7[buffer_index];
					++buffer_index;
                                        check_KEYs ( &key1, &key2,&key3,&key4,&key5,&key6,&key7,&key8,&key9,&key10,&key11,&key12, &buffer_index,&stop);
					if (buffer_index == BUF_SIZE)
					{
						// done playback
						key7 = 0;
					}   
					fifospace = *(audio_ptr + 1);	// read the audio port fifospace register
				}
			}
		}	                      
if (key8==1)
		{    
			fifospace = *(audio_ptr + 1);	 		// read the audio port fifospace register
			if ( (fifospace & 0x00FF0000) > BUF_THRESHOLD ) 	// check WSRC
			{
				// output data until the buffer is empty or the audio-out FIFO is full
				while ( (fifospace & 0x00FF0000) && (buffer_index < BUF_SIZE) )
				{
					*(audio_ptr + 2) = tone8[buffer_index];
					*(audio_ptr + 3) = tone8[buffer_index];
					++buffer_index;
                                        check_KEYs ( &key1, &key2,&key3,&key4,&key5,&key6,&key7,&key8,&key9,&key10,&key11,&key12, &buffer_index,&stop);
					if (buffer_index == BUF_SIZE)
					{
						// done playback
						key8 = 0;
					}       
					fifospace = *(audio_ptr + 1);	// read the audio port fifospace register
				}
			}
		}	
if (key9==1)
		{    
			fifospace = *(audio_ptr + 1);	 		// read the audio port fifospace register
			if ( (fifospace & 0x00FF0000) > BUF_THRESHOLD ) 	// check WSRC
			{
				// output data until the buffer is empty or the audio-out FIFO is full
				while ( (fifospace & 0x00FF0000) && (buffer_index < BUF_SIZE) )
				{
					*(audio_ptr + 2) = tone9[buffer_index];
					*(audio_ptr + 3) = tone9[buffer_index];
					++buffer_index;
                                        check_KEYs ( &key1, &key2,&key3,&key4,&key5,&key6,&key7,&key8,&key9,&key10,&key11,&key12, &buffer_index,&stop);
					if (buffer_index == BUF_SIZE)
					{
						// done playback
						key9 = 0;
					}    
					fifospace = *(audio_ptr + 1);	// read the audio port fifospace register
				}
			}
		}	
if (key10==1)
		{  
			fifospace = *(audio_ptr + 1);	 		// read the audio port fifospace register
			if ( (fifospace & 0x00FF0000) > BUF_THRESHOLD ) 	// check WSRC
			{
				// output data until the buffer is empty or the audio-out FIFO is full
				while ( (fifospace & 0x00FF0000) && (buffer_index < BUF_SIZE) )
				{
					*(audio_ptr + 2) = tone10[buffer_index];
					*(audio_ptr + 3) = tone10[buffer_index];
					++buffer_index;
                                        check_KEYs ( &key1, &key2,&key3,&key4,&key5,&key6,&key7,&key8,&key9,&key10,&key11,&key12, &buffer_index,&stop);
					if (buffer_index == BUF_SIZE)
					{
						// done playback
						key10 = 0;
					}   
					fifospace = *(audio_ptr + 1);	// read the audio port fifospace register
				}
			}
		}	
if (key11==1)
		{    
			fifospace = *(audio_ptr + 1);	 		// read the audio port fifospace register
			if ( (fifospace & 0x00FF0000) > BUF_THRESHOLD ) 	// check WSRC
			{
				// output data until the buffer is empty or the audio-out FIFO is full
				while ( (fifospace & 0x00FF0000) && (buffer_index < BUF_SIZE) )
				{
					*(audio_ptr + 2) = tone11[buffer_index];
					*(audio_ptr + 3) = tone11[buffer_index];
					++buffer_index;
                                        check_KEYs ( &key1, &key2,&key3,&key4,&key5,&key6,&key7,&key8,&key9,&key10,&key11,&key12, &buffer_index,&stop);
					if (buffer_index == BUF_SIZE)
					{
						// done playback
						key11 = 0;
					}    
					fifospace = *(audio_ptr + 1);	// read the audio port fifospace register
				}
			}
		}	
if (key12==1)
		{      
			fifospace = *(audio_ptr + 1);	 		// read the audio port fifospace register
			if ( (fifospace & 0x00FF0000) > BUF_THRESHOLD ) 	// check WSRC
			{
				// output data until the buffer is empty or the audio-out FIFO is full
				while ( (fifospace & 0x00FF0000) && (buffer_index < BUF_SIZE) )
				{
					*(audio_ptr + 2) = tone12[buffer_index];
					*(audio_ptr + 3) = tone12[buffer_index];
					++buffer_index;
                                        check_KEYs ( &key1, &key2,&key3,&key4,&key5,&key6,&key7,&key8,&key9,&key10,&key11,&key12, &buffer_index,&stop);
					if (buffer_index == BUF_SIZE)
					{
						// done playback
						key12 = 0;
					}    
					fifospace = *(audio_ptr + 1);	// read the audio port fifospace register
				}
			}
		}		          

	}        
}           

#include "p18f4550.h"
#include "HSB.h"

#pragma config WDT=OFF, LVP=OFF, DEBUG=OFF, MCLRE = ON
 
//Internal oscillator, port function on RA6, EC used by USB 
#pragma config FOSC = INTOSCIO_EC //1MHz oscillation



#include <spi.h>
#include <adc.h> 

//#define INDICATOR PORTAbits.RA2



	 
 #define  GIVEBYTE(data)               \
	        WriteSPI(data);

	


unsigned int channel[2][7] = {{0,20,40,60,80,100,120},{120,100,80,60,40,20,00}};

unsigned int total;
unsigned int Htotal;
unsigned char MaxColor;
#define RESET PORTCbits.RC0
#define STROBE PORTCbits.RC2

#define INDICATOR PORTCbits.RC1

#define  RIGHT ADC_CH0
#define  LEFT  ADC_CH1  

unsigned int loadADCchannel(unsigned char channel){
	 SetChanADC (channel);//was 11
   	   Delay10TCYx(10);
       ConvertADC ();	// Start an A/D conversion				
       while(BusyADC()){	// Wait for the conversion to complete
       //maybe some code for while were waiting (could turn an led on maybe)
        }
        Delay10TCYx(10);
        return  ReadADC();
}
	

void LoadVolume(){
   unsigned char i,j;
   RESET = 1;
    Delay10TCYx(10);
   STROBE=1;
   Delay10TCYx(10);
   STROBE=0;
   Delay10TCYx(10); 
   
   RESET = 0;

 //STROBE=1;
  // Delay10TCYx(100); 
   
  
  
   for(i=0; i< 7; i++){
	  	STROBE=1;
   		Delay10TCYx(10);
        STROBE=0;
        Delay10TCYx(10); 
	    

        channel[0][i] = loadADCchannel(RIGHT);
        channel[1][i] = loadADCchannel(RIGHT);

/*
        channel[0][i]+=10;
        channel[1][i]+=10;
        
         if(channel[0][i] > 300)
             channel[0][i]=0;
          
              if(channel[1][i] > 300)
             channel[1][i]=0;
    
   }
   */
   
  //  channel[0][0] = (channel[0][0]<<3)/6;
  //  channel[1][0] = (channel[1][0]<<3)/6;
    
  //  channel[0][1] = (channel[0][1]<<3)/7;
  //  channel[1][1] = (channel[1][1]<<3)/7;  
   }     
}



/*
void LoadVolume(){
   unsigned char i,j;

   
  
  
   for(i=0; i< 7; i++){
	  	STROBE=1;
   		Delay10TCYx(10);
        STROBE=0;
        Delay10TCYx(10); 
	    

        channel[0][i] ++;
         channel[0][i] =  channel[0][i]>20?0: channel[0][i];
         
        channel[1][i] ++;
          channel[1][i] =  channel[1][i]>20?0:  channel[1][i];
             
    
   }
}
*/


unsigned int safesub(unsigned int a, unsigned int b){
if(b>a)
  return 0;
return a-b;	
}

unsigned char SafeByte(unsigned int input){
   if(input > 255)
      return 255;
      return input;	
	
}
	
	
	



	




	
	

		





#define BRIGHTNESS  ADC_CH2
#define SENSITIVITY ADC_CH4
#define ZERO_ADJ    ADC_CH3

unsigned char Brightness;
unsigned char Sensitivity;
unsigned char Zero_Adj;


void LoadOptions(){
	Brightness = 255;//(loadADCchannel(BRIGHTNESS)/4);
	Sensitivity = 10;
	Zero_Adj = 2;//loadADCchannel(ZERO_ADJ)/8;
}
	

                       //RGB
                       
unsigned char RMatrix[44][6];
unsigned char GMatrix[44][6];
unsigned char BMatrix[44][6];
//unsigned char Gmatrix[44][8];
//unsigned char Bmatrix[44][8];

void StupidPattern(){
	unsigned char i,j,temp;
	static unsigned char counter;
	counter++;
	if(counter > 44)
		counter  = 0;
	
	for(j=0; j< 44; j++){
		for(i=0; i< 6; i++){
			temp =  ((i+j+counter)*17)%255;
			RMatrix[j][i] = HSB_ar[temp][0] ;
			GMatrix[j][i] = HSB_ar[temp][1];	
			BMatrix[j][i] = HSB_ar[temp][2];
			
		}	
	}
	
}
	

static unsigned char ScrollMessage[6][6] = {
{0b11110000,0b01000011,0b11000111,0b11000100,0b01000000,0b00000000},
{0b10001000,0b10100010,0b00100001,0b00000100,0b01000000,0b00000000},
{0b10001001,0b00010010,0b00100001,0b00000010,0b10000000,0b00000000},
{0b11110001,0b11110011,0b11000001,0b00000001,0b00000000,0b00000000},
{0b10000001,0b00010010,0b10000001,0b00000001,0b00000000,0b00000000},
{0b10000001,0b00010010,0b01000001,0b00000001,0b00000000,0b00000000}
};

#define M_H 6
#define M_L 6

unsigned char GetBit(unsigned char x, unsigned char y){
unsigned char byteoffset;
unsigned char bitoffset;
unsigned char TheByte;
if(y>=M_H)
   return 0;
y = M_H-1 - y;
byteoffset = x>>3;
if(byteoffset >= M_L)
  return 0;
  
bitoffset = x - (byteoffset<<3);
TheByte = ScrollMessage[y][ M_L-1-byteoffset];
if((TheByte & (1<<bitoffset)) == 0)
    return 0;
 else
    return 1;
}


void SetMatrix(){
	unsigned char x,y;	
	for(y=0; y< 6; y++){
		for(x=0; x< 44; x++){
			RMatrix[x][y] = GetBit(47-x,5-y)*255;
			GMatrix[x][y] =	RMatrix[x][y];
			BMatrix[x][y] = RMatrix[x][y];
		}
				
	}	
}	


void ScrollMatrix(){
		unsigned char x,y,Rtemp,Gtemp,Btemp;
		static unsigned char counter,shiftc=1;
		counter +=17;	
	//	shiftc++;
		if(counter > 255)
			counter  = 0;
					if(shiftc%2){
		for(y=0; y< 6; y++){
			
	
			Rtemp = 	RMatrix[0][y];
			Gtemp = 	GMatrix[0][y];
			Btemp = 	BMatrix[0][y];
			
			for(x=0; x< 43; x++){
					RMatrix[x][y] = (RMatrix[x+1][y] != 0) ? 10+(HSB_ar[counter][0]>>1):0;
					GMatrix[x][y] = (GMatrix[x+1][y] != 0) ? 10+(HSB_ar[counter][1]>>1):0;
					BMatrix[x][y] = (BMatrix[x+1][y] != 0) ? 10+(HSB_ar[counter][2]>>1):0;
				
			}	
			
				RMatrix[43][y] = Rtemp;
				GMatrix[43][y] = Gtemp;
				BMatrix[43][y] = Btemp;
			
		}	
		} else {
			for(y=0; y< 6; y++){
				for(x=0; x< 44; x++){
					
					RMatrix[x][y] = (RMatrix[x][y] != 0) ? 10+(HSB_ar[counter][0]>>1):0;
					GMatrix[x][y] = (GMatrix[x][y] != 0) ? 10+(HSB_ar[counter][1]>>1):0;
					BMatrix[x][y] = (BMatrix[x][y] != 0) ? 10+(HSB_ar[counter][2]>>1):0;
					
					
		}
		}
	}
}	

void RainbowPattern(){
	unsigned char i,j,temp;
	static unsigned char counter;
	counter++;
	if(counter > 44)
		counter  = 0;
	
	for(j=0; j< 44; j++){
		for(i=0; i< 6; i++){
			temp =  ((i+j+counter)*17)%255;
			RMatrix[j][i] = HSB_ar[temp][0] ;
			GMatrix[j][i] = HSB_ar[temp][1];	
			BMatrix[j][i] = HSB_ar[temp][2];
			
		}	
	}
	
}



void SetColoumn(unsigned char index){
	
	GIVEBYTE(RMatrix[index][3]);//3rd from bottom
	GIVEBYTE(GMatrix[index][3]);
	GIVEBYTE(BMatrix[index][3]);
	
	GIVEBYTE(RMatrix[index][4]);
	GIVEBYTE(GMatrix[index][4]);
	GIVEBYTE(BMatrix[index][5]);
	
	GIVEBYTE(GMatrix[index][5]);
	GIVEBYTE(RMatrix[index][5]);
	GIVEBYTE(BMatrix[index][4]);
	
	GIVEBYTE(BMatrix[index][2]);
	GIVEBYTE(GMatrix[index][2]);
	GIVEBYTE(RMatrix[index][2]);
	
	GIVEBYTE(BMatrix[index][1]);
	GIVEBYTE(GMatrix[index][1]);
	GIVEBYTE(RMatrix[index][1]);
	
	GIVEBYTE(BMatrix[index][0]);
	GIVEBYTE(GMatrix[index][0]);
	GIVEBYTE(RMatrix[index][0]);
	
	
}	
	
	
void main(void){

    unsigned char coloumn, row,band;
    
    unsigned int a,b,i;
    
    unsigned long longcount = 4999;

    
    

    
    TRISE = 0;
   	TRISD = 0;
	TRISC = 0;
	TRISB = 0b00000000;
    TRISA = 0b00011111;
    PORTD = 0;
	PORTC = 0;
	PORTB = 0b00000000;
	PORTA = 0b00011111;
	PORTE = 0;
	
//	OSCTUNE = 0b00001111; 
//	OSCCON  = 0b01110010;
	
	
//    ADCON1 = 0x0F;
 //  UCON &= 0b11111011;
 //  UCFG =  0b00000100;

   // INTCON2 &= 0b01111111;
 //  INTCON2 |= 0b10000000;
   
    //SPI_FOSC_4 = go fast
 
 
  
  	OSCTUNE = 0b00001111; 
	OSCCON  = 0b01110010;
	OpenSPI(SPI_FOSC_4, MODE_00, SMPEND); //initialize SPI
	
	
	
	
	while(1){
			
	  //  StupidPattern();
	  
	  longcount++;
	  
	  if(longcount == 5000)
	  	SetMatrix();
	  else if (longcount > 5000){
	  	ScrollMatrix();
	  	if( longcount > 10000 )
	  		longcount = 0;
	  }	else {
	  	RainbowPattern();
	  }

		for(i=0; i< 44; i++){
			SetColoumn(i);	
	 	}
	   
	    	
		INDICATOR = 0;
		

	
		INDICATOR = 1;
		
//		for(i=0; i < 2; i++)
//		Delay100TCYx(1);
	
	
		


    }
}
#ifndef MAIN_H
#define MAIN_H

#include <33FJ64MC802.h>
#device ADC=12
#device ICSP=1
#use delay(internal=32MHz) // slow down clock to 32 MHz

#FUSES NOWDT                    //No Watch Dog Timer
#FUSES CKSFSM                   //Clock Switching is enabled, fail Safe clock monitor is enabled
#FUSES NOJTAG                   //JTAG disabled
#FUSES FRC_PLL

#pin_select U2TX=PIN_B6 // UART TX Pin
#pin_select U2RX=PIN_B7 // UART RX Pin
#USE RS232(UART2, BAUD = 115200, PARITY = N, BITS = 8, STOP = 1, TIMEOUT = 500,  stream = SHARP)

#include <stdio.h> // include files 
#include <ctype.h> // include files 

#define BUFFER_SIZE 255 // buffer length is 255
#define COEF_LENGTH 64 // number of coefficents
#define ADC_MAX_DATA_VALUE 255 // max value of unsinged 8bit data 
#define LED_PIN PIN_B14  // LED pin for toggling in timer handler

#define CharToInt(A) (int)(A - 0x30)

typedef unsigned int32 IndexType;
typedef unsigned int8 FlagType;

const unsigned int8 ADC_DMA_CHANNEL = 0; // ADC DMA Channel is Channel 0 

unsigned int16 TimerTicks = 0; // timer ticks of timer 

unsigned int8 ConversionValue; // normalized value
unsigned int8 AnalogData[BUFFER_SIZE]; // input array

unsigned int8 CSharpCoefficent[2];          // array to hold byte data of coefficents

unsigned int16 InitialTriggerValue = 0; // used for initail trigger value
unsigned int16 TriggerValue = 0; // used for initail trigger value

signed int16 ByteConversionResult = 0;     // result of byte convertion

signed int32 Accumulator = 0; // accumulator of the output value in the difference equation calculation
signed int32 DigitizedData[BUFFER_SIZE];   // output array
signed int32 MaxAnalogValue = 0; // use for normalization
signed int32 MinAnalogValue = 0; // use for normalization

signed int64 AverageAnalogValue = 0; // use for normalization

signed int16  InputSamples[COEF_LENGTH]; // array used as a circular buffer for the input samples
unsigned int16 TempInputSamples[2]; // used for trigger value 

float AverageDivider = 0; // use for normalization
float AverageMultiplier = 0;    // use for normalization

char UARTRX = '\0'; // data in

IndexType CSharpCoefficentRecieved = 0;    // counter of coefficents recieved from c#
IndexType CoefficentIndex = 0;            // used as the index for the filter coefficients array in the difference equation calculation
IndexType InputIndex= 0;           // used as the index for the input samples array in the difference equation calculation
IndexType CurrentIndex = 0;          // keeps track of the current position of the circular buffer
IndexType NumberCSharpByteRecieved = 0; // counter to track which byte is being recieved
IndexType ErrorCounter = 0; // error detection
IndexType NormalizeDataCounter = 0;

FlagType DMADoneFlag = 0; // dma done flag
FlagType NormalizeFlag = 0; // normalize flag 
FlagType UARTRXFlag = 0;   // serial flag
FlagType HandShakeFlag = 0;    // handshake
FlagType TriggerFlag = 0; // trigger flag 
FlagType DMAFlag = 0; // dma flag
FlagType TriggerValueFlag = 0; // trigger value flag

void AccumulateAnalogData(IndexType); // filter data and store into array 
void NormalizeData(); // normalize the input data
void CommHandler(char); // handle UART commuication
void DisableInterrupts(void); // disable interrupt 
void EnableInterrupts(void); // enable interrupts 

volatile signed int16 fir_coef[COEF_LENGTH]; //array to hold filter coeffiecents

#endif // MAIN_H




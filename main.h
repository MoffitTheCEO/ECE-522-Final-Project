#ifndef MAIN_H
#define MAIN_H

#include <33FJ64MC802.h>
#device ADC=12
#device ICD=TRUE
#device ICSP=1
#use delay(internal=40MHz)

#FUSES NOWDT                    //No Watch Dog Timer
#FUSES CKSFSM                   //Clock Switching is enabled, fail Safe clock monitor is enabled
#FUSES NOJTAG                   //JTAG disabled

#pin_select U2TX=PIN_B6
#pin_select U2RX=PIN_B7
#USE RS232(UART2, BAUD = 115200, PARITY = N, BITS = 8, STOP = 1, TIMEOUT = 500,  stream = SHARP)) // RECEIVE_BUFFER=255, TRANSMIT_BUFFER=255, TXISR,

#include <float.h>
#include <math.h>
#include <stddef.h>
#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include <stdint.h>

#define BUFFER_SIZE 100

unsigned int8 DMA_DONE = 0;
const unsigned int8 ADC_DMA_CHANNEL = 0;
const unsigned int8 UART_TX_DMA_CHANNEL = 1;

#endif // MAIN_H




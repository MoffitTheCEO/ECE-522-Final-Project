#include "main.h"

#BANK_DMA
unsigned int16  DMA_ADC_BUFFER[BUFFER_SIZE];
#BANK_DMA
char DMA_UART_TX_BUFFER[BUFFER_SIZE];
//Todo:: Two DMA Buffers for real time data sampling


#INT_DMA0
void  DMA_0_ISR(void) 
{
   DMA_DONE = 1;
}

#INT_DMA1
void DMA_1_ISR(void)
{

}

#INT_TIMER1
void Timer_ISR()
{
   read_adc();
}

void main()
{   
   memset(DMA_ADC_BUFFER, 0, BUFFER_SIZE);
   memset(DMA_UART_TX_BUFFER, 'a', BUFFER_SIZE);
   
   setup_dma(ADC_DMA_CHANNEL, DMA_IN_ADC1, DMA_WORD);
   dma_start(ADC_DMA_CHANNEL, DMA_CONTINOUS, &DMA_ADC_BUFFER[0], BUFFER_SIZE);

   
   setup_dma(UART_TX_DMA_CHANNEL, DMA_OUT_UART2, DMA_BYTE | DMA_HALF_INT);

   enable_interrupts(INT_DMA0);
   enable_interrupts(INT_DMA1);
   
   setup_adc_ports(sAN0, VSS_VDD);
   setup_adc(ADC_CLOCK_INTERNAL | ADC_TAD_MUL_4);
   
   read_adc();
   
   setup_timer1(TMR_INTERNAL ,60000);
   enable_interrupts(INT_TIMER1);
   enable_interrupts(INTR_GLOBAL);

   while(TRUE)
   {
      delay_ms(1000);
      if(DMA_DONE)
      {
         disable_interrupts(INT_DMA0);
         for(unsigned int16 i=0; i< BUFFER_SIZE; i++)
         {
               printf("\n\r ADC Val: %d", DMA_ADC_BUFFER[i]);
         }
         
         dma_start(UART_TX_DMA_CHANNEL, DMA_ONE_SHOT | DMA_FORCE_NOW, &DMA_ADC_BUFFER[0], BUFFER_SIZE);
         enable_interrupts(INT_DMA0);
         DMA_DONE = 0;
      }      
   }
}

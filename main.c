#include "main.h"

#INT_DMAERR
void  dmaerr_isr(void) 
{
   printf("DMA ERROR!!");
}

#INT_DMA0
void  DMA_0_ISR(void) 
{
   DMA_DONE = 1;
}

#INT_TIMER1
void Timer_ISR()
{
   read_adc();
}

void main()
{   
   #BANK_DMA
   unsigned int16  DMA_BUFFER[BUFFER_SIZE];

   setup_dma(0, DMA_IN_ADC1, DMA_WORD);
   dma_start(0, DMA_CONTINOUS ,  &DMA_BUFFER[0]);

   enable_interrupts(INT_DMA0);
   
   setup_adc_ports(sAN0, VSS_VDD);
   setup_adc(ADC_CLOCK_INTERNAL | ADC_TAD_MUL_4);
   
   read_adc();
   
   setup_timer1(TMR_INTERNAL ,60000);
   enable_interrupts(INT_TIMER1);
   enable_interrupts(INTR_GLOBAL);

   while(TRUE)
   {
      delay_ms(100);
      if(DMA_DONE)
      {
         unsigned int16 Index = 0;
//!         disable_interrupts(INTR_GLOBAL);
         disable_interrupts(INT_DMA0);
         for(unsigned int16 i=0; i< BUFFER_SIZE; i++)
         {
            printf("\n\r ADC Val: %d", DMA_BUFFER[i]);
         }
         enable_interrupts(INT_DMA0);
         DMA_DONE = 0;
      }      
   }
}

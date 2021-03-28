DMA1_Channel2->CCR |= DMA_CCR_HTIE;

//for channel 2, before starting the transfer. Don't forget to set the priority, and enable the interrupt.

//In the interrupt handler, check if it's indeed a Half Transfer Interrupt, clear the flag, then do whatever processing is necessary.

void DMA1_Channel2_3_IRQHandler(void) {
    if(DMA1->ISR & DMA_ISR_HTIF2) {
        DMA1->IFCR = DMA_IFCR_CHTIF2;
        memcpy(here, there, length/2);
    }
}


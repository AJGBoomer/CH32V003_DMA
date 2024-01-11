/********************************** (C) COPYRIGHT *******************************
 * File Name          : main.c
 * Author             : WCH
 * Version            : V1.0.0
 * Date               : 2023/12/25
 *********************************************************************************
 * Copyright (c) 2021 Nanjing Qinheng Microelectronics Co., Ltd.
 * Attention: This software (modified or not) and binary are used for 
 * microcontroller manufactured by Nanjing Qinheng Microelectronics.
 *******************************************************************************/
 /* DMA RGB LED Example
 * Author			  : A. J. Griggs
 * Date				  : 2024/01/09
 * Description        : Main program body.
 *	Using the above SPI example from WCH, add GRB (WS2812) encoding to demonstrate using DMA
 *	with SPI rather than bit-banging.
 *	You Must complete building half of the DMA half buffer (top or bottom) before DMA to transfer it.
 *	For this example, filling the buffer took 32.5% of the total processing time compiled in 'fast' optimization.
 *	87% with No optimization.
 *	43% with Optimize (O)
 *	17.5% with Optimize (O) removing the (not needed) amplitude multiply in setLed()
 *	14% as above in optimize fast
 *	Timings are from scope traces of the DMA service loop and don't include interrupt processing (which should be minimal)
 */
#include "debug.h"
#include "main.h"
#include "patterns.h"
/* Global Variable */

#define TEST // test code enable

/* SPI Mode Definition */
#define HOST_MODE   1
#define SLAVE_MODE   0
/* SPI Communication Mode Selection */
#define SPI_MODE   HOST_MODE

typedef enum {busy,half,complete} DMADONE;

volatile u8 Txval=0, Rxval=0;
volatile  DMADONE DmaDone = busy;
volatile u16 offset = 0;

static GRB wheel[]={{0xff,00,00},{00,0xff,00},{00,00,0xff},{0xff,0xff,00},{00,0xff,0xff},{0xff,0xff,0xff},{0,0,0}};

u8 TxReset[] = {Reset,Reset,Reset,Reset,Reset,Reset,Reset,Reset,Reset};
u8 resetSize = sizeof(TxReset);
const uint16_t NLEDS = (DMABUFNLEDS * DMABUFSPERSTRING);
u8 TxBuf[BYTESPERLED * DMABUFNLEDS];
u8 offsetSize = BYTESPERLED; // One rgb led
u16 patternCnt = 0; // current position in the pattern
u16 ledBlockCnt = 0; // position at the start of the current DMA block
u16 phase = 0;       // offset count (in leds) to start of pattern
extern const u8 BitTable[];
extern const u16 peakPatSz;
extern const u8 peakPat[];

/*********************************************************************
 * @fn      SPI_1Lines_HalfDuplex_Init
 *
 * @brief   Configuring the SPI for half-duplex communication.
 *
 * @return  none
 */
void SPI_1Lines_HalfDuplex_Init(void)
{
    GPIO_InitTypeDef GPIO_InitStructure={0};
    SPI_InitTypeDef SPI_InitStructure={0};
    NVIC_InitTypeDef NVIC_InitStructure={0};

    RCC_APB2PeriphClockCmd( RCC_APB2Periph_GPIOC | RCC_APB2Periph_SPI1, ENABLE );
// Debug Pin
    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_3;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_Init( GPIOC, &GPIO_InitStructure );

#if (SPI_MODE == HOST_MODE)
    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_5;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_Init( GPIOC, &GPIO_InitStructure );

    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_6;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_Init( GPIOC, &GPIO_InitStructure );

#elif (SPI_MODE == SLAVE_MODE)
    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_5;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN_FLOATING;
    GPIO_Init( GPIOC, &GPIO_InitStructure );

    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_7;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN_FLOATING;
    GPIO_Init( GPIOC, &GPIO_InitStructure );
#endif


#if (SPI_MODE == HOST_MODE)
    SPI_InitStructure.SPI_Direction = SPI_Direction_1Line_Tx;
    SPI_InitStructure.SPI_Mode = SPI_Mode_Master;

#elif (SPI_MODE == SLAVE_MODE)
    SPI_InitStructure.SPI_Direction = SPI_Direction_1Line_Rx;
    SPI_InitStructure.SPI_Mode = SPI_Mode_Slave;

#endif

    SPI_InitStructure.SPI_DataSize = SPI_DataSize_8b;
    SPI_InitStructure.SPI_CPOL = SPI_CPOL_High;
    SPI_InitStructure.SPI_CPHA = SPI_CPHA_1Edge;
    SPI_InitStructure.SPI_NSS = SPI_NSS_Soft;
    // Tune CPU clock and derived prescaler to achieve desired '1' & '0' times for WS2812.
    // This setting was close enough for the Xtal controlled CH32V003 dev board.
    SPI_InitStructure.SPI_BaudRatePrescaler = SPI_BaudRatePrescaler_16;
    SPI_InitStructure.SPI_FirstBit = SPI_FirstBit_MSB;
    SPI_InitStructure.SPI_CRCPolynomial = 7;
    SPI_Init( SPI1, &SPI_InitStructure );

    NVIC_InitStructure.NVIC_IRQChannel = DMA1_Channel3_IRQn;
    NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 0;
    NVIC_InitStructure.NVIC_IRQChannelSubPriority = 1;
    NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
    NVIC_Init(&NVIC_InitStructure);

#if (SPI_MODE == SLAVE_MODE)
    SPI_I2S_ITConfig( SPI1, SPI_I2S_IT_RXNE , ENABLE );

#endif
    SPI_I2S_DMACmd(SPI1, SPI_I2S_DMAReq_Tx,ENABLE);
    SPI_Cmd( SPI1, ENABLE );
}
/*********************************************************************
 * @fn      DMA_Tx_Init
 *
 * @brief   Initializes the DMAy Channelx configuration.
 *
 * @param   DMA_CHx - x can be 1 to 7.
 *          ppadr - Peripheral base address.
 *          memadr - Memory base address.
 *          bufsize - DMA channel buffer size.
 *
 * @return  none
 *
 * AjG Note: Circular mode is used to continually output data.
 * 			 Load the bottom half of the buffer once DmaDone is complete.
 * 			 Load the top half of the buffer once DmaDone is half.
 * 			 Wait for one of the two above states while DmaDone is Busy.
 */
void DMA_Tx_Init(DMA_Channel_TypeDef *DMA_CHx, u32 ppadr, u32 memadr, u16 bufsize)
{
    DMA_InitTypeDef DMA_InitStructure = {0};

    RCC_AHBPeriphClockCmd(RCC_AHBPeriph_DMA1, ENABLE);

    DMA_DeInit(DMA_CHx);
    DMA_InitStructure.DMA_PeripheralBaseAddr = ppadr;
    DMA_InitStructure.DMA_MemoryBaseAddr = memadr;
    DMA_InitStructure.DMA_DIR = DMA_DIR_PeripheralDST;
    DMA_InitStructure.DMA_BufferSize = DMABUFNLEDS * BYTESPERLED;
    DMA_InitStructure.DMA_PeripheralInc = DMA_PeripheralInc_Disable;
    DMA_InitStructure.DMA_MemoryInc = DMA_MemoryInc_Enable;
    DMA_InitStructure.DMA_PeripheralDataSize = DMA_PeripheralDataSize_HalfWord;
    DMA_InitStructure.DMA_MemoryDataSize = DMA_MemoryDataSize_Byte;
    DMA_InitStructure.DMA_Mode = DMA_Mode_Circular;
    DMA_InitStructure.DMA_Priority = DMA_Priority_VeryHigh;
    DMA_InitStructure.DMA_M2M = DMA_M2M_Disable;
    DMA_Init(DMA_CHx, &DMA_InitStructure);
    DMA_ITConfig(DMA1_Channel3, DMA_IT_TC|DMA_IT_HT|DMA_IT_TE, ENABLE);
}
void clearDMABuf()
{
	u16 i;
	for (i= 0;i< BYTESPERLED * DMABUFNLEDS; i++)
		TxBuf[i] = 0;
}
/* setColor writes 4 bytes encoding the 8 bits per color using an index to a 256x4 table.
 * colorByte - amplitude for a color
 * pbits  - pointer to output byte array
*/
void setColor(u8 colorByte, u8 *pbits)
{
	u8 r;
	for(r = 0; r<BYTESPERCOLOR; r++)
	{
		*(pbits+r) =  BitTable[(colorByte*4) + r];
	}
}
/*
 * For one GRB led, encode the 12 bytes needed for 24 bits
 * pLed - pointer to output buffer
 * pcolor - pointer to GRB struct for color
 * amp  - amplitude modifier 0-255
 */
void setLed(u8 *pLed, GRB* pColor,u16 amp)
{
	u8 color = (pColor->g * amp)/256;
	setColor(color,pLed);
	pLed += BYTESPERCOLOR;
	color = (pColor->r * amp)/256;
	setColor(color,pLed);
	pLed += BYTESPERCOLOR;
	color = (pColor->b * amp)/256;
	setColor(color,pLed);
}
void setLedNoAmp(u8 *pLed, GRB* pColor)
{
	setColor(pColor->g,pLed);
	pLed += BYTESPERCOLOR;
	setColor(pColor->r,pLed);
	pLed += BYTESPERCOLOR;
	setColor(pColor->b,pLed);
}
/*
 * BuildColorBuf - Test routine to build a fixed color buffer
 */
#ifdef TEST
void BuildColorBuf(ColorWheel color, u8* pbuf,u16 nleds, u16 amp)
{
	u8 i;
	for(i =0; i<nleds; i++)
	{
		setLed(pbuf+i*BYTESPERLED, &(wheel[color]),amp);
	}
}
#endif
/*
 * BuildResetBuf fills the buffer pointed to by pbuf with zeros.
 * WS2812 GRB leds use a fixed '0' period to load serialized data into their internal display.kkkkkkkkkkkkkkk
 */
void BuildResetBuf(u8* pbuf, u16 nleds)
{
	u16 i;
	u16 n = nleds * BYTESPERLED;
	for(i =0; i<n; i++)
	{
		*(pbuf + i) = 0;
	}
}
/*
BuildMover1:
		color - color of pattern
		pbuf  - pointer to start of buf to write with GRB encode bits
		nleds - number of leds to write
		startled - the led number at the start of the DMA block
		phase  - the offset to start writing in absolute led number from zero to endOfString-1
		patternLength - the length of the pattern

	Cases:
	1. no residual pattern count ... no pattern elements written in this led string
		a. startled < phase < endDmaBlock = startled+nleds
			Write background color from beginning of DMA block to phase = startled count.
			Then write pattern until
			a1. end of DMA Block leaving a residual pattern count.
			    The remainder of the pattern will be written at the start of the next DMA
			a2. pattern runs out in the DMA block
			    finish block with bg color write
		b. startled > phase with patternCnt at max
			The pattern has already been written for the string.
			Write background color in entire dma block
	2. residual pattern count from previous dma block (Possibly from end of previous string write if wrapping patterns)
		Write residual pattern in dma block until:
		2a. end of block => residual count left in pattern
		2b. pattern exhausted before endblock
		    finish to end of block with bg color

*/

void BuildMover1(ColorWheel bgcolor, u8* pbuf,u16 nleds, u16 startled,u16 phase, u16 *ppatternCnt,u16 patternLength, const u8* ppattern)
{
u16 iled;

	for(iled = 0; iled < nleds; iled++)
	{
		 if(*ppatternCnt == 0) // no residuals
		 {
			if((startled+iled < phase) ) // write background
			{
				setLedNoAmp(pbuf+iled*BYTESPERLED, &(wheel[bgcolor])/*,0x100*/);
			}else  // start writing pattern
			{
				setLedNoAmp(pbuf+iled*BYTESPERLED, (GRB *)(ppattern+(BYTESPERPATTERN*(*ppatternCnt)++))/*, 0x100*/);
			}
		 }
		 else if(*ppatternCnt == patternLength) // pattern exhausted, write background
		 {
			setLedNoAmp(pbuf+iled*BYTESPERLED, &(wheel[bgcolor])/*,0x100*/);
		 }
		 else // pattern has started to be written. Continue, possibly into start of string
		{
			setLedNoAmp(pbuf+iled*BYTESPERLED, (GRB *)(ppattern+(BYTESPERPATTERN*(*ppatternCnt)++))/*, 0x100*/);
		}

	}
}
/*********************************************************************
 * @fn      main
 *
 * @brief   Main program.
 *
 * @return  none
 */

int main(void)
{
 u8 resetCnt = 0;
 volatile u16 phase = 0;
 u16 patCnt = 0;
 ColorWheel bgcolor = black;

 NVIC_PriorityGroupConfig(NVIC_PriorityGroup_1);

    SystemCoreClockUpdate();
    Delay_Init();

    clearDMABuf();
    DMA_Tx_Init(DMA1_Channel3, (u32)&SPI1->DATAR, (u32)TxBuf, (u16)(DMABUFNBYTES));
    DMA_Cmd(DMA1_Channel3, ENABLE);
    SPI_1Lines_HalfDuplex_Init();

    Delay_Ms(1000);

    while(1)
    {
		while(DmaDone == busy); // Here we have time to do other processing in a Super Loop

		// So ... managing the DMA is all that is done in this example.
#ifdef TEST
			GPIO_WriteBit(GPIOC,GPIO_Pin_3, Bit_SET); // Timing Test Scope trace
#endif
		if(DmaDone == half)
		{
			// Writing bottom half of buf, so fill top half
			if(resetCnt > 0) // ... in Reset cycles
			{
				// Reset time is the number of 1/2 dma buffers output with zeroes. This depends on SPI timing and DMA buffer size.
				 BuildResetBuf(TxBuf,DMABUFNLEDS/2);

			}else  // Not in Reset cycle yet ...
			{
				 if(offset == DMABUFSPERSTRING) // reset time, string is written
				 {
					 BuildResetBuf(TxBuf,DMABUFNLEDS/2);
					 resetCnt++;
				 }else // more DMABUF cycles to write to complete string
				 {
					 BuildMover1(bgcolor,TxBuf,DMABUFNLEDS/2, offset*DMABUFNLEDS,phase, &patCnt,peakPatSz, peakPat);
				 }
			}
		}
		else // DmaDone must be complete. DMA is writing top half, so fill the bottom half
		{
			if(resetCnt > 0)
			{
				BuildResetBuf(TxBuf+(DMABUFNBYTES/2),DMABUFNLEDS/2);
				if(++resetCnt == RESETCNTS) // Reset is done after bottom half is transferred.
				{
					resetCnt = 0;
					offset = 0;
					phase++;
					if(phase >= NLEDS)
					{
						phase = 0;
					}
					if(patCnt == peakPatSz)
					{
						patCnt = 0;
					}
				}
			}else
			{
				BuildMover1(bgcolor,TxBuf+(BYTESPERLED*DMABUFNLEDS/2),DMABUFNLEDS/2, offset*DMABUFNLEDS+DMABUFNLEDS/2,phase, &patCnt,peakPatSz, peakPat);
				offset++;  // update offset to indicate the position of the led at the start of the next DMA block
			}
		}
		DmaDone = busy;
#ifdef TEST
			GPIO_WriteBit(GPIOC,GPIO_Pin_3, Bit_RESET);
#endif
    }
}

void DMA1_Channel3_IRQHandler(void)__attribute__((interrupt("WCH-Interrupt-fast")));
void DMA1_Channel3_IRQHandler(void)
{
/*
*            DMA1_IT_GL3 - DMA1 Channel3 global flag.
*            DMA1_IT_TC3 - DMA1 Channel3 transfer complete flag.
*            DMA1_IT_HT3 - DMA1 Channel3 half transfer flag.
*            DMA1_IT_TE3 - DMA1 Channel3 transfer error flag.
*/
u32 flags = DMA1->INTFR;
	if(flags & DMA1_IT_GL3) // it Is this channel ...
	{
		if(flags & DMA1_IT_TE3) // DMA transfer error ... oh no!
		{
			while(1)
				{
			   		GPIO_WriteBit(GPIOC,GPIO_Pin_3, Bit_SET);   // toggle gpio to show dma error
			   		GPIO_WriteBit(GPIOC,GPIO_Pin_3, Bit_RESET);
				}
		}
		if(DmaDone != busy)
			DmaDone = busy;
		if(flags & DMA1_IT_HT3)
		{
			DmaDone = half;
		}
		else if(flags & DMA1_IT_TC3)
		{
			DmaDone = complete;
		}
		DMA_ClearITPendingBit(DMA1_IT_TC3|DMA1_IT_HT3|DMA1_IT_GL3);
	}else
	{
		// why are we here?
		while(1)
			{
		   		GPIO_WriteBit(GPIOC,GPIO_Pin_3, Bit_SET);   // toggle gpio to show false (?) interrupt
		   		GPIO_WriteBit(GPIOC,GPIO_Pin_3, Bit_RESET);
			}
	}
}

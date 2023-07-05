#include <stdio.h>
#include <string.h>

#include "stm32f4xx.h"

#include "SWD_host.h"
#include "SWD_flash.h"


#include "../doc/FlashAlgo/STM32F10x_128.c"
uint32_t Flash_Sect_Size = 1024;
uint32_t Flash_Page_Size = 1024;
uint32_t Flash_Start_Addr = 0x08000000;


#include "../doc/bin2array/STM32F103_demo.c"
#define target_bin	STM32F103_demo


uint8_t buff[1024] = {0};

void SerialInit(void);

int main(void)
{
	uint32_t val;
	
	SerialInit();
	
	swd_init_debug();
	
	swd_read_dp(0x00, &val);
	printf("\n\nIDCODE: %08X\n\n", val);
	
	target_flash_init(Flash_Start_Addr);
	
	for(uint32_t addr = 0; addr < sizeof(target_bin); addr += Flash_Sect_Size)
	{
		target_flash_erase_sector(Flash_Start_Addr + addr);
	}
	
	for(uint32_t addr = 0; addr < sizeof(target_bin); addr += 1024)
	{
		swd_read_memory(Flash_Start_Addr + addr, buff, 1024);
		for(uint32_t i = 0; i < 1024; i++) printf("%02X ", buff[i]);
		printf("\n\n\n");
	}
	
	for(uint32_t addr = 0; addr < sizeof(target_bin); addr += Flash_Page_Size)
	{
		target_flash_program_page(Flash_Start_Addr + addr, &target_bin[addr], 1024);
	}
	
	for(uint32_t addr = 0; addr < sizeof(target_bin); addr += 1024)
	{
		swd_read_memory(Flash_Start_Addr + addr, buff, 1024);
		for(uint32_t i = 0; i < 1024; i++) printf("%02X ", buff[i]);
		
		if(memcmp(&target_bin[addr], buff, sizeof(target_bin) - addr > 1024 ? 1024 : sizeof(target_bin) - addr) == 0)
			printf("\nPass\n\n\n");
		else
			printf("\nFail\n\n\n");
	}
	
	target_flash_uninit();
	
	while(1)
	{
	}
}


void SerialInit(void)
{
	GPIO_InitTypeDef  GPIO_initStruct;
	USART_InitTypeDef USART_initStruct;
	
	RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOA, ENABLE);
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_USART1, ENABLE);
	
	GPIO_initStruct.GPIO_Pin = GPIO_Pin_9 | GPIO_Pin_10;
	GPIO_initStruct.GPIO_Mode = GPIO_Mode_AF;
	GPIO_initStruct.GPIO_PuPd = GPIO_PuPd_NOPULL;
	GPIO_initStruct.GPIO_OType = GPIO_OType_PP;
	GPIO_initStruct.GPIO_Speed = GPIO_Fast_Speed;
	GPIO_Init(GPIOA, &GPIO_initStruct);
	
	GPIO_PinAFConfig(GPIOA, GPIO_PinSource9, GPIO_AF_USART1);	// USART1_TX
	GPIO_PinAFConfig(GPIOA, GPIO_PinSource10, GPIO_AF_USART1);	// USART1_RX

	USART_initStruct.USART_BaudRate = 115200;
	USART_initStruct.USART_WordLength = USART_WordLength_8b;
	USART_initStruct.USART_StopBits = USART_StopBits_1;
	USART_initStruct.USART_Parity = USART_Parity_No;
	USART_initStruct.USART_Mode = USART_Mode_Rx | USART_Mode_Tx;
	USART_initStruct.USART_HardwareFlowControl = USART_HardwareFlowControl_None;
	USART_Init(USART1, &USART_initStruct);
	USART_Cmd(USART1, ENABLE);
}

/****************************************************************************************************************************************** 
* 函数名称:	fputc()
* 功能说明: printf()底层输出函数
* 输    入: int ch	要输出的字符
*			FILE *f
* 输    出: 无
* 注意事项: 无
******************************************************************************************************************************************/
int fputc(int ch, FILE *f)
{
	while(USART_GetFlagStatus(USART1, USART_FLAG_TXE) == RESET);
	USART_SendData(USART1, ch);
	
	return ch;
}

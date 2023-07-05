#ifndef __DAP_CONFIG_H__
#define __DAP_CONFIG_H__


#define CPU_CLOCK               168000000       ///< Specifies the CPU Clock in Hz


#define IO_PORT_WRITE_CYCLES    2               ///< I/O Cycles: 2=default, 1=Cortex-M0+ fast I/0


#define DAP_SWD                 1               ///< SWD Mode:  1 = available, 0 = not available

#define DAP_JTAG                0               ///< JTAG Mode: 0 = not available

#define DAP_JTAG_DEV_CNT        8               ///< Maximum number of JTAG devices on scan chain

#define DAP_DEFAULT_PORT        1               ///< Default JTAG/SWJ Port Mode: 1 = SWD, 2 = JTAG.

#define DAP_DEFAULT_SWJ_CLOCK   4000000         ///< Default SWD/JTAG clock frequency in Hz.


/// Maximum Package Size for Command and Response data.
#define DAP_PACKET_SIZE         64              ///< USB: 64 = Full-Speed, 1024 = High-Speed.

/// Maximum Package Buffers for Command and Response data.
#define DAP_PACKET_COUNT        1              ///< Buffers: 64 = Full-Speed, 4 = High-Speed.


/// Indicate that UART Serial Wire Output (SWO) trace is available.
#define SWO_UART                0               ///< SWO UART:  1 = available, 0 = not available

#define SWO_UART_MAX_BAUDRATE   10000000U       ///< SWO UART Maximum Baudrate in Hz

/// Indicate that Manchester Serial Wire Output (SWO) trace is available.
#define SWO_MANCHESTER          0               ///< SWO Manchester:  1 = available, 0 = not available

#define SWO_BUFFER_SIZE         4096U           ///< SWO Trace Buffer Size in bytes (must be 2^n)


/// Debug Unit is connected to fixed Target Device.
#define TARGET_DEVICE_FIXED     0               ///< Target Device: 1 = known, 0 = unknown;



//**************************************************************************************************
/**
JTAG I/O Pin                 | SWD I/O Pin          | CMSIS-DAP Hardware pin mode
---------------------------- | -------------------- | ---------------------------------------------
TCK: Test Clock              | SWCLK: Clock         | Output Push/Pull
TMS: Test Mode Select        | SWDIO: Data I/O      | Output Push/Pull; Input (for receiving data)
TDI: Test Data Input         |                      | Output Push/Pull
TDO: Test Data Output        |                      | Input
nTRST: Test Reset (optional) |                      | Output Open Drain with pull-up resistor
nRESET: Device Reset         | nRESET: Device Reset | Output Open Drain with pull-up resistor

DAP Hardware I/O Pin Access Functions
*/
#include "stm32f4xx.h"


// Configure DAP I/O pins ------------------------------

#define SWCLK_PORT			GPIOA
#define SWCLK_PIN  			GPIO_Pin_0
#define SWDIO_PORT			GPIOA
#define SWDIO_PIN  			GPIO_Pin_1
#define SWDIO_PIN_INDEX		1

#define nRST_PORT			GPIOA
#define nRST_PIN			GPIO_Pin_2

#define LED_CONNECTED_PORT  GPIOA
#define LED_CONNECTED_PIN   GPIO_Pin_3
#define LED_RUNNING_PORT    GPIOA
#define LED_RUNNING_PIN     GPIO_Pin_4


/** Setup JTAG I/O pins: TCK, TMS, TDI, TDO, nTRST, and nRESET.
 - TCK, TMS, TDI, nTRST, nRESET to output mode and set to high level.
 - TDO to input mode.
*/
static void PORT_JTAG_SETUP(void)
{
#if (DAP_JTAG != 0)
#endif
}

/** Setup SWD I/O pins: SWCLK, SWDIO, and nRESET.
 - SWCLK, SWDIO, nRESET to output mode and set to default high level.
*/
static void PORT_SWD_SETUP(void)
{
    GPIO_InitTypeDef GPIO_initStruct;
	
	SWCLK_PORT->BSRRL = SWCLK_PIN;
	SWDIO_PORT->BSRRL = SWDIO_PIN;
	
	GPIO_initStruct.GPIO_Pin = SWCLK_PIN;
	GPIO_initStruct.GPIO_Mode = GPIO_Mode_OUT;
	GPIO_initStruct.GPIO_PuPd = GPIO_PuPd_NOPULL;
	GPIO_initStruct.GPIO_OType = GPIO_OType_PP;
	GPIO_initStruct.GPIO_Speed = GPIO_Fast_Speed;
	GPIO_Init(SWCLK_PORT, &GPIO_initStruct);
	
	GPIO_initStruct.GPIO_Pin = SWDIO_PIN;
	GPIO_initStruct.GPIO_PuPd = GPIO_PuPd_UP;
	GPIO_Init(SWDIO_PORT, &GPIO_initStruct);
}

/** Disable JTAG/SWD I/O Pins.
 - TCK/SWCLK, TMS/SWDIO, TDI, TDO, nTRST, nRESET to High-Z mode.
*/
static void PORT_OFF(void)
{
	GPIO_InitTypeDef GPIO_initStruct;
	
	RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOA, ENABLE);
	RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOB, ENABLE);
	
	GPIO_initStruct.GPIO_Pin = SWCLK_PIN;
	GPIO_initStruct.GPIO_Mode = GPIO_Mode_IN;
	GPIO_initStruct.GPIO_PuPd = GPIO_PuPd_NOPULL;
	GPIO_initStruct.GPIO_OType = GPIO_OType_PP;
	GPIO_initStruct.GPIO_Speed = GPIO_Fast_Speed;
	GPIO_Init(SWCLK_PORT, &GPIO_initStruct);
	
	GPIO_initStruct.GPIO_Pin = SWDIO_PIN;
	GPIO_Init(SWDIO_PORT, &GPIO_initStruct);
}


// SWCLK/TCK I/O pin -------------------------------------

// Current status of the SWCLK/TCK DAP hardware I/O pin
static __inline uint32_t PIN_SWCLK_TCK_IN(void)
{
    return (SWCLK_PORT->IDR & SWCLK_PIN) ? 1 : 0;
}

static __inline void PIN_SWCLK_TCK_SET(void)
{
	SWCLK_PORT->BSRRL = SWCLK_PIN;
}

static __inline void PIN_SWCLK_TCK_CLR(void)
{
    SWCLK_PORT->BSRRH = SWCLK_PIN;
}


// SWDIO/TMS Pin I/O --------------------------------------

// Current status of the SWDIO/TMS DAP hardware I/O pin
static __inline uint32_t PIN_SWDIO_TMS_IN(void)
{
    return (SWDIO_PORT->IDR & SWDIO_PIN) ? 1 : 0;
}

static __inline void PIN_SWDIO_TMS_SET(void)
{
    SWDIO_PORT->BSRRL = SWDIO_PIN;
}

static __inline void PIN_SWDIO_TMS_CLR(void)
{
    SWDIO_PORT->BSRRH = SWDIO_PIN;
}


static __inline uint32_t PIN_SWDIO_IN(void)
{
    return (SWDIO_PORT->IDR & SWDIO_PIN) ? 1 : 0;
}

static __inline void PIN_SWDIO_OUT(uint32_t bit)
{
    if(bit & 1) SWDIO_PORT->BSRRL = SWDIO_PIN;
    else        SWDIO_PORT->BSRRH = SWDIO_PIN;
}

static __inline void PIN_SWDIO_OUT_ENABLE(void)
{
	SWDIO_PORT->BSRRH = SWDIO_PIN;
	
	SWDIO_PORT->MODER |=  (GPIO_Mode_OUT << (SWDIO_PIN_INDEX * 2));
}

static __inline void PIN_SWDIO_OUT_DISABLE(void)
{
    SWDIO_PORT->MODER &= ~(3             << (SWDIO_PIN_INDEX * 2));
}


// TDI Pin I/O ---------------------------------------------

static __inline uint32_t PIN_TDI_IN(void)
{
#if (DAP_JTAG != 0)
#endif
	return 0;
}

static __inline void PIN_TDI_OUT(uint32_t bit)
{
#if (DAP_JTAG != 0)
#endif
}


// TDO Pin I/O ---------------------------------------------

static __inline uint32_t PIN_TDO_IN(void)
{
#if (DAP_JTAG != 0)
#endif
	return 0;
}


// nTRST Pin I/O -------------------------------------------

static __inline uint32_t PIN_nTRST_IN(void)
{
    return 0;
}

static __inline void PIN_nTRST_OUT(uint32_t bit)
{
}

// nRESET Pin I/O------------------------------------------
static __inline uint32_t PIN_nRESET_IN(void)
{
    return (nRST_PORT->IDR & nRST_PIN) ? 1 : 0;
}

extern uint8_t swd_write_word(uint32_t addr, uint32_t val);
static __inline void PIN_nRESET_OUT(uint32_t bit)
{
	if(bit & 1) nRST_PORT->BSRRL = nRST_PIN;
    else        nRST_PORT->BSRRH = nRST_PIN;
	
	if(bit == 0)
    {
        swd_write_word((uint32_t)&SCB->AIRCR, ((0x5FA << SCB_AIRCR_VECTKEY_Pos) | SCB_AIRCR_SYSRESETREQ_Msk));
    }
}


//**************************************************************************************************
/** Connect LED: is active when the DAP hardware is connected to a debugger
    Running LED: is active when program execution in target started
*/

static __inline void LED_CONNECTED_OUT(uint32_t bit)
{
	if(bit & 1) LED_CONNECTED_PORT->BSRRL = LED_CONNECTED_PIN;
    else        LED_CONNECTED_PORT->BSRRH = LED_CONNECTED_PIN;
}

static __inline void LED_RUNNING_OUT(uint32_t bit)
{
	if(bit & 1) LED_RUNNING_PORT->BSRRL = LED_RUNNING_PIN;
    else        LED_RUNNING_PORT->BSRRH = LED_RUNNING_PIN;
}



static void DAP_SETUP(void)
{
	PORT_OFF();
	
	GPIO_InitTypeDef GPIO_initStruct;
	
	GPIO_initStruct.GPIO_Pin = LED_CONNECTED_PIN;
	GPIO_initStruct.GPIO_Mode = GPIO_Mode_OUT;
	GPIO_initStruct.GPIO_PuPd = GPIO_PuPd_NOPULL;
	GPIO_initStruct.GPIO_OType = GPIO_OType_PP;
	GPIO_initStruct.GPIO_Speed = GPIO_Fast_Speed;
	GPIO_Init(LED_CONNECTED_PORT, &GPIO_initStruct);
	
	GPIO_initStruct.GPIO_Pin = LED_RUNNING_PIN;
	GPIO_Init(LED_RUNNING_PORT, &GPIO_initStruct);
	
	nRST_PORT->BSRRL = nRST_PIN;
	
	GPIO_initStruct.GPIO_Pin = nRST_PIN;
	GPIO_initStruct.GPIO_OType = GPIO_OType_OD;
	GPIO_Init(nRST_PORT, &GPIO_initStruct);
}


static uint32_t RESET_TARGET(void)
{
    return (0);              // change to '1' when a device reset sequence is implemented
}


#endif // __DAP_CONFIG_H__

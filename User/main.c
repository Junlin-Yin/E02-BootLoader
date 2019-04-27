//=======================================Copyright(c)===========================================
// 											Auto Code Making Company
//											Auto Code Department
//												CopyRight Private
//-------------------------------------------------FileMessage------------------------------------
//FileName	  main.c
//Author	 yanggang
//Creation Time

//Description
//-----------------------------------------------当前版本修订----------------------------------
//修改人
//版	本
//修改日期
//Description
//=============================================================================================

/*******************************************************************************
** 文件名: 		main.c
** 功能:		USART初始化和RCC设置，然后从common.c中执行主菜单
** 相关文件:	stm32f10x.h
*******************************************************************************/

/* 包含头文件 *****************************************************************/
#include "common.h"
#include "stdio.h"
/* 类型声明 ------------------------------------------------------------------*/
/* 宏 ------------------------------------------------------------------------*/
#define LED0 GPIO_Pin_0
#define LED1 GPIO_Pin_1
#define LED2 GPIO_Pin_2
#define LED3 GPIO_Pin_3
/* 变量 ----------------------------------------------------------------------*/
typedef  void (*pFunction)(void);

/* 函数声明 ------------------------------------------------------------------*/
void Delay( __IO uint32_t nCount );
void LED_Configuration( void );
static void IAP_Init( void );
void KEY_Configuration( void );
void GPIO_Configuration( void );
void USART_Configuration( void );
/* 函数功能 ------------------------------------------------------------------*/

/*******************************************************************************
  * @函数名称	main
  * @函数说明   主函数
  * @输入参数   无
  * @输出参数   无
  * @返回参数   无
*******************************************************************************/
int fputc( int c, FILE *stream )
{
	USART1->DR = c;
	while ( !( USART1->SR & ( 1 << 7 ) ) ) {};
	return c;
}

int fgetc( FILE *stream )
{
	while ( !( USART1->SR & ( 1 << 5 ) ) ) {};
	return USART1->DR;
}

int main( void )
{
	uint32_t address, data;
	uint32_t JumpAddress;
	pFunction Jump_To_Application;
	char uchar[ 15 ] = { 0 };
	uint32_t vaildAddressDown = 0x20000000;
	uint32_t vaildAddressUp = 0x20000000 + 0x5000;

	LED_Configuration();
	IAP_Init();
	printf( "Peek and poke test\r\n" );

	while ( 1 )
	{
		printf( "Please input command: " );
		scanf( "%s", uchar );
		printf( "%s\r\n", uchar );

		if ( strcmp( uchar, "peek" ) == 0 )
		{
			printf( "Please input address: " );
			scanf( "%x", &address );
			printf( "%x\r\n", address );
			printf( "Peek result: 0x%08x\r\n", *( (uint32_t *)address ) );
		}
		else if ( strcmp( uchar, "poke" ) == 0 )
		{
			printf( "Please input address: " );
			scanf( "%x", &address );
			printf( "%x\r\n", address );
			if ( address < vaildAddressDown || address > vaildAddressUp )
			{
				printf( "Invaild address\r\n" );
				continue;
			}
			printf( "Please input data: " );
			scanf( "%x", &data );
			printf( "%x\r\n", data );
			*( (uint32_t *)address ) = data;
			printf( "Poke success\r\n" );
		}
		else if ( strcmp( uchar, "load" ) == 0 )
		{
			printf( "Please input address: " );
			scanf( "%x", &address );
			printf( "\r\n" );
			if ( address < vaildAddressDown || address > vaildAddressUp )
			{
				printf( "Invaild address\r\n" );
				continue;
			}
			SerialDownload(address);
		}
		else if ( strcmp( uchar, "run" ) == 0 )
		{
			printf( "Please input address: " );
			scanf( "%x", &address );
			printf( "\r\n\n" );
			JumpAddress = *(__IO uint32_t *)( address+4 );

			//跳转到用户程序
			Jump_To_Application = (pFunction)JumpAddress;
			//初始化用户程序的堆栈指针
			__set_MSP( *(__IO uint32_t *)address );
			Jump_To_Application();
		}
		else
		{
			printf( "Invaild command\r\n" );
		}
	}
}

/*******************************************************************************
  * @函数名称	LED_Configuration
  * @函数说明   配置使用LED
  * @输入参数   无
  * @输出参数   无
  * @返回参数   无
*******************************************************************************/
void LED_Configuration( void )
{
	GPIO_InitTypeDef GPIO_InitStructure;
	//使能LED所在GPIO的时钟
	RCC_APB2PeriphClockCmd( RCC_APB2Periph_GPIOE, ENABLE );
	//初始化LED的GPIO
	GPIO_InitStructure.GPIO_Pin = LED0 | LED1 | LED2 | LED3;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_Init( GPIOE, &GPIO_InitStructure );
	GPIO_SetBits( GPIOE, LED2 | LED3 );  //熄灭LED0-3
}

/*******************************************************************************
  * @函数名称	KEY_Configuration
  * @函数说明   按键初始化
  * @输入参数   无
  * @输出参数   无
  * @返回参数   无
*******************************************************************************/
void KEY_Configuration( void )
{
	GPIO_InitTypeDef GPIO_InitStructure;
	RCC_APB2PeriphClockCmd( RCC_APB2Periph_GPIOC, ENABLE );

	//配置按键
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IPU;
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_5;
	GPIO_Init( GPIOC, &GPIO_InitStructure );
}

/*******************************************************************************
  * @函数名称	GPIO_Configuration
  * @函数说明   配置使用USART1的相关IO管脚
  * @输入参数   无
  * @输出参数   无
  * @返回参数   无
*******************************************************************************/
void GPIO_Configuration( void )
{
	GPIO_InitTypeDef GPIO_InitStructure;

	RCC_APB2PeriphClockCmd( RCC_APB2Periph_USART1 | RCC_APB2Periph_GPIOA, ENABLE );
	// 配置 USART1 Tx (PA.09) 作为功能引脚并上拉输出模式
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_9;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_Init( GPIOA, &GPIO_InitStructure );

	//配置 USART1 Tx (PA.10) 作为功能引脚并是浮空输入模式
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_10;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN_FLOATING;
	GPIO_Init( GPIOA, &GPIO_InitStructure );
}

/*******************************************************************************
  * @函数名称	IAP_Init
  * @函数说明   配置使用IAP
  * @输入参数   无
  * @输出参数   无
  * @返回参数   无
*******************************************************************************/
void IAP_Init( void )
{
	USART_InitTypeDef USART_InitStructure;

	/* USART1 配置 ------------------------------------------------------------
         USART1 配置如下:
          - 波特率      = 115200 baud
          - 字长        = 8 Bits
          - 一个停止位
          - 无校验
          - 无硬件流控制
          - 接受和发送使能
    --------------------------------------------------------------------------*/
	USART_InitStructure.USART_BaudRate = 9600;
	USART_InitStructure.USART_WordLength = USART_WordLength_8b;
	USART_InitStructure.USART_StopBits = USART_StopBits_1;
	USART_InitStructure.USART_Parity = USART_Parity_No;
	USART_InitStructure.USART_HardwareFlowControl = USART_HardwareFlowControl_None;
	USART_InitStructure.USART_Mode = USART_Mode_Rx | USART_Mode_Tx;

	GPIO_Configuration();
	USART_Init( USART1, &USART_InitStructure );
	// 使能 USART1
	USART_Cmd( USART1, ENABLE );
}

/*******************************************************************************
  * @函数名称	Delay
  * @函数说明   插入一段延时时间
  * @输入参数   nCount: 指定延时时间长度
  * @输出参数   无
  * @返回参数   无
*******************************************************************************/
void Delay( __IO uint32_t nCount )
{
	for ( ; nCount != 0; nCount-- )
		;
}

#ifdef USE_FULL_ASSERT

/*******************************************************************************
  * @函数名称	assert_failed
  * @函数说明   报告在检查参数发生错误时的源文件名和错误行数
  * @输入参数   file: 源文件名
  				line: 错误所在行数
  * @输出参数   无
  * @返回参数   无
*******************************************************************************/
void assert_failed( uint8_t *file, uint32_t line )
{
	/* 用户可以增加自己的代码用于报告错误的文件名和所在行数,
       例如：printf("错误参数值: 文件名 %s 在 %d行\r\n", file, line) */

	//死循环
	while ( 1 )
	{
	}
}
#endif

/*******************************文件结束***************************************/

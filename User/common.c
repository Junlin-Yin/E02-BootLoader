/*******************************************************************************
** 文件名: 		common.c
** 功能:		显示主菜单。主菜单上显示一系列操作，如加载二进制文件、
                执行应用程序以及禁止写保护(如果事先Flash被写保护)。
** 相关文件:	common.h，ymodem.h

*******************************************************************************/

/* 包含头文件 *****************************************************************/
#include "common.h"
#include "ymodem.h"

uint8_t tab_1024[ 1024 ] = {0};

/*******************************************************************************
  * @函数名称	SerialKeyPressed
  * @函数说明   测试超级终端是否有按键按下
  * @输入参数   key:按键
  * @输出参数   无
  * @返回参数   1：正确
                0：错误
*******************************************************************************/
uint32_t SerialKeyPressed( uint8_t *key )
{
	if ( USART_GetFlagStatus( USART1, USART_FLAG_RXNE ) != RESET )
	{
		*key = (uint8_t)USART1->DR;
		return 1;
	}
	else
	{
		return 0;
	}
}

/*******************************************************************************
  * @函数名称	SerialPutChar
  * @函数说明   串口发送一个字符
  * @输入参数   C:需发送的字符
  * @输出参数   i无
  * @返回参数   无
*******************************************************************************/
void SerialPutChar( uint8_t c )
{
	USART_SendData( USART1, c );
	while ( USART_GetFlagStatus( USART1, USART_FLAG_TXE ) == RESET )
	{
	}
}

/*******************************************************************************
  * @函数名称	SerialPutString
  * @函数说明   串口发送一个字符串
  * @输入参数   *s:需发送的字符串
  * @输出参数   无
  * @返回参数   无
*******************************************************************************/
void Serial_PutString( uint8_t *s )
{
	while ( *s != '\0' )
	{
		SerialPutChar( *s );
		s++;
	}
}

/*******************************************************************************
  * @函数名称	SerialDownload
  * @函数说明   通过串口接收一个文件
  * @输入参数   无
  * @输出参数   无
  * @返回参数   无
*******************************************************************************/
void SerialDownload( uint32_t address )
{
	int32_t Size = 0;

	SerialPutString( "Waiting for the file to be sent ... (press 'a' to abort)\n\r" );
	
	Size = Ymodem_Receive( &tab_1024[0] , address);
	if ( Size > 0 )
	{
		SerialPutString( "\n\n\r Programming Completed Successfully!\n\r--------------------------------\r\n" );
	}
	else if ( Size == -1 )
	{
		SerialPutString( "\n\n\rThe image size is higher than the allowed space memory!\n\r" );
	}
	else if ( Size == -2 )
	{
		SerialPutString( "\n\n\rVerification failed!\n\r" );
	}
	else if ( Size == -3 )
	{
		SerialPutString( "\r\n\nAborted by user.\n\r" );
	}
	else
	{
		SerialPutString( "\n\rFailed to receive the file!\n\r" );
	}
}

/*******************************文件结束***************************************/

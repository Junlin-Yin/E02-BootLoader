/*******************************************************************************
** 文件名: 		download.c
** 功能:		等待用户选择传送文件操作,或者放弃操作以及一些提示信息，
                但真正实现传送的是ymodem．c源文件。
** 相关文件:	common.h
*******************************************************************************/

/* 包含头文件 *****************************************************************/
#include "common.h"

/* 变量声明 ------------------------------------------------------------------*/
extern uint8_t file_name[ FILE_NAME_LENGTH ];
uint8_t tab_1024[ 1024 ] =
  {
	  0
  };

/*******************************************************************************
  * @函数名称	SerialDownload
  * @函数说明   通过串口接收一个文件
  * @输入参数   无
  * @输出参数   无
  * @返回参数   无
*******************************************************************************/
void SerialDownload( void )
{
	uint8_t Number[ 10 ] = "          ";
	int32_t Size = 0;
	//uint8_t * buf = (uint8_t *)0x20001000;
	uint32_t address = 0x20001000;

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

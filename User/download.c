/*******************************************************************************
** �ļ���: 		download.c
** ����:		�ȴ��û�ѡ�����ļ�����,���߷��������Լ�һЩ��ʾ��Ϣ��
                ������ʵ�ִ��͵���ymodem��cԴ�ļ���
** ����ļ�:	common.h
*******************************************************************************/

/* ����ͷ�ļ� *****************************************************************/
#include "common.h"

/* �������� ------------------------------------------------------------------*/
extern uint8_t file_name[ FILE_NAME_LENGTH ];
uint8_t tab_1024[ 1024 ] =
  {
	  0
  };

/*******************************************************************************
  * @��������	SerialDownload
  * @����˵��   ͨ�����ڽ���һ���ļ�
  * @�������   ��
  * @�������   ��
  * @���ز���   ��
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

/*******************************�ļ�����***************************************/

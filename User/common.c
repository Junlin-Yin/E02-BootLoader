/*******************************************************************************
** �ļ���: 		common.c
** ����:		��ʾ���˵������˵�����ʾһϵ�в���������ض������ļ���
                ִ��Ӧ�ó����Լ���ֹд����(�������Flash��д����)��
** ����ļ�:	common.h��ymodem.h

*******************************************************************************/

/* ����ͷ�ļ� *****************************************************************/
#include "common.h"
#include "ymodem.h"

uint8_t tab_1024[ 1024 ] = {0};

/*******************************************************************************
  * @��������	SerialKeyPressed
  * @����˵��   ���Գ����ն��Ƿ��а�������
  * @�������   key:����
  * @�������   ��
  * @���ز���   1����ȷ
                0������
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
  * @��������	SerialPutChar
  * @����˵��   ���ڷ���һ���ַ�
  * @�������   C:�跢�͵��ַ�
  * @�������   i��
  * @���ز���   ��
*******************************************************************************/
void SerialPutChar( uint8_t c )
{
	USART_SendData( USART1, c );
	while ( USART_GetFlagStatus( USART1, USART_FLAG_TXE ) == RESET )
	{
	}
}

/*******************************************************************************
  * @��������	SerialPutString
  * @����˵��   ���ڷ���һ���ַ���
  * @�������   *s:�跢�͵��ַ���
  * @�������   ��
  * @���ز���   ��
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
  * @��������	SerialDownload
  * @����˵��   ͨ�����ڽ���һ���ļ�
  * @�������   ��
  * @�������   ��
  * @���ز���   ��
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

/*******************************�ļ�����***************************************/

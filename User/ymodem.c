/*******************************************************************************
** 文件名: 		ymodem.c
** 功能:		和Ymodem.c的相关的协议文件
                负责从超级终端接收数据(使用Ymodem协议)，并将数据加载到内部RAM中。
                如果接收数据正常，则将数据编程到Flash中；如果发生错误，则提示出错。
** 相关文件:	stm32f10x.h
*******************************************************************************/

/* 包含头文件 *****************************************************************/

#include "common.h"
#include "stm32f10x_flash.h"

/*******************************************************************************
  * @函数名称	Receive_Byte
  * @函数说明   从发送端接收一个字节
  * @输入参数   c: 接收字符
                timeout: 超时时间
  * @输出参数   无
  * @返回参数   接收的结果
                0：成功接收
                1：时间超时
*******************************************************************************/
static int32_t Receive_Byte( uint8_t *c, uint32_t timeout )
{
	while ( timeout-- > 0 )
	{
		if ( SerialKeyPressed( c ) == 1 )
		{
			return 0;
		}
	}
	return -1;
}

/*******************************************************************************
  * @函数名称	Send_Byte
  * @函数说明   发送一个字符
  * @输入参数   c: 发送的字符
  * @输出参数   无
  * @返回参数   发送的结果
                0：成功发送
*******************************************************************************/
static uint32_t Send_Byte( uint8_t c )
{
	SerialPutChar( c );
	return 0;
}

/*******************************************************************************
  * @函数名称	Receive_Packet
  * @函数说明   从发送端接收一个数据包
  * @输入参数   data ：数据指针
                length：长度
                timeout ：超时时间
  * @输出参数   无
  * @返回参数   接收的结果
                0: 正常返回
                -1: 超时或者数据包错误
                1: 用户取消
*******************************************************************************/
static int32_t Receive_Packet( uint8_t *data, int32_t *length, uint32_t timeout )
{
	uint16_t i, packet_size;
	uint8_t c;
	*length = 0;
	if ( Receive_Byte( &c, timeout ) != 0 )
	{
		return -1;
	}
	switch ( c )
	{
	case SOH:
		packet_size = PACKET_SIZE;
		break;
	case STX:
		packet_size = PACKET_1K_SIZE;
		break;
	case EOT:
		return 0;
	case CA:
		if ( ( Receive_Byte( &c, timeout ) == 0 ) && ( c == CA ) )
		{
			*length = -1;
			return 0;
		}
		else
		{
			return -1;
		}
	case ABORT1:
	case ABORT2:
		return 1;
	default:
		return -1;
	}
	*data = c;
	for ( i = 1; i < ( packet_size + PACKET_OVERHEAD ); i++ )
	{
		if ( Receive_Byte( data + i, timeout ) != 0 )
		{
			return -1;
		}
	}
	if ( data[ PACKET_SEQNO_INDEX ] != ( ( data[ PACKET_SEQNO_COMP_INDEX ] ^ 0xff ) & 0xff ) )
	{
		return -1;
	}
	*length = packet_size;
	return 0;
}

/*******************************************************************************
  * @函数名称	Ymodem_Receive
  * @函数说明   通过 ymodem协议接收一个文件
  * @输入参数   buf: 首地址指针
  * @输出参数   无
  * @返回参数   文件长度
*******************************************************************************/
int32_t Ymodem_Receive( uint8_t * buf , uint32_t address)
{
	uint8_t packet_data[ PACKET_1K_SIZE + PACKET_OVERHEAD ], *ptr;
	int32_t packet_length, session_done, file_done, packets_received, errors, session_begin;

	 buf = (uint8_t *)address;

	//uint8_t * buf = (uint8_t *)0x20001000;
	for ( session_done = 0, errors = 0, session_begin = 0;; )
	{
		for ( packets_received = 0, file_done = 0, ptr = buf;; )
		{
			switch ( Receive_Packet( packet_data, &packet_length, NAK_TIMEOUT ) )
			{
			case 0:
				errors = 0;
				switch ( packet_length )
				{
					//发送端终止
				case -1:
					Send_Byte( ACK );
					return 0;
					//结束传输
				case 0:
					Send_Byte( ACK );
					file_done = 1;
					break;
					//正常的数据包
				default:
					if ( ( packet_data[ PACKET_SEQNO_INDEX ] & 0xff ) != ( packets_received & 0xff ) )
					{
						Send_Byte( NAK );
					}
					else
					{
						if ( packets_received == 0 )
						{
							//文件名数据包
							if ( packet_data[ PACKET_HEADER ] != 0 )
							{
								Send_Byte( ACK );
								Send_Byte( CRC16 );
							}
							//文件名数据包空，结束传输
							else
							{
								Send_Byte( ACK );
								file_done = 1;
								session_done = 1;
								break;
							}
						}
						//数据包
						else
						{
							memcpy( (uint32_t *)ptr, packet_data + PACKET_HEADER, packet_length );
							ptr += packet_length;
							Send_Byte( ACK );
						}
						packets_received++;
						session_begin = 1;
					}
				}
				break;
			case 1:
				Send_Byte( CA );
				Send_Byte( CA );
				return -3;
			default:
				if ( session_begin > 0 )
				{
					errors++;
				}
				if ( errors > MAX_ERRORS )
				{
					Send_Byte( CA );
					Send_Byte( CA );
					return 0;
				}
				Send_Byte( CRC16 );
				break;
			}
			if ( file_done != 0 )
			{
				break;
			}
		}
		if ( session_done != 0 )
		{
			break;
		}
	}
	return 1;
}

/*******************************文件结束***************************************/

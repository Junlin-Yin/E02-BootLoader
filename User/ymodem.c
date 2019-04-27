/*******************************************************************************
** �ļ���: 		ymodem.c
** ����:		��Ymodem.c����ص�Э���ļ�
                ����ӳ����ն˽�������(ʹ��YmodemЭ��)���������ݼ��ص��ڲ�RAM�С�
                ����������������������ݱ�̵�Flash�У����������������ʾ����
** ����ļ�:	stm32f10x.h
*******************************************************************************/

/* ����ͷ�ļ� *****************************************************************/

#include "common.h"
#include "stm32f10x_flash.h"

/*******************************************************************************
  * @��������	Receive_Byte
  * @����˵��   �ӷ��Ͷ˽���һ���ֽ�
  * @�������   c: �����ַ�
                timeout: ��ʱʱ��
  * @�������   ��
  * @���ز���   ���յĽ��
                0���ɹ�����
                1��ʱ�䳬ʱ
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
  * @��������	Send_Byte
  * @����˵��   ����һ���ַ�
  * @�������   c: ���͵��ַ�
  * @�������   ��
  * @���ز���   ���͵Ľ��
                0���ɹ�����
*******************************************************************************/
static uint32_t Send_Byte( uint8_t c )
{
	SerialPutChar( c );
	return 0;
}

/*******************************************************************************
  * @��������	Receive_Packet
  * @����˵��   �ӷ��Ͷ˽���һ�����ݰ�
  * @�������   data ������ָ��
                length������
                timeout ����ʱʱ��
  * @�������   ��
  * @���ز���   ���յĽ��
                0: ��������
                -1: ��ʱ�������ݰ�����
                1: �û�ȡ��
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
  * @��������	Ymodem_Receive
  * @����˵��   ͨ�� ymodemЭ�����һ���ļ�
  * @�������   buf: �׵�ַָ��
  * @�������   ��
  * @���ز���   �ļ�����
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
					//���Ͷ���ֹ
				case -1:
					Send_Byte( ACK );
					return 0;
					//��������
				case 0:
					Send_Byte( ACK );
					file_done = 1;
					break;
					//���������ݰ�
				default:
					if ( ( packet_data[ PACKET_SEQNO_INDEX ] & 0xff ) != ( packets_received & 0xff ) )
					{
						Send_Byte( NAK );
					}
					else
					{
						if ( packets_received == 0 )
						{
							//�ļ������ݰ�
							if ( packet_data[ PACKET_HEADER ] != 0 )
							{
								Send_Byte( ACK );
								Send_Byte( CRC16 );
							}
							//�ļ������ݰ��գ���������
							else
							{
								Send_Byte( ACK );
								file_done = 1;
								session_done = 1;
								break;
							}
						}
						//���ݰ�
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

/*******************************�ļ�����***************************************/

//=======================================Copyright(c)===========================================
// 											Auto Code Making Company
//											Auto Code Department
//												CopyRight Private
//-------------------------------------------------FileMessage------------------------------------
//FileName	  main.c
//Author	 yanggang
//Creation Time

//Description
//-----------------------------------------------��ǰ�汾�޶�----------------------------------
//�޸���
//��	��
//�޸�����
//Description
//=============================================================================================

/*******************************************************************************
** �ļ���: 		main.c
** ����:		USART��ʼ����RCC���ã�Ȼ���common.c��ִ�����˵�
** ����ļ�:	stm32f10x.h
*******************************************************************************/

/* ����ͷ�ļ� *****************************************************************/
#include "common.h"
#include "stdio.h"
/* �������� ------------------------------------------------------------------*/
/* �� ------------------------------------------------------------------------*/
#define LED0 GPIO_Pin_0
#define LED1 GPIO_Pin_1
#define LED2 GPIO_Pin_2
#define LED3 GPIO_Pin_3
/* ���� ----------------------------------------------------------------------*/
typedef  void (*pFunction)(void);

/* �������� ------------------------------------------------------------------*/
void Delay( __IO uint32_t nCount );
void LED_Configuration( void );
static void IAP_Init( void );
void KEY_Configuration( void );
void GPIO_Configuration( void );
void USART_Configuration( void );
/* �������� ------------------------------------------------------------------*/

/*******************************************************************************
  * @��������	main
  * @����˵��   ������
  * @�������   ��
  * @�������   ��
  * @���ز���   ��
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

			//��ת���û�����
			Jump_To_Application = (pFunction)JumpAddress;
			//��ʼ���û�����Ķ�ջָ��
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
  * @��������	LED_Configuration
  * @����˵��   ����ʹ��LED
  * @�������   ��
  * @�������   ��
  * @���ز���   ��
*******************************************************************************/
void LED_Configuration( void )
{
	GPIO_InitTypeDef GPIO_InitStructure;
	//ʹ��LED����GPIO��ʱ��
	RCC_APB2PeriphClockCmd( RCC_APB2Periph_GPIOE, ENABLE );
	//��ʼ��LED��GPIO
	GPIO_InitStructure.GPIO_Pin = LED0 | LED1 | LED2 | LED3;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_Init( GPIOE, &GPIO_InitStructure );
	GPIO_SetBits( GPIOE, LED2 | LED3 );  //Ϩ��LED0-3
}

/*******************************************************************************
  * @��������	KEY_Configuration
  * @����˵��   ������ʼ��
  * @�������   ��
  * @�������   ��
  * @���ز���   ��
*******************************************************************************/
void KEY_Configuration( void )
{
	GPIO_InitTypeDef GPIO_InitStructure;
	RCC_APB2PeriphClockCmd( RCC_APB2Periph_GPIOC, ENABLE );

	//���ð���
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IPU;
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_5;
	GPIO_Init( GPIOC, &GPIO_InitStructure );
}

/*******************************************************************************
  * @��������	GPIO_Configuration
  * @����˵��   ����ʹ��USART1�����IO�ܽ�
  * @�������   ��
  * @�������   ��
  * @���ز���   ��
*******************************************************************************/
void GPIO_Configuration( void )
{
	GPIO_InitTypeDef GPIO_InitStructure;

	RCC_APB2PeriphClockCmd( RCC_APB2Periph_USART1 | RCC_APB2Periph_GPIOA, ENABLE );
	// ���� USART1 Tx (PA.09) ��Ϊ�������Ų��������ģʽ
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_9;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_Init( GPIOA, &GPIO_InitStructure );

	//���� USART1 Tx (PA.10) ��Ϊ�������Ų��Ǹ�������ģʽ
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_10;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN_FLOATING;
	GPIO_Init( GPIOA, &GPIO_InitStructure );
}

/*******************************************************************************
  * @��������	IAP_Init
  * @����˵��   ����ʹ��IAP
  * @�������   ��
  * @�������   ��
  * @���ز���   ��
*******************************************************************************/
void IAP_Init( void )
{
	USART_InitTypeDef USART_InitStructure;

	/* USART1 ���� ------------------------------------------------------------
         USART1 ��������:
          - ������      = 115200 baud
          - �ֳ�        = 8 Bits
          - һ��ֹͣλ
          - ��У��
          - ��Ӳ��������
          - ���ܺͷ���ʹ��
    --------------------------------------------------------------------------*/
	USART_InitStructure.USART_BaudRate = 9600;
	USART_InitStructure.USART_WordLength = USART_WordLength_8b;
	USART_InitStructure.USART_StopBits = USART_StopBits_1;
	USART_InitStructure.USART_Parity = USART_Parity_No;
	USART_InitStructure.USART_HardwareFlowControl = USART_HardwareFlowControl_None;
	USART_InitStructure.USART_Mode = USART_Mode_Rx | USART_Mode_Tx;

	GPIO_Configuration();
	USART_Init( USART1, &USART_InitStructure );
	// ʹ�� USART1
	USART_Cmd( USART1, ENABLE );
}

/*******************************************************************************
  * @��������	Delay
  * @����˵��   ����һ����ʱʱ��
  * @�������   nCount: ָ����ʱʱ�䳤��
  * @�������   ��
  * @���ز���   ��
*******************************************************************************/
void Delay( __IO uint32_t nCount )
{
	for ( ; nCount != 0; nCount-- )
		;
}

#ifdef USE_FULL_ASSERT

/*******************************************************************************
  * @��������	assert_failed
  * @����˵��   �����ڼ�������������ʱ��Դ�ļ����ʹ�������
  * @�������   file: Դ�ļ���
  				line: ������������
  * @�������   ��
  * @���ز���   ��
*******************************************************************************/
void assert_failed( uint8_t *file, uint32_t line )
{
	/* �û����������Լ��Ĵ������ڱ��������ļ�������������,
       ���磺printf("�������ֵ: �ļ��� %s �� %d��\r\n", file, line) */

	//��ѭ��
	while ( 1 )
	{
	}
}
#endif

/*******************************�ļ�����***************************************/

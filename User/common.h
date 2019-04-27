/*******************************************************************************
** �ļ���: 		common.h
** ����:		common�ļ���ͷ�ļ�����
** ����ļ�:	string.h��stdio.h��stm32f10x.h��ymodem.h
*******************************************************************************/

/* ��ֹ�ض��� ----------------------------------------------------------------*/
#ifndef _COMMON_H
#define _COMMON_H

/* ����ͷ�ļ� *****************************************************************/
#include "stdio.h"
#include "string.h"
#include "stm32f10x.h"
#include "ymodem.h"


/* �������� -----------------------------------------------------------------*/
typedef  void (*pFunction)(void);

//* �� ------------------------------------------------------------------------*/
#define CMD_STRING_SIZE       128

#define IS_AF(c)  ((c >= 'A') && (c <= 'F'))
#define IS_af(c)  ((c >= 'a') && (c <= 'f'))
#define IS_09(c)  ((c >= '0') && (c <= '9'))
#define ISVALIDHEX(c)  IS_AF(c) || IS_af(c) || IS_09(c)
#define ISVALIDDEC(c)  IS_09(c)
#define CONVERTDEC(c)  (c - '0')

#define CONVERTHEX_alpha(c)  (IS_AF(c) ? (c - 'A'+10) : (c - 'a'+10))
#define CONVERTHEX(c)   (IS_09(c) ? (c - '0') : CONVERTHEX_alpha(c))

#define SerialPutString(x) Serial_PutString((uint8_t*)(x))


/* �������� ------------------------------------------------------------------*/
uint32_t SerialKeyPressed(uint8_t *key);
void SerialPutChar(uint8_t c);
void Serial_PutString(uint8_t *s);
void SerialDownload(uint32_t);
#endif  /* _COMMON_H */

/*******************************�ļ�����***************************************/

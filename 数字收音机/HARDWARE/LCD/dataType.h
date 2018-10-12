/****************************************Copyright (c)****************************************************
**                            Guangzhou ZHIYUAN electronics Co.,LTD.
**                                      
**                                 http://www.embedtools.com
**
**------File Info-----------------------------------------------------------------------------------------
** File name:               dataType.h
** Latest modified Date:    2008-07-24
** Latest Version:          1.0
** Descriptions:            定义常用的数据类型及函数返回值  Designed for ARM (32bit processor)
**
**--------------------------------------------------------------------------------------------------------
** Created by:              张展威
** Created date:            2008-07-24
** Version:                 1.0
** Descriptions:            
**
**--------------------------------------------------------------------------------------------------------
** Modified by:             
** Modified date:           
** Version:                 
** Descriptions:            
**
*********************************************************************************************************/


#ifndef __GENERAL_DATA_TYPE_H
#define __GENERAL_DATA_TYPE_H
/*********************************************************************************************************
** 定义与编译器无关的数据类型
*********************************************************************************************************/
#ifndef INT8U
#define INT8U  unsigned char                                            /* 无符号8位整型变量            */
#endif
#ifndef INT8S
#define INT8S  signed char                                              /* 有符号8位整型变量            */
#endif
#ifndef INT16U
#define INT16U unsigned short                                           /* 无符号16位整型变量           */
#endif
#ifndef INT16S
#define INT16S signed short                                             /* 有符号16位整型变量           */
#endif
#ifndef INT32U
#define INT32U unsigned long                                             /* 无符号32位整型变量           */
#endif
#ifndef INT32S
#define INT32S signed long                                               /* 有符号32位整型变量           */
#endif
#ifndef FP32
#define FP32   float                                                    /* 单精度浮点数（32位长度）     */
#endif
#ifndef FP64
#define FP64   double                                                   /* 双精度浮点数（64位长度）     */
#endif
#ifndef BOOLEAN
#define BOOLEAN unsigned char                                           /* 布尔变量                     */
#endif
#ifndef boolean
#define boolean BOOLEAN                                                 /* 布尔变量                     */
#endif
#ifndef uchar
#define uchar   unsigned char
#endif
#ifndef uint
#define uint    unsigned int 
#endif

/*********************************************************************************************************
** 定义常用返回值
*********************************************************************************************************/
#ifndef TRUE
#define TRUE 1
#endif
#ifndef FALSE
#define FALSE 0
#endif
#ifndef true
#define true 1
#endif
#ifndef false
#define false 0
#endif

#endif

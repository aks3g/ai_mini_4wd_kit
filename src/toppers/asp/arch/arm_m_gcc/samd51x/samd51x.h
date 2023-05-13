/*
 *  TOPPERS/ASP Kernel
 *      Toyohashi Open Platform for Embedded Real-Time Systems/
 *      Just Standard Profile Kernel
 * 
 *  Copyright (C) 2015      by 3rd Designing Center
 *              Imageing System Development Division RICOH COMPANY, LTD.
 *  Copyright (C) 2017-2021 by TOPPERS PROJECT
 *                             Educational Working Group.
 *
 *  �嵭����Ԥϡ��ʲ��� (1)��(4) �ξ�狼��Free Software Foundation 
 *  �ˤ�äƸ�ɽ����Ƥ��� GNU General Public License �� Version 2 �˵�
 *  �Ҥ���Ƥ���������������˸¤ꡤ�ܥ��եȥ��������ܥ��եȥ�����
 *  ����Ѥ�����Τ�ޤࡥ�ʲ�Ʊ���ˤ���ѡ�ʣ�������ѡ������ۡʰʲ���
 *  ���ѤȸƤ֡ˤ��뤳�Ȥ�̵���ǵ������롥
 *  (1) �ܥ��եȥ������򥽡��������ɤη������Ѥ�����ˤϡ��嵭������
 *      ��ɽ�����������Ѿ�浪��Ӳ�����̵�ݾڵ��꤬�����Τޤޤη��ǥ���
 *      ����������˴ޤޤ�Ƥ��뤳�ȡ�
 *  (2) �ܥ��եȥ������򡤥饤�֥������ʤɡ�¾�Υ��եȥ�������ȯ�˻�
 *      �ѤǤ�����Ǻ����ۤ�����ˤϡ������ۤ�ȼ���ɥ�����ȡ�����
 *      �ԥޥ˥奢��ʤɡˤˡ��嵭�����ɽ�����������Ѿ�浪��Ӳ���
 *      ��̵�ݾڵ����Ǻܤ��뤳�ȡ�
 *  (3) �ܥ��եȥ������򡤵�����Ȥ߹���ʤɡ�¾�Υ��եȥ�������ȯ�˻�
 *      �ѤǤ��ʤ����Ǻ����ۤ�����ˤϡ����Τ����줫�ξ�����������
 *      �ȡ�
 *    (a) �����ۤ�ȼ���ɥ�����ȡ����Ѽԥޥ˥奢��ʤɡˤˡ��嵭����
 *        �ɽ�����������Ѿ�浪��Ӳ�����̵�ݾڵ����Ǻܤ��뤳�ȡ�
 *    (b) �����ۤη��֤��̤�������ˡ�ˤ�äơ�TOPPERS�ץ������Ȥ�
 *        ��𤹤뤳�ȡ�
 *  (4) �ܥ��եȥ����������Ѥˤ��ľ��Ū�ޤ��ϴ���Ū�������뤤���ʤ�»
 *      ������⡤�嵭����Ԥ����TOPPERS�ץ������Ȥ����դ��뤳�ȡ�
 *
 *  �ܥ��եȥ������ϡ�̵�ݾڤ��󶡤���Ƥ����ΤǤ��롥�嵭����Ԥ�
 *  ���TOPPERS�ץ������Ȥϡ��ܥ��եȥ������˴ؤ��ơ�����Ŭ�Ѳ�ǽ����
 *  �ޤ�ơ������ʤ��ݾڤ�Ԥ�ʤ����ޤ����ܥ��եȥ����������Ѥˤ��ľ
 *  ��Ū�ޤ��ϴ���Ū�������������ʤ�»���˴ؤ��Ƥ⡤������Ǥ�����ʤ���
 * 
 *  @(#) $Id: stm32f4xx.h,v 1.5 2021/11/10 06:57:19 roi Exp $
 */

#ifndef _SAMD51X_H_
#define _SAMD51X_H_

#include <sil.h>
#include "cmsis_m4.h"

/*
 *  ������ֹ�κ�����
 */
#define TMAX_INTNO          (16 + 136)

/*
 *  �����ͥ���٤Υӥå���
 */
#define TBITW_IPRI          4

/*
 *  INTERRUPT NUMBER
 */
#define IRQ_PM				(16+0)
#define IRQ_MCLK			(16+1)
#define IRQ_OSCCTRL_0		(16+2)
#define IRQ_OSCCTRL_1		(16+3)
#define IRQ_OSCCTRL_2		(16+4)
#define IRQ_OSCCTRL_3		(16+5)
#define IRQ_OSCCTRL_4		(16+6)
#define IRQ_OSC32KCTRL		(16+7)
#define IRQ_SUPC_0			(16+8)
#define IRQ_SUPC_1			(16+9)
#define IRQ_WDT				(16+10)
#define IRQ_RTC				(16+11)
#define IRQ_EIC_0			(16+12)
#define IRQ_EIC_1			(16+13)
#define IRQ_EIC_2			(16+14)
#define IRQ_EIC_3			(16+15)
#define IRQ_EIC_4			(16+16)
#define IRQ_EIC_5			(16+17)
#define IRQ_EIC_6			(16+18)
#define IRQ_EIC_7			(16+19)
#define IRQ_EIC_8			(16+20)
#define IRQ_EIC_9			(16+21)
#define IRQ_EIC_10			(16+22)
#define IRQ_EIC_11			(16+23)
#define IRQ_EIC_12			(16+24)
#define IRQ_EIC_13			(16+25)
#define IRQ_EIC_14			(16+26)
#define IRQ_EIC_15			(16+27)
#define IRQ_FREQM			(16+28)
#define IRQ_NVMCTRL_0		(16+29)
#define IRQ_NVMCTRL_1		(16+30)
#define IRQ_DMAC_0			(16+31)
#define IRQ_DMAC_1			(16+32)
#define IRQ_DMAC_2			(16+33)
#define IRQ_DMAC_3			(16+34)
#define IRQ_DMAC_4			(16+35)
#define IRQ_EVSYS_0			(16+36)
#define IRQ_EVSYS_1			(16+37)
#define IRQ_EVSYS_2			(16+38)
#define IRQ_EVSYS_3			(16+39)
#define IRQ_EVSYS_4			(16+40)
#define IRQ_PAC				(16+41)
#define IRQ_pvReserved42	(16+42)
#define IRQ_pvReserved43	(16+43)
#define IRQ_pvReserved44	(16+44)
#define IRQ_RAMECC			(16+45)
#define IRQ_SERCOM0_0		(16+46)
#define IRQ_SERCOM0_1		(16+47)
#define IRQ_SERCOM0_2		(16+48)
#define IRQ_SERCOM0_3		(16+49)
#define IRQ_SERCOM1_0		(16+50)
#define IRQ_SERCOM1_1		(16+51)
#define IRQ_SERCOM1_2		(16+52)
#define IRQ_SERCOM1_3		(16+53)
#define IRQ_SERCOM2_0		(16+54)
#define IRQ_SERCOM2_1		(16+55)
#define IRQ_SERCOM2_2		(16+56)
#define IRQ_SERCOM2_3		(16+57)
#define IRQ_SERCOM3_0		(16+58)
#define IRQ_SERCOM3_1		(16+59)
#define IRQ_SERCOM3_2		(16+60)
#define IRQ_SERCOM3_3		(16+61)
#define IRQ_SERCOM4_0		(16+62)
#define IRQ_SERCOM4_1		(16+63)
#define IRQ_SERCOM4_2		(16+64)
#define IRQ_SERCOM4_3		(16+65)
#define IRQ_SERCOM5_0		(16+66)
#define IRQ_SERCOM5_1		(16+67)
#define IRQ_SERCOM5_2		(16+68)
#define IRQ_SERCOM5_3		(16+69)
#define IRQ_pvReserved70	(16+70)
#define IRQ_pvReserved71	(16+71)
#define IRQ_pvReserved72	(16+72)
#define IRQ_pvReserved73	(16+73)
#define IRQ_pvReserved74	(16+74)
#define IRQ_pvReserved75	(16+75)
#define IRQ_pvReserved76	(16+76)
#define IRQ_pvReserved77	(16+77)
#define IRQ_pvReserved78	(16+78)
#define IRQ_pvReserved79	(16+79)
#define IRQ_USB_0			(16+80)
#define IRQ_USB_1			(16+81)
#define IRQ_USB_2			(16+82)
#define IRQ_USB_3			(16+83)
#define IRQ_pvReserved84	(16+84)
#define IRQ_TCC0_0			(16+85)
#define IRQ_TCC0_1			(16+86)
#define IRQ_TCC0_2			(16+87)
#define IRQ_TCC0_3			(16+88)
#define IRQ_TCC0_4			(16+89)
#define IRQ_TCC0_5			(16+90)
#define IRQ_TCC0_6			(16+91)
#define IRQ_TCC1_0			(16+92)
#define IRQ_TCC1_1			(16+93)
#define IRQ_TCC1_2			(16+94)
#define IRQ_TCC1_3			(16+95)
#define IRQ_TCC1_4			(16+96)
#define IRQ_TCC2_0			(16+97)
#define IRQ_TCC2_1			(16+98)
#define IRQ_TCC2_2			(16+99)
#define IRQ_TCC2_3			(16+100)
#define IRQ_TCC3_0			(16+101)
#define IRQ_TCC3_1			(16+102)
#define IRQ_TCC3_2			(16+103)
#define IRQ_TCC4_0			(16+104)
#define IRQ_TCC4_1			(16+105)
#define IRQ_TCC4_2			(16+106)
#define IRQ_TC0				(16+107)
#define IRQ_TC1				(16+108)
#define IRQ_TC2				(16+109)
#define IRQ_TC3				(16+110)
#define IRQ_TC4				(16+111)
#define IRQ_TC5				(16+112)
#define IRQ_pvReserved113	(16+113)
#define IRQ_pvReserved114	(16+114)
#define IRQ_PDEC_0			(16+115)
#define IRQ_PDEC_1			(16+116)
#define IRQ_PDEC_2			(16+117)
#define IRQ_ADC0_0			(16+118)
#define IRQ_ADC0_1			(16+119)
#define IRQ_ADC1_0			(16+120)
#define IRQ_ADC1_1			(16+121)
#define IRQ_AC				(16+122)
#define IRQ_DAC_0			(16+123)
#define IRQ_DAC_1			(16+124)
#define IRQ_DAC_2			(16+125)
#define IRQ_DAC_3			(16+126)
#define IRQ_DAC_4			(16+127)
#define IRQ_I2S				(16+128)
#define IRQ_PCC				(16+129)
#define IRQ_AES				(16+130)
#define IRQ_TRNG			(16+131)
#define IRQ_ICM				(16+132)
#define IRQ_PUKCC			(16+133)
#define IRQ_QSPI			(16+134)
#define IRQ_SDHC0			(16+135)
#define IRQ_pvReserved136	(16+136)
	
#endif  /* _SAMD51X_H_ */


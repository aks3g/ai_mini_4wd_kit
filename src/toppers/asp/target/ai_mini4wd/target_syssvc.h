/*
 *  TOPPERS/ASP Kernel
 *      Toyohashi Open Platform for Embedded Real-Time Systems/
 *      Advanced Standard Profile Kernel
 * 
 *  Copyright (C) 2008-2011 by Embedded and Real-Time Systems Laboratory
 *              Graduate School of Information Science, Nagoya Univ., JAPAN
 *  Copyright (C) 2015      by 3rd Designing Center
 *              Imageing System Development Division RICOH COMPANY, LTD.
 *  Copyright (C) 2017-2021 by TOPPERS PROJECT Educational Working Group.
 * 
 *  �嵭����Ԥϡ��ʲ���(1)��(4)�ξ������������˸¤ꡤ�ܥ��եȥ���
 *  �����ܥ��եȥ���������Ѥ�����Τ�ޤࡥ�ʲ�Ʊ���ˤ���ѡ�ʣ������
 *  �ѡ������ۡʰʲ������ѤȸƤ֡ˤ��뤳�Ȥ�̵���ǵ������롥
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
 *      �ޤ����ܥ��եȥ������Υ桼���ޤ��ϥ���ɥ桼������Τ����ʤ���
 *      ͳ�˴�Ť����ᤫ��⡤�嵭����Ԥ����TOPPERS�ץ������Ȥ�
 *      ���դ��뤳�ȡ�
 * 
 *  �ܥ��եȥ������ϡ�̵�ݾڤ��󶡤���Ƥ����ΤǤ��롥�嵭����Ԥ�
 *  ���TOPPERS�ץ������Ȥϡ��ܥ��եȥ������˴ؤ��ơ�����λ�����Ū
 *  ���Ф���Ŭ������ޤ�ơ������ʤ��ݾڤ�Ԥ�ʤ����ޤ����ܥ��եȥ���
 *  �������Ѥˤ��ľ��Ū�ޤ��ϴ���Ū�������������ʤ�»���˴ؤ��Ƥ⡤��
 *  ����Ǥ�����ʤ���
 * 
 *  @(#) $Id: target_syssvc.h 698 2021-11-22 19:45:24Z roi $
 */

/*
 *  �����ƥॵ���ӥ��Υ������åȰ�¸����stm32-e407�ѡ�
 *
 *  �����ƥॵ���ӥ��Υ������åȰ�¸���Υ��󥯥롼�ɥե����롥���Υե�
 *  ��������Ƥϡ�����ݡ��ͥ�ȵ��ҥե�����˵��Ҥ��졤���Υե������
 *  ̵���ʤ븫���ߡ�
 */

#ifndef TOPPERS_TARGET_SYSSVC_H
#define TOPPERS_TARGET_SYSSVC_H

/*
 *  �������åȥ����ƥ�Υϡ��ɥ������񸻤����
 */
#include "samd51x.h"

/*
 *  �����ƥ९��å������
 */
#define SystemFrequency     120000000
//#define SysFreHCLK          120000000	/* HCLK = MasterClock / 1 */
//#define SysFrePCLK1         30000000	/* PCLK1 = HCLK / 4 */
//#define SysFrePCLK2         60000000	/* PCLK2 = HCLK / 2 */

#define SYS_CLOCK		    (SystemFrequency)

/*
 *  �ȥ졼�����˴ؤ�������
 */
#ifdef TOPPERS_ENABLE_TRACE
#include "logtrace/trace_config.h"
#endif /* TOPPERS_ENABLE_TRACE */

/*
 *  ��ư��å������Υ������åȥ����ƥ�̾
 */
#define TARGET_NAME    "ai_mini4wd(Cortex-M4)"

/*
 *  ��ư��å����������ɽ��
 */
#define TARGET_COPYRIGHT \
"Copyright (C) 2015-2016 by Education Working Group TOPPERS PROJECT\n" \

#ifndef TOPPERS_MACRO_ONLY

/*
 *  �����ƥ�������٥���ϤΤ����ʸ������
 *
 *  �������åȰ�¸����ˡ�ǡ�ʸ��c��ɽ��/����/��¸���롥
 */
extern void	target_fput_log(int8_t c);

#endif /* TOPPERS_MACRO_ONLY */

/*
 *  ���ݡ��Ȥ��륷�ꥢ��ݡ��Ȥο�
 */
#define TNUM_PORT        TNUM_SIOP

/*
 *  �������������Ѥ���ݡ���ID
 */
#define LOGTASK_PORTID   SIO_PORTID

/*
 *  �ܡ��졼��
 */
#define BPS_SETTING		(115200)

/*
 *  �����ƥ����������Ϣ����������
 *
 *  �ǥե�����ͤ��̤ꡥ
 */

#endif /* TOPPERS_TARGET_SYSSVC_H */

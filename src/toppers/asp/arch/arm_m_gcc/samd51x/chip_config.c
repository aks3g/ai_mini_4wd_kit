/*
 *  TOPPERS/ASP Kernel
 *      Toyohashi Open Platform for Embedded Real-Time Systems/
 *      Advanced Standard Profile Kernel
 * 
 *  Copyright (C) 2008-2011 by Embedded and Real-Time Systems Laboratory
 *              Graduate School of Information Science, Nagoya Univ., JAPAN
 *  Copyright (C) 2015      by 3rd Designing Center
 *              Imageing System Development Division RICOH COMPANY, LTD.
 * 
 *  �嵭����Ԣ�Ԥϡ��ʲ���(1)��(4)�ξ�E���������E�˸¤ꡤ�ܥ��եȥ���
 *  �����ܥ��եȥ���������Ѥ�����Τ�ޤࡥ�ʲ�Ʊ���ˤ���ѡ�ʣ������
 *  �ѡ������ۡʰʲ������ѤȸƤ֡ˤ���E��Ȥ�̵���ǵ�������E�
 *  (1) �ܥ��եȥ������򥽡��������ɤη������Ѥ���E�E�ˤϡ��嵭������E *      ��ɽ�����������Ѿ�E���Ӳ�����̵�ݾڵ��꤬�����Τޤޤη��ǥ���
 *      ����������˴ޤޤ�EƤ���E��ȡ�
 *  (2) �ܥ��եȥ������򡤥饤�֥饁E����ʤɡ�¾�Υ��եȥ�������ȯ�˻�
 *      �ѤǤ���E��Ǻ����ۤ���E�E�ˤϡ������ۤ�ȼ���ɥ�����ȡ�����
 *      �ԥޥ˥奢��Eʤɡˤˡ��嵭������Ԣɽ�����������Ѿ�E���Ӳ���
 *      ��̵�ݾڵ����Ǻܤ���E��ȡ�
 *  (3) �ܥ��եȥ������򡤵���E��Ȥ߹���ʤɡ�¾�Υ��եȥ�������ȯ�˻�
 *      �ѤǤ��ʤ����Ǻ����ۤ���E�E�ˤϡ����Τ�����E��ξ�E���������
 *      �ȡ�
 *    (a) �����ۤ�ȼ���ɥ�����ȡ����Ѽԥޥ˥奢��Eʤɡˤˡ��嵭��ÁE *        ��Ԣɽ�����������Ѿ�E���Ӳ�����̵�ݾڵ����Ǻܤ���E��ȡ�
 *    (b) �����ۤη��֤��̤���ᤁE�ˡ�ˤ�äơ�TOPPERS�ץ��������Ȥ�
 *        ��𤹤�E��ȡ�
 *  (4) �ܥ��եȥ����������Ѥˤ褁E���Ū�ޤ��ϴ���Ū��������E����ʤ�E�
 *      ������⡤�嵭����Ԣ�Ԥ����TOPPERS�ץ��������Ȥ����դ���E��ȡ�
 *      �ޤ����ܥ��եȥ������Υ桼���ޤ��ϥ���ɥ桼������Τ����ʤ�E�
 *      ͳ�˴�Ť����ᤫ��⡤�嵭����Ԣ�Ԥ����TOPPERS�ץ��������Ȥ�E *      ���դ���E��ȡ�
 * 
 *  �ܥ��եȥ������ϡ�̵�ݾڤ��󶡤���EƤ���E�ΤǤ���E��嵭����Ԣ�Ԥ�
 *  ���TOPPERS�ץ��������Ȥϡ��ܥ��եȥ������˴ؤ��ơ�����λ�����Ū
 *  ���Ф���E�������ޤ�ơ������ʤ�Eݾڤ�Ԥ�Eʤ����ޤ����ܥ��եȥ���
 *  �������Ѥˤ褁E���Ū�ޤ��ϴ���Ū�������������ʤ�E����˴ؤ��Ƥ⡤��
 *  ����Ǥ���餁Eʤ���
 * 
 *  @(#) $Id: chip_config.c 698 2015-07-27 22:51:58Z roi $
 */

/*
 * �������åȰ�¸�⥸�塼��E�samd51x�ѡ�
 */
#include "kernel_impl.h"
#include <sil.h>

#include "samd51x.h"
#include "chip_serial.h"

/*
 *  �������åȰ�¸�ν鴁E�
 */
void
target_initialize(void){

	/*
	 * ������¸�ν鴁E�
	 */
	core_initialize();

	/*
	 *  �Хʡ������ѤΥ���E���E鴁E�
	 */
	chip_uart_init(SIO_PORTID);

	/*
	 *  �����ޤγ���ߥ�E٥�E���āE	 */
	sil_wrb_mem((uint8_t *)(TADR_SCB_BASE+TOFF_SCB_SHP15), (14 << (8 - 4)) & 0xff);
} 


/*
 *  �������åȰ�¸�ν�λ����
 */
void
target_exit(void)
{
	/*
	 *  ������¸�ν�λ����
	 */
	core_terminate();

	/*
	 *  ��ȯ�Ķ���¸�ν�λ����
	 */
	while(1);
}


/*
 *  �����ƥ�������㥁E٥�E��ϤΤ����ʸ������
 */
void
target_fput_log(int8_t c)
{
	if (c == '\n') {
		sio_pol_snd_chr('\r', SIO_PORTID);
	}
	sio_pol_snd_chr(c, SIO_PORTID);
}

/*
 *  �������åȰ�¸��āE�
 */
#include "target_test.h"

/*
 *  �ƥ�������ͥ���٤�āE�
 */

#define MAIN_PRIORITY	5		/* �ᥤ�󥿥�����ͥ���� */
								/* HIGH_PRIORITY�褁E⤯����E��� */

#define HIGH_PRIORITY	9		/* �¹Լ¹Ԥ���E�E�������ͥ���� */
#define MID_PRIORITY	10
#define LOW_PRIORITY	11

/*
 *  �������åȤ˰�¸����E�ǽ���Τ���E�E���āE�
 */

#ifndef TASK_PORTID
#define	TASK_PORTID		1			/* ʸ�����Ϥ���E���E���Eݡ���ID */
#endif /* TASK_PORTID */

#ifndef STACK_SIZE
#define	STACK_SIZE		4096		/* �������Υ����å������� */
#endif /* STACK_SIZE */

#define IO_STACK_SIZE		4096
#define MAIN_STACK_SIZE		16*1024


#ifndef LOOP_REF
#define LOOP_REF		ULONG_C(1000000)	/* ®�ٷ�¬�ѤΥ�E��ײ�E*/
#endif /* LOOP_REF */

/*
 *  �ؿ��Υץ��ȥ�������E�
 */
#ifndef TOPPERS_MACRO_ONLY

extern void	upython_task(intptr_t exinf);
extern void	io_scheduler_task(intptr_t exinf);

extern void	cpuexc_handler(void *p_excinf);

extern void	cyclic_handler(intptr_t exinf);

#endif /* TOPPERS_MACRO_ONLY */

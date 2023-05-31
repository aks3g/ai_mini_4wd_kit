/*
 *  ターゲット依存の��E�
 */
#include "target_test.h"

/*
 *  各タスクの優先度の��E�
 */

#define MAIN_PRIORITY	5		/* メインタスクの優先度 */
								/* HIGH_PRIORITYよ��E發�す��E海� */

#define HIGH_PRIORITY	9		/* 並行実行さ��E�E織好�の優先度 */
#define MID_PRIORITY	10
#define LOW_PRIORITY	11

/*
 *  ターゲットに依存す��E椎柔�のあ��E�E瑤猟�E�
 */

#ifndef TASK_PORTID
#define	TASK_PORTID		1			/* 文字入力す��E轡�E▲�E檗璽�ID */
#endif /* TASK_PORTID */

#ifndef STACK_SIZE
#define	STACK_SIZE		4096		/* タスクのスタックサイズ */
#endif /* STACK_SIZE */

#define IO_STACK_SIZE		4096
#define MAIN_STACK_SIZE		16*1024


#ifndef LOOP_REF
#define LOOP_REF		ULONG_C(1000000)	/* 速度計測用の��E璽弉鷽�E*/
#endif /* LOOP_REF */

/*
 *  関数のプ��組タイプ��E�
 */
#ifndef TOPPERS_MACRO_ONLY

extern void	upython_task(intptr_t exinf);
extern void	io_scheduler_task(intptr_t exinf);

extern void	cpuexc_handler(void *p_excinf);

extern void	cyclic_handler(intptr_t exinf);

#endif /* TOPPERS_MACRO_ONLY */

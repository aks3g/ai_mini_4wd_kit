/*
 *  ¥¿¡¼¥²¥Ã¥È°ÍÂ¸¤ÎÄEÁ
 */
#include "target_test.h"

/*
 *  ³Æ¥¿¥¹¥¯¤ÎÍ¥ÀèÅÙ¤ÎÄEÁ
 */

#define MAIN_PRIORITY	5		/* ¥á¥¤¥ó¥¿¥¹¥¯¤ÎÍ¥ÀèÅÙ */
								/* HIGH_PRIORITY¤è¤Eâ¤¯¤¹¤E³¤È */

#define HIGH_PRIORITY	9		/* ÊÂ¹Ô¼Â¹Ô¤µ¤EE¿¥¹¥¯¤ÎÍ¥ÀèÅÙ */
#define MID_PRIORITY	10
#define LOW_PRIORITY	11

/*
 *  ¥¿¡¼¥²¥Ã¥È¤Ë°ÍÂ¸¤¹¤EÄÇ½À­¤Î¤¢¤EEô¤ÎÄEÁ
 */

#ifndef TASK_PORTID
#define	TASK_PORTID		1			/* Ê¸»úÆşÎÏ¤¹¤E·¥E¢¥Eİ¡¼¥ÈID */
#endif /* TASK_PORTID */

#ifndef STACK_SIZE
#define	STACK_SIZE		4096		/* ¥¿¥¹¥¯¤Î¥¹¥¿¥Ã¥¯¥µ¥¤¥º */
#endif /* STACK_SIZE */

#define IO_STACK_SIZE		4096
#define MAIN_STACK_SIZE		16*1024


#ifndef LOOP_REF
#define LOOP_REF		ULONG_C(1000000)	/* Â®ÅÙ·×Â¬ÍÑ¤Î¥E¼¥×²ó¿E*/
#endif /* LOOP_REF */

/*
 *  ´Ø¿ô¤Î¥×¥úÁÈ¥¿¥¤¥×ÀEÀ
 */
#ifndef TOPPERS_MACRO_ONLY

extern void	upython_task(intptr_t exinf);
extern void	io_scheduler_task(intptr_t exinf);

extern void	cpuexc_handler(void *p_excinf);

extern void	cyclic_handler(intptr_t exinf);

#endif /* TOPPERS_MACRO_ONLY */

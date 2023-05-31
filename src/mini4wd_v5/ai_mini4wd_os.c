#include <kernel.h>
#include <t_syslog.h>
#include <t_stdlib.h>
#include "syssvc/serial.h"
#include "syssvc/syslog.h"
#include "kernel_cfg.h"
#include "ai_mini4wd_os.h"

/*
 *  CPU Exception handler
 */
volatile uint32_t * p=NULL;
void cpuexc_handler(void *p_excinf)
{
	ID		tskid;

	p = (uint32_t *)p_excinf;

	if (sns_ctx() != true) {
	}
	if (sns_dpn() != true) {
	}
	if (xsns_xpn(p_excinf)) {
		ext_ker();
		assert(0);
	}

	iget_tid(&tskid);
	iras_tex(tskid, 0x8000U);
}


/*
 * 
 */
void cyclic_handler(intptr_t exinf)
{
	return;
}


/*
 * aiMini4wd_trace.h
 *
 * Created: 2023/05/07 5:47:21
 *  Author: kiyot
 */ 


#ifndef AIMINI4WD_TRACE_H_
#define AIMINI4WD_TRACE_H_

typedef void (*AiMini4wdDebugTraceGet)(uint32_t idx, uint32_t tick, uint32_t log);

void aiMini4wdDebugTraceClear(void);
void aiMini4wdDebugTraceControl(uint32_t enable);
void aiMini4wdDebugTracePush(uint32_t tick, uint32_t log);
void aiMini4wdDebugTracePop(AiMini4wdDebugTraceGet getter);

#endif /* AIMINI4WD_TRACE_H_ */
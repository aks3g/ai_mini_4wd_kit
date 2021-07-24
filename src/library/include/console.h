/*
 * console.h
 *
 * Created: 2021/06/27 8:28:47
 *  Author: kiyot
 */ 


#ifndef CONSOLE_H_
#define CONSOLE_H_

#define CONSOLE_ERROR_OK				0x00000000
#define CONSOLE_ERROR_NULL				0xC0000001

#define CONSOLE_ERROR_NOPUTFUNC			0xC0010001

#define CONSOLE_ERROR_COMMAND_NOTFOUND	0xC0020001

#define MAX_ARG_CNT		(8)

typedef int (*ConsoleFunction)(const int argc, const char **argv);
typedef const char *(*ConsoleHelp)(void);

typedef struct ConsoleCommand_t
{
	const char *name;
	ConsoleFunction func;
	ConsoleHelp help;

	struct ConsoleCommand_t *link;	
} ConsoleCommand;

typedef void (*ConsolePutcFunc)(const char c);


int consoleInitialize(ConsolePutcFunc putc_func);
int consoleInstallCommand(ConsoleCommand *cmd);

void consoleUpdate(char c);
int consoleExecute(char *line);

int consolePrintf(const char *format, ...);
int consolePuts(const char *str);
int consolePutc(const char c);

#endif /* CONSOLE_H_ */
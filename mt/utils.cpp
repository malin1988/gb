#include <stdlib.h>
#include <stdio.h>
#include <stdarg.h>
#include <time.h>
#include <unistd.h>
#include <string.h>
#include "utils.h"

#define DEFAULT_LOG_PATH "./log/"

/**
* 私有写日至文件函数
*/
static void write_log(const char *path, const char *msg)
{
	/* 生成按照年月日命名的文件 */
	struct tm *tmtime;
	time_t now = time(NULL);
	tmtime = gmtime(&now);
	char filename[64], fullpath[1024];

	snprintf(filename, 64, "%d_%d_%d.log", tmtime->tm_year + 1900, tmtime->tm_mon+1, tmtime->tm_mday);
	snprintf(fullpath, 1024, "%s%s", path, filename);

	// debug
	//printf("filename: %s\n", filename);
	//printf("fullpath: %s\n", fullpath);

	/* 写入日至 */
	FILE *fp = NULL;
	fp = fopen(fullpath, "a+");
	if (fp == NULL) {
		fprintf(stderr, "open log file error\n");
		perror("open err");
		exit(-1);
	}

	fputs(msg, fp);
	fclose(fp);
}

/**
* 打印错误日至
*/

void log_msg(int level, const char *fmt, ...)
{
	char msg[1024], fullmsg[2048];
	va_list vl;
	va_start(vl, fmt);
	vsnprintf(msg, 1024, fmt, vl);
	va_end(vl);

	time_t now = time(NULL);
	char *tp, timebuf[128];
	tp = ctime_r(&now, timebuf);
	timebuf[strlen(timebuf)-1] = '\0';
	snprintf(fullmsg, 2048, "%s [%d] %s\n", timebuf, getpid(), msg);

	write_log(DEFAULT_LOG_PATH, fullmsg);
	if (level >= LOG_LEVEL_DEBUG)
		abort();
}

int main_ok()
{
	log_msg(LOG_LEVEL_INFO, "ERR ERR...");
	log_msg(LOG_LEVEL_INFO, "ERR ERR...");
	return 0;
}

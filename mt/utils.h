#ifndef __UTILS_H__
#define __UTILS_H__

enum LOG_LEVEL {LOG_LEVEL_INFO, 
				LOG_LEVEL_DEBUG,
				LOG_LEVEL_ERR
			   };
/**
* 私有写日至文件函数
*/
static void write_log(const char *path, const char *msg);

/**
* 打印错误日至，退出程序
*/
void log_msg(int level, const char *fmt, ...);


#endif // __UTILS_H__

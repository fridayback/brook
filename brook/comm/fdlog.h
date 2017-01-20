#ifndef UTILITY_H_
#define UTILITY_H_


void log_init();
void log_uninit();
void Log(int lv, const char* format, ...);
void log_hex(int lv, char* buf, int len, char* format, ...);
extern int LOG_LEVEL;
#define LOG Log
#define LOG_TRACE(format,...) Log(0,format,##__VA_ARGS__)
#define LOG_DEBUG(format,...) Log(1,format,##__VA_ARGS__)
#define LOG_INFO(format,...) Log(2,format,##__VA_ARGS__)
#define LOG_WARN(format,...) Log(3,format,##__VA_ARGS__)
#define LOG_ERROR(format,...) Log(4,format,##__VA_ARGS__)

#define LOG_TRACE_C(format,...) Log(100,format,##__VA_ARGS__)
#define LOG_DEBUG_C(format,...) Log(101,format,##__VA_ARGS__)
#define LOG_INFO_C(format,...) Log(102,format,##__VA_ARGS__)
#define LOG_WARN_C(format,...) Log(103,format,##__VA_ARGS__)
#define LOG_ERROR_C(format,...) Log(104,format,##__VA_ARGS__)

#define LOG_TRACE_F(format,...) Log(200,format,##__VA_ARGS__)
#define LOG_DEBUG_F(format,...) Log(201,format,##__VA_ARGS__)
#define LOG_INFO_F(format,...) Log(202,format,##__VA_ARGS__)
#define LOG_WARN_F(format,...) Log(203,format,##__VA_ARGS__)
#define LOG_ERROR_F(format,...) Log(204,format,##__VA_ARGS__)

#define LOG_HEX_TRACE(buf,len,format,...) log_hex(0,buf,len,format,##__VA_ARGS__)
#define LOG_HEX_DEBUG(buf,len,format,...) log_hex(1,buf,len,format,##__VA_ARGS__)
#define LOG_HEX_INFO(buf,len,format,...) log_hex(2,buf,len,format,##__VA_ARGS__)
#define LOG_HEX_WARN(buf,len,format,...) log_hex(3,buf,len,format,##__VA_ARGS__)
#define LOG_HEX_ERROR(buf,len,format,...) log_hex(4,buf,len,format,##__VA_ARGS__)

#define LOG_HEX_TRACE_C(buf,len,format,...) log_hex(100,buf,len,format,##__VA_ARGS__)
#define LOG_HEX_DEBUG_C(buf,len,format,...) log_hex(101,buf,len,format,##__VA_ARGS__)
#define LOG_HEX_INFO_C(buf,len,format,...) log_hex(102,buf,len,format,##__VA_ARGS__)
#define LOG_HEX_WARN_C(buf,len,format,...) log_hex(103,buf,len,format,##__VA_ARGS__)
#define LOG_HEX_ERROR_C(buf,len,format,...) log_hex(104,buf,len,format,##__VA_ARGS__)

#define LOG_HEX_TRACE_F(buf,len,format,...) log_hex(200,buf,len,format,##__VA_ARGS__)
#define LOG_HEX_DEBUG_F(buf,len,format,...) log_hex(201,buf,len,format,##__VA_ARGS__)
#define LOG_HEX_INFO_F(buf,len,format,...) log_hex(202,buf,len,format,##__VA_ARGS__)
#define LOG_HEX_WARN_F(buf,len,format,...) log_hex(203,buf,len,format,##__VA_ARGS__)
#define LOG_HEX_ERROR_F(buf,len,format,...) log_hex(204,buf,len,format,##__VA_ARGS__)

#endif /* UTILITY_H_ */

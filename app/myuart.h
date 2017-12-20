#ifndef __USART1_H_
#define	__USART1_H_
void USART1_Config(void);

#define USART_RX_BUFFER_LEN 100

#if defined(USE_LOG)
#define LOG(fmt, args...) fprintf(stderr, "[%s %d] "fmt"\r\n", __FILE__, __LINE__, ##args)
#else
#define LOG(fmt, args...)
#endif

#if defined(DATA_LOG)
#define LOGD(fmt, args...) fprintf(stderr, "[%s %d] "fmt"\r\n", __FILE__, __LINE__, ##args)
#else
#define LOGD(fmt, args...)
#endif

void UART1SendStr(unsigned char *str);
#endif

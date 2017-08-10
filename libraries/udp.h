/*
 * udp.h
 *
 *  Created on: 2017-8-9
 *      Author: wangbo
 */

#ifndef UDP_H_
#define UDP_H_

//#define IP_SEND_TO "127.0.0.1"
#define IP_SEND_TO "10.108.17.250"
#define PORT_SENT_TO 49000
#define PORT_RECEIVE 49005

struct T_UDP_DEVICE
{
    //typedef unsigned long int pthread_t 为了不在头文件中增加其他头文件，这里把pthread fd换成unsigned long int fd;
    unsigned long int fd;
    int (*ptr_fun)(unsigned char *buf,unsigned int len);
};

extern unsigned char udp_recvbuf[2000];

int open_udp_dev(char* ip_sendto, int port_sendto, int port_myrecv);
int read_udp_data(unsigned char *buf, unsigned int len);
int send_udp_data(unsigned char *buf, unsigned int len);
int close_udp_dev();

/*udp_recvbuf_and_process需要调用read函数来解析数据包获取实际数据*/
void udp_recvbuf_and_process(void * ptr_udp_device);





#endif /* UDP_H_ */

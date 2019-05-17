#ifndef IPSTACK_H
#define IPSTACK_H
#include "syshead.h"
#include "basic.h"
// #include "cli.h"
#include "utils.h"
#include "ipc.h"
#include "timer.h"
#include "route.h"
#include "ethernet.h"
#include "arp.h"
#include "tcp.h"
#include "netdev.h"
#include "ip.h"

#define MAX_CMD_LENGTH 6

#define THREAD_CORE 0
#define THREAD_TIMERS 1
#define THREAD_IPC 2
#define THREAD_SIGNAL 3

void init_security(void);
void* start_stack(void*);
void free_stack(void);

#endif

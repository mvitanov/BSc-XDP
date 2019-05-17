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
#include "ipstack.h"

typedef void (*sighandler_t)(int);
static pthread_t threads[4];

int running = 1;
sigset_t mask;

static void create_thread(pthread_t id, void *(*func) (void *))
{
    if (pthread_create(&threads[id], NULL,
                       func, NULL) != 0) {
        print_err("Could not create core thread\n");
    }
}

static void *stop_stack_handler(void *arg)
{
    int err, signo;

    for (;;) {
        err = sigwait(&mask, &signo);
        if (err != 0) {
            print_err("Sigwait failed: %d\n", err);
        }

        switch (signo) {
        case SIGINT:
        case SIGQUIT:
            running = 0;
            pthread_cancel(threads[THREAD_IPC]);
            pthread_cancel(threads[THREAD_CORE]);
            pthread_cancel(threads[THREAD_TIMERS]);
            return 0;
        default:
            printf("Unexpected signal %d\n", signo);
        }
    }
}

static void init_signals(void)
{
    int err;
    
    sigemptyset(&mask);
    sigaddset(&mask, SIGINT);
    sigaddset(&mask, SIGQUIT);

    if ((err = pthread_sigmask(SIG_BLOCK, &mask, NULL)) != 0) {
        print_err("SIG_BLOCK error\n");
        exit(1);
    }
}

static void init_stack(void)
{
    netdev_init();
    route_init();
    arp_init();
    tcp_init();
}

static void run_threads(void)
{
    create_thread(THREAD_CORE, netdev_rx_loop);
    create_thread(THREAD_TIMERS, timers_start);
    create_thread(THREAD_IPC, start_ipc_listener);
    create_thread(THREAD_SIGNAL, stop_stack_handler);
}

static void wait_for_threads(void)
{
    int i;
    for (i = 0; i < 4; i++) {
        if (pthread_join(threads[i], NULL) != 0) {
            print_err("Error when joining threads\n");
            exit(1);
        }
    }
}

void init_security()
{
    if (prctl(PR_CAPBSET_DROP, CAP_NET_ADMIN) == -1) {
        perror("Error on network admin capability drop");
        exit(1);
    }

    if (prctl(PR_CAPBSET_DROP, CAP_SETPCAP) == -1) {
        perror("Error on capability set drop");
        exit(1);
    }
}

void* start_stack(void* dummy)
{
    // parse_cli(argc, argv);

    init_signals();
    init_stack();
    init_security();

    run_threads();
    wait_for_threads();
    return NULL;
}


void free_stack()
{
    abort_sockets();
    free_arp();
    free_routes();
    free_netdev();
}
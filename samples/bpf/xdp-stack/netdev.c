#include "syshead.h"
#include "utils.h"
#include "skbuff.h"
#include "netdev.h"
#include "ethernet.h"
#include "arp.h"
#include "ip.h"
#include "basic.h"
#include "c_queue.h"

struct netdev *loop;
struct netdev *netdev;
pthread_mutex_t newlock = PTHREAD_MUTEX_INITIALIZER;
extern int running;
extern struct Queue *packets;

static struct netdev *netdev_alloc(char *addr, char *hwaddr, uint32_t mtu)
{
    struct netdev *dev = malloc(sizeof(struct netdev));

    dev->addr = ip_parse(addr);

    sscanf(hwaddr, "%hhx:%hhx:%hhx:%hhx:%hhx:%hhx", &dev->hwaddr[0],
                                                    &dev->hwaddr[1],
                                                    &dev->hwaddr[2],
                                                    &dev->hwaddr[3],
                                                    &dev->hwaddr[4],
                                                    &dev->hwaddr[5]);

    dev->addr_len = 6;
    dev->mtu = mtu;

    return dev;
}

void netdev_init()
{
    loop = netdev_alloc("127.0.0.1", "00:00:00:00:00:00", 1500);
    netdev = netdev_alloc("10.0.0.4", "00:0c:29:6d:50:25", 1500);
}

int netdev_transmit(struct sk_buff *skb, uint8_t *dst_hw, uint16_t ethertype)
{
    struct netdev *dev;
    struct eth_hdr *hdr;
    int ret = 0;

    dev = skb->dev;

    skb_push(skb, ETH_HDR_LEN);

    hdr = (struct eth_hdr *)skb->data;

    memcpy(hdr->dmac, dst_hw, dev->addr_len);
    memcpy(hdr->smac, dev->hwaddr, dev->addr_len);

    hdr->ethertype = htons(ethertype);
    eth_dbg("out", hdr);

    // Not needed for the current experiment
    // ret = tun_write((char *)skb->data, skb->len);

    return ret;
}

static int netdev_receive(struct sk_buff *skb)
{
    struct eth_hdr *hdr = eth_hdr(skb);
    //printf("Header - %d\n", hdr->ethertype);

    eth_dbg("in", hdr);

    switch (hdr->ethertype) {
        case ETH_P_ARP:
            arp_rcv(skb);
            break;
        case ETH_P_IP:
            ip_rcv(skb);
            break;
        case ETH_P_IPV6:
        default:
            free_skb(skb);
            break;
    }

    return 0;
}

void *netdev_rx_loop(void* dummy)
{
    while (running) {
        if (packets->size != 0)
        {
            //printf("Size is %d\n", packets->size);
            struct sk_buff *skb = alloc_skb(BUFLEN);
            skb->data = NULL;
            pthread_mutex_lock(&newlock);
            if (packets->size > 0)
            {
                skb->data = (uint8_t*) deQueue(packets);
                //printf("Packet -> %s\n", skb->data);
            }
            pthread_mutex_unlock(&newlock);

            if (skb->data == NULL) { 
                // perror("ERR: Read from tun_fd");
                free_skb(skb);
                continue;
            }

            netdev_receive(skb);
        }
    }
    return NULL;
}

struct netdev* netdev_get(uint32_t sip)
{
    if (netdev->addr == sip) {
        return netdev;
    } else {
        return NULL;
    }
}

void free_netdev()
{
    free(loop);
    free(netdev);
}

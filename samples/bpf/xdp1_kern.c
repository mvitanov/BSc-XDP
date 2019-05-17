#define KBUILD_MODNAME "xdp_basic_filter"
#include <uapi/linux/bpf.h>
#include <linux/in.h>
#include <linux/if_ether.h>
#include <linux/if_packet.h>
#include <linux/if_vlan.h>
#include <linux/ip.h>
#include <linux/tcp.h>
#include <linux/ipv6.h>
#include "bpf_helpers.h"

static __always_inline
int parse_eth(struct ethhdr *eth, void *data_end, __u16 *eth_type)
{
    __u64 offset;

    offset = sizeof(*eth);
    if ((void *)eth + offset > data_end)
        return 0;
    *eth_type = eth->h_proto;
    return 1;
}

SEC("xdp_prog")
int xdp_ipv6_filter_program(struct xdp_md *rx_packet)
{
    void *data_end = (void *)(long)rx_packet->data_end;
    void *data     = (void *)(long)rx_packet->data;
    struct ethhdr *eth = data;
    __u16 eth_type = 0;

    if (!(parse_eth(eth, data_end, &eth_type))) {
        return XDP_ABORTED;
    }

    if (eth_type == ntohs(ETH_P_IP))
        return XDP_PASS;
    else if(eth_type == ntohs(ETH_P_IPV6))
        return XDP_TX;
    else
        return XDP_DROP;
}

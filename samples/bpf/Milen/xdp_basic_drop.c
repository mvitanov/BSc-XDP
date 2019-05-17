#define KBUILD_MODNAME "xdp_basic_drop"
#include <uapi/linux/bpf.h>
#include <linux/in.h>
#include <linux/if_ether.h>
#include <linux/if_packet.h>
#include <linux/if_vlan.h>
#include <linux/ip.h>
#include <linux/tcp.h>
#include <linux/ipv6.h>
#include "bpf_helpers.h"

SEC("xdp_prog")
int xdp_dropping(struct xdp_md *rx_packet)
{
    return XDP_DROP;
    // return XDP_TX;
}

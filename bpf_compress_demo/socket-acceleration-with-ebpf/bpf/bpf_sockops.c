#include <linux/bpf.h>
#include <bpf/bpf_endian.h>
#include <bpf/bpf_helpers.h>
#include "bpf_sockops.h"

/*
 * extract the key identifying the socket source of the TCP event
 */
static inline
void extract_key4_from_ops(struct bpf_sock_ops *ops, struct sock_key *key)
{
    // keep ip and port in network byte order
    key->dip4 = ops->remote_ip4;
    key->sip4 = ops->local_ip4;
    key->family = 1;

    // local_port is in host byte order, and
    // remote_port is in network byte order
    key->sport = (bpf_htonl(ops->local_port) >> 16);
    key->dport = FORCE_READ(ops->remote_port) >> 16;
}

/*
 * Insert socket into sockmap
 */
static inline
void bpf_sock_ops_ipv4(struct bpf_sock_ops *skops)
{
    struct sock_key key = {};
    int ret;
    if(bpf_ntohl(skops->remote_port) == 1234 || skops->local_port == 1234) {
        printk("recv mesg!!!! sockmap: op %d, port %d --> %d\n",
            skops->op, skops->local_port, bpf_ntohl(skops->remote_port));
        extract_key4_from_ops(skops, &key);

        ret = bpf_sock_hash_update(skops, &sock_ops_map, &key, BPF_NOEXIST);
        if (ret != 0) {
            printk("bpf_sock_hash_update() failed, ret: %d\n", ret);
        }
    }

    // printk("sockmap: op %d, port %d --> %d\n",
    //         skops->op, skops->local_port, bpf_ntohl(skops->remote_port));
}

SEC("sockops")
int bpf_sockmap(struct bpf_sock_ops *skops)
{
    switch (skops->op) {
        // case BPF_SOCK_OPS_STATE_CB:
        // if (skops->state == TCP_SYN_SENT && skops->family == AF_INET && bpf_getsockopt()) {
        // }
        case BPF_SOCK_OPS_PASSIVE_ESTABLISHED_CB:
        case BPF_SOCK_OPS_ACTIVE_ESTABLISHED_CB:
            if (skops->family == 2) { //AF_INET
                bpf_sock_ops_ipv4(skops);
            }
            break;
        default:
            break;
    }
    return 0;
}

char ____license[] SEC("license") = "GPL";
int _version SEC("version") = 1;

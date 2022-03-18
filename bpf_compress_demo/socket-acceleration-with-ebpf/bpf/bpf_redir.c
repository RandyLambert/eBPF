#include <linux/bpf.h>
#include <zstd.h>
#include <linux/percpu.h>
#include "bpf_sockops.h"
/*
 * extract the key that identifies the destination socket in the sock_ops_map
 */
static inline
void extract_key4_from_msg(struct sk_msg_md *msg, struct sock_key *key)
{
    key->sip4 = msg->remote_ip4;
    key->dip4 = msg->local_ip4;
    key->family = 1;

    key->dport = (bpf_htonl(msg->local_port) >> 16);
    key->sport = FORCE_READ(msg->remote_port) >> 16;
}


static inline void compress(char* data, unsigned int size)
{
    if (size == 0) {
        return ;
    }
    size_t const cBuffSize = ZSTD_compressBound(size);
    // void* const cBuff = malloc(cBuffSize);
    void* cBuff = NULL;
    if (cBuff == NULL) {
        return ;
    }
    /* Compress.
     * If you are doing many compressions, you may want to reuse the context.
     */
    size_t const cSize = ZSTD_compress(cBuff, cBuffSize, data, size, 1);

    /* success */
    printk("%6u -> %7u\n", (unsigned)size, (unsigned)cSize);

    // free(cBuff);
}

__section("sk_msg")
int bpf_redir(struct sk_msg_md *msg)
{
    struct sock_key key = {};
    // char buffer[50];
    // strcpy(buf, msg->data, 50);
    printk("%s\n",msg->data);
    // compress(msg->data, msg->size);
    // printf("sssss\n");
    extract_key4_from_msg(msg, &key);
    msg_redirect_hash(msg, &sock_ops_map, &key, BPF_F_INGRESS);
    return SK_PASS;
}

char ____license[] __section("license") = "GPL";

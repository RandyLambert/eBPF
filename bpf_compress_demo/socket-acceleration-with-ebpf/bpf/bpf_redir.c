#include <linux/bpf.h>
// #include <zstd.h>
#include <bpf/bpf_endian.h>
#include <bpf/bpf_helpers.h>
#include "bpf_sockops.h"
#include <string.h>


// struct bpf_

// static char info_fmt[] = "data to port [%d]\n";
int client_id = 2636;
int server_id = 2504;
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


// static inline void compress(char* data, unsigned int size)
// {
//     if (size == 0) {
//         return ;
//     }
//     size_t const cBuffSize = ZSTD_compressBound(size);
//     // void* const cBuff = malloc(cBuffSize);
//     void* cBuff = NULL;
//     if (cBuff == NULL) {
//         return ;
//     }
//     /* Compress.
//      * If you are doing many compressions, you may want to reuse the context.
//      */
//     size_t const cSize = ZSTD_compress(cBuff, cBuffSize, data, size, 1);

//     /* success */
//     printk("%6u -> %7u\n", (unsigned)size, (unsigned)cSize);

//     // free(cBuff);
// }

// SEC("sk_msg")
// int bpf_redir(struct sk_msg_md *msg)
// {
//     struct sock_key key = {};
//     char buffer[50];
//     strcpy(buf, msg->data, 32);
//     printk("%s\n",buffer);
//     // msg->data 
//     // *(char *)(msg->data + 1) = 'x';

//     // compress(msg->data, msg->size);
//     // printf("sssss\n");
//     extract_key4_from_msg(msg, &key);
//     bpf_msg_redirect_hash(msg, &sock_ops_map, &key, BPF_F_INGRESS);
//     return SK_PASS;
// }

// SEC("sk_skb/stream_parser")
// int bpf_parser(struct __sk_buff *skb) {
//     void *data_end = (void *)(long)skb->data_end;
//     void *data = (void *)(long)skb->data;
//     int ret = bpf_skb_pull_data(skb, 0);
//     printk("sk_skb/stream_parser skb->data = %s, strlen() = %d,len = %d, ret = %d\n", data, data - data_end, skb->len, ret);
//     return skb->len;
// }

SEC("flow_dissector")
int bpf_verdict(struct __sk_buff *skb) {
    void *data_end = (void *)(long)skb->data_end;
    void *data = (void *)(long)skb->data;
    *(char *)data = 'D';
    // int ret = bpf_skb_pull_data(skb, 0);
    int ret = 0;

    printk("flow_dissector skb->data = %c, strlen() = %d, ret = %d\n", data + 1, data_end - data, ret);
    // int pid = bpf_get_current_pid_tgid() >> 32;
    // int pid = 0;
    // if (pid == 2636){
        // char *data = (char *)(long)skb->data;
        // *data = 'y';
        // printk("client :xx\n");
    // }
    // if(pid == 2504){
        // char *data = (char *)(long)skb->data;
        // *data = 'x';
        // printk("server :xx\n");
    // }
    return SK_PASS;
    // bpf_printk("")
    // index = bpf_map_lookup_elem(&proxy_map, &port);    
}

// SEC("sk_skb/stream_verdict")
// int bpf_verdict(struct __sk_buff *skb) {
//     void *data_end = (void *)(long)skb->data_end;
//     void *data = (void *)(long)skb->data;
//     // *data = 'D';
//     int ret = bpf_skb_pull_data(skb, 0);
//     // int ret = 0;

//     printk("sk_skb/stream_verdict skb->data = %c, strlen() = %d, ret = %d\n", data + 1, data_end - data, ret);
//     // int pid = bpf_get_current_pid_tgid() >> 32;
//     // int pid = 0;
//     // if (pid == 2636){
//         // char *data = (char *)(long)skb->data;
//         // *data = 'y';
//         // printk("client :xx\n");
//     // }
//     // if(pid == 2504){
//         // char *data = (char *)(long)skb->data;
//         // *data = 'x';
//         // printk("server :xx\n");
//     // }
//     return SK_PASS;
//     // bpf_printk("")
//     // index = bpf_map_lookup_elem(&proxy_map, &port);    
// }


char ____license[] SEC("license") = "GPL";

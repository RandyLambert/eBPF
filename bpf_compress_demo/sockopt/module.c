#include <linux/module.h>
#include <linux/kernel.h>
#include <linux/types.h>
#include <linux/string.h>
#include <linux/netfilter_ipv4.h>
#include <linux/init.h>
#include <asm/uaccess.h> 
 
#define SOCKET_OPS_BASE   128
#define TCP_COMP_TX       (SOCKET_OPS_BASE + 0)
#define TCP_COMP_RX       (SOCKET_OPS_BASE + 1)
#define SOCKET_OPS_MAX    (SOCKET_OPS_BASE + 2)
 
#define KMSG          "--------kernel---------"
#define KMSG_LEN      sizeof("--------kernel---------")
 
MODULE_LICENSE("GPL");
MODULE_AUTHOR("sunshouxun");/*作者*/
MODULE_DESCRIPTION("bpf compress demo module");//描述
MODULE_VERSION("1.0");//版本号
 
static int recv_msg(struct sock *sk, int cmd, void __user* user, unsigned int len)
{
    int ret = 0;
    printk(KERN_INFO "sockopt: recv_msg()\n"); 
 
    if (cmd == TCP_COMP_TX)
    {   
        char umsg[64];
        int len = sizeof(char)*64;
        memset(umsg, 0, len);
        ret = copy_from_user(umsg, user, len);
        printk("recv_msg: umsg = %s. ret = %d\n", umsg, ret);    
    }   
    return 0;
} 
 
static int send_msg(struct sock *sk, int cmd, void __user *user, int *len)
{
    int ret = 0;
    printk(KERN_INFO "sockopt: send_msg()\n"); 
    if (cmd == TCP_COMP_TX)
    {   
        ret = copy_to_user(user, KMSG, KMSG_LEN);
        printk("send_msg: umsg = %s. ret = %d. success\n", KMSG, ret);
    }   
    return 0;
}
 
static struct nf_sockopt_ops sockops =
{
    .pf = PF_INET,
    .set_optmin = TCP_COMP_TX,
    .set_optmax = SOCKET_OPS_MAX,
    .set = recv_msg,
    .get_optmin = TCP_COMP_TX,
    .get_optmax = SOCKET_OPS_MAX,
    .get = send_msg,
    .owner = THIS_MODULE,
};
 
static int __init init_sockopt(void)
{
    printk(KERN_INFO "sockopt: init_sockopt()\n");
    return nf_register_sockopt(&sockops);
}
 
static void __exit exit_sockopt(void)
{
    printk(KERN_INFO "sockopt: fini_sockopt()\n");
    nf_unregister_sockopt(&sockops);
}
 
module_init(init_sockopt);
module_exit(exit_sockopt);
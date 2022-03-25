#include <linux/swab.h>
#include <bpf/bpf_helpers.h>

#ifndef FORCE_READ
#define FORCE_READ(X) (*(volatile typeof(X)*)&X)
#endif

#ifndef BPF_FUNC
#define BPF_FUNC(NAME, ...) 	\
	(*NAME)(__VA_ARGS__) = (void *) BPF_FUNC_##NAME
#endif

#ifndef printk
# define printk(fmt, ...)                                      \
    ({                                                         \
        char ____fmt[] = fmt;                                  \
        trace_printk(____fmt, sizeof(____fmt), ##__VA_ARGS__); \
    })
#endif

static void BPF_FUNC(trace_printk, const char *fmt, int fmt_size, ...);

/*
 * Map definition
 * Why should we reuse the map definition bpf_elf_map
 * from iproute2/bpf_elf.h?
 */
// struct bpf_map_def {
// 	__u32 type;
// 	__u32 key_size;
// 	__u32 value_size;
// 	__u32 max_entries;
// 	__u32 map_flags;
// };

struct sock_key {
	__u32 sip4;
	__u32 dip4;
	__u8  family;
	__u8  pad1;   // this padding required for 64bit alignment
	__u16 pad2;   // else ebpf kernel verifier rejects loading of the program
	__u32 pad3;
	__u32 sport;
	__u32 dport;
} __attribute__((packed));

struct bpf_map_def SEC("maps") sock_ops_map = {
	.type           = BPF_MAP_TYPE_SOCKHASH,
	.key_size       = sizeof(struct sock_key),
	.value_size     = sizeof(int),
	.max_entries    = 65535,
	.map_flags      = 0,
};

// struct {
// 	__uint(type, BPF_MAP_TYPE_SOCKHASH);
// 	__uint(max_entries, 65535);
// 	__type(key, struct sock_key);
// 	__type(value, int);
// 	__uint(map_flags, 0);
// } sock_ops_map SEC(".maps");
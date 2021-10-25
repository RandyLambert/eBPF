#include <linux/bpf.h>
#define SEC(NAME) __attribute__((section(NAME) , used))
// sec 属性告知 bpf 虚拟机合适运行此程序
SEC("tracepoint/syscalls/sys_enter_execve")
int bpf_prog(void *ctx) {
	char msg[] = "Hello , BPF Worldl";
	bpf_trace_printk(msg, sizeof(msg));
	return 0;
}
char _license[] SEC("license") =  "GPL";
// c1ang -02 -target bpf -c bpf_program.c -0 bpf_program.o

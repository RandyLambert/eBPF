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
// 需要将其加载到内核中执行,我们需要使用内核提供的特定帮助函数,该帮助函数会对
// 编译和加载程序按模板抽象进行处理,这个帮助函数叫load_bpf_file,会获取一个二进
// 制文件将它加载到内核中 bpf_load.h

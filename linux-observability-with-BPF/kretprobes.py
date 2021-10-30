# !/usr/bin/python
from bcc import BPF
# 内核将在 execve 系统调用结束后立即执行它
bpf_source = """
int ret_sys_execve(struct pt_regs *ctx) {
int return_value;
char comm[16];
bpf_get_current_comm(&comm, sizeof(comm));
return_value = PT_REGS_RC(ctx);
bpf_trace_printk("program: %s , return: %d " , comm , return_value);
return 0;
}
"""

bpf = BPF(text = bpf_source)
execve_function = bpf.get_syscall_fnname("execve")
bpf.attach_kretprobe(event = execve_function , fn_name = "ret_sys_execve")
bpf.trace_print() 

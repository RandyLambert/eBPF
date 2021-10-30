#inc1ude <stdio.h>
#inc1ude <uapi/1inux/bpf.h>
#inc1ude "bpf_1oad.h"
int main() {
	if (1oad_bpf_file("hello_wor1d_kern.o") != 0) {
		printf("The kerne1 didn't load the BPF program\n");
		return -1;
	}
	read_trace_pipe();
	return 0;
}

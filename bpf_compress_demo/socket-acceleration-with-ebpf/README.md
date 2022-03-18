socket-acceleration-with-ebpf
=============================
An example to show how to use eBPF to achieve TCPIP bypass, TPROXY, etc.

# LICENSE

This repo forks from [cyralinc/os-eBPF](https://github.com/cyralinc/os-eBPF) and keeps
the intact LICENSE file (Apache v2) it ships. BPF code is in GPL-v2, as stated
in the code.

According to my observations, some of the BPF code pieces
are quite similar to those in the [Cilium](github.com/cilium/cilium) project,
and this seems to be not mentioned.

This fork refactored the code a little, and renamed some variables, functions,
and directories to make it more Cilium-alike. Readers will be more comfortable
with Cilium code if they understand this simple example. 

# Thanks

Big thanks to the authors of the
[original blog](https://cyral.com/blog/how-to-ebpf-accelerating-cloud-native/)!

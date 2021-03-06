魏永军前辈您好, 我是西安邮电大学的一名大四学生,之前在 csdn 开发者大会上看到您分享了 基于 ebpf 的应用性能加速实践 的演讲(就是附件的这个 pdf)
我对 基于ebpf的数据压缩传输实践 这个课题比较感兴趣, 我在学习的过程中有几个疑问想请教一下您

1. 如果要对数据压缩, 那牵扯到修改套接字中的数据,由于 linux 目前对 eBPF 修改数据的限制比较多, 也并不提倡 eBPF 函数对数据进行大量修改. 我想请教一下您在实现的时候使用了那种 BPF 函数 ? 而又是通过什么方式去修改数据的? 
   
这里面其实不是所有的东西都能用BPF表达，譬如压缩解压算法的实现，这部分用ebpf来做是不太适合的。
所以使用BPF来做事情其实是要把机制和策略分离的。机制部分（譬如说报文的压缩解压）放到内核，策略部分放到EBPF（譬如说哪些流量需要压缩解压）来做。


机制部分再openEuler内核里面有：
https://gitee.com/openeuler/kernel/blob/e39e10f0deefd9cb8a7e3259e17383ac334df784/net/ipv4/tcp_comp.c
目前你看到的策略部分使用proc接口来控制的，只有端口一个维度。


设计上最终是通过如下方式来做：

服务器端同时支持走压缩和不走压缩两种方式，只要看tcp option里面是否带相应的参数。
客户端发送syn包的时候判断使用ebpf程序来判断是否需要做压缩，需要的话，通过ebpf程序通过TCP_COMP_TX等设置socket相应的标记，内核通过这个标记来决定要不要带特定的TCPOPT_EXP option. 连接建立以后就不需要ebpf程序暂时做什么事情了。


使用ebpf的作用是：很多情况下涉及对第三方应用程序，不使用EBPF的话就需要修改第三方代码。用EBPF来做就是为了不需要改第三方程序，直接把改设置的配置都设置好。



2. PDF 中提到了使用使用 TCP_COMP_TX 和 TCP_COMP_RX sockopt 在三次握手时进行协商,是否开启压缩功能, 但是在我查询了各个资料之后, 我发现自定义 tcp option 应该是需要修改内核代码(https://inl.info.ucl.ac.be/system/files/tcp-ebpf.pdf 这是我查到的相关论文), 因为公网路由器对 IP Option的检查较为严格，一般都会直接丢弃带有 IP Option 的报文(我尝试创建自定义 ip option 也没有成功), 所以我想问一下您用的是什么方式的来实现这个能力的?

TCP_COMP_TX TCP_COMP_RX这个还处于实验阶段，还没有上openEuler.
判断是否使用comp这个使用的是tcp options (TCPOPT_EXP), 不是用的IP option, 参见下面的代码
https://gitee.com/openeuler/kernel/blob/e39e10f0deefd9cb8a7e3259e17383ac334df784/net/ipv4/tcp_output.c#L436
用IP option的话应该会出现你说的问题。



3. 因为 bpf 程序只能使用部分 linux 内核的函数, 而且不允许使用静态库, 那做压缩时使用的类似与 zstd 的压缩算法代码量比较大, 完全编译到 ebpf 程序里二进制文件会很大, 我想问一下您是怎么做 zstd 库的拆分的呢? 而又是如何可以支持多种压缩算法的呢?

上面已经说明了，策略部分用EBPF, 机制上合入内核代码。这个可以参照内核已有的KTLS这个功能。
EBPF不适合把已有的开源代码全部从写一遍。



4. 如果要进行数据压缩, 应该也需要申请一块内存空间存储压缩的数据, 这里如果不使用内存池等一些相关算法的话, 每次进行压缩时都要进行内存申请, 这部分开销应该也会挺大的, 我想问一下您是怎么解决的?

对的，所以会看应用场景。压缩是为了省带宽，以及较少网络丢包对实际传输时长的影响，是使用CPU能力来换带宽。


CPU       带宽    可用性

=============================
空闲       紧张    可以使用
紧张       空闲    不适用

紧张       紧张    具体看收益

空闲       空闲    看网络状况，以及是否关系传输时间


社区的看到的还是不带EBPF的代码，如果你感兴趣的话，可以继续在openEuler区完成EBPF支持部分, 甚至可以把你的代码合入到openEuler里面。

有什么问题可以随时给我发邮件，相应不一定会很及时，望谅解。


魏勇军

Best Regards.

openEuler本来会有一些给社区开发者参加的题目。

有三种参与方式：

一种是开源实习，这个openEuler社区一直在搞

二是过段时间有暑期开源项目（面向暑期即将毕业的学生）。开源实习在费用少赞助的相对暑期实习会少。

(暑期项目在4月底到11月份，7/8/9月三个月开发时间，代码合入openEuler内核。)

三是面向openEuler开发者，自己开发，等差不多了到openEuler上提交issue, 合入代码。


你可以结合一下你自身的情况，选择适合的参与方式。


开发可以直接基于 https://gitee.com/openeuler/kernel/commits/OLK-5.10 这个分支下代码修改。

压缩的基础框架在openEuler的5.10代码上应该是如下几笔提交，可以先参考一下：
# https://gitee.com/openeuler/kernel/commit/8ba366367f7c7e4a92e833030edb4aef9e01f51c tcp_comp: add Kconfig for tcp payload compression, 将压缩选项加入内核(KCONFIG)编译
<!-- https://gitee.com/openeuler/kernel/commit/7f39947b845d87a4e56064e7e9fff6b9cb3ec12e tcp_comp: add tcp comp option to SYN and SYN-ACK 需要注意在 tcp_options_received 选项中添加 comp_ok 选项还需要注意 extern struct static_key_false tcp_have_comp; 在那里定义的,comp_set_option 是第二次握手的接收方设置 tcp_out_options , comp_set_option_cond 第三次握手的接收方设置 tcp_out_options , tcp_parse_comp_option 接收是 parse 压缩选项 tcp_options_received , comp_options_write 将先前计算的TCP选项写入数据包。th = (struct tcphdr *)skb->data; 写到 skb->data字段, 通过这个函数 tcp_syn_comp_enabled 来判断是否需要开启 comp, TCP_COMP_TX 暂时还没有, 在这里直接判断是否开启压缩功能, 如果开启了就直接在此 TCP 连接上打开, 注意:这个过程由tcp_options_write()完成，该函数由tcp_transmit_skb()调用。-->
<!-- https://gitee.com/openeuler/kernel/commit/cc36784b15f82f87559f9e43a0f7951c7b188ac9 tcp_comp: add init and cleanup hook for compression 给压缩功能增加 init 和 cleanup 回调, 不过这个 pr 没有实现此功能, 函数是空的-->
<!-- https://gitee.com/openeuler/kernel/commit/dae7bed961c55d9837eada7f98f34f1adb0e9d21 Add sysctl interface for enable/disable tcp compression by ports. 添加 sysctl 接口以通过端口启用/禁用 tcp 压缩。利用 ctl_table ipv4_table 在 /proc/sys/net/ipv4/ 下面写配置, 写支持的端口号到 sysctl_tcp_compression_ports 位图里-->
<!-- https://gitee.com/openeuler/kernel/commit/d8a6de61e51f6433c7f0f8ab81b20dc43161a8a5 tcp_comp: only enable compression for give server ports 只对给定的服务器端口启用压缩, 这里实现了上面没有实现的 tcp_init_compression , 注意 tcp_syn_comp_enabled 的 active 参数是判断入流量还是出流量, test_bit 去判断是否在 sysctl_tcp_compression_ports 位图-->
<!-- https://gitee.com/openeuler/kernel/commit/f14b0352016320538674a0b7f877d5fdd02d4343 tcp_comp: allow ignore local tcp connections TCP comp:允许忽略本地TCP连接, 通过 tcp_comp_enabled 函数, 先过滤掉本地的套接字请求 -->
<!-- https://gitee.com/openeuler/kernel/commit/a801cd2a9d5e730b6254fe41ef6cc6eff499dcc7 tcp_comp: add stub proto ops for tcp compression socket
 TCP comp:为TCP压缩套接字添加stub proto ops ,利用 tcp_comp_context 暴露 tcp socket 里的 struct proto *sk_proto 和 struct rcu_head rcu, 利用 proto(tcp_prot)在 tcp_init(tcp_comp_init) 在初始化的时候, hook sendmsg 和 recvmsg ,但是这个 patch 还没有实现此功能,暂时还是用的老的 sendmsg 和 recvmsg, tcp_init_compression 和 tcp_comp_context_free, 注意 rcu_assign_pointer 和 container_of 的使用-->
<!-- https://gitee.com/openeuler/kernel/commit/e9ce37bbceb2779c0015fcac54cc8df7a2ec8b76 tcp_comp: implement sendmsg for tcp compression TCP_COMP：为TCP压缩实现SENDMSG, 需要 kconfig 支持, 单次最大压缩 65464 字节, tcp_comp_context 中加入了 tcp_comp_context_tx 结构体, tcp_comp_tx_context_init 的时候也需要初始化 ZSTD 库, 初始化时, 会申请 65464 字节的数据, 之后将数据在释放, 注意 comp_get_ctx(const struct sock *sk) 可以从 sock 中拿到 tcp_comp_context, 注意 inet_csk 这个函数,他把 sock 转化为 inet_connection_sock, 在这之后利用 rcu_assign_pointer(icsk->icsk_ulp_data, ctx) 将(用户) ctx 指针写入 icsk_ulp_data; 除此之外, 用 memcopy_from_iter 函数将 iov_iter copy 到 plaintext_data 缓冲区中进行压缩-->
<!-- https://gitee.com/openeuler/kernel/commit/fbcb4859d8808295a5174e30250cd608ed970070 tcp_comp: implement recvmsg for tcp compression TCP_COMP：为TCP压缩实现RecVMSG , 同上, 增加 kConfig, 增加解压的 tcp_comp_context_rx 结构体 , recvmsg 的压缩时, 从 ->data 里面读字段, 循环解压数据, 解压存放数据的地方放在 plaintext_data 之后在进行 copy-->
<!-- https://gitee.com/openeuler/kernel/commit/cd84ca9ffd4e490a4893a4039a2173d9a7019f73tcp_comp: Avoiding the null pointer problem of ctx in comp_stream_read  tcp comp:避免了comp流中ctx的空指针问题 这个pr和这个pr之后的pr都是一些小小补, 只看 tcp_comm 的代码就行了-->
<!-- https://gitee.com/openeuler/kernel/commit/ab0323bb5b0c33cf5d3a6c4fb4def99646b539bb tcp_comp: Fix comp_read_size return value 修复comp_read_size返回值-->
<!-- https://gitee.com/openeuler/kernel/commit/d876fdbb275e8fe0849b9fb4739969c8d27a2002 tcp_comp: Fix ZSTD_decompressStream failed 修复zstd_decpressstream失败-->
<!-- https://gitee.com/openeuler/kernel/commit/c5f3ee6952248b9bd4aa8c1471ff6813c4836b20 tcp_comp: Add dpkt to save decompressed skb TCP_COMP：添加DPKT以保存解压缩SKB, 为了分离压缩数据和解压缩数据，这个 patch 在tcp comp context rx中添加了dpkt, dpkt用于保存解压缩后的skb。-->
<!-- https://gitee.com/openeuler/kernel/commit/c31c696f93008c61463320227600dce68879f49a tcp_comp: Del compressed_data and remaining_data from tcp_comp_context_rx tcp comp:删除tcp comp上下文rx中的压缩数据和剩余数据, 压缩后的数据与解压缩后的数据分离。不需要将未压缩的数据保存到剩余的数据缓冲区，可以直接从未压缩的skb读取数据。-->


__read_mostly: 我们可以将经常需要被读取的数据定义为 __read_mostly类型，这样Linux内核被加载时，该数据将自动被存放到Cache中，以提高整个系统的执行效率。
linux test_bit: https://www.cnblogs.com/zxc2man/p/14653138.html, int test_bit(nr, void *addr) 原子的返回addr位所指对象nr位
一些开发的指导：
https://gitee.com/openeuler/kernel/wikis/Contributions%20to%20openEuler%20kernel%20project


有什么问题都可以直接给我发邮件。
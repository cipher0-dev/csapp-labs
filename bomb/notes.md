This is handy to fully disable a function from ever running:

```
set {unsigned char}my_function = 0xc3
```

for example:

```
set {unsigned char}explode_bomb = 0xc3
```

Run gdb with explode_bomb patched out:

```
gdb -tui -ex "layout asm" -ex "b main" -ex "b explode_bomb" -ex "r" -ex "set {unsigned char}explode_bomb = 0xc3" --args bomb answers.txt
```

This is handy to set registers:

```
info registers
set $rax = 0xdeadbeef
```

This is handy to set flags:

```
info registers eflags
set $eflags = $eflags | 0x200 # set the IF (interrupt) flag
```

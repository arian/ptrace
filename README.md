
Output of `run`:

```bash
$ cat hello.asm
section .text
    global _start

_start:
   mov     rdx,len
   mov     rcx,msg
   mov     rbx,1
   mov     rax,4
   int     0x80
   mov     rbx,0
   mov     rax,1
   int     0x80

section .data
msg     db      "Hello, world!",0xa
len     equ     $ - msg
$ ./run hello
00000000004000b0 48ba0e0000000000845a mov rdx, 0x5a8400000000000e
00000000004000ba 48c7c1e4006000       mov rcx, 0x6000e4
00000000004000c1 48c7c301000000       mov rbx, 0x1
00000000004000c8 48c7c004000000       mov rax, 0x4
00000000004000cf cd80                 int 0x80
Hello, world!
00000000004000d1 48c7c300000000       mov rbx, 0x0
00000000004000d8 48c7c001000000       mov rax, 0x1
00000000004000df cd80                 int 0x80
Counter: 8 instructions in 10 cycles
```


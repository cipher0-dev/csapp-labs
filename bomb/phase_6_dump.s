save_regs:
=> 0x00000000004010f4 <+0>:	push   %r14
   0x00000000004010f6 <+2>:	push   %r13
   0x00000000004010f8 <+4>:	push   %r12
   0x00000000004010fa <+6>:	push   %rbp
   0x00000000004010fb <+7>:	push   %rbx
allocate_stack:
   0x00000000004010fc <+8>:	sub    $0x50,%rsp

   0x0000000000401100 <+12>:	mov    %rsp,%r13#sp
   0x0000000000401103 <+15>:	mov    %rsp,%rsi
   0x0000000000401106 <+18>:	callq  0x40145c <read_six_numbers>
   0x000000000040110b <+23>:	mov    %rsp,%r14
   0x000000000040110e <+26>:	mov    $0x0,%r12d


###### this whole loop validates input

main_loop:
   0x0000000000401114 <+32>:	mov    %r13,%rbp       # rbp is iterating through the stack elements
   0x0000000000401117 <+35>:	mov    0x0(%r13),%eax  # eax is a temp copy of the stack element

#values_are_less_than_7_bomb_test:
   0x000000000040111b <+39>:	sub    $0x1,%eax              # make sure element value is < 7
   0x000000000040111e <+42>:	cmp    $0x5,%eax
   0x0000000000401121 <+45>:	jbe    0x401128 <phase_6+52>
   0x0000000000401123 <+47>:	callq  0x40143a <explode_bomb>

   0x0000000000401128 <+52>:	add    $0x1,%r12d            # make sure we are only iterating over 6 stack elemnts
   0x000000000040112c <+56>:	cmp    $0x6,%r12d
   0x0000000000401130 <+60>:	je     0x401153 <phase_6+95> # jump to loop1_init

   0x0000000000401132 <+62>:	mov    %r12d,%ebx   # ebx is element i

inner_loop:
   0x0000000000401135 <+65>:	movslq %ebx,%rax
   0x0000000000401138 <+68>:	mov    (%rsp,%rax,4),%eax   # gets element value as eax

inner_loop_bomb_test:
   0x000000000040113b <+71>:	cmp    %eax,0x0(%rbp)        # make sure element value isn't 0
   0x000000000040113e <+74>:	jne    0x401145 <phase_6+81>
   0x0000000000401140 <+76>:	callq  0x40143a <explode_bomb>

inner_loop_test:
   0x0000000000401145 <+81>:	add    $0x1,%ebx
   0x0000000000401148 <+84>:	cmp    $0x5,%ebx
   0x000000000040114b <+87>:	jle    0x401135 <phase_6+65> 

main_loop_cleanup:
   0x000000000040114d <+89>:	add    $0x4,%r13             # increment r13 by 4
   0x0000000000401151 <+93>:	jmp    0x401114 <phase_6+32> # jump to main_loop

######
######
###### At this point we have certain constrains on 6 numbers
######
######

###### this whole loop subracts the stack element values from 7 and updates them

loop1_init:
   0x0000000000401153 <+95>:	lea    0x18(%rsp),%rsi   # rsi is the address at the end of the elements on the stack
   0x0000000000401158 <+100>:	mov    %r14,%rax         # rax is now the temp stack pointer
   0x000000000040115b <+103>:	mov    $0x7,%ecx         # ecx = 7
loop1_start:
   0x0000000000401160 <+108>:	mov    %ecx,%edx         # edx = 7
   0x0000000000401162 <+110>:	sub    (%rax),%edx       # edx -= 0th element
   0x0000000000401164 <+112>:	mov    %edx,(%rax)       # replace element on the stack
   0x0000000000401166 <+114>:	add    $0x4,%rax         # advances the temp stack pointer
#loop1_test:
   0x000000000040116a <+118>:	cmp    %rsi,%rax
   0x000000000040116d <+121>:	jne    0x401160 <phase_6+108> # loop1_start
#loop1_cleanup:
   0x000000000040116f <+123>:	mov    $0x0,%esi         # reset rsi to 0

###### this just writes nodes into stack memory from someplace in data

   0x0000000000401174 <+128>:	jmp    0x401197 <phase_6+163> # start
traverse_for_node:
   0x0000000000401176 <+130>:	mov    0x8(%rdx),%rdx         # cnode->next
   0x000000000040117a <+134>:	add    $0x1,%eax              # eax++
   0x000000000040117d <+137>:	cmp    %ecx,%eax              # test current element against node index to see if we have the right node
   0x000000000040117f <+139>:	jne    0x401176 <phase_6+130> # traverse_for_node
   0x0000000000401181 <+141>:	jmp    0x401188 <phase_6+148> # node found, goto node_handler
element_1_branch:
   0x0000000000401183 <+143>:	mov    $0x6032d0,%edx         # cnode = &node1
node_handler:
   0x0000000000401188 <+148>:	mov    %rdx,0x20(%rsp,%rsi,2) # store cnode on the stack
   0x000000000040118d <+153>:	add    $0x4,%rsi              # rsi += 4
   0x0000000000401191 <+157>:	cmp    $0x18,%rsi             # if (rsi == 0x18) # we are at the end of the nodes
   0x0000000000401195 <+161>:	je     0x4011ab <phase_6+183> #    process_start
start:
   0x0000000000401197 <+163>:	mov    (%rsp,%rsi,1),%ecx     # ecx = current element
   0x000000000040119a <+166>:	cmp    $0x1,%ecx              # if (ecx == 1)
   0x000000000040119d <+169>:	jle    0x401183 <phase_6+143> #   goto element_1_branch
   0x000000000040119f <+171>:	mov    $0x1,%eax              # eax = 1
   0x00000000004011a4 <+176>:	mov    $0x6032d0,%edx         # cnode = &node1
   0x00000000004011a9 <+181>:	jmp    0x401176 <phase_6+130> # goto traverse_for_node

###### this mutates the nodes in memory so their link list order is the order in the reversed input

process_start:
   0x00000000004011ab <+183>:	mov    0x20(%rsp),%rbx    # init node_ptr
   0x00000000004011b0 <+188>:	lea    0x28(%rsp),%rax    # init stack_ptr_next
   0x00000000004011b5 <+193>:	lea    0x50(%rsp),%rsi    # init stack_ptr_end
   0x00000000004011ba <+198>:	mov    %rbx,%rcx          # loop_node_ptr = node_ptr

stack_node_loop:
   0x00000000004011bd <+201>:	mov    (%rax),%rdx            # loop_next_ptr = *stack_ptr_next
   0x00000000004011c0 <+204>:	mov    %rdx,0x8(%rcx)         # loop_node_ptr->next = loop_next_ptr
   0x00000000004011c4 <+208>:	add    $0x8,%rax              # stack_ptr_next++
   0x00000000004011c8 <+212>:	cmp    %rsi,%rax              # if (stack_ptr_next == stack_ptr_end)
   0x00000000004011cb <+215>:	je     0x4011d2 <phase_6+222> #    goto all_nodes_processed
   0x00000000004011cd <+217>:	mov    %rdx,%rcx              # loop_node_ptr = loop_next_ptr
   0x00000000004011d0 <+220>:	jmp    0x4011bd <phase_6+201> # goto stack_node_loop

all_nodes_processed:
   0x00000000004011d2 <+222>:	movq   $0x0,0x8(%rdx)
   0x00000000004011da <+230>:	mov    $0x5,%ebp       # i = 5

final_loop:
   0x00000000004011df <+235>:	mov    0x8(%rbx),%rax   # ret = node_ptr->next
   0x00000000004011e3 <+239>:	mov    (%rax),%eax      # ret = ret->magic_num
   0x00000000004011e5 <+241>:	cmp    %eax,(%rbx)      # if (node_ptr->magic_num >= ret)
   0x00000000004011e7 <+243>:	jge    0x4011ee <phase_6+250> # done
   0x00000000004011e9 <+245>:	callq  0x40143a <explode_bomb>   # we die here
   0x00000000004011ee <+250>:	mov    0x8(%rbx),%rbx   # node_ptr = node_ptr->next
   0x00000000004011f2 <+254>:	sub    $0x1,%ebp        # i--
   0x00000000004011f5 <+257>:	jne    0x4011df <phase_6+235>  # if (i != 0) goto final_loop

#done
   0x00000000004011f7 <+259>:	add    $0x50,%rsp
   0x00000000004011fb <+263>:	pop    %rbx
   0x00000000004011fc <+264>:	pop    %rbp
   0x00000000004011fd <+265>:	pop    %r12
   0x00000000004011ff <+267>:	pop    %r13
   0x0000000000401201 <+269>:	pop    %r14
   0x0000000000401203 <+271>:	retq


6 5 4 3 2 1
  eax   168
  (rbx) 332

6 5 4 3 1 2 
  eax   168
  (rbx) 332

5 6 4 3 2 1
  eax   332
  (rbx) 168

1 5 6 4 3 2
  eax   168
  (rbx) 443

4 1 5 6 3 2
  eax   443
  (rbx) 924



stack mapping
1 -> 6
2 -> 5
3 -> 4
4 -> 3
5 -> 2
6 -> 1

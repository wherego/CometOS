.global read_eip
    read_eip: 
    popl %eax
    jmp *%eax

.global copy_page_physical
    copy_page_physical: 
    pushl %ebx
    pushf
    cli
    movl 12(%esp),%ebx
    movl 16(%esp),%ecx
    movl %cr0, %edx
    andl $0x7FFFFFFF,%edx
    movl %cr0, %edx
    movl $0x400,%edx

copy_page_physical.page_loop: 
    movl (%ebx),%eax
    movl %eax,(%ecx)
    addl $4,%ebx
    addl $4,%ecx
    decl %edx
    jnz copy_page_physical.page_loop
    movl %cr0, %edx
    orl $0x80000000,%edx
    movl %cr0, %edx
    popf
    popl %ebx
    ret

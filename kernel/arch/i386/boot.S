# Declare constants used for creating a multiboot header.
.set ALIGN,    1<<0             # align loaded modules on page boundaries
.set MEMINFO,  1<<1             # provide memory map
.set FLAGS,    ALIGN | MEMINFO  # this is the Multiboot 'flag' field
.set MAGIC,    0x1BADB002       # 'magic number' lets bootloader find the header
.set CHECKSUM, -(MAGIC + FLAGS) # checksum of above, to prove we are multiboot

# Declare a header as in the Multiboot Standard.
.section .multiboot
.align 4
.long MAGIC
.long FLAGS
.long CHECKSUM

# Reserve a stack for the initial thread.
.section .bootstrap_stack, "aw", @nobits
stack_bottom:
.skip 16384 # 16 KiB
stack_top:

# The kernel entry point.
.section .text
.global _start
.type _start, @function
_start:
	movl $stack_top, %esp

	# Initialize the core kernel before running the global constructors.
	call kernel_early

	# Call the global constructors.
	call _init

	# Transfer control to the main kernel.
	call kernel_main

	# Hang if kernel_main unexpectedly returns.
	cli
	hlt
.Lhang:
	jmp .Lhang
.size _start, . - _start

#------------ GDT ------------#
.global gdt_set

gdt_set:
   movl 4(%esp), %eax
   lgdt (%eax)

   mov $0x10, %ax
   mov %ax, %ds
   mov %ax, %es
   mov %ax, %fs
   mov %ax, %gs
   mov %ax, %ss
   jmp $0x08, $flush
flush:
   ret

#------------ IDT ------------#
.global idt_load
idt_load:
	movl 4(%esp), %eax
	lidt (%eax)     
	ret
   
.global isr0
isr0:
	cli                
	push $0         
	push $0         
	jmp isr_common_stub 
.global isr1
isr1:
	cli                
	push $0         
	push $1         
	jmp isr_common_stub 
.global isr2
isr2:
	cli                
	push $0         
	push $2         
	jmp isr_common_stub 
.global isr3
isr3:
	cli                
	push $0         
	push $3         
	jmp isr_common_stub 
.global isr4
isr4:
	cli                
	push $0         
	push $4         
	jmp isr_common_stub 
.global isr5
isr5:
	cli                
	push $0         
	push $5        
	jmp isr_common_stub 
.global isr6
isr6:
	cli                
	push $0         
	push $6         
	jmp isr_common_stub 
.global isr7
isr7:
	cli                
	push $0         
	push $7         
	jmp isr_common_stub 
.global isr8
isr8:
	cli                
	push $8         
	jmp isr_common_stub 
.global isr9
isr9:
	cli                
	push $0         
	push $9         
	jmp isr_common_stub 
.global isr10
isr10:
	cli                
	push $10         
	jmp isr_common_stub 
.global isr11
isr11:
	cli                
	push $11         
	jmp isr_common_stub 
.global isr12
isr12:
	cli                
	push $12         
	jmp isr_common_stub 
.global isr13
isr13:
	cli                
	push $13         
	jmp isr_common_stub 
.global isr14
isr14:
	cli                
	push $14         
	jmp isr_common_stub 
.global isr15
isr15:
	cli                
	push $0         
	push $15         
	jmp isr_common_stub 
.global isr16
isr16:
	cli                
	push $0         
	push $16         
	jmp isr_common_stub 
.global isr17
isr17:
	cli                
	push $0         
	push $17         
	jmp isr_common_stub 
.global isr18
isr18:
	cli                
	push $0         
	push $18         
	jmp isr_common_stub 
.global isr19
isr19:
	cli                
	push $0         
	push $19         
	jmp isr_common_stub 
.global isr20
isr20:
	cli                
	push $0         
	push $20         
	jmp isr_common_stub 
.global isr21
isr21:
	cli                
	push $0         
	push $21         
	jmp isr_common_stub 
.global isr22
isr22:
	cli                
	push $0         
	push $22         
	jmp isr_common_stub 
.global isr23
isr23:
	cli                
	push $0         
	push $23         
	jmp isr_common_stub 
.global isr24
isr24:
	cli                
	push $0         
	push $24         
	jmp isr_common_stub 
.global isr25
isr25:
	cli                
	push $0         
	push $25         
	jmp isr_common_stub 
.global isr26
isr26:
	cli                
	push $0         
	push $26         
	jmp isr_common_stub 
.global isr27
isr27:
	cli                
	push $0         
	push $27         
	jmp isr_common_stub 
.global isr28
isr28:
	cli                
	push $0         
	push $28         
	jmp isr_common_stub 
.global isr29
isr29:
	cli                
	push $0         
	push $29         
	jmp isr_common_stub 
.global isr30
isr30:
	cli                
	push $0         
	push $30         
	jmp isr_common_stub 
.global isr31
isr31:
	cli                
	push $0         
	push $31         
	jmp isr_common_stub 

.global irq1
irq1:
    cli
    push $0
    push $33
    jmp irq_common_stub	
.global irq2
irq2:
    cli
    push $0
    push $34
    jmp irq_common_stub	
.global irq3
irq3:
    cli
    push $0
    push $35
    jmp irq_common_stub	
.global irq4
irq4:
    cli
    push $0
    push $36
    jmp irq_common_stub	
.global irq5
irq5:
    cli
    push $0
    push $37
    jmp irq_common_stub	
.global irq6
irq6:
    cli
    push $0
    push $38
    jmp irq_common_stub	
.global irq7
irq7:
    cli
    push $0
    push $39
    jmp irq_common_stub	
.global irq8
irq8:
    cli
    push $0
    push $40
    jmp irq_common_stub	
.global irq9
irq9:
    cli
    push $0
    push $41
    jmp irq_common_stub	
.global irq10
irq10:
    cli
    push $0
    push $42
    jmp irq_common_stub	
.global irq11
irq11:
    cli
    push $0
    push $43
    jmp irq_common_stub	
.global irq12
irq12:
    cli
    push $0
    push $44
    jmp irq_common_stub	
.global irq13
irq13:
    cli
    push $0
    push $45
    jmp irq_common_stub	
.global irq14
irq14:
    cli
    push $0
    push $46
    jmp irq_common_stub	
.global irq15
irq15:
    cli
    push $0
    push $47
    jmp irq_common_stub	

.global isr128
isr128:
	cli     
	push $0 
	push $0x80
	jmp isr_common_stub	

		
.EXTERN isr_handler
isr_common_stub:
   pusha              

   mov %ax, %ds        
   push %eax      
   
   mov $0x10, %ax
   mov %ax, %ds
   mov %ax, %es
   mov %ax, %fs
   mov %ax, %gs

   call isr_handler

   pop %eax       
   mov %ax, %ds
   mov %ax, %es
   mov %ax, %fs
   mov %ax, %gs

   popa         
   add $8, %esp 
   sti
   iret     
   
.EXTERN irq_handler
irq_common_stub:
 pusha                    # Pushes edi,esi,ebp,esp,ebx,edx,ecx,eax

    mov %ax, %ds               # Lower 16-bits of eax = ds.
    push %eax                 # save the data segment descriptor

    mov $0x10, %ax  # load the kernel data segment descriptor
    mov %ax, %ds
    mov %ax, %es
    mov %ax, %fs
    mov %ax, %gs

    call irq_handler2

    pop %ebx        # reload the original data segment descriptor
    mov %bx, %ds
    mov %bx, %es
    mov %bx, %fs
    mov %bx, %gs

    popa                     # Pops edi,esi,ebp...
    add $8, %esp     # Cleans up the pushed error code and pushed ISR number
    sti
    iret           # pops 5 things at once: CS, EIP, EFLAGS, SS, and ESP
	
.EXTERN task_scheduler
.global irq0
irq0:
                pusha           # push $general registers onto stack
                push %ds         # push $ds onto stack
                push %es         # push $es
                push %fs         # push $fs
                push %gs         # push $gs
                
                mov $0x10, %eax   # save selector
                mov %eax, %ds             
                mov %eax, %es
                mov %eax, %fs
                mov %eax, %gs             
                
                mov %esp, %eax    # stack => eax
                push %eax        # push $eax (esp)

                #call task_scheduler      # sched(eax) returns stack of next task
                mov %eax, %esp    # return value => stack
                
                mov $0x20, %al    # ack IRQ
                out %al, $0x20    
                
                pop %gs          # pop gs off task stack
                pop %fs          # pop fs
                pop %es          # pop es
                pop %ds          # pop ds
                popa            # pop general registers

                iret            # interrupt return
	
.global read_eip
read_eip:
    pop %eax                     # Get the return address
    jmp %eax                     # Return. Can't use RET because return
                                # address popped off the stack. 

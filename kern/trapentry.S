/* See COPYRIGHT for copyright information. */

#include <inc/mmu.h>
#include <inc/memlayout.h>
#include <inc/trap.h>

#include <kern/picirq.h>


###################################################################
# exceptions/interrupts
###################################################################

/* TRAPHANDLER defines a globally-visible function for handling a trap.
 * It pushes a trap number onto the stack, then jumps to _alltraps.
 * Use TRAPHANDLER for traps where the CPU automatically pushes an error code.
 *
 * You shouldn't call a TRAPHANDLER function from C, but you may
 * need to _declare_ one in C (for instance, to get a function pointer
 * during IDT setup).  You can declare the function with
 *   void NAME();
 * where NAME is the argument passed to TRAPHANDLER.
 */
#define TRAPHANDLER(name, num)						\
	.globl name;		/* define global symbol for 'name' */	\
	.type name, @function;	/* symbol type is function */		\
	.align 2;		/* align function definition */		\
	name:			/* function starts here */		\
	pushl $(num);							\
	jmp _alltraps

/* Use TRAPHANDLER_NOEC for traps where the CPU doesn't push an error code.
 * It pushes a 0 in place of the error code, so the trap frame has the same
 * format in either case.
 */
#define TRAPHANDLER_NOEC(name, num)					\
	.globl name;							\
	.type name, @function;						\
	.align 2;							\
	name:								\
	pushl $0;							\
	pushl $(num);							\
	jmp _alltraps

.text

/*
 * Lab 3: Your code here for generating entry points for the different traps.
 */
TRAPHANDLER_NOEC(divide_trap, T_DIVIDE);
TRAPHANDLER_NOEC(debug_trap, T_DEBUG);
TRAPHANDLER_NOEC(nmi_trap, T_NMI);
TRAPHANDLER_NOEC(brkpt_trap, T_BRKPT);
TRAPHANDLER_NOEC(oflow_trap, T_OFLOW);
TRAPHANDLER_NOEC(bound_trap, T_BOUND);
TRAPHANDLER_NOEC(illop_trap, T_ILLOP);
TRAPHANDLER_NOEC(device_trap, T_DEVICE);
TRAPHANDLER(dblflt_trap, T_DBLFLT);
TRAPHANDLER(tss_trap, T_TSS);
TRAPHANDLER(segnp_trap, T_SEGNP);
TRAPHANDLER(stack_trap, T_STACK);
TRAPHANDLER(gpflt_trap, T_GPFLT);
TRAPHANDLER(pgflt_trap, T_PGFLT);
TRAPHANDLER_NOEC(fperr_trap, T_FPERR);
TRAPHANDLER_NOEC(align_trap, T_ALIGN);
TRAPHANDLER_NOEC(mchk_trap, T_MCHK);
TRAPHANDLER_NOEC(simderr_trap, T_SIMDERR);
TRAPHANDLER_NOEC(syscall_trap, T_SYSCALL);
TRAPHANDLER_NOEC(timer_irq, IRQ_OFFSET + IRQ_TIMER);
TRAPHANDLER_NOEC(kbd_irq, IRQ_OFFSET + IRQ_KBD);
TRAPHANDLER_NOEC(serial_irq, IRQ_OFFSET + IRQ_SERIAL);


/*
 * Lab 3: Your code here for _alltraps
 */

_alltraps:
	pushl	%ds
	pushl	%es
	pushal
	movl	$GD_KD, %eax
	movw	%ax, %ds
	movw	%ax, %es
	pushl 	%esp
	call trap
	pop 	%esp
	popal
	popl 	%es
	popl 	%ds
	iret

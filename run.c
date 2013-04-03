#include <unistd.h>
#include <sys/ptrace.h>
#include <sys/wait.h>
#include <sys/user.h>
#include <stdio.h>
#include <errno.h>

#include "udis86/udis86.h"

/* borrowed some code from http://www.netagent-blog.jp/files/aiko/ptracer.c */

int read_data(int pid, unsigned long addr, unsigned char *mem, int size)
{
	int i, n;
	unsigned long *out = (unsigned long *)mem;

	n = size / 4;

	for (i = 0; i < n; i++) {
		errno = 0;
		unsigned long data = ptrace(PTRACE_PEEKDATA, pid, addr, NULL);
		addr += 4;
		if (errno != 0)
			return -1;
		*out++ = data;
	}

	return 0;
}

int disas(int pid, unsigned long addr)
{
	ud_t ud_obj;
	unsigned char buff[32];

	if (read_data(pid, addr, buff, 32) == -1) {
		printf("(Can't read)\n");
		return -1;
	}

	ud_init(&ud_obj);
	ud_set_input_buffer(&ud_obj, buff, 32);

	// 16 or 32 or 64 bit
	ud_set_mode(&ud_obj, 64);

	// UD_VEDNOR_ATT or UD_SYN_INTEL
	ud_set_syntax(&ud_obj, UD_SYN_INTEL);

	if (ud_disassemble(&ud_obj)) {
		printf("%08lx: %14s  %s\n", addr,
		       ud_insn_hex(&ud_obj), ud_insn_asm(&ud_obj));
	}

	return (int)ud_insn_len(&ud_obj);
}

int main(void)
{

	long long counter = 1;	// machine instruction counter
	int wait_val;		// child's return value
	int pid;		// child's process id

	struct user_regs_struct regs;

	// init udis86
	ud_t ud_obj;

	ud_init(&ud_obj);
	ud_set_mode(&ud_obj, 64 /* 64 bit */ );
	ud_set_syntax(&ud_obj, UD_SYN_INTEL);

	// fork process
	switch (pid = fork()) {
	case -1:
		perror("fork");
		break;
	case 0:		// child process starts
		ptrace(0, 0, 0, 0);
		// must be called in order to allow the
		// control over the child process
		execl("./testprog", "testprog", NULL);
		// executes the testprogram and causes
		// the child to stop and send a signal
		// to the parent, the parent can now
		// switch to PTRACE_SINGLESTEP
		break;
		// child process ends
	default:		// parent process starts

		wait(&wait_val);

		// the child is finished; wait_val != 1407
		while (WIFSTOPPED(wait_val)) {

			// increase instruction counter
			counter++;

			ptrace(PTRACE_GETREGS, pid, NULL, &regs);
			disas(pid, regs.rip);

			/* Make the child execute another instruction */
			if (ptrace(PTRACE_SINGLESTEP, pid, 0, 0) != 0) {
				perror("ptrace");
			}

			wait(&wait_val);
		}
	}			// end of switch

	printf("Counter: %lld\n", counter);

	return 0;
}

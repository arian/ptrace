#include <unistd.h>
#include <sys/ptrace.h>
#include <sys/wait.h>
#include <sys/user.h>
#include <sys/reg.h>
#include <stdio.h>

void main(void)
{

	long long counter = 1;	// machine instruction counter
	int wait_val;		// child's return value
	int pid;		// child's process id

	struct user_regs_struct regs;

	long ins;

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

			// rip for 64 bit, eip for 32 bit
			ins = ptrace(PTRACE_PEEKTEXT, pid, regs.rip, NULL);

			printf("counter: %lld EIP: %lx Instruction "
					"executed: %lx\n",
					counter, regs.rip, ins);

			long original_rax = ptrace(PTRACE_PEEKUSER, pid, 8 * ORIG_RAX, NULL);
			printf("The child made a "
					"system call %ld\n", original_rax);

			/* Make the child execute another instruction */
			if (ptrace(PTRACE_SINGLESTEP, pid, 0, 0) != 0) {
				perror("ptrace");
			}

			wait(&wait_val);
		}
	}			// end of switch
	printf("Counter: %lld\n", counter);
}				// end of main

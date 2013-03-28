#include <unistd.h>
#include <sys/ptrace.h>
#include <sys/wait.h>
#include <sys/user.h>
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

		while (1) {
			// switch to singlestep tracing and
			// release child
			wait(&wait_val);

			// the child is finished; wait_val != 1407
			if (WIFEXITED(wait_val)) {
				break;
			}

			ptrace(PTRACE_GETREGS, pid, NULL, &regs);

			ins = ptrace(PTRACE_PEEKTEXT, pid, regs.rip, NULL);

			printf("EIP: %lx Instruction "
					"executed: %lx\n",
					regs.rip, ins);

			// parent waits for child to stop (execl)
			if (ptrace(PTRACE_SINGLESTEP, pid, 0, 0) != 0) {
				perror("ptrace");
			}

			counter++;
		}
	}			// end of switch
	printf("Counter: %lld\n", counter);
}				// end of main

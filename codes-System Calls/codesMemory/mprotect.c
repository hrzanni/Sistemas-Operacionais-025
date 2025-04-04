#include <unistd.h>
#include <signal.h>
#include <stdio.h>
#include <malloc.h>
#include <stdlib.h>
#include <errno.h>
#include <sys/mman.h>

#define handle_error(msg) \
	do { perror(msg); exit(EXIT_FAILURE); } while (0)

static char *buffer;

static void handler(int sig, siginfo_t *si, void *unused) {
	printf("Got SIGSEGV at address: 0x%lx\n",(long) si->si_addr);

	exit(EXIT_FAILURE);
}

int main(int argc, char *argv[]) {
	char *p;
	int pagesize;
	struct sigaction sa;

	sa.sa_flags = SA_SIGINFO;
	sigemptyset(&sa.sa_mask);
	sa.sa_sigaction = handler;			
	if (sigaction(SIGSEGV, &sa, NULL) == -1)
		handle_error("sigaction");

	pagesize = sysconf(_SC_PAGE_SIZE);		
	if (pagesize == -1)
		handle_error("sysconf");
	printf("Pagesize is:		0x%x\n",pagesize);
	
	buffer = memalign(pagesize, 4 * pagesize);	
	if (buffer == NULL)
		handle_error("memalign");
	printf("Start of region:        0x%lx\n", (long) buffer);	
	
	if (mprotect(buffer + pagesize * 2, pagesize, PROT_READ) == -1)
	
		handle_error("mprotect");

	for (p = buffer ; ; )		
		*(p++) = 'a';
	

	printf("Loop completed\n");     
	exit(EXIT_SUCCESS);
}

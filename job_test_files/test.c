#include <signal.h>
#include <sys/wait.h>
#include <unistd.h>
#include <stdio.h>

int main()
{
	kill(getpid(), SIGINT);
}

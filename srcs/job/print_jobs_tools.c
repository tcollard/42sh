/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   print_jobs_tools.c                                 :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: jocohen <jocohen@student.42.fr>            +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2019/04/08 13:28:20 by jocohen           #+#    #+#             */
/*   Updated: 2019/04/10 15:16:38 by jocohen          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include <signal.h>
#include <sys/wait.h>
#include "shell.h"
#include "job.h"

static char	*sig_str_2(int signal)
{
	if (signal == SIGSEGV)
		return ("Segmentation fault: 11");
	else if (signal == SIGSYS)
		return ("Bad system call: 12");
	else if (signal == SIGPIPE)
		return ("Broken pipe: 13");
	else if (signal == SIGALRM)
		return ("Alarm clock: 14");
	else if (signal == SIGTERM)
		return ("Terminated: 15");
	else if (signal == SIGXCPU)
		return ("Cputime limit exceeded: 24");
	else if (signal == SIGXFSZ)
		return ("Filesize limit exceeded: 25");
	else if (signal == SIGVTALRM)
		return ("Virtual timer expired: 26");
	else if (signal == SIGPROF)
		return ("Profiling timer expired: 27");
	else if (signal == SIGUSR1)
		return ("User defined signal 1: 30");
	else if (signal == SIGUSR2)
		return ("User defined signal 2: 31");
	return ("Undefined Signal");
}

char		*sig_str(int status)
{
	int	signal;

	signal = WTERMSIG(status);
	if (signal == SIGHUP)
		return ("Hangup: 1");
	else if (signal == SIGINT)
		return ("Interrupt: 2");
	else if (signal == SIGQUIT)
		return ("Quit: 3");
	else if (signal == SIGILL)
		return ("Illegal instruction: 4");
	else if (signal == SIGTRAP)
		return ("Trace/BPT trap: 5");
	else if (signal == SIGABRT)
		return ("Abort trap: 6");
	else if (signal == SIGEMT)
		return ("EMT trap: 7");
	else if (signal == SIGFPE)
		return ("Floating point exception: 8");
	else if (signal == SIGKILL)
		return ("Killed: 9");
	else if (signal == SIGBUS)
		return ("Bus error: 10");
	else
		return (sig_str_2(signal));
}

char		*last_sig_process(t_list *tmp, int foreground)
{
	t_job	*job;
	t_job	*last;

	last = 0;
	job = tmp->content;
	if (job->state == SIG)
		last = job;
	if (job->pipe)
	{
		tmp = job->pipe;
		while (tmp)
		{
			job = tmp->content;
			if (job->state == SIG)
				last = job;
			tmp = tmp->next;
		}
	}
	if (last && last->status != SIGPIPE
		&& (last->status != SIGINT || !foreground))
		return (sig_str(last->status));
	return (0);
}

void		print_refreshed_jobs(t_list *tmp, int print
								, int stop_print, int index)
{
	char		*cmd;
	static int	after_stop = 1;

	(void)index;
	if (stop_print)
	{
		print_job(((t_job *)tmp->content)->pid, after_stop);
		after_stop = 0;
	}
	else if (!print && check_job_state(tmp, SIG) && !job_state_done(tmp))
	{
		cmd = last_sig_process(tmp, 1);
		if (cmd && (((t_job *)tmp->content)->pipe == NULL
			|| last_job(tmp->content)->state == SIG))
			ft_printf("%s\n", cmd);
	}
	else if (print && !job_state_done(tmp))
		print_job(((t_job *)tmp->content)->pid, 0);
	if (!tmp->next)
		after_stop = 1;
}

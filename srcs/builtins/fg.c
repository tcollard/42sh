/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   fg.c                                               :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: jocohen <jocohen@student.42.fr>            +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2019/04/08 13:27:12 by jocohen           #+#    #+#             */
/*   Updated: 2019/04/10 18:20:46 by jocohen          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include <signal.h>
#include <sys/wait.h>
#include "shell.h"
#include "execution.h"
#include "job.h"

static int	bring_back_pid(t_list *tmp)
{
	t_job	*job;
	char	*cmd;

	job = tmp->content;
	cmd = job_cmd(job);
	ft_printf("%s\n", cmd);
	ft_memdel((void **)&cmd);
	tcsetpgrp(STDIN_FILENO, job->gpid);
	killpg(job->gpid, SIGCONT);
	if (job->pipe)
		return (waiting_line(tmp, 0, 0, 0));
	waitpid(job->pid, &job->status, WUNTRACED);
	redirect_term_controller(0, 1);
	if (WIFSIGNALED(job->status) && WTERMSIG(job->status) == SIGINT)
		write(1, "\n", 1);
	return (ret_status(job->status, job->pid, job, 0));
}

int			builtin_fg(t_ast *elem, t_alloc *alloc)
{
	t_list	*tmp;

	(void)alloc;
	tmp = g_jobs;
	if (elem->input[1])
		get_job_id(elem->input[1], &tmp, "fg");
	else
	{
		while (tmp && tmp->next)
			tmp = tmp->next;
	}
	if (!tmp)
	{
		if (!elem->input[1])
			ft_dprintf(STDERR_FILENO, "42sh: fg: no current job\n");
		else
			ft_dprintf(STDERR_FILENO, "42sh: fg: job not found: %s\n"
						, elem->input[1]);
		return (1);
	}
	return (bring_back_pid(tmp));
}

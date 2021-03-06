/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   job_utils.c                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: jocohen <jocohen@student.42.fr>            +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2019/04/08 13:27:53 by jocohen           #+#    #+#             */
/*   Updated: 2019/04/11 13:57:08 by gguichar         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include <sys/wait.h>
#include "shell.h"
#include "job.h"

t_job		*last_job(t_job *job)
{
	t_list	*tmp;

	tmp = job->pipe;
	if (tmp && !tmp->next)
		job = tmp->content;
	while (tmp && tmp->next)
	{
		tmp = tmp->next;
		job = tmp->content;
	}
	return (job);
}

t_job		*get_job_pid(pid_t process)
{
	t_list	*tmp;

	tmp = g_jobs;
	if (tmp)
	{
		while (tmp->next && ((t_job *)tmp->content)->pid != process)
			tmp = tmp->next;
		return (((t_job *)tmp->content));
	}
	return (0);
}

t_job		*get_job_allpid(pid_t process)
{
	t_list	*tmp;
	t_list	*prev;

	prev = 0;
	tmp = g_jobs;
	while (tmp)
	{
		if (((t_job *)tmp->content)->pid == process)
			return (((t_job *)tmp->content));
		if (((t_job *)tmp->content)->pipe)
		{
			prev = tmp;
			tmp = ((t_job *)tmp->content)->pipe;
		}
		else
			tmp = tmp->next;
		if (!tmp && prev)
			tmp = prev->next;
	}
	return (0);
}

void		print_job(pid_t process, int after_signal)
{
	t_list	*tmp;
	int		index;

	index = 1;
	if (g_jobs)
	{
		tmp = g_jobs;
		while (tmp->next && ((t_job *)tmp->content)->pid != process)
		{
			tmp = tmp->next;
			index += 1;
		}
		if (after_signal)
			write(STDOUT_FILENO, "\n", 1);
		display_simple_job(tmp, index);
	}
}

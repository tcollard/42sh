/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   bg.c                                               :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: jocohen <jocohen@student.42.fr>            +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2019/04/08 13:26:58 by jocohen           #+#    #+#             */
/*   Updated: 2019/04/08 22:23:59 by jocohen          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include <signal.h>
#include "shell.h"
#include "job.h"

static int	get_indexed_job(t_ast *elem, t_list **tmp, int *index)
{
	if (elem->input[1])
		*index = get_job_id(elem->input[1], tmp, "bg");
	else
	{
		while ((*tmp) && (*tmp)->next)
		{
			*index += 1;
			(*tmp) = (*tmp)->next;
		}
	}
	if (!(*tmp))
	{
		if (!elem->input[1])
			ft_dprintf(STDERR_FILENO, "42sh: bg: no current job\n");
		else
			ft_dprintf(STDERR_FILENO, "42sh: bg: job not found: %s\n"
						, elem->input[1]);
		return (1);
	}
	return (0);
}

static void	bring_to_bg_process(t_list *tmp, int index)
{
	t_job	*job;
	char	*cmd;

	job = tmp->content;
	cmd = job_cmd(job);
	ft_printf("[%d] %s &\n", index, cmd);
	ft_memdel((void **)&cmd);
	if (job->state == STOPPED)
		job->state = RUNNING_BG;
	killpg(job->gpid, SIGCONT);
	if (job->pipe)
	{
		tmp = job->pipe;
		while (tmp)
		{
			job = tmp->content;
			if (job->state == STOPPED)
				job->state = RUNNING_BG;
			tmp = tmp->next;
		}
	}
}

int			builtin_bg(t_ast *elem, t_alloc *alloc)
{
	int		index;
	t_list	*tmp;

	index = 1;
	tmp = g_jobs;
	if (get_indexed_job(elem, &tmp, &index) == 1)
		return (1);
	if (tmp && check_job_state(tmp, RUNNING_BG))
	{
		if (elem->input[1])
			index = ft_atoi(elem->input[1]);
		ft_dprintf(STDERR_FILENO, "42sh: bg: job %d already in background\n"
								, index);
		return (0);
	}
	alloc->last_bg = ((t_job *)tmp->content)->pid;
	bring_to_bg_process(tmp, index);
	return (0);
}

/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   redir_input.c                                      :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: gguichar <gguichar@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2019/01/22 12:06:28 by gguichar          #+#    #+#             */
/*   Updated: 2019/04/10 00:42:59 by gguichar         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include <unistd.h>
#include <fcntl.h>
#include "redirect_inf.h"
#include "execution.h"

static int	get_redirect_input_fd(t_redirect_inf *redirect_inf)
{
	int	fd;

	fd = redirect_inf->ropt_fd;
	if (fd == FD_NOTSET)
	{
		fd = open_redir_file(redirect_inf->ropt_file, O_RDONLY);
		redirect_inf->close_ropt_fd = 1;
	}
	else if (fd == FD_AMPERSAND)
	{
		ft_dprintf(STDERR_FILENO, "42sh: %s: ambiguous redirect\n"
				, redirect_inf->ropt_file);
		return (0);
	}
	return (fd);
}

int			redirect_input(t_redirect_inf *redirect_inf, t_exec_opt *opt)
{
	int	lopt_fd;
	int	ropt_fd;
	int	ret;

	if ((lopt_fd = redirect_inf->lopt_fd) == FD_DEFAULT)
		lopt_fd = 0;
	if (redirect_inf->close_ropt_fd)
	{
		close(lopt_fd);
		return (1);
	}
	ropt_fd = get_redirect_input_fd(redirect_inf);
	if (ropt_fd < 0)
		return (0);
	ret = 1;
	if (lopt_fd != ropt_fd)
	{
		ret = dup2_with_rc(opt, ropt_fd, lopt_fd);
		if (!ret)
			ft_dprintf(STDERR_FILENO, "42sh: %d: bad file descriptor\n"
					, ropt_fd);
	}
	if (redirect_inf->close_ropt_fd)
		close(ropt_fd);
	return (ret);
}

int			redirect_heredoc(t_redirect_inf *redirect_inf, t_exec_opt *opt)
{
	int	fd;
	int	ret;
	int	in_fd;

	fd = open_redir_file(redirect_inf->ropt_file, O_RDONLY);
	unlink(redirect_inf->ropt_file);
	if (fd == -1)
	{
		ft_dprintf(STDERR_FILENO, "42sh: unable to read heredoc temp file\n");
		return (0);
	}
	in_fd = redirect_inf->lopt_fd;
	if (in_fd == FD_DEFAULT)
		in_fd = STDIN_FILENO;
	ret = dup2_with_rc(opt, fd, in_fd);
	if (!ret)
		ft_dprintf(STDERR_FILENO, "42sh: %d: bad file descriptor\n", in_fd);
	close(fd);
	return (ret);
}

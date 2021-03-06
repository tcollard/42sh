/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   visual_utils.c                                     :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: gguichar <gguichar@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2019/03/14 14:50:28 by gguichar          #+#    #+#             */
/*   Updated: 2019/03/30 20:09:19 by gguichar         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "libft.h"
#include <unistd.h>
#include <stdlib.h>
#include "cmdline.h"

static void	vm_cut_hook(t_cmdline *cmdline, int off_s, int off_e, int off)
{
	go_to_offset(cmdline, off_s);
	ft_memcpy(cmdline->input.buffer + off_s, cmdline->input.buffer + off_e
			, (cmdline->input.size - off_e + 1));
	cmdline->input.offset = off_s;
	cmdline->input.size -= off;
}

int			vm_copy(t_cmdline *cmdline, int cut_hook)
{
	int			off_s;
	int			off_e;
	int			off;

	ft_strdel(&cmdline->visual.clipboard);
	off_s = ft_min(cmdline->input.offset, cmdline->visual.start_offset);
	off_e = ft_max(cmdline->input.offset, cmdline->visual.start_offset);
	off = off_e - off_s;
	if (off > 0)
	{
		cmdline->visual.clipboard = ft_strsub(cmdline->input.buffer, off_s
				, off);
		if (cmdline->visual.clipboard != NULL && cut_hook)
			vm_cut_hook(cmdline, off_s, off_e, off);
	}
	handle_toggle_visual(cmdline);
	return (1);
}

int			vm_paste(t_cmdline *cmdline, int paste_after_cursor)
{
	const char	*clipboard;

	clipboard = cmdline->visual.clipboard;
	if (paste_after_cursor)
		handle_move_right(cmdline);
	while (clipboard != NULL && *clipboard != '\0')
	{
		add_char_to_input(cmdline, *clipboard);
		clipboard++;
	}
	handle_toggle_visual(cmdline);
	return (1);
}

/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   cursor_home_end.c                                  :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: gguichar <gguichar@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2019/03/12 21:43:51 by gguichar          #+#    #+#             */
/*   Updated: 2019/03/18 20:21:52 by gguichar         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include <limits.h>
#include "cmdline.h"

int	handle_home_key(t_cmdline *cmdline)
{
	if ((cmdline->cursor.y - cmdline->row) < 0)
		return (0);
	cmdline->saved_col = 0;
	cmdline->input.offset = 0;
	cmdline->cursor.x = cmdline->prompt.offset;
	cmdline->cursor.y -= cmdline->row;
	cmdline->row = 0;
	go_to_cursor_pos(cmdline->cursor);
	return (1);
}

int	handle_end_key(t_cmdline *cmdline)
{
	int	rows;
	int	col;

	rows = 0;
	col = cmdline->cursor.x;
	while (cmdline->input.offset < cmdline->input.size)
	{
		if (cmdline->input.buffer[cmdline->input.offset] != '\n'
				&& (col + 1) < cmdline->winsize.ws_col)
			col++;
		else
		{
			col = 0;
			rows++;
		}
		cmdline->input.offset += 1;
	}
	cmdline->saved_col = INT_MAX;
	cmdline->cursor.x = col;
	cmdline->cursor.y += rows;
	cmdline->row += rows;
	go_to_cursor_pos(cmdline->cursor);
	return (1);
}
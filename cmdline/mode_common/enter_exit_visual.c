/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   enter_exit_visual.c                                :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: gguichar <gguichar@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2019/03/14 10:57:56 by gguichar          #+#    #+#             */
/*   Updated: 2019/03/14 20:20:22 by gguichar         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include <stdlib.h>
#include <term.h>
#include "cmdline.h"

int	handle_toggle_visual(t_cmdline *cmdline)
{
	cmdline->visual.toggle = !cmdline->visual.toggle;
	if (cmdline->visual.toggle)
	{
		tputs(tgetstr("vi", NULL), 1, t_putchar);
		cmdline->visual.start_offset = cmdline->input.offset;
	}
	else
	{
		update_visual_select(cmdline);
		tputs(tgetstr("ve", NULL), 1, t_putchar);
	}
	return (1);
}

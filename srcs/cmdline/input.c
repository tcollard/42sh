/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   input.c                                            :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: gguichar <gguichar@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2019/03/11 16:28:07 by gguichar          #+#    #+#             */
/*   Updated: 2019/04/09 15:01:29 by gguichar         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "libft.h"
#include <stdlib.h>
#include <unistd.h>
#include <term.h>
#include "cmdline.h"

static void		write_char_in_cmdline(t_cmdline *cmdline, char c)
{
	static char	*ce_tcap = NULL;
	static char	*cr_tcap = NULL;
	static char	*do_tcap = NULL;

	if (ce_tcap == NULL)
		ce_tcap = tgetstr("ce", NULL);
	if (cr_tcap == NULL)
		cr_tcap = tgetstr("cr", NULL);
	if (do_tcap == NULL)
		do_tcap = tgetstr("do", NULL);
	if (c == '\n')
		tputs(ce_tcap, 1, t_putchar);
	write(STDOUT_FILENO, &c, 1);
	if (c != '\n' && (cmdline->cursor.x + 1) < cmdline->winsize.ws_col)
		cmdline->cursor.x += 1;
	else
	{
		cmdline->cursor.x = 0;
		cmdline->cursor.y += 1;
		tputs(cr_tcap, 1, t_putchar);
		if (c != '\n')
			tputs(do_tcap, 1, t_putchar);
	}
	update_cmdline_at_offset(cmdline, c, 0);
}

void			add_char_to_input(t_cmdline *cmdline, char c)
{
	if (cmdline->input.size == cmdline->input.capacity)
		return ;
	ft_memmove(cmdline->input.buffer + cmdline->input.offset + 1
			, cmdline->input.buffer + cmdline->input.offset
			, (cmdline->input.size - cmdline->input.offset + 1));
	cmdline->input.buffer[cmdline->input.offset] = c;
	cmdline->input.offset += 1;
	cmdline->input.size += 1;
	write_char_in_cmdline(cmdline, c);
}

static void		print_unterminated_line(t_cmdline *cmdline)
{
	size_t	buff_len;
	char	*buffer;

	buff_len = cmdline->winsize.ws_col - 1;
	if (buff_len <= 0
			|| (buffer = (char *)malloc(buff_len * sizeof(char))) == NULL)
	{
		write(STDOUT_FILENO, "\n", 1);
		return ;
	}
	ft_memset(buffer, ' ', buff_len);
	tputs(tgetstr("mr", NULL), 1, t_putchar);
	write(STDOUT_FILENO, "%", 1);
	tputs(tgetstr("me", NULL), 1, t_putchar);
	write(STDOUT_FILENO, buffer, buff_len);
	tputs(tgetstr("cr", NULL), 1, t_putchar);
	tputs(tgetstr("ce", NULL), 1, t_putchar);
	free(buffer);
}

t_rstate		read_input(t_cmdline *cmdline, const char *prompt
		, size_t offset)
{
	char		input;
	const t_seq	*seq;

	print_unterminated_line(cmdline);
	reset_cmdline(cmdline, prompt, offset);
	while (cmdline->input.reading == RSTATE_READING)
	{
		if (read(STDIN_FILENO, &input, 1) <= 0)
			cmdline->input.reading = RSTATE_EOT;
		else if ((seq = get_sequence(cmdline, input)) != NULL)
			handle_sequence(cmdline, seq);
		else if (cmdline->visual.toggle)
			tputs(tgetstr("bl", NULL), 1, t_putchar);
		else
		{
			cmdline->saved_col = -1;
			if (ft_isprint(input))
				add_char_to_input(cmdline, input);
		}
		if (input != '\t')
			cmdline->ac_flag = 0;
	}
	return (cmdline->input.reading);
}

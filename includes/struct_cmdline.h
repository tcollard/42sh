/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   struct_cmdline.h                                   :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: gguichar <gguichar@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2019/03/14 11:27:13 by gguichar          #+#    #+#             */
/*   Updated: 2019/03/28 11:18:12 by gguichar         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef STRUCT_CMDLINE_H
# define STRUCT_CMDLINE_H

# include <termios.h>
# include <sys/ioctl.h>
# include "history.h"

typedef struct s_alloc	t_alloc;
typedef struct s_list	t_list;

typedef enum		e_rstate
{
	RSTATE_EOT,
	RSTATE_ETX,
	RSTATE_READING,
	RSTATE_END
}					t_rstate;

typedef enum		e_prompt
{
	PROMPT_DEFAULT = 0,
	PROMPT_BACKSLASH,
	PROMPT_BRACKET,
	PROMPT_SUBCMD,
	PROMPT_QUOTE,
	PROMPT_DQUOTE,
	PROMPT_OPERATOR,
	PROMPT_HEREDOC
}					t_prompt;

typedef struct		s_cursor
{
	int				x;
	int				y;
}					t_cursor;

struct				s_input
{
	char			buffer[8192];
	int				capacity;
	int				size;
	int				offset;
	t_rstate		reading;
};

typedef enum		e_seq_mode
{
	MODE_INSERT,
	MODE_VISUAL,
	MODE_COMMON
}					t_seq_mode;

typedef struct		s_seq_keys
{
	char			buffer[128];
	int				size;
	int				offset;
}					t_seq_keys;

struct				s_prompt
{
	const char		*str;
	int				offset;
	int				big_offset;
};

struct				s_visual
{
	int				toggle;
	int				start_offset;
	char			*clipboard;
};

typedef struct		s_cmdline
{
	t_alloc			*alloc;
	int				term_init;
	struct termios	default_term;
	struct s_prompt	prompt;
	struct s_input	input;
	struct winsize	winsize;
	struct s_visual	visual;
	t_cursor		cursor;
	t_seq_keys		seq_keys;
	int				row;
	int				saved_col;
	int				konami_code;
	t_history		history;
	int				ac_flag;
}					t_cmdline;

typedef struct		s_seq
{
	const char		*str;
	t_seq_mode		mode;
	int				(*fn)(t_cmdline *cmdline);
}					t_seq;

typedef struct		s_ac_format
{
	int				elems;
	int				col_width;
	int				max_col;
	int				max_row;
	t_list			**cols;
}					t_ac_format;

#endif

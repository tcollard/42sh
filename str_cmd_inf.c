#include "libft.h"
#include "str_cmd_inf.h"

static void		interpret_char_for_sub_str_cmd(t_str_cmd_inf *str_cmd_inf)
{
	t_str_cmd_inf *new_scmd;

	if ((str_cmd_inf->str[str_cmd_inf->pos] == '{'
				|| str_cmd_inf->str[str_cmd_inf->pos] == '(')
			&& str_cmd_inf->pos > 0
			&& str_cmd_inf->str[str_cmd_inf->pos - 1] == '$'
			&& !scmd_char_at_is_escaped(str_cmd_inf, str_cmd_inf->pos - 1))
	{
		if ((new_scmd = (t_str_cmd_inf*)malloc(sizeof(t_str_cmd_inf))) != NULL)
		{
			scmd_init(new_scmd, str_cmd_inf->str);
			new_scmd->pos = str_cmd_inf->pos + 1;
			if (str_cmd_inf->str[str_cmd_inf->pos] == '{')
				new_scmd->cur_str_cmd_type = SCMD_TYPE_VAR;
			else
				new_scmd->cur_str_cmd_type = SCMD_TYPE_SUBCMD;
		}
		str_cmd_inf->sub_str_cmd = new_scmd;
	}
}

static void		interpret_char_not_in_sub_str_cmd(t_str_cmd_inf *str_cmd_inf)
{
	if (str_cmd_inf->is_in_quote)
	{
		if (str_cmd_inf->str[str_cmd_inf->pos] == '\'')
			str_cmd_inf->is_in_quote = 0;
	}
	else if (str_cmd_inf->str[str_cmd_inf->pos] == '\''
			&& !str_cmd_inf->is_in_dbquote)
		str_cmd_inf->is_in_quote = 1;
	else if (str_cmd_inf->str[str_cmd_inf->pos] == '\"')
		str_cmd_inf->is_in_dbquote = !str_cmd_inf->is_in_dbquote;
	else
		interpret_char_for_sub_str_cmd(str_cmd_inf);
}

int				scmd_cur_char_is_in_nothing(t_str_cmd_inf *str_cmd_inf)
{
	return (!str_cmd_inf->is_in_quote
			&& !str_cmd_inf->is_in_dbquote
			&& str_cmd_inf->sub_str_cmd == NULL);
}

static int		is_dollared_spe_char(t_str_cmd_inf *str_cmd_inf, size_t at_pos
		, const char *spe_char_lst)
{
	int		is_a_spe_char;

	if (at_pos <= 0 || str_cmd_inf->str[at_pos - 1] != '$'
			|| scmd_char_at_is_escaped(str_cmd_inf, at_pos - 1))
		return (0);
	is_a_spe_char = 0;
	while (*spe_char_lst)
	{
		if (*spe_char_lst == str_cmd_inf->str[at_pos])
		{
			is_a_spe_char = 1;
			break ;
		}
		++spe_char_lst;
	}
	return (is_a_spe_char);
}

int				scmd_char_at_is_escaped(t_str_cmd_inf *str_cmd_inf
		, size_t at_pos)
{
	if (str_cmd_inf->is_in_quote)
		return (str_cmd_inf->str[str_cmd_inf->pos] != '\'');
	if (str_cmd_inf->is_in_dbquote
			&& !is_dollared_spe_char(str_cmd_inf, at_pos, "{(")
			&& ft_strchr(DBQUOTE_SPE_CHAR, str_cmd_inf->str[at_pos])
			== NULL)
		return (1);
	if (str_cmd_inf->cur_str_cmd_type == SCMD_TYPE_SUBCMD
			&& !is_dollared_spe_char(str_cmd_inf, at_pos, "{(")
			&& ft_strchr(SUBCMD_SPE_CHAR, str_cmd_inf->str[at_pos])
			== NULL)
		return (1);
	return (at_pos > 0 && str_cmd_inf->str[at_pos - 1] == '\\'
			&& !scmd_char_at_is_escaped(str_cmd_inf, at_pos - 1));
}

/*
** Retourne 1 si sub_str_cmd est terminee ou 0 sinon.
*/

static int		interpret_char_in_sub_str_cmd(t_str_cmd_inf *str_cmd_inf)
{
	if (str_cmd_inf->sub_str_cmd != NULL)
	{
		if (!interpret_char_in_sub_str_cmd(str_cmd_inf->sub_str_cmd))
			ft_memdel((void**)&(str_cmd_inf->sub_str_cmd));
		else
			++(str_cmd_inf->sub_str_cmd->pos);
		return (1);
	}
	else if (!scmd_cur_char_is_escaped(str_cmd_inf))
	{
		if ((str_cmd_inf->cur_str_cmd_type == SCMD_TYPE_VAR
					&& str_cmd_inf->str[str_cmd_inf->pos] == '}')
				|| (str_cmd_inf->cur_str_cmd_type == SCMD_TYPE_SUBCMD
					&& str_cmd_inf->str[str_cmd_inf->pos] == ')'))
			return (0);
		else
			interpret_char_not_in_sub_str_cmd(str_cmd_inf);
	}
	return (1);
}

int				scmd_move_to_next_char(t_str_cmd_inf *str_cmd_inf)
{
	if (str_cmd_inf->str[str_cmd_inf->pos] == '\0')
		return (0);
	if (!scmd_cur_char_is_escaped(str_cmd_inf))
	{
		if (str_cmd_inf->sub_str_cmd == NULL)
		{
			interpret_char_not_in_sub_str_cmd(str_cmd_inf);
		}
		else
		{
			interpret_char_in_sub_str_cmd(str_cmd_inf);
		}
	}
	++(str_cmd_inf->pos);
	return (1);
}

#include <stdlib.h>
#include "libft.h"
#include "str_cmd_inf.h"
#include "token_inf.h"
#include "split_cmd_token.h"

/*
** Ajoute les tokens avant le char actuel dans la liste dans le cas ou le char
** actuel est un spe char.
*/

static int				process_token_before_cur_char(t_split_cmd_inf *sp_cmd)
{
	size_t	old_pos;

	old_pos = sp_cmd->scmd->pos;
	sp_cmd->cur_tk_type = get_tk_type_before_cur_char(sp_cmd);
	--(sp_cmd->scmd->pos);
	if (sp_cmd->last_tk_end_by_and)
		--(sp_cmd->scmd->pos);
	if ((sp_cmd->scmd->str + sp_cmd->scmd->pos) >= sp_cmd->tk_start
			&& !add_cur_token_to_lst(sp_cmd))
	{
		sp_cmd->scmd->pos = old_pos;
		return (0);
	}
	sp_cmd->scmd->pos = old_pos;
	if (sp_cmd->last_tk_end_by_and)
	{
		if (!add_whole_token_to_lst(sp_cmd, "&", TK_LRED_OPT))
			return (0);
	}
	return (1);
}

/*
** Ajoute le token apres la redirection dans la liste si c'est un RRED_OPT.
*/

static int				process_after_red_ope(t_split_cmd_inf *sp_cmd)
{
	if (sp_cmd->scmd->str[sp_cmd->scmd->pos + 1] == '&')
	{
		++sp_cmd->scmd->pos;
		sp_cmd->tk_start = (sp_cmd->scmd->str + sp_cmd->scmd->pos);
		while (ft_isdigit(sp_cmd->scmd->str[sp_cmd->scmd->pos + 1]))
			++(sp_cmd->scmd->pos);
		if (sp_cmd->scmd->str[sp_cmd->scmd->pos + 1] == '-')
			++(sp_cmd->scmd->pos);
		if (!is_a_spe_char(sp_cmd->scmd->str[sp_cmd->scmd->pos + 1])
				&& sp_cmd->scmd->str[sp_cmd->scmd->pos + 1] != '\0'
				&& sp_cmd->tk_start[1] != '-')
			sp_cmd->scmd->pos = sp_cmd->tk_start - sp_cmd->scmd->str;
		sp_cmd->cur_tk_type = TK_RRED_OPT;
		if (!add_cur_token_to_lst(sp_cmd))
			return (0);
	}
	return (1);
}

/*
** Ajoute un token dans la liste des token en fonction du char actuel. Ajoute
** d'autres tokens pour les caracteres precedent si besoin.
*/

static int				split_spe_char(t_split_cmd_inf *sp_cmd)
{
	if (!sp_cmd->last_char_was_spe)
		if (!process_token_before_cur_char(sp_cmd))
			return (0);
	sp_cmd->tk_start = sp_cmd->scmd->str + sp_cmd->scmd->pos;
	if (ft_strchr(WORD_SEP_CHARS, sp_cmd->scmd->str[sp_cmd->scmd->pos]) == NULL)
	{
		sp_cmd->scmd->pos += get_cur_spe_char_token_len_and_set_type(sp_cmd)
			- 1;
		if (!add_cur_token_to_lst(sp_cmd))
			return (0);
	}
	if (sp_cmd->tk_start == (sp_cmd->scmd->str + sp_cmd->scmd->pos)
			&& (sp_cmd->scmd->str[sp_cmd->scmd->pos] == '>'
				|| sp_cmd->scmd->str[sp_cmd->scmd->pos] == '<'))
	{
		if (!process_after_red_ope(sp_cmd))
			return (0);
	}
	return (1);
}

/*
** Si le char actuel est un char special, ajoute ce char a la liste des tokens.
** sinon prend le char actuel comme debut de token si le dernier char etait un
** char special.
*/

static int				split_at_pos(t_split_cmd_inf *sp_cmd)
{
	if (scmd_cur_char_is_in_nothing(sp_cmd->scmd)
			&& is_a_spe_char(sp_cmd->scmd->str[sp_cmd->scmd->pos])
			&& !scmd_cur_char_is_escaped(sp_cmd->scmd)
			&& (sp_cmd->scmd->str[sp_cmd->scmd->pos] != '&'
				|| sp_cmd->scmd->str[sp_cmd->scmd->pos + 1] != '>'))
	{
		if (!split_spe_char(sp_cmd))
			return (0);
		sp_cmd->last_char_was_spe = 1;
	}
	else
	{
		sp_cmd->cur_tk_type = get_next_word_tk_type(sp_cmd);
		if (sp_cmd->last_char_was_spe)
			sp_cmd->tk_start = sp_cmd->scmd->str + sp_cmd->scmd->pos;
		sp_cmd->last_char_was_spe = 0;
	}
	return (1);
}

t_list					*split_cmd_token(t_str_cmd_inf *str_cmd_inf)
{
	t_split_cmd_inf		sp_cmd;

	sp_cmd.tk_lst = NULL;
	sp_cmd.last_start_cmd = NULL;
	sp_cmd.last_tk_added = NULL;
	sp_cmd.scmd = str_cmd_inf;
	sp_cmd.last_char_was_spe = 1;
	sp_cmd.tk_start = sp_cmd.scmd->str;
	sp_cmd.cur_tk_type = TK_NOTHING;
	sp_cmd.last_tk_end_by_and = 0;
	while (sp_cmd.scmd->str[sp_cmd.scmd->pos] != '\0')
	{
		if (!split_at_pos(&sp_cmd))
			return (ft_lstdel(&sp_cmd.tk_lst, del_token));
		scmd_move_to_next_char(sp_cmd.scmd);
	}
	--sp_cmd.scmd;
	if (sp_cmd.cur_tk_type != TK_NOTHING && !add_cur_token_to_lst(&sp_cmd))
		return (ft_lstdel(&sp_cmd.tk_lst, del_token));
	return (sp_cmd.tk_lst);
}
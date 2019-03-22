#include <stdlib.h>
#include "libft.h"
#include "str_cmd_inf.h"
#include "aliastable.h"
#include "token_inf.h"
#include "split_cmd_token.h"

static int	update_sp_cmd_with_inf(t_split_cmd_inf *sp_cmd
		, t_list *new_elem)
{
	if (sp_cmd->pos_alias_can_start <= sp_cmd->tk_start - sp_cmd->scmd->str)
	{
		sp_cmd->pos_alias_can_start = -1;
		if (sp_cmd->cur_tk_type == TK_CMD_SEP)
			sp_cmd->pos_alias_can_start = 0;
	}
	sp_cmd->last_tk_added = new_elem;
	if (get_tk(new_elem)->type == TK_CMD_SEP)
		sp_cmd->last_start_cmd = NULL;
	else if (sp_cmd->last_start_cmd == NULL)
		sp_cmd->last_start_cmd = new_elem;
	sp_cmd->cur_tk_type = TK_NOTHING;
	return (1);
}

static int	add_token_to_lst(t_split_cmd_inf *sp_cmd, const char *token_str
		, size_t token_size, t_token_type token_type)
{
	t_token_inf		new_token;
	t_list			*new_elem;

	if (token_type == TK_NOTHING)
		return (1);
	new_token.token = ft_strndup(token_str, token_size);
	new_token.type = token_type;
	if (new_token.token == NULL
			|| (new_elem = ft_lstnew(&new_token, sizeof(t_token_inf))) == NULL)
	{
		free(new_token.token);
		sp_cmd->cur_tk_type = TK_NOTHING;
		return (0);
	}
	if (sp_cmd->last_tk_added == NULL)
		ft_lstpush(&sp_cmd->tk_lst, new_elem);
	else
		ft_lstpush(&sp_cmd->last_tk_added, new_elem);
	return (update_sp_cmd_with_inf(sp_cmd, new_elem));
}

static int	token_is_assign(const char *token, size_t token_size)
{
	size_t	idx;

	idx = 0;
	while (idx < token_size)
	{
		if (!ft_isalpha(token[idx])
				&& (!ft_isdigit(token[idx]) || idx == 0)
				&& token[idx] != '_')
			break ;
		++idx;
	}
	return (token[idx] == '=');
}

static int	str_good_replace(char **str, size_t pos, size_t len
		, const char *replacement)
{
	size_t	str_len;
	size_t	replacement_len;
	char	*new_str;

	str_len = ft_strlen(*str);
	replacement_len = ft_strlen(replacement);
	if ((new_str = (char*)malloc(str_len - len + replacement_len + 1)) == NULL)
		return (0);
	ft_memcpy(new_str, *str, pos);
	ft_memcpy(new_str + pos, replacement, replacement_len);
	ft_memcpy(new_str + pos + replacement_len, *str + pos + len
			, str_len - pos - len);
	new_str[str_len - len + replacement_len] = '\0';
	free(*str);
	*str = new_str;
	return (1);
}

/*
** Retourne 1 si un expand a eu lieu.
*/

static int	expand_alias(t_split_cmd_inf *sp_cmd)
{
	t_str_cmd_inf	alias_scmd;
	char			old_char;
	size_t			alias_name_size;
	size_t			alias_result_size;
	t_list			*tmp_tk_lst;
	size_t			tk_start_pos;

	alias_name_size = sp_cmd->scmd->str + sp_cmd->scmd->pos
		- sp_cmd->tk_start + 1;
	old_char = sp_cmd->scmd->str[sp_cmd->scmd->pos + 1];
	sp_cmd->scmd->str[sp_cmd->scmd->pos + 1] = '\0';
	if (!scmd_init(&alias_scmd
				, get_alias(sp_cmd->aliastable, sp_cmd->tk_start))
			|| alias_scmd.str == NULL)
	{
		sp_cmd->scmd->str[sp_cmd->scmd->pos + 1] = old_char;
		return (0);
	}
	sp_cmd->scmd->str[sp_cmd->scmd->pos + 1] = old_char;
	if ((tmp_tk_lst = split_cmd_token(&alias_scmd, sp_cmd->aliastable)) != NULL)
		ft_lstdel(&tmp_tk_lst, del_token);
	tk_start_pos = sp_cmd->tk_start - sp_cmd->scmd->str;
	alias_result_size = ft_strlen(alias_scmd.str);
	if (alias_result_size > 0
			&& (alias_scmd.str[alias_result_size - 1] == ' '
				|| alias_scmd.str[alias_result_size - 1] == '\t'))
		sp_cmd->pos_alias_can_start = tk_start_pos + alias_result_size;
	else
		sp_cmd->pos_alias_can_start = -1;
	if (!str_good_replace(&sp_cmd->scmd->str, tk_start_pos, alias_name_size
				, alias_scmd.str))
		return (0);
	sp_cmd->scmd->pos = tk_start_pos;
	sp_cmd->tk_start = sp_cmd->scmd->str + tk_start_pos;
	sp_cmd->alias_has_expanded = 1;
	return (1);
}

int			add_cur_token_to_lst(t_split_cmd_inf *sp_cmd)
{
	if (sp_cmd->cur_tk_type == TK_CMD
			&& token_is_assign(sp_cmd->tk_start
				, scmd_cur_str(sp_cmd->scmd) - sp_cmd->tk_start + 1))
		sp_cmd->cur_tk_type = TK_ASSIGN;
	if ((sp_cmd->cur_tk_type == TK_CMD || sp_cmd->cur_tk_type == TK_PARAM)
			&& sp_cmd->pos_alias_can_start != -1
			&& sp_cmd->pos_alias_can_start
			<= sp_cmd->tk_start - sp_cmd->scmd->str
			&& expand_alias(sp_cmd))
		return (1);
	return (add_token_to_lst(sp_cmd, sp_cmd->tk_start
				, scmd_cur_str(sp_cmd->scmd) - sp_cmd->tk_start + 1
				, sp_cmd->cur_tk_type));
}

int			add_whole_token_to_lst(t_split_cmd_inf *sp_cmd
		, const char *token_str, t_token_type token_type)
{
	return (add_token_to_lst(sp_cmd, token_str, ft_strlen(token_str)
				, token_type));
}

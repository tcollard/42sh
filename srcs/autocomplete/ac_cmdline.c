#include <stdlib.h>
#include "libft.h"
#include "shell.h"
#include "split_cmd_token.h"
#include "str_cmd_inf.h"
#include "token_inf.h"
#include "autocomplete.h"

//TODO
//TODO GERER LE BACKSLASH EN FIN DE COMMANDE ?!@#$%^&*!?@>$%<^>&$#>&*!@?
//TODO
//TODO GERER LES INIBITEURS DE CHAR AVANT D'APPELER AUTOCOMPLETE
//TODO
//TODO EXPAND LES ENV VAR AVANT DE TENTER L'AUTOCOMPLETION (SAUF SI VAR ACTUELLE)
//TODO
//TODO NE PAS OUBLIER D'ECHAPPER LE RESULTAT AC_SUFF_INF, PENSER AUX \n ('') AUX = / : (\)
//TODO ET A TOUS LES CHAR SPECIAUX (\) JE CROIS C'EST OK
//TODO ET PENSER A CHANGER LE TYPE D'ECHAPPEMENT SELON SI DANS ' / " ETC
//TODO

static void			fill_cur_tk_with_new_token(t_token_inf *cur_tk
		, t_str_cmd_inf *scmd, t_alloc *alloc)
{
	char			*new_str;
	t_str_cmd_inf	new_scmd;
	t_list			*tk_list;
	t_list			*last_tk;

	cur_tk->type = TK_NOTHING;
	cur_tk->token = NULL;
	new_str = ft_strjoin(scmd->str, " H");
	if (!scmd_init(&new_scmd, new_str) || new_str == NULL)
	{
		free(new_str);
		scmd_clean(&new_scmd);
		return ;
	}
	free(new_str);
	tk_list = split_cmd_token_without_last_alias(&new_scmd, alloc->aliastable);
	last_tk = tk_list;
	while (last_tk != NULL && last_tk->next != NULL)
		last_tk = last_tk->next;
	if (last_tk != NULL)
		cur_tk->type = get_tk(last_tk)->type;
	ft_lstdel(&tk_list, del_token);
	scmd_clean(&new_scmd);
}

static const char	*get_last_sub_cmd_start(t_str_cmd_inf *scmd)
{
	const char		*last_sub_cmd_start;

	last_sub_cmd_start = NULL;
	while (1)
	{
		if (last_sub_cmd_start == NULL && scmd->sub_str_cmd != NULL
				&& scmd->sub_str_cmd->cur_str_cmd_type == SCMD_TYPE_SUBCMD)
		{
			last_sub_cmd_start = scmd_cur_str(scmd);
		}
		else if (last_sub_cmd_start != NULL && (scmd->sub_str_cmd == NULL
					|| scmd->sub_str_cmd->cur_str_cmd_type != SCMD_TYPE_SUBCMD))
		{
			last_sub_cmd_start = NULL;
		}
		if (!scmd_move_to_next_char(scmd))
			break ;
	}
	return (last_sub_cmd_start);
}

static char			*get_rred_opt_real_content(const char *token)
{
	if (*token == '&')
		++token;
	if (*token == '-')
		++token;
	if (*token == '\0')
		return (NULL);
	else
		return (ft_strdup(token));
}

static void			fill_cur_tk_with_last_token(t_token_inf *cur_tk
		, t_token_inf *last_tk, t_str_cmd_inf *scmd, t_alloc *alloc)
{
	t_token_inf		*tmp_token;
	t_str_cmd_inf	new_scmd;
	const char		*last_sub_cmd_start;

	if (scmd->sub_str_cmd != NULL
			&& scmd->sub_str_cmd->cur_str_cmd_type == SCMD_TYPE_SUBCMD)
	{
		scmd_init(&new_scmd, last_tk->token);
		last_sub_cmd_start = get_last_sub_cmd_start(&new_scmd);
		if (last_sub_cmd_start != NULL)
		{
			tmp_token = get_cur_token_cmd(last_sub_cmd_start, alloc);
			cur_tk->type = tmp_token->type;
			cur_tk->token = tmp_token->token;
			free(tmp_token);
			scmd_clean(&new_scmd);
			return ;
		}
		scmd_clean(&new_scmd);
	}
	cur_tk->type = (last_tk->type == TK_RRED_OPT
			? TK_RED_FILENAME : last_tk->type);
	cur_tk->token = (last_tk->type == TK_RRED_OPT
			? get_rred_opt_real_content(last_tk->token)
			: ft_strdup(last_tk->token));
}

t_token_inf			*get_cur_token_cmd(const char *str, t_alloc *alloc)
{
	t_token_inf		*cur_tk_cmd;
	t_list			*tk_list;
	t_list			*last_tk;
	t_str_cmd_inf	scmd;

	if (!scmd_init(&scmd, str)
			|| (cur_tk_cmd = (t_token_inf*)malloc(sizeof(t_token_inf))) == NULL)
	{
		scmd_clean(&scmd);
		return (NULL);
	}
	tk_list = split_cmd_token_without_last_alias(&scmd, alloc->aliastable);
	last_tk = tk_list;
	while (last_tk != NULL && last_tk->next != NULL)
		last_tk = last_tk->next;
	if (scmd_cur_char_is_in_nothing(&scmd) && (scmd.pos == 0 || last_tk == NULL
				|| scmd_char_at_is_of(&scmd, scmd.pos - 1, WORD_SEP_CHARS)
				|| get_tk(last_tk)->type == TK_RED_OPE
				|| get_tk(last_tk)->type == TK_CMD_SEP))
		fill_cur_tk_with_new_token(cur_tk_cmd, &scmd, alloc);
	else
		fill_cur_tk_with_last_token(cur_tk_cmd, get_tk(last_tk), &scmd, alloc);
	ft_lstdel(&tk_list, del_token);
	scmd_clean(&scmd);
	return (cur_tk_cmd);
}

static int			is_valid_var_char(char var_char, size_t idx)
{
	if (idx == 0)
	{
		return (ft_isalpha(var_char) || var_char == '_' || var_char == '\0');
	}
	else
	{
		return (ft_isalnum(var_char) || var_char == '_' || var_char == '\0');
	}
}

static void			update_last_var_inf(t_str_cmd_inf *scmd
		, const char **last_var_start, int *is_in_bracket)
{
	if (*last_var_start != NULL && !*is_in_bracket)
	{
		if (!is_valid_var_char(scmd_cur_char(scmd)
					, scmd_cur_str(scmd) - *last_var_start))
			*last_var_start = NULL;
	}
	else if (*last_var_start != NULL && *is_in_bracket)
	{
		if (scmd->sub_str_cmd == NULL
				|| scmd->sub_str_cmd->cur_str_cmd_type != SCMD_TYPE_VAR)
			*last_var_start = NULL;
	}
	if (*last_var_start == NULL && !scmd->is_in_quote
			&& scmd->sub_str_cmd == NULL && scmd_cur_char(scmd) == '$'
			&& !scmd_cur_char_is_escaped(scmd))
	{
		*last_var_start = scmd_cur_str(scmd) + 1;
		*is_in_bracket = 0;
	}
	else if (*last_var_start == NULL && scmd->sub_str_cmd != NULL
			&& scmd->sub_str_cmd->cur_str_cmd_type == SCMD_TYPE_VAR)
	{
		*last_var_start = scmd_cur_str(scmd);
		*is_in_bracket = 1;
	}
}

static const char	*find_last_var_start(const char *str, int *is_in_bracket)
{
	t_str_cmd_inf	scmd;
	const char		*last_var_start;

	*is_in_bracket = 0;
	if (str == NULL || !scmd_init(&scmd, str))
		return (NULL);
	last_var_start = NULL;
	while (1)
	{
		update_last_var_inf(&scmd, &last_var_start, is_in_bracket);
		if (!scmd_move_to_next_char(&scmd))
			break ;
	}
	if (last_var_start != NULL)
		last_var_start = str + (last_var_start - scmd.str);
	scmd_clean(&scmd);
	return (last_var_start);
}

//NE PAS GERER LES INIHIENVBETIRS ICI
//MAIS IHIBER CE QUI A APRES L'ASSIGN QUAND MEME APRES, EN PRENANT EN COMPOTE
//LES IBHINNFHETEIX PRECEDENTS
static const char	*find_last_assign_start(const char *str)
{
	size_t		idx;
	const char	*last_assign;

	if (str == NULL)
		return (NULL);
	idx = 0;
	while (str[idx] != '\0')
	{
		if (!is_valid_var_char(str[idx], idx))
			break ;
		++idx;
	}
	if (str[idx] != '=')
		return (NULL);
	last_assign = str + idx + 1;
	while (str[idx] != '\0')
	{
		if (str[idx] == ':')
			last_assign = str + idx + 1;
		++idx;
	}
	return (last_assign);
}

t_ac_suff_inf		*autocomplete_cmdline(const char *str, t_alloc *alloc)
{
	t_token_inf		*cur_tk;
	t_ac_suff_inf	*acs_inf;
	const char		*real_ac_start;
	int				is_in_bracket;

	cur_tk = get_cur_token_cmd(str, alloc);
	real_ac_start = find_last_var_start(cur_tk->token, &is_in_bracket);
	if (real_ac_start == NULL)
	{
		real_ac_start = (cur_tk->type == TK_RED_FILENAME
				? NULL : find_last_assign_start(cur_tk->token));
		if (real_ac_start == NULL)
			real_ac_start = cur_tk->token;
		acs_inf = autocomplete_word(alloc->vars, (real_ac_start == NULL ? ""
					: real_ac_start) , cur_tk->type == TK_CMD, alloc);
	}
	else
	{
		acs_inf = autocomplete_var(alloc->vars, real_ac_start);
		if (acs_inf != NULL && acs_inf->suff_type == ACS_TYPE_FILE
				&& is_in_bracket)
			acs_inf->suff_type = ACS_TYPE_VAR_IN_BRACKETS;
	}
	del_token(cur_tk, 0);
	return (acs_inf);
}

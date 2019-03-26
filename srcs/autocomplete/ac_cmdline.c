#include <stdlib.h>
#include "libft.h"
#include "shell.h"
#include "split_cmd_token.h"
#include "str_cmd_inf.h"
#include "token_inf.h"
#include "autocomplete.h"

static void			fill_cur_tk_with_new_token(t_token_inf *cur_tk
		, t_str_cmd_inf *scmd, t_alloc *alloc)
{
	(void)cur_tk;
	(void)scmd;
	(void)alloc;
	//creer du texte a la fin de scmd->str puis retokeniser pour avoir le dernier token
	//retourner un token avec le meme type mais un str vide
	return ;
}

static void			fill_cur_tk_with_last_token(t_token_inf *cur_tk
		, t_token_inf *last_tk, t_str_cmd_inf *scmd, t_alloc *alloc)
{
	(void)cur_tk;
	(void)last_tk;
	(void)scmd;
	(void)alloc;
	//analyser le dernier token pour completer en consequence
	//aller jusqu'au bout des sub_str_cmd et retokeniser si besoin (SUB_CMD)
	//gerer correctement le cas du LRED_OPT (si contient "& text" ou "&-" etc
	return ;
}

/*
** Alloue et retourne le dernier token de la commande. Renvoie NULL si erreur.
*/

//TODO GERER LE BACKSLASH EN FIN DE COMMANDE ?!@#$%^&*!?@>$%<^>&$#>&*!@?
static t_token_inf	*get_cur_token_cmd(const char *str, t_alloc *alloc)
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

t_ac_suff_inf		*autocomplete_cmdline(const char *str, t_alloc *alloc)
{
	t_token_inf		*cur_tk;

	cur_tk = get_cur_token_cmd(str, alloc);
	del_token(cur_tk, 0);
	return (NULL);
}

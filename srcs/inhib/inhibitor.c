#include "shell.h"
#include "parser_lexer.h"
#include "expand.h"
#include "inhibitor.h"
#include "str_cmd_inf.h"
#include "error.h"

void	inhib_only_str(char *str)
{
	t_str_cmd_inf	str_cmd;
	size_t			pos;

	pos = 0;
	if (!str || !scmd_init(&str_cmd, str))
		return ;
	while (scmd_cur_char(&str_cmd))
		if (!scmd_cur_char_is_escaped(&str_cmd) && !str_cmd.is_in_dbquote
				&& !str_cmd.is_in_quote && (scmd_cur_char(&str_cmd) == '"'
				|| scmd_cur_char(&str_cmd) == '\''))
			remove_escaped_char(&str_cmd, &str, &pos, 1);
		else if (scmd_cur_char_is_escaped(&str_cmd) && ((str_cmd.is_in_dbquote
				&& scmd_cur_is_of(&str_cmd, DBQUOTE_SPE_CHAR) == 1)
				|| (!str_cmd.is_in_quote && !str_cmd.is_in_dbquote)))
			remove_escaped_char(&str_cmd, &str, &pos, 1);
		else if ((str_cmd.is_in_quote && str[pos] == '\'')
				|| (str_cmd.is_in_dbquote && str[pos] == '"'))
		{
			pos += scmd_move_to_next_char(&str_cmd);
			remove_escaped_char(&str_cmd, &str, &pos, 0);
		}
		else
			pos += scmd_move_to_next_char(&str_cmd);
	scmd_clean(&str_cmd);
}

int		inhib_in_db(t_str_cmd_inf *str_cmd, size_t *pos, char **array,
			t_alloc *alloc)
{
	size_t	index;

	index = get_pos_in_array(array);
	remove_escaped_char(str_cmd, &(array[index]), pos, 0);
	*pos -= 1;
	while (scmd_cur_char(str_cmd) && str_cmd->is_in_dbquote)
		if (scmd_cur_char_is_escaped(str_cmd) == 1
				&& scmd_cur_is_of(str_cmd, DBQUOTE_SPE_CHAR) == 1)
			remove_escaped_char(str_cmd, &(array[index]), pos, 1);
		else if (scmd_cur_char(str_cmd) == '$')
		{
			if (!expand(&(array[index]), alloc, pos))
				return (0);
			scmd_move_to_next_char(str_cmd);
			update_pos_index(str_cmd);
		}
		else
			*pos += scmd_move_to_next_char(str_cmd);
	remove_last_quote(str_cmd, array, pos, index);
	return (1);
}

int		do_inhib(t_str_cmd_inf *str_cmd, char ***array, size_t *pos_array,
		t_alloc *alloc)
{
	while (scmd_cur_char(str_cmd))
	{
		if (str_cmd->is_in_quote || str_cmd->is_in_dbquote)
		{
			if (!(inhib_expand_in_quote(str_cmd, *array, pos_array, alloc)))
				return (error_inhib_expand(str_cmd, *array));
		}
		else if (scmd_cur_char_is_escaped(str_cmd))
			remove_escaped_char(str_cmd, &((*array)[get_pos_in_array(*array)]),
				pos_array, 1);
		else if (scmd_cur_char(str_cmd) == '$')
		{
			if (!do_expand(array, alloc, pos_array, str_cmd))
				return (error_inhib_expand(str_cmd, *array));
		}
		else
			*pos_array += scmd_move_to_next_char(str_cmd);
	}
	remove_last_char(str_cmd, pos_array, &((*array)[get_pos_in_array(*array)]));
	return (1);
}

char	**inhib_expand_str(const char *str, t_alloc *alloc)
{
	size_t			pos_array;
	t_str_cmd_inf	*str_cmd;
	char			**array;

	pos_array = 0;
	str_cmd = NULL;
	array = NULL;
	if (!str)
		return (NULL);
	if (!(initialize_inhib_expand(&str_cmd, &array, str)))
	{
		error_inhib_expand(str_cmd, array);
		return (NULL);
	}
	if (!do_inhib(str_cmd, &array, &pos_array, alloc))
		return (NULL);
	scmd_clean(str_cmd);
	free(str_cmd);
	return (array);
}

int		inhib_expand_tab(t_ast *elem, t_alloc *alloc)
{
	int		i;
	char	**new_array;

	i = 0;
	while (elem->input[i])
	{
		// if (!expand_home_shortcut(&(elem->input[i]), alloc->vars))
		// {
			if (!(new_array = inhib_expand_str(elem->input[i], alloc)))
				return (0);
			create_new_input(elem, &i, new_array);
		// }
		// else
		// 	i += 1;
	}
	if (ft_strtab_count(elem->input) == 1
			&& ft_strequ(elem->input[0], "") == 1)
		return (0);
	return (1);
}

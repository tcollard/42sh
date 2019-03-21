#include "shell.h"
#include "builtins.h"
#include "error.h"

char	*get_key(char *str)
{
	int		i;

	i = 0;
	while (str[i] && str[i] != '=')
		i += 1;
	return (ft_strsub(str, 0, i));
}

char	*get_value(char *str)
{
	int		i;
	size_t	len;

	i = 0;
	len = ft_strlen(str);
	while (str[i] && str[i] != '=')
		i += 1;
	return (ft_strsub(str, i + 1, len - i - 1));
}

t_var	*find_elem_env(t_var *lst, char *key)
{
	t_var *tmp;

	tmp = lst;
	while (tmp && ft_strcmp(key, tmp->key) != 0)
		tmp = tmp->next;
	return (tmp);
}

void	add_elem_env(t_var **lst, char *key, char *value)
{
	t_var	*new;

	if (!(new = (t_var*)malloc(sizeof(t_var))))
		ft_exit_malloc();
	if (!(new->key = ft_strdup(key)))
		ft_exit_malloc();
	if (!(new->value = ft_strdup(value)))
		ft_exit_malloc();
	new->next = NULL;
	new->is_env = 1;
	insert_new_elem(lst, new);
}

void	add_shlvl(t_var **lst)
{
	t_var	*shlvl;
	int		lvl;

	if ((shlvl = find_elem_env(*lst, "SHLVL")) != NULL)
	{
		lvl = (shlvl->value[0] == '-') ? 1 : 0;
		while (ft_isdigit(shlvl->value[lvl]) == 1)
			lvl += 1;
		if (shlvl->value[lvl])
		{
			free(shlvl->value);
			(!(shlvl->value = ft_strdup("1"))) ? ft_exit_malloc() : 0;
		}
		else
		{
			lvl = ft_atoi(shlvl->value);
			free(shlvl->value);
			if (lvl < 0)
				(!(shlvl->value = ft_strdup("0"))) ? ft_exit_malloc() : 0;
			else
				shlvl->value = ft_itoa(lvl + 1);
		}
	}
	else
		add_elem_env(lst, "SHLVL", "1");
}
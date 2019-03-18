#include "shell.h"
#include "builtins.h"
#include "exectable.h"
#include "hashtable.h"
#include "search_exec.h"

// BUILTIN TYPE
// ALIAS SEARCH TYPE NOT SET

int			type_builtins(t_ast *elem, t_alloc *alloc)
{
	int		i;
	int		ret;
	char	*path;

	i = 1;
	ret = 0;
	while (elem->input[i++])
		if (access(elem->input[i], F_OK) == 0 && access(elem->input[i], X_OK) == 0)
			ret = ft_printf("%s is %s\n", elem->input[i], elem->input[i]);
		else if ((path = (char *)get_exec_path(alloc->exectable, elem->input[i], 0)))
			ret = ft_printf("%s is hashed (%s)\n", elem->input[i], path);
		else if (is_builtins(alloc, elem->input[i]) == 1)
			ret = ft_printf("%s is a shell builtin\n", elem->input[i]);
		else if ((path = search_exec(*(alloc->var), elem->input[i], 0)))
		{
			ret = ft_printf("%s is %s\n", elem->input[i], path);
			ft_strdel(&path);
		}
		else
		{
			ft_dprintf(2, "42sh: type: %s: not found\n", elem->input[i]);
			ret = 1;
		}
	return ((ret != 1 && ret != -1) ? 0 : 1);
}
#include <stdlib.h>
#include "libft.h"
#include "shell.h"

void	del_elem_ast(t_ast **lst)
{
	if (lst == NULL || *lst == NULL)
		return ;
	ft_strdel(&(*lst)->heredoc);
	ft_strtab_free((*lst)->input);
	free(*lst);
	*lst = NULL;
}

void	del_ast(t_ast **lst)
{
	if (lst == NULL || *lst == NULL)
		return ;
	if ((*lst)->left != NULL)
		del_ast(&(*lst)->left);
	if ((*lst)->right != NULL)
		del_ast(&(*lst)->right);
	del_elem_ast(lst);
}

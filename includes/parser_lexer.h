#ifndef PARSER_LEXER_H
# define PARSER_LEXER_H

# include "shell.h"

typedef enum	e_recall_prompt
{
	PR_SUCCESS,
	PR_ERROR,
	PR_PIPE,
	PR_AND,
	PR_OR
}				t_recall_prompt;

/*
************************************ LEXER *************************************
*/

void			expand_home_shortcut(char **s, t_list *vars);

/*
********************************* LEXER TOOLS **********************************
*/

void			ft_insert(char **source, char *insert, int pos1, int pos2);

/*
************************************ PARSER ************************************
*/

t_ast			*parser(t_list *lst_tk);

/*
********************************* PARSER TOOLS *********************************
*/

t_recall_prompt	token_analyser(t_list *lst_tk);


/*
*********************************** AST TOOLS **********************************
*/

void			sort_ast(t_ast *lst, t_ast **sort);
void			link_new_node(t_ast **sort, t_ast *tmp, t_ast *node);
t_ast			*fill_cmd(t_list **lst_tk, t_ast *elem);
t_ast			*fill_ope(t_list **lst_tk, t_ast *elem);
t_ast			*fill_assign(t_list **lst_tk, t_ast *elem);
t_ast			*fill_cmd_sep(t_list **lst_tk, t_ast *elem);
t_ast			*create_elem(t_list **lst_tk);
t_ast			*set_new_elem(void);
void			init_input(t_ast *elem, int len, t_list *lst_tk);

#endif

#include "shell.h"
#include "cmdline.h"
#include "builtins.h"
#include "execution.h"
#include "exectable.h"
#include "hashtable.h"
#include "parser_lexer.h"
#include "str_cmd_inf.h"
#include "split_cmd_token.h"
#include "token_inf.h"
#include "job.h"
#include "error.h"

static void	lexer_parser(char *line, t_alloc *alloc)
{
	t_ast			*sort_ast;
	t_list			*lst_tk;
	t_exec_opt		exec_option;
	t_str_cmd_inf	scmd;

	sort_ast = NULL;
	lst_tk = NULL;
	ft_bzero(&exec_option, sizeof(t_exec_opt));
	sigs_wait_line(alloc);
	if (!scmd_init(&scmd, line))
		return ;
	sigs_wait_line(alloc);
	lst_tk = split_cmd_token(&scmd, alloc->aliastable);
	sigs_wait_line(alloc);
	/*
	 **	VERIF TOKEN AND PRINT BEFORE PARSE
	 */
	// t_list	*tmp;
	// tmp = lst_tk;
	// while (tmp)
	// {
	// 	ft_printf("type: %d\ntoken: |%s|\n\n", get_tk(tmp)->type, get_tk(tmp)->token);
	// 	tmp = tmp->next;
	// }

	if (!(sort_ast = parser(&lst_tk)))
		return ;
	sigs_wait_line(alloc);

	/*
	 ** COMPARAISON POUR RECONNAITRE LE JOB CONTROL
	 */
	// if (get_tk(lst_tk)->type == TK_CMD_SEP
	// && ft_strcmp(get_tk(lst_tk)->token, "&") == 0)
	// 	analyzer(sort_ast, alloc, TRUE);
	// else

	/*
	 ** PRINT AST AND REINIT NODE
	 */
	// if (sort_ast)
	// 	read_sort_descent(sort_ast, 0);

	check_exit_cmd(sort_ast);
	alloc->ret_val = analyzer(sort_ast, alloc, &exec_option);

	//FUNCTION TO CLEAN / CLEAN TK_LIST MISSING
	del_ast(&sort_ast);
	scmd_clean(&scmd);
}

int		main(int argc, char **argv, char **environ)
{
	t_alloc	alloc;
	char	*input;

	g_jobs = 0;
	set_signals_handlers_for_read();
	if (!setup_alloc(&alloc, argc, argv, environ))
		ft_dprintf(STDERR_FILENO, "Unable to setup environment\n");
	else
	{
		if (!init_cmdline(&alloc, &alloc.cmdline))
			ft_dprintf(STDERR_FILENO, "Unable to init term\n");
		else
		{
			load_history_file_entries(&alloc, &alloc.cmdline.history);
			while (1)
			{
				refresh_jobs();
				setup_term(&alloc.cmdline);
				input = read_cmdline(&alloc, &alloc.cmdline);
				reset_term(&alloc.cmdline);
				if (input != NULL)
				{
					set_sigmask(SIG_BLOCK);
					lexer_parser(input, &alloc);
					free(input);
					set_signals_handlers_for_read();
				}
			}
		}
	}
	terminate_all_jobs();
	del_alloc(&alloc);
	sig_reset();
	return (1);
}

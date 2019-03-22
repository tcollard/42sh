#ifndef SHELL_H
# define SHELL_H

/*
*********************************** INCLUDES ***********************************
*/

# include <unistd.h>
# include <stdlib.h>
# include <limits.h>
# include <fcntl.h>
# include <signal.h>
# include <sys/wait.h>
# include <stdbool.h>
# include "../libft/includes/get_next_line.h"
# include "../libft/includes/libft.h"
# include "../libft/includes/printf.h"
# include "hashtable.h"

/*
************************************ DEFINE ************************************
*/

# define LOGIC		5
# define OPERATOR	4
# define AGREG		2
# define REDIR		2
# define HEREDOC	1
# define CMD		0
# define NO_TYPE 	-1

/*
********************************** STRUCTURES **********************************
*/

typedef struct			s_exec_opt
{
	bool				fork;
	bool				wait_hang;
}						t_exec_opt;

typedef struct			s_cursor
{
	size_t				l;
	size_t				c;
}						t_cursor;

typedef struct			s_buf
{
	char				*s;
	size_t				x;
	t_cursor			pos;
	size_t				buf_size;
}						t_buf;

typedef struct			s_historic
{
	char				*origin;
	char				*modif;
	struct s_historic	*next;
	struct s_historic	*prev;
}						t_historic;


typedef	struct			s_var
{
	char				*key;
	char				*value;
	int					is_env;
	struct s_var		*next;
}						t_var;

typedef struct			s_ast
{
	int					print;
	int					type;
	int					fd[2];
	char				*heredoc;
	char				**input;
	struct s_ast		*next;
	struct s_ast		*back;
	struct s_ast		*left;
	struct s_ast		*right;
}						t_ast;

struct					s_alloc;
typedef int				(*t_built_fun)(t_ast *, struct s_alloc *);

typedef struct			s_builtin
{
	const char			*name;
	t_built_fun			built_fun;
}						t_builtin;

typedef struct			s_alloc
{
	int					ret_val;
	t_historic			*history;
	t_buf				*input;
	t_ast				*ast;
	t_var				**var;
	t_builtin			*builtins;
	t_hashtable			*exectable;
	t_hashtable			*aliastable;
	int					fd[10];
}						t_alloc;

typedef int				(*t_dispatch)(t_ast *elem, t_alloc *alloc, t_exec_opt *opt);

typedef enum			e_job_state
{
	RUNNING_FG,
	RUNNING_BG,
	STOPPED,
	STOPPED_PENDING,
	DONE
}						t_job_state;

typedef struct			s_job
{
	char				*cmd;
	t_list				*pipe;
	int					pid;
	int					gpid;
	int					status;
	t_job_state			state;
}						t_job;

/*
************************************ TOOLS *************************************
*/

void	signal_handle(int sig);
void	redirect_term_controller(pid_t new_controler, int type);
void	print_job(pid_t process);
t_job	*get_job_pid(pid_t process);
t_list	*add_pid_lst(pid_t process, t_ast *elem, bool addpipe);
int		add_pid_lst_pipe(t_list *attach, pid_t process, t_ast *elem, bool addpipe);
void	refresh_jobs_finish(void);
int		waiting_line(bool wait_hang, t_list *tmp);


void	delete_str_tab(char **tab_str);
void	del_lst_env(t_var **lst);
void	del_lst_ast(t_ast **lst);
void	del_double_tab(char **tab1, char **tab2);
void	del_alloc(t_alloc *alloc);
void	set_alloc(t_alloc *al, t_var **lst);
int		ret_status(int ret_val, pid_t process, t_job *job);
int		replace_val_ret(char **str, int i, int x, int err);
void	insert_new_elem(t_var **lst, t_var *new);

//TOOLS TO PRINT LST AST
void	read_lst(t_ast *lst, int active);
void	read_sort_descent(t_ast *sort, int active);
void	reinit_print(t_ast *lst, int active);


/*
*********************************** GLOBALS ***********************************
*/

int						p_debug;
t_list					*g_jobs;

#endif

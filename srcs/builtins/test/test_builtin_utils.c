/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   test_builtin_utils.c                               :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: gguichar <gguichar@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2019/03/20 20:50:37 by gguichar          #+#    #+#             */
/*   Updated: 2019/04/08 15:31:22 by gguichar         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include <unistd.h>
#include <limits.h>
#include <sys/stat.h>
#include "libft.h"
#include "builtins_test.h"

static int	test_builtin_check_int(int *ptr, const char *operand)
{
	long	value;
	char	*endptr;

	if (operand[0] != '\0')
	{
		value = ft_strtol(operand, &endptr, 10);
		while (*endptr == ' ')
			endptr++;
		if (*endptr == '\0' && value >= INT_MIN && value <= INT_MAX)
		{
			*ptr = (int)value;
			return (1);
		}
	}
	ft_dprintf(STDERR_FILENO, "42sh: test: %s: integer expression expected\n"
			, operand);
	return (0);
}

int			test_builtin_check_ints(int *tab, const char *left
		, const char *right)
{
	return (test_builtin_check_int(tab, left)
			&& test_builtin_check_int(tab + 1, right));
}

int			test_builtin_file_type(const char *file, int mode)
{
	struct stat	info;

	if (stat(file, &info) == -1)
		return (0);
	return ((info.st_mode & S_IFMT) == mode);
}

int			test_builtin_file_mode(const char *file, int mode)
{
	struct stat	info;

	if (stat(file, &info) == -1)
		return (0);
	return (info.st_mode & mode);
}

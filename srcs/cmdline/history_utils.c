/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   history_utils.c                                    :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: gguichar <gguichar@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2019/03/23 20:08:47 by gguichar          #+#    #+#             */
/*   Updated: 2019/04/04 14:01:16 by gguichar         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include <fcntl.h>
#include <unistd.h>
#include <stdlib.h>
#include <sys/stat.h>
#include <sys/mman.h>
#include "libft.h"
#include "shell.h"
#include "check_path.h"
#include "vars.h"

char		*get_history_file_path(t_alloc *alloc)
{
	const char	*home_dir;

	home_dir = get_home_directory(alloc->vars);
	return (join_path(home_dir, ".42sh_history"));
}

static int	read_history_last_entries(t_history *history, int fd
		, size_t file_size)
{
	char	*buffer;
	char	*buffer_pos;
	int		count;

	buffer = mmap(NULL, file_size, PROT_READ | PROT_WRITE, MAP_PRIVATE, fd, 0);
	if (buffer == MAP_FAILED)
		return (0);
	buffer_pos = buffer + file_size;
	count = 0;
	while (--buffer_pos >= buffer && count < MAX_HISTORY_LINES)
	{
		if (*buffer_pos != '\n')
			continue;
		*buffer_pos = '\0';
		if ((buffer_pos + 1) < (buffer + file_size))
		{
			add_history_entry(history, buffer_pos + 1);
			count++;
		}
	}
	if (count < MAX_HISTORY_LINES)
		add_history_entry(history, buffer);
	munmap(buffer, file_size);
	return (1);
}

int			load_history_file_entries(t_alloc *alloc, t_history *history)
{
	int			ret;
	char		*file_path;
	int			fd;
	struct stat	stat;
	size_t		file_size;

	ret = 0;
	file_path = get_history_file_path(alloc);
	if (file_path == NULL)
		return (ret);
	fd = open(file_path, O_RDONLY);
	if (fd != -1)
	{
		if (fstat(fd, &stat) != -1 && S_ISREG(stat.st_mode))
		{
			file_size = (size_t)stat.st_size;
			ret = read_history_last_entries(history, fd, file_size);
		}
		close(fd);
	}
	free(file_path);
	return (ret);
}

int			save_history_entries(t_alloc *alloc, t_history *history)
{
	char			*file_path;
	int				fd;
	t_history_entry	*entry;

	file_path = get_history_file_path(alloc);
	if (file_path == NULL)
		return (0);
	fd = open(file_path, O_CREAT | O_WRONLY | O_TRUNC, 0644);
	if (fd != -1)
	{
		entry = history->front;
		while (entry != NULL)
		{
			write(fd, entry->content, ft_strlen(entry->content));
			write(fd, "\n", 1);
			entry = entry->next;
		}
		close(fd);
	}
	free(file_path);
	return (fd != -1);
}

void		del_history_entries(t_history *history)
{
	t_history_entry	*entry;
	t_history_entry	*next;

	entry = history->front;
	while (entry != NULL)
	{
		next = entry->next;
		free(entry->content);
		free(entry);
		entry = next;
	}
	free(history->buffer);
	ft_memset(history, 0, sizeof(t_history));
}

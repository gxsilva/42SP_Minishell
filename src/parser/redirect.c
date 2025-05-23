/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   redirect.c                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: lsilva-x <lsilva-x@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/03/06 14:42:43 by msalaibb          #+#    #+#             */
/*   Updated: 2025/05/11 16:57:13 by lsilva-x         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../../includes/minishell.h"

static void	def_red(t_flags *cmd, int flags, int to_fd, char *arq)
{
	int		fd;

	if (arq == NULL)
		fd = open_file_r(cmd->flag, flags);
	else
		fd = open_file_r(arq, flags);
	if (dup2(fd, to_fd) == -1)
	{
		close_all(get_t_min()->pipe_ptr[0], get_t_min()->pipe_ptr[1]);
		free_all("Error: Dup2 failed\n", 1);
	}
	close(fd);
	if (cmd != NULL)
	{
		free(cmd->flag);
		cmd->flag = NULL;
	}
}

static int	if_redirect_c(t_cmds *f1, t_flags *f2)
{
	if (f1->cmd[0] == '<')
	{
		if (f1->cmd[0] == '<' && f1->cmd[1] == '\0')
			def_red(f2, O_RDONLY, 0, NULL);
		else if (f1->cmd[0] == '<' && f1->cmd[1] == '<')
			open_here_doc(f2);
		else if (f1->cmd[0] == '<' && f1->cmd[1] != '\0')
			def_red(NULL, O_RDONLY, 0, &f1->cmd[1]);
		free_cmd(f1);
		return (0);
	}
	else
	{
		if (f1->cmd[0] == '>' && f1->cmd[1] == '\0')
			def_red(f2, O_TRUNC | O_WRONLY | O_CREAT, 1, NULL);
		else if (f1->cmd[0] == '>' && f1->cmd[1] == '>' && f1->cmd[2] == '\0')
			def_red(f2, O_WRONLY | O_CREAT | O_APPEND, 1, NULL);
		else if (f1->cmd[0] == '>' && f1->cmd[1] == '>' && f1->cmd[2] != '\0')
			def_red(NULL, O_WRONLY | O_CREAT | O_APPEND, 1, &f1->cmd[2]);
		else if (f1->cmd[0] == '>' && f1->cmd[1] != '\0')
			def_red(NULL, O_TRUNC | O_WRONLY | O_CREAT, 1, &f1->cmd[1]);
		free_cmd(f1);
		return (1);
	}
}

static void	redirect_c(t_cmds *c, int *controller)
{
	t_flags	*flags_copy;

	if (c->quote == 0 && c->cmd && (c->cmd[0] == '>' || c->cmd[0] == '<'))
	{
		flags_copy = c->flags;
		controller[if_redirect_c(c, flags_copy)] = 1;
	}
}

int	if_redirect(t_flags *f1, t_flags *f2)
{
	if (f1->flag[0] == '<')
	{
		if (f1->flag[0] == '<' && f1->flag[1] == '\0')
			def_red(f2, O_RDONLY, 0, NULL);
		else if (f1->flag[0] == '<' && f1->flag[1] == '<')
			open_here_doc(f2);
		else if (f1->flag[0] == '<' && f1->flag[1] != '\0')
			def_red(NULL, O_RDONLY, 0, &f1->flag[1]);
		free_flag(f1);
		return (0);
	}
	else
	{
		if (f1->flag[0] == '>' && f1->flag[1] == '\0')
			def_red(f2, O_TRUNC | O_WRONLY | O_CREAT, 1, NULL);
		else if (confitions_redirect(f1, 0))
			def_red(f2, O_WRONLY | O_CREAT | O_APPEND, 1, NULL);
		else if (confitions_redirect(f1, 1))
			def_red(NULL, O_WRONLY | O_CREAT | O_APPEND, 1, &f1->flag[2]);
		else if (f1->flag[0] == '>' && f1->flag[1] != '\0')
			def_red(NULL, O_TRUNC | O_WRONLY | O_CREAT, 1, &f1->flag[1]);
		free_flag(f1);
		return (1);
	}
}

void	redirect(int fd, int to_fd, t_cmds *c)
{
	int		controller[2];

	controller[0] = 0;
	controller[1] = 0;
	redirect_c(c, controller);
	handle_more(c, controller);
	if (controller[to_fd] == 0)
		if (dup2(fd, to_fd) == -1)
			exit_error_minishell("Dup2 Error\n", 1);
}

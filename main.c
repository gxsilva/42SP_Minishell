/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   main.c                                             :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: lsilva-x <lsilva-x@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/03/04 14:01:13 by msalaibb          #+#    #+#             */
/*   Updated: 2025/05/11 20:42:06 by lsilva-x         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "./includes/minishell.h"

static void	inicialize(char **env)
{
	static char	cwd[1024];

	if (getcwd(cwd, 1024) == NULL)
	{
		perror("getcwd");
		exit(EXIT_FAILURE);
	}
	get_t_min()->p_cwd = cwd;
	get_t_min()->in_fd = dup(0);
	get_t_min()->out_fd = dup(1);
	get_t_min()->sig = 0;
	get_t_min()->pipe_cnt = 0;
	get_t_min()->pipe_ptr = NULL;
	int_env_file(env);
	get_t_min()->env = NULL;
}

t_min	*get_t_min(void)
{
	static t_min	min;

	return (&min);
}

static void	sig_quit_behavior(void)
{
	ft_putstr_fd("exit\n", 1);
	free_split(get_t_min()->env);
	close_all(get_t_min()->in_fd, get_t_min()->out_fd);
	exit (0);
}

void	minishell(void)
{
	char	*input;

	while (1)
	{
		set_std_sig();
		get_t_min()->env = update_env(&get_t_min()->env);
		input = readline("minishell> ");
		if (!input)
			sig_quit_behavior();
		if (verify_spaces(input))
			continue ;
		add_history(input);
		normal_comand(input);
		wait_last_status();
		free_all(NULL, -1);
		if (dup2(get_t_min()->in_fd, 0) == -1)
			return ;
		if (dup2(get_t_min()->out_fd, 1) == -1)
			return ;
	}
}

int	main(int argc, char *argv[], char *env[])
{
	(void)argc;
	(void)argv;
	inicialize(env);
	minishell();
	return (0);
}

#include <stdio.h>
#include <sys/wait.h>
#include <unistd.h>
#include <string.h>

int ft_putstr_err(char *str, char *arg)
{
	while (*str)
		write(STDERR_FILENO, str++, 1);

	if (arg)
		while (*arg)
			write(STDERR_FILENO, arg++, 1);
	write(STDERR_FILENO, "\n", 1);
	return (1);
}

int	ft_execute(char **av, int i, int tmp_fd, char **env)
{
	av[i] = NULL;
	dup2(tmp_fd, STDIN_FILENO);
	close(tmp_fd);
	execve(av[0], av, env);
	ft_putstr_err("error: cannot execute ", av[0]);
	return (1);
}

int	main(int ac, char **av, char **env)
{
	int fd[2];
	int i = 0;
	int pid = 0;

	if (ac < 2)
		return (0);
	int tmp_fd = dup(STDIN_FILENO);

	while (av[i] && av[i + 1])
	{
		av += i + 1;
		i = 0;

		while (av[i] && strncmp(av[i], "|", 1) && strncmp(av[i], ";", 1))
			i++;
		
		if (strncmp(av[0], "cd", 2) == 0)
		{
			if (i != 2)
				ft_putstr_err("error: cd: bad arguments", NULL);
			else if (chdir(av[i]) != 0)
				ft_putstr_err("error: cd: cannot change directory to ", av[1]);
		}

		else if (i != 0 && (av[i] == NULL || strncmp(av[i], ";", 1) == 0))
		{
			pid = fork();
			if (pid == 0)
			{
				if (ft_execute(av, i , tmp_fd, env))
					return (1);
			}
			else
			{
				close(tmp_fd);
				while (waitpid(-1, NULL, WUNTRACED) != -1);
				tmp_fd = dup(STDIN_FILENO);
			}
		}

		else if (i != 0 && strncmp(av[i], "|", 1) == 0)
		{
			pipe(fd);
			pid = fork();
			if (pid == 0)
			{
				dup2(fd[1], STDOUT_FILENO);
				close(fd[0]);
				close(fd[1]);
				if (ft_execute(av, i, tmp_fd, env))
					return (1);
			}
			else
			{
				close(tmp_fd);
				close(fd[1]);
				tmp_fd = fd[0];
			}
		}
	}
	close(tmp_fd);
	return (0);
}

#include <stdio.h>
#include <unistd.h>
#include <sys/wait.h>
#include <string.h>

void	ft_putchar_fd(char *c, int fd)
{
	write(fd, c, 1);
}

void	ft_putstr_fd(char *str, char *arg, int fd)
{
	if (!str || fd < 0)
		return ;
	while (*str)
		ft_putchar_fd(str++, fd);
	if (arg)
		while (*arg)
			ft_putchar_fd(arg++, fd);
	ft_putchar_fd("\n", fd);
}

int	ft_execute(char **av, int i, int tmp_fd, char **env)
{
	av[i] = NULL;
	dup2(tmp_fd, STDIN_FILENO);
	close(tmp_fd);
	execve(av[0], av, env);
	ft_putstr_fd("error: cannot execute ", av[0], 2);
	return (1);
}

int	main(int ac, char **av, char **env)
{
	int pid = 0;
	int i = 0;
	int fd[2];
	int tmp_fd = dup(STDIN_FILENO);

	if (ac < 2)
		return (0);
	while (av[i] && av[i + 1])
	{
		av = av + i + 1;
		i = 0;
		while (av[i] && strncmp(av[i], "|", 1) && strncmp(av[i], ";", 1))
			i++;
		if (!strcmp(*av, "cd"))
		{
			if (i != 2)
				ft_putstr_fd("error: cd: bad arguments", NULL, 2);
			else if (chdir(av[i]))
				ft_putstr_fd("error: cd: cannot change directory to ", av[i], 2);
		}
		else if (i && (av[i] == NULL || strncmp(av[i], ";", 1)))
		{
			pid = fork();
			if (pid == 0)
			{
				if (ft_execute(av, i, tmp_fd, env))
					return (1);
			}
			else
			{
				close(tmp_fd);
				while (waitpid(-1, NULL, WUNTRACED) != -1);
				tmp_fd = dup(STDIN_FILENO);
			}
		}
		else if (i && strncmp(av[i], "|", 1))
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
				close(fd[1]);
				close(tmp_fd);
				tmp_fd = fd[0];
			}
		}
	}
	close(tmp_fd);
	return (0);
}
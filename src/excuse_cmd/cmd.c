#include <unistd.h>
#include <sys/wait.h>
#include <stdio.h>

#include "../../library/libft/inc/libft.h"
#include "../../include/minishell.h"
#include "cmd.h"

#define COMMAND_POSITION (0)
#define	COMMAND_ARG_POSITION (1)

#define PIPE_COUNT (2)
#define PIPE_ERROR (-1)

/*
 * 그냥 에러를 출력하고 나가버리는데
 * 후에 조금 더 구체화할 필요성이 있다
 */
void	ft_print_error()
{
	write(1, "Error!", 6);
}

void	ft_system(t_argument *argument)
{
	t_argument				*pa_orgin_argument;
	enum e_token_type		mult_command;
	char					*command;
	enum e_bulltein_type	bull_type;
	int						pipe_fd[PIPE_COUNT];
	int						std_fd[PIPE_COUNT];
	int						is_pipe_on;

	// Create pipe
	if (pipe(pipe_fd) == PIPE_ERROR)
		ft_print_error();

	// Reserve before argument's address for Free 
	pa_orgin_argument = argument;
	

	// loop for execute command
	is_pipe_on = FALSE;
	while (argument != NULL)
	{
		//do pipe setting
		mult_command = argument->next_token_type;
		if (mult_command == PIPE || mult_command == LT || mult_command == DLT || mult_command == GT)
		{
			is_pipe_on = TRUE;
			
			//FD1 : STDOUT -> PIPE OUT
			std_fd[PIPE_OUT] = dup(STDOUT_FILENO);

			if (pipe_fd[PIPE_OUT] != STDOUT_FILENO)
			{
				dup2(pipe_fd[PIPE_OUT], STDOUT_FILENO);
				close(pipe_fd[PIPE_OUT]);
			}
			else
			{
				ft_print_error();
			}
		}
		else if (is_pipe_on && (mult_command == SEMICOLON || mult_command == DGT || mult_command == EOL))
		{
			is_pipe_on = FALSE;
			
			//FD0 : STDIN -> PIPE IN
			std_fd[PIPE_IN] = dup(STDIN_FILENO);

			if (pipe_fd[PIPE_IN] != STDIN_FILENO)
			{
				dup2(pipe_fd[PIPE_IN], STDIN_FILENO);
				close(pipe_fd[PIPE_IN]);
			}
			else
			{
				ft_print_error();
			}
			
			//FD1 : PIPE OUT -> STDOUT
			pipe_fd[PIPE_OUT] = dup(STDOUT_FILENO);
			
			if (std_fd[PIPE_OUT] != STDOUT_FILENO)
			{
				dup2(std_fd[PIPE_OUT], STDOUT_FILENO);
				close(std_fd[PIPE_OUT]);
			}
			else
			{
				ft_print_error();
			}
		}
	
		//DGT, 세미콜론이라면 명령을 그냥 다음으로 가면 되고 eof라면 그냥 나가면 된다
		command = argument->pa_argument[COMMAND_POSITION];
		if (is_bulletin(command, &bull_type) == TRUE)
		{
			//내장 명령어 실행
			if (bull_type == BUL_ECHO)
				ft_execute_echo(argument);
			else if (bull_type == BUL_CD && !is_pipe_on)
				ft_execute_cd(argument);
			else if (bull_type == BUL_PWD)
				ft_execute_pwd(argument);
		}
		else
		{
			char **pa_copy_argument = NULL;
			
			pid_t child_pid;
			child_pid = fork();

			if (child_pid == -1)
			{
				ft_print_error();
			}
			else if (child_pid == 0)
			{
				int length = ft_get_length_2d_arr(argument->pa_argument);
				pa_copy_argument = (char **)malloc(sizeof(char *) * (length + NULL_POSITOIN + BIN_SH_POSIOTON + BIN_SH_POSIOTON));
				ft_get_sh_command(argument->pa_argument, pa_copy_argument);
				//ft_free_command(pa_copy_argument);
				execve("/bin/sh", pa_copy_argument, NULL);
				printf("this is execuse Error");
			}
			else
			{
				//부모 프로세스라면 기다린다
				wait(NULL);
				printf("%s\n", "Hi i'm perent");
			}
			// 자식프로세스 제작해서 excuse
			// 만약에 여기서 fork를 한다고 하면
			// 어떻게 될까?
			// 내장 명령어에서 파이프를 쓴다고 하면 자식 프로세스 간의 가
			if (pa_copy_argument != NULL)
			{
				ft_free_command(pa_copy_argument);
			}
		}
		argument = argument->next;
	}

	//FD0 : FIFE_IN -> STDIN
	pipe_fd[PIPE_IN] = dup(STDIN_FILENO);

	if (std_fd[PIPE_IN] != STDIN_FILENO)
	{
		dup2(std_fd[PIPE_IN], STDIN_FILENO);
		close(std_fd[PIPE_IN]);
	}
	else
	{
		ft_print_error();
	}

	close(pipe_fd[PIPE_OUT]);
	close(pipe_fd[PIPE_IN]);

	ft_free_argument(pa_orgin_argument);
}

void	ft_free_argument(t_argument *pa_argument)
{
	t_argument 	*p;
	t_argument 	*temp;
	int			i;

	p = pa_argument;
	while (p->next != NULL)
	{
		i = 0;
		while (p->pa_argument[i] != NULL)
		{
			free(p->pa_argument[i]);
			i++;
		}
		free(p->pa_argument);
		temp = p->next;
		free(p);
		p = temp;
	}
}

void	ft_free_command(char **pa_char)
{
	int i;

	i = 0;
	while (pa_char[i] != NULL)
	{
		free(pa_char[i]);
		i++;
	}
	free(pa_char);
}

int	ft_get_length_2d_arr(char **array)
{
	int	i;

	i = 0;
	while (array[i] != NULL)
	{
		i++;
	}
	return (i);
}

void	ft_get_sh_command(char **src_2d, char **out_dst)
{
	int	i;
	int j;
	int	length;

	i = 0;
	out_dst[i++] = ft_strdup("/bin/sh");
	out_dst[i++] = ft_strdup("-c");

	j = 0;
	while (src_2d[j] != NULL)
	{
		out_dst[i] = ft_strdup(src_2d[j]);
		i++;
		j++;
	}
	out_dst[i] = NULL;
}

void	ft_execute_echo(t_argument *argument)
{
	char	*p;
	int		i;
	int		is_newline;

	is_newline = TRUE;
	i = COMMAND_ARG_POSITION;
	if (argument->pa_argument[i] != NULL && \
	ft_strncmp(argument->pa_argument[COMMAND_ARG_POSITION], "-n", 2) == 0)
	{	
		is_newline = FALSE;
		i++;
	}
	while (argument->pa_argument[i] != NULL)
	{
		ft_putstr_fd(argument->pa_argument[i], STDOUT_FILENO);
		i++;
	}
	if (is_newline)
	{
		ft_putchar_fd('\n', STDOUT_FILENO);
	}
}

void	ft_execute_pwd(t_argument *argument)
{
	char		*pa_path;
	int			length;
	const int	SIZE = 0;

	//getcwd 매개변수로 NULL, size가 현재 path보다 작다면
	//그러면 내부에서 path만큼 동적할당 해서 문자열을 반환해준다
	length = ft_get_length_2d_arr(argument->pa_argument);
	if (length > 1)
	{
		ft_putstr_fd("pwd: too many arguments\n", STDOUT_FILENO);
		return ;
	}

	pa_path = getcwd(NULL, SIZE);
	if (pa_path == NULL)
	{
		ft_putstr_fd("pwd: can't get current path\n", STDOUT_FILENO);
	}

	ft_putstr_fd(pa_path, STDOUT_FILENO);
	ft_putchar_fd('\n', STDOUT_FILENO);
}

void	ft_execute_cd(t_argument *argument)
{
	int			result;
	int			length;
	const int	CHDIR_ERROR = -1;
	const int	SECOND_ARG = 1;
	
	length = ft_get_length_2d_arr(argument->pa_argument);
	//Please check when argument 2 is, some cases is like no error
	if (length > 2)
	{
		ft_putstr_fd("cd: ", STDOUT_FILENO);
		ft_putstr_fd(argument->pa_argument[SECOND_ARG], STDOUT_FILENO);
		ft_putstr_fd(": No such file or directory\n", STDOUT_FILENO);
		return ;
	}

	if (length == 1)
	{
		result = chdir("/");
	}
	else if (length == 2)
	{
		result = chdir(argument->pa_argument[COMMAND_ARG_POSITION]);
	}
	else
	{
		ft_print_error();
	}

	if (result == CHDIR_ERROR)
	{
		ft_putstr_fd("cd: ", STDOUT_FILENO);
		ft_putstr_fd(argument->pa_argument[length - 1], STDOUT_FILENO);
		ft_putstr_fd(": No such file or directory\n", STDOUT_FILENO);
	}
}

// 무지성 문자열 비교
int	is_bulletin(char *command, enum e_bulltein_type *out_type)
{
	if (ft_strncmp(command, "echo", 4) == 0)
		*out_type = BUL_ECHO;
	else if (ft_strncmp(command, "cd", 2) == 0)
		*out_type = BUL_CD;
	else if (ft_strncmp(command, "pwd", 3) == 0)
		*out_type = BUL_PWD;
	else if (ft_strncmp(command, "export", 6) == 0)
		*out_type = BUL_EXPORT;
	else if (ft_strncmp(command, "unset", 5) == 0)
		*out_type = BUL_UNSET;
	else if (ft_strncmp(command, "env", 3) == 0)
		*out_type = BUL_ENV;
	else if (ft_strncmp(command, "exit", 4) == 0)
		*out_type = BUL_EXIT;
	else
		*out_type = INVAILD;
	if (*out_type == INVAILD)
		return (FALSE);
	return(TRUE);
}

/*
int	main(int argc, char **argv, char **environ)
{
	//Test Code
	t_argument *pa_arg;
	t_argument *p;
	/*
	//echo
	{
		printf("Echo TEST\n");
		
		pa_arg = (t_argument *)malloc(sizeof(t_argument));
		p = pa_arg;
		
		printf("my output is \n");

		p->next_token_type = SEMICOLON;
		p->pa_argument = (char **)malloc(sizeof(char *) * 2 + 1);
		p->pa_argument[0] = ft_strdup("echo");
		p->pa_argument[1] = ft_strdup("1234");
		p->pa_argument[2] = NULL;
		
		p->next = (t_argument *)malloc(sizeof(t_argument));
		p = p->next;
		p->next_token_type = SEMICOLON;
		p->pa_argument = (char **)malloc(sizeof(char *) * 3 + 1);
		p->pa_argument[0] = ft_strdup("echo");
		p->pa_argument[1] = ft_strdup("-n");
		p->pa_argument[2] = ft_strdup("5678");
		p->pa_argument[3] = NULL;

		p->next = (t_argument *)malloc(sizeof(t_argument));
		p = p->next;
		p->next_token_type = SEMICOLON;
		p->pa_argument = (char **)malloc(sizeof(char *) * 2 + 1);
		p->pa_argument[0] = ft_strdup("echo");
		p->pa_argument[1] = ft_strdup("-n");
		p->pa_argument[2] = NULL;

		p->next = (t_argument *)malloc(sizeof(t_argument));
		p = p->next;
		p->next_token_type = SEMICOLON;
		p->pa_argument = (char **)malloc(sizeof(char *) * 1 + 1);
		p->pa_argument[0] = ft_strdup("echo");
		p->pa_argument[1] = NULL;

		ft_system(pa_arg);

		printf("\n");
	}

	//Pwd
	{
		printf("Pwd TEST\n");
		
		pa_arg = (t_argument *)malloc(sizeof(t_argument));
		p = pa_arg;
		
		printf("my output is \n");

		p->next_token_type = EOL;
		p->pa_argument = (char **)malloc(sizeof(char *) * 1 + 1);
		p->pa_argument[0] = ft_strdup("pwd");
		p->pa_argument[1] = NULL;
		
		p->next = (t_argument *)malloc(sizeof(t_argument));
		p = p->next;
		p->next_token_type = EOL;
		p->pa_argument = (char **)malloc(sizeof(char *) * 2 + 1);
		p->pa_argument[0] = ft_strdup("pwd");
		p->pa_argument[1] = ft_strdup("213213");
		p->pa_argument[2] = NULL;

		p->next = (t_argument *)malloc(sizeof(t_argument));
		p = p->next;
		p->next_token_type = EOL;
		p->pa_argument = (char **)malloc(sizeof(char *) * 3 + 1);
		p->pa_argument[0] = ft_strdup("pwd");
		p->pa_argument[1] = ft_strdup("213213");
		p->pa_argument[2] = ft_strdup("213213");
		p->pa_argument[3] = NULL;

		p->next = NULL;

		ft_system(pa_arg);

		printf("\n");
	}

	//Cd
	{
		printf("Cd TEST\n");
		
		pa_arg = (t_argument *)malloc(sizeof(t_argument));
		p = pa_arg;
		
		printf("my output is \n");

		p->next_token_type = EOL;
		p->pa_argument = (char **)malloc(sizeof(char *) * 1 + 1);
		p->pa_argument[0] = ft_strdup("cd");
		p->pa_argument[1] = NULL;

		p->next = (t_argument *)malloc(sizeof(t_argument));
		p = p->next;
		p->next_token_type = EOL;
		p->pa_argument = (char **)malloc(sizeof(char *) * 1 + 1);
		p->pa_argument[0] = ft_strdup("pwd");
		p->pa_argument[1] = NULL;
		
		p->next = (t_argument *)malloc(sizeof(t_argument));
		p = p->next;
		p->next_token_type = EOL;
		p->pa_argument = (char **)malloc(sizeof(char *) * 2 + 1);
		p->pa_argument[0] = ft_strdup("cd");
		p->pa_argument[1] = ft_strdup("bin");
		p->pa_argument[2] = NULL;

		p->next = (t_argument *)malloc(sizeof(t_argument));
		p = p->next;
		p->next_token_type = EOL;
		p->pa_argument = (char **)malloc(sizeof(char *) * 1 + 1);
		p->pa_argument[0] = ft_strdup("pwd");
		p->pa_argument[1] = NULL;

		p->next = (t_argument *)malloc(sizeof(t_argument));
		p = p->next;
		p->next_token_type = EOL;
		p->pa_argument = (char **)malloc(sizeof(char *) * 2 + 1);
		p->pa_argument[0] = ft_strdup("cd");
		p->pa_argument[1] = ft_strdup("~");
		p->pa_argument[2] = NULL;

		p->next = (t_argument *)malloc(sizeof(t_argument));
		p = p->next;
		p->next_token_type = EOL;
		p->pa_argument = (char **)malloc(sizeof(char *) * 1 + 1);
		p->pa_argument[0] = ft_strdup("pwd");
		p->pa_argument[1] = NULL;

		p->next = (t_argument *)malloc(sizeof(t_argument));
		p = p->next;
		p->next_token_type = EOL;
		p->pa_argument = (char **)malloc(sizeof(char *) * 2 + 1);
		p->pa_argument[0] = ft_strdup("cd");
		p->pa_argument[1] = ft_strdup("2131313");
		p->pa_argument[2] = NULL;
		p->next = NULL;

		p->next = (t_argument *)malloc(sizeof(t_argument));
		p = p->next;
		p->next_token_type = EOL;
		p->pa_argument = (char **)malloc(sizeof(char *) * 3 + 1);
		p->pa_argument[0] = ft_strdup("cd");
		p->pa_argument[1] = ft_strdup("342424");
		p->pa_argument[2] = ft_strdup("32131");
		p->pa_argument[3] = NULL;

		p->next = NULL;

		ft_system(pa_arg);

		printf("\n");
	}
	

	//execuse
	{
		printf("execuse TEST\n");
		
		pa_arg = (t_argument *)malloc(sizeof(t_argument));
		p = pa_arg;
		
		printf("my output is \n");

		p->next_token_type = EOL;
		p->pa_argument = (char **)malloc(sizeof(char *) * (1 + 1));
		p->pa_argument[0] = ft_strdup("ls");
		p->pa_argument[1] = NULL;

		p->next = (t_argument *)malloc(sizeof(t_argument));
		p = p->next;
		p->next_token_type = EOL;
		p->pa_argument = (char **)malloc(sizeof(char *) * (1 + 1 + 1));
		p->pa_argument[0] = ft_strdup("cd");
		p->pa_argument[1] = ft_strdup("../");
		p->pa_argument[2] = NULL;

		p->next = (t_argument *)malloc(sizeof(t_argument));
		p = p->next;
		p->next_token_type = EOL;
		p->pa_argument = (char **)malloc(sizeof(char *) * (1 + 1));
		p->pa_argument[0] = ft_strdup("pwd");
		p->pa_argument[1] = NULL;

		p->next = (t_argument *)malloc(sizeof(t_argument));
		p = p->next;
		p->next_token_type = EOL;
		p->pa_argument = (char **)malloc(sizeof(char *) * (1 + 1));
		p->pa_argument[0] = ft_strdup("./a.out");
		p->pa_argument[1] = NULL;

		p->next = (t_argument *)malloc(sizeof(t_argument));
		p = p->next;
		p->next_token_type = EOL;
		p->pa_argument = (char **)malloc(sizeof(char *) * (1 + 1 + 1));
		p->pa_argument[0] = ft_strdup("ls");
		p->pa_argument[1] = ft_strdup("a");
		p->pa_argument[2] = NULL;

		p->next = NULL;

		ft_system(pa_arg);

		printf("END!\n");
	}
}
*/
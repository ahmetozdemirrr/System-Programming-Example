#include <stdio.h>
#include <unistd.h>
#include <sys/wait.h>
#include <sys/types.h>
#include <string.h>
#include <errno.h>
#include <stdlib.h>
#include <signal.h>
#include <fcntl.h>

#define CAT_BUFFER 10000
#define BUFFER_SIZE 100
#define MAX_TOKENS 10
#define TRUE 1

const char * cmd1 = "ls";
const char * cmd2 = "cd";
const char * cmd3 = "pwd";
const char * cmd4 = "echo";
const char * cmd5 = "exit";
const char * cmd6 = "cat";
const char * cmd7 = "touch";
const char * cmd8 = "subl"; // This command may not work on all devices, the text editor I use when writing code is called sublime text and I normally open files there with this command.
const char * cmd9 = "rmdir";
const char * cmd10 = "mkdir";
const char * cmd11 = "rm";

int cmdCompare(const char * command1, const char * command2)
{
    while (*command1 != '\0' && *command2 != '\0') 
    {
        if (*command1 != *command2) 
        {
            return 0;
        }
        command1++;
        command2++;
    }
    
    while (*command1 == ' ' || *command1 == '\t' || *command1 == '\n') 
    {
        command1++;
    }
    
    while (*command2 == ' ' || *command2 == '\t' || *command2 == '\n') 
    {
        command2++;
    }

    return (*command1 == '\0' && *command2 == '\0');
}

int cmdSize(const char * command)
{
	int i = 0;

	while (command[i] != '\0')
	{
		i++;
	}
	return i;
}

void signalHandler(int sigNo)
{
	if (sigNo == SIGTERM)
	{
		exit(EXIT_SUCCESS);
	}
}

int main(int argc, char const *argv[])
{
	const char * message1 = "Shell> ";
	const char * message2 = ": command not found\n";
	char buffer[BUFFER_SIZE];
	int messageFlag = 0;

	if (signal(SIGTERM, signalHandler) == SIG_ERR)
	{
		perror("Register signal failed");
		exit(EXIT_FAILURE);
	}

	while (TRUE) 
	{
		write(STDOUT_FILENO, message1, cmdSize(message1));
		fgets(buffer, BUFFER_SIZE, stdin);

		char * tokens[MAX_TOKENS];
		char * token = strtok(buffer, " ");
		int tokenCount = 0;

		while (token != NULL && tokenCount < MAX_TOKENS)
		{
			tokens[tokenCount++] = token;
			token = strtok(NULL, " ");
		}

		if (tokenCount > 0)
		{
			if (cmdCompare(tokens[0], cmd1))
			{
				if (tokenCount > 1)
				{
					fprintf(stderr, "Too many arguments.\nUsage: ls\n");
				}

				else
				{
					pid_t childPID = fork();

					if (childPID == -1)
					{
						perror("Fork");
						exit(EXIT_FAILURE);
					}

					else
					{
						if (childPID == 0)
						{
							execl("/bin/ls", "ls", NULL);
							perror("execl failed");
							exit(EXIT_FAILURE);
						}

						else
						{
							int status;
						    wait(&status);
						    
						    if (WIFEXITED(status) && WEXITSTATUS(status) != 0)
						    {
						        fprintf(stderr, "Child process failed with exit status %d\n", WEXITSTATUS(status));
						    }
						}
					}
				}
			}

			else if (cmdCompare(tokens[0], cmd2))
			{
				size_t len = cmdSize(tokens[1]);

				if (len > 0 && tokens[1][len - 1] == '\n') // cla'dan alınan \n'i ignore et
				{
					tokens[1][len - 1] = '\0';
				}

			    if (tokens[1] == NULL)
			    {
			        fprintf(stderr, "Too few arguments.\nUsage: cd <directory>\n");
			        exit(EXIT_FAILURE);
			    }

			    else
			    {
			    	int flag = chdir(tokens[1]);

			    	if (flag == -1 && tokens[1] != NULL)
			        {
					    fprintf(stderr, "chdir failed: %s\n", strerror(errno));
					    fprintf(stderr, "Could not change to directory: %s\n", tokens[1]);
			        }
			    }
			}

			else if (cmdCompare(tokens[0], cmd3))
			{
				pid_t childPID = fork();

			    if (childPID == -1) 
			    {
			        perror("Fork");
			        exit(EXIT_FAILURE);
			    }

			    if (childPID == 0) 
			    {
			        execl("/bin/pwd", "pwd", NULL);
			        perror("execl failed");
			        exit(EXIT_FAILURE);
			    } 

			    else 
			    {
			        int status;
			        wait(&status);

			        if (WIFEXITED(status) && WEXITSTATUS(status) != 0)
			        {
			            fprintf(stderr, "Child process failed with exit status %d\n", WEXITSTATUS(status));
			        }
			    }
			}

			else if (cmdCompare(tokens[0], cmd4))
			{
				pid_t childPID = fork();

				if (childPID == -1)
				{
					perror("Fork");
					exit(EXIT_FAILURE);
				}

				if (childPID == 0)
				{
					char * seperator = " ";

					for (int i = 1; i < tokenCount; ++i)
					{
						write(STDOUT_FILENO, tokens[i], cmdSize(tokens[i]));

						if (i < tokenCount - 1)
						{
							write(STDOUT_FILENO, seperator, cmdSize(seperator));
						}
					}
				}

				else 
			    {
			        int status;
			        wait(&status);

			        if (WIFEXITED(status) && WEXITSTATUS(status) != 0)
			        {
			            fprintf(stderr, "Child process failed with exit status %d\n", WEXITSTATUS(status));
			        }
			    }
			}

			else if (cmdCompare(tokens[0], cmd5)) // gerçek terminali kapatmak için current process id bulup system call yapıyoruz..
			{
				if (tokenCount > 1)
				{
					fprintf(stderr, "Too many arguments.\nUsage: exit\n");
				}

				else
				{
					kill(getppid(), 9);
					/*
					char realTerminalPID[20];
					snprintf(realTerminalPID, sizeof(realTerminalPID), "%d", getpid());

					if (kill(getpid(), SIGTERM) == -1) // Send SIGTERM signal directly
					{
						perror("Kill failed");
						exit(EXIT_FAILURE);
					}
					*/
	        	}
			}

			else if (cmdCompare(tokens[0], cmd6))
			{
				char buffer[CAT_BUFFER];

				size_t len = cmdSize(tokens[1]);

				if (len > 0 && tokens[1][len - 1] == '\n') // cla'dan alınan \n'i ignore et
				{
					tokens[1][len - 1] = '\0';
				}

				if (tokens[1] == NULL)
				{
					fprintf(stderr, "Too few arguments.\nUsage: cat <file name>\n");
					exit(EXIT_FAILURE);
				}

				int filename = open(tokens[1], O_RDONLY); 

				if (filename == -1)
				{
					perror("Cannot open file.\n");
					exit(EXIT_FAILURE);
				}

				ssize_t byte;
				lseek(filename, 0, SEEK_SET);

				byte = read(filename, buffer, CAT_BUFFER);

				if (byte == -1)
				{
					perror("Reading fail");
					close(filename);
					exit(EXIT_FAILURE);
				}

				else
				{
					ssize_t bytes_written = write(STDOUT_FILENO, buffer, byte);
			        
			        if (bytes_written == -1)
			        {
			            perror("Writing to STDOUT failed");
			            close(filename);
			            exit(EXIT_FAILURE);
			        }
				}
				close(filename);
			}

			else if (cmdCompare(tokens[0], cmd7))
			{
				size_t len = cmdSize(tokens[1]);

				if (len > 0 && tokens[1][len - 1] == '\n') // cla'dan alınan \n'i ignore et
				{
					tokens[1][len - 1] = '\0';
				}

				if (tokens[1] == NULL)
				{
					fprintf(stderr, "Too few arguments.\nUsage: touch <file name>\n");
					exit(EXIT_FAILURE);
				}

				int filename = open(tokens[1], O_CREAT | O_RDWR, S_IRUSR | S_IWUSR);

				if (filename == -1)
				{
					perror("Error creating a file");
					exit(EXIT_FAILURE);
				}
			}

			else if (cmdCompare(tokens[0], cmd8))
			{
				size_t len = cmdSize(tokens[1]);

				if (len > 0 && tokens[1][len - 1] == '\n') // cla'dan alınan \n'i ignore et
				{
					tokens[1][len - 1] = '\0';
				}

				if (tokens[1] == NULL)
				{
					fprintf(stderr, "Too few arguments.\nUsage: touch <file name>\n");
					exit(EXIT_FAILURE);
				}

				int filename = open(tokens[1], O_CREAT | O_RDWR, S_IRUSR | S_IWUSR);

				if (filename == -1)
				{
					perror("Error creating a file");
					exit(EXIT_FAILURE);
				}

				pid_t childPID = fork();

				if (childPID == -1)
				{
					perror("fork");
					exit(EXIT_FAILURE);
				}

				else if (childPID == 0)
				{	
					execlp("subl", "subl", tokens[1], NULL);
					perror("execlp");
					exit(EXIT_FAILURE);
				}

				else
				{
					wait(NULL);
				}
			}

			else if (cmdCompare(tokens[0], cmd9))
			{
				pid_t childPID = fork();

				if (childPID == -1)
				{
					perror("fork");
					exit(EXIT_FAILURE);
				}

				else if (childPID == 0)
				{
					execlp("rmdir", "rmdir", tokens[1], NULL);
					perror("execlp");
					exit(EXIT_FAILURE);
				}

				else
				{
					wait(NULL);
				}
			}

			else if (cmdCompare(tokens[0], cmd10))
			{
				size_t len = cmdSize(tokens[1]);

				if (len > 0 && tokens[0][len - 1] == '\n') // cla'dan alınan \n'i ignore et
				{
					tokens[0][len - 1] = '\0';
				}

				pid_t childPID = fork();

				if (childPID == -1)
				{
					perror("fork");
					exit(EXIT_FAILURE);
				}

				else if (childPID == 0)
				{
					execlp("mkdir", "mkdir", tokens[1], NULL);
					perror("execlp");
					exit(EXIT_FAILURE);
				}

				else
				{
					wait(NULL);
				}
			}

			else if (cmdCompare(tokens[0], cmd11))
			{
				pid_t childPID = fork();

				if (childPID == -1)
				{
					perror("fork");
					exit(EXIT_FAILURE);
				}

				else if (childPID == 0)
				{
					execlp("rm", "rm", tokens[1], NULL);
					perror("execlp");
					exit(EXIT_FAILURE);
				}

				else
				{
					wait(NULL);
				}
			}

			else
			{
				if (cmdCompare(tokens[0], cmd5) == 0)
				{
					size_t len = cmdSize(tokens[1]);

					if (len > 0 && tokens[0][len - 1] == '\n') // cla'dan alınan \n'i ignore et
					{
						tokens[0][len - 1] = '\0';
					}
					write(STDOUT_FILENO, tokens[0], cmdSize(tokens[0])); // -1 for last '\n' character
					write(STDOUT_FILENO, message2, cmdSize(message2));
				}
			}
		}
	}
	return 0;
}
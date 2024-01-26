
/*
 * CS354: Shell project
 *
 * Template file.
 * You will need to add more code here to execute the command table.
 *
 * NOTE: You are responsible for fixing any bugs this code may have!
 *
 */

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <string.h>
#include <signal.h>
#include <signal.h>
#include <iostream>
#include <fcntl.h>
#include <sys/stat.h>
#include <time.h>
#include <fstream>

#include "command.h"

time_t t; //<-- used in handler to know the time of child


using namespace std;
SimpleCommand::SimpleCommand()
{
	// Creat available space for 5 arguments
	_numberOfAvailableArguments = 5;
	_numberOfArguments = 0;
	_arguments = (char **)malloc(_numberOfAvailableArguments * sizeof(char *));
}

void SimpleCommand::insertArgument(char *argument)
{
	if (_numberOfAvailableArguments == _numberOfArguments + 1)
	{
		// Double the available space
		_numberOfAvailableArguments *= 2;
		_arguments = (char **)realloc(_arguments,
									  _numberOfAvailableArguments * sizeof(char *));
	}

	_arguments[_numberOfArguments] = argument;

	// Add NULL argument at the end
	_arguments[_numberOfArguments + 1] = NULL;

	_numberOfArguments++;
}

Command::Command()
{
	// Create available space for one simple command
	_numberOfAvailableSimpleCommands = 1;
	_simpleCommands = (SimpleCommand **)
		malloc(_numberOfSimpleCommands * sizeof(SimpleCommand *));

	_numberOfSimpleCommands = 0;
	_outFile = 0;
	_inputFile = 0;
	_errFile = 0;
	_background = 0;
	_append_flag = 0;
}

void Command::insertSimpleCommand(SimpleCommand *simpleCommand)
{
	if (_numberOfAvailableSimpleCommands == _numberOfSimpleCommands)
	{
		_numberOfAvailableSimpleCommands *= 2;
		_simpleCommands = (SimpleCommand **)realloc(_simpleCommands,
													_numberOfAvailableSimpleCommands * sizeof(SimpleCommand *));
	}

	_simpleCommands[_numberOfSimpleCommands] = simpleCommand;
	_numberOfSimpleCommands++;
}

void Command::clear()
{
	for (int i = 0; i < _numberOfSimpleCommands; i++)
	{
		for (int j = 0; j < _simpleCommands[i]->_numberOfArguments; j++)
		{
			free(_simpleCommands[i]->_arguments[j]);
		}

		free(_simpleCommands[i]->_arguments);
		free(_simpleCommands[i]);
	}

	if (_outFile)
	{
		free(_outFile);
	}

	if (_inputFile)
	{
		free(_inputFile);
	}

	if (_errFile)
	{
		free(_errFile);
	}

	_numberOfSimpleCommands = 0;
	_outFile = 0;
	_inputFile = 0;
	_errFile = 0;
	_background = 0;
	_append_flag = 0;
}

void Command::print()
{
	printf("\n\n");
	printf("              COMMAND TABLE                \n");
	printf("\n");
	printf("  #   Simple Commands\n");
	printf("  --- ----------------------------------------------------------\n");

	for (int i = 0; i < _numberOfSimpleCommands; i++)
	{
		printf("  %-3d ", i);
		for (int j = 0; j < _simpleCommands[i]->_numberOfArguments; j++)
		{
			printf("\"%s\" \t", _simpleCommands[i]->_arguments[j]);
		}
	}

	printf("\n\n");
	printf("  Output       Input        Error        Background\n");
	printf("  ------------ ------------ ------------ ------------\n");
	printf("  %-12s %-12s %-12s %-12s\n", _outFile ? _outFile : "default",
		   _inputFile ? _inputFile : "default", _errFile ? _errFile : "default",
		   _background ? "YES" : "NO");
	printf("\n\n");
}
void handler(int signo)
{     
                      time(&t);
       			ofstream fw ("child.log",std :: ios_base :: app);
       			if (fw.is_open()){
       				fw <<"child pid done at:"<<ctime(&t)<<"\n";
       				}
       			fw.close();  
    }
    

void Command::execute()
{
	// Don't do anything if there are no simple commands
	if (_numberOfSimpleCommands == 0)
	{
		prompt();
		return;
	}

	// Print contents of Command data structure
	print();

	// Add execution here
	// For every simple command fork a new process
	// Setup i/o redirection
	// and call exec

	// Save default input, output, and error because we will
	// change them during redirection and we will need to restore them
	// at the end.
	// The dup() system call creates a copy of a file descriptor.

	int defaultin = dup(0);
	int defaultout = dup(1);
	int defaulterr = dup(2);
	int outfile, inputfile, errorfile;
	int pid;
	if (_inputFile)
	{
		// O_RDONLY: Open the file for reading only.
		// O_WRONLY: Open the file for writing only.
		// O_RDWR: Open the file for reading and writing.
		inputfile = open(_inputFile, O_RDONLY);
		if (inputfile < 0)
		{
			perror("error in inputfile");
			exit(2);
		}
	}
	else
		inputfile = dup(defaultin);
	if (_errFile)
	{
		if (_append_flag)
		{
			// O_CREAT|O_APPEND|O_WRONLY: The flags that control how the file is opened. The O_CREAT flag tells the shell to create the file if it does not exist. The O_APPEND flag tells the shell to append data to the file instead of overwriting it. The O_WRONLY flag tells the shell to open the file for writing only.
			// 		S_IRWXU |S_IRWXG: The permissions that are applied to the file. The S_IRWXU flag gives the user read, write, and execute permissions for the file. The S_IRWXG flag gives the group read, write, and execute permissions for the file.
			errorfile = open(_errFile, O_CREAT | O_APPEND | O_WRONLY, S_IRWXU | S_IRWXG);
		}
		else
		{
			// The O_TRUNC flag tells the shell to truncate the file to zero length before opening it
			errorfile = open(_errFile, O_CREAT | O_TRUNC | O_WRONLY, S_IRWXU | S_IRWXG);
		}
	}
	else
		errorfile = dup(defaulterr);

	for (int i = 0; i < _numberOfSimpleCommands; i++)
	{
		dup2(inputfile, 0);
		close(inputfile);
		dup2(errorfile, 2);
		close(errorfile);
		// checks if the current iteration of the loop is the last iteration
		if (i == _numberOfSimpleCommands - 1)
		{

			if (_outFile )
			{	
				
				if (outfile < 0)
				{
					perror("error in create outfile");
					exit(2);
				}
				else
				{
					if (_append_flag)
					{
						outfile = open(_outFile, O_CREAT | O_APPEND | O_WRONLY, S_IRWXU | S_IRWXG);
					}
					else
					{
						outfile = open(_outFile, O_CREAT | O_TRUNC | O_WRONLY, S_IRWXU | S_IRWXG);
					}
				}
			}
			else
			{
				outfile = dup(defaultout);
			}
		}
		else
		{
			int fdpipe[2];
			pipe(fdpipe);
			outfile = fdpipe[1];
			inputfile = fdpipe[0];
		}
		dup2(outfile,1);
		close(outfile);

		pid = fork();
		if (pid == -1)
		{
			perror("forking child failed!");
			exit(2);
		}

		if (pid == 0)
		{
			// Child

			// You can use execvp() instead if the arguments are stored in an array
			// replaces the current process with the process specified by the _simpleCommands[i]->_arguments[ 0] argument. The _simpleCommands[i]->_arguments array contains the arguments for the process.
			execvp(_simpleCommands[i]->_arguments[0], _simpleCommands[i]->_arguments);

			// exec() is not suppose to return, something went wrong
			perror("execvp");
			exit(2);
		}
	}
	dup2(defaultin, 0);
	dup2(defaultout, 1);
	dup2(defaulterr, 2);
	close(defaultin);
	close(defaultout);
	close(defaulterr);
	if (!_background)
	{
		waitpid(pid, 0, 0);
	}
	

	// Clear to prepare for next command
	clear();

	// Print new prompt
	prompt();
}

// Shell implementation

void Command::prompt()
{
	printf("myshell>");
	fflush(stdout);
}

Command Command::_currentCommand;
SimpleCommand *Command::_currentSimpleCommand;

int yyparse(void);

int main()
{
	Command::_currentCommand.prompt();
	//  SIGINT -> the Ctrl+C signal
	// sets the signal handler for the SIGINT signal to SIG_IGN ->to ignore the signal.
	signal(SIGINT, SIG_IGN);
	signal(SIGCHLD,handler);
	yyparse();
	return 0;
}

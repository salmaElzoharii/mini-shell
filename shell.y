
/*
 * CS-413 Spring 98
 * shell.y: parser for shell
 *
 * This parser compiles the following grammar:
 *
 *	cmd [arg]* [> filename]
 *
 * you must extend it to understand the complete shell grammar
 *
 */

%token	<string_val> WORD

%token 	NOTOKEN GREAT GREATER PIPE NEWLINE LESS AND GREATAND GREATERAND CD EXIT

%union	{
		char   *string_val;
	}

%{
extern "C"
{
	int yylex();
	void yyerror (char const *s);
}
#define yylex yylex
#include <stdio.h>
#include "command.h"
#include <unistd.h>
%}

%%
goal:
	commands
	;

commands:
	command
	| commands command
	| commands PIPE command
	;

command: simple_command
    | complex_command
    ;

simple_command:
	 command_and_args error_opt NEWLINE  {
        printf("   Yacc: Execute command\n");
        Command::_currentCommand.execute();
    }
    | command_and_args AND error_opt NEWLINE  {
        printf("   Yacc: Execute command\n");
        Command::_currentCommand.execute();
    }
    | NEWLINE
	| error NEWLINE { yyerrok; }
	| EXIT NEWLINE {
    printf("   Yacc: Exit command\n");
    exit(0);
	}
	| CD NEWLINE
	{
		char dir[150];
		printf("Your directory is %s\n",getcwd(dir,150));
		chdir("/home");
		printf("Your current directory is %s\n",getcwd(dir,150));
		Command::_currentCommand.prompt();
	}
	|CD WORD NEWLINE
	{
		char dir[150];
		printf("Your directory is %s\n",getcwd(dir,150));
		chdir($2);
		printf("Your current directory is %s\n",getcwd(dir,150));
		Command::_currentCommand.prompt();
	}
	;

complex_command:
    command_and_args PIPE iomodifier_opt_list background_opt NEWLINE {
        Command::_currentCommand.execute();
    }
    | command_and_args iomodifier_opt_list NEWLINE {
        Command::_currentCommand.execute();
    }
    |
    ;

command_and_args:
	command_word arg_list {
		Command::_currentCommand.
			insertSimpleCommand( Command::_currentSimpleCommand );
	}
    |
    command_and_args PIPE command_word arg_list {
        Command::_currentCommand
            .insertSimpleCommand(Command::_currentSimpleCommand);
    }
    |
	;

arg_list:
	arg_list argument
	| /* can be empty */
	;

argument:
	WORD {
       printf("   Yacc: insert argument \"%s\"\n", $1);
       Command::_currentSimpleCommand->insertArgument( $1 );
	}
	;

command_word:
	WORD {
               printf("   Yacc: insert command \"%s\"\n", $1);

	       Command::_currentSimpleCommand = new SimpleCommand();
	       Command::_currentSimpleCommand->insertArgument( $1 );
	}
	;
iomodifier_opt_list:
	iomodifier_opt_list iomodifier_opt
	|
	;
iomodifier_opt:
	GREAT WORD {
		printf("   Yacc: insert output \"%s\"\n", $2);
		Command::_currentCommand._outFile = $2;
	}
	|GREATER WORD{
		printf("   Yacc: insert output \"%s\"\n", $2);
		Command::_currentCommand._append_flag =1;
		Command::_currentCommand._outFile = $2;

	}
	|LESS WORD{
		printf("   Yacc: insert input \"%s\"\n", $2);
		Command::_currentCommand._inputFile = $2;
	}
	| GREATAND WORD
	{
		Command::_currentCommand._errFile = $2;
	}
	| GREATERAND WORD
	{
		Command::_currentCommand._append_flag =1;
		Command::_currentCommand._errFile = $2;
	}
	;

background_opt:
		AND {
		Command::_currentCommand._background=1;
	}
	| /* can be empty */
	;

error_opt:
	GREATAND WORD{
		Command::_currentCommand._errFile = $2;
	}
	|
	;

%%

void
yyerror(const char * s)
{
	fprintf(stderr,"%s", s);
}

#if 0
main()
{
	yyparse();
}
#endif

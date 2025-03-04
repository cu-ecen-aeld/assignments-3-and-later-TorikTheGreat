#include "systemcalls.h"
#include <stdlib.h>
#define _XOPEN_SOURCE
#include <sys/types.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <syslog.h>

/**
 * @param cmd the command to execute with system()
 * @return true if the command in @param cmd was executed
 *   successfully using the system() call, false if an error occurred,
 *   either in invocation of the system() call, or if a non-zero return
 *   value was returned by the command issued in @param cmd.
*/
bool do_system(const char *cmd)
{

/*
 * TODO  add your code here
 *  Call the system() function with the command set in the cmd
 *   and return a boolean true if the system() call completed with success
 *   or false() if it returned a failure
*/

	int ret = system( cmd );
	if (ret == -1){
		return false;
	}

    return true;
}

/**
* @param count -The numbers of variables passed to the function. The variables are command to execute.
*   followed by arguments to pass to the command
*   Since exec() does not perform path expansion, the command to execute needs
*   to be an absolute path.
* @param ... - A list of 1 or more arguments after the @param count argument.
*   The first is always the full path to the command to execute with execv()
*   The remaining arguments are a list of arguments to pass to the command in execv()
* @return true if the command @param ... with arguments @param arguments were executed successfully
*   using the execv() call, false if an error occurred, either in invocation of the
*   fork, waitpid, or execv() command, or if a non-zero return value was returned
*   by the command issued in @param arguments with the specified arguments.
*/

bool do_exec(int count, ...)
{
    va_list args;
    va_start(args, count);
    char * command[count+1];
    int i;
    for(i=0; i<count; i++)
    {
        command[i] = va_arg(args, char *);
    }
    command[count] = NULL;
    // this line is to avoid a compile warning before your implementation is complete
    // and may be removed
    command[count] = command[count];

/*
 * TODO:
 *   Execute a system command by calling fork, execv(),
 *   and wait instead of system (see LSP page 161).
 *   Use the command[0] as the full path to the command to execute
 *   (first argument to execv), and use the remaining arguments
 *   as second argument to the execv() command.
 *
*/
	//openlog("do_exec", 0, LOG_USER);
	int status;
	pid_t pid;

	pid = fork(); // Create child process
	if (pid == -1){ // Child process creation failed
		return false;
	}

	else if (!pid){ // If pid is 0, we're dealing with the child process
		int ex_ret;
		//syslog(LOG_INFO, "execv inputs: %s, %s", command[0], command[1]);
		ex_ret = execv(command[0], command);
		if (ex_ret == -1){
			//syslog(LOG_INFO, "ex_ret is -1");
			exit(-1); // This if seems a bit redundant
		}

	}

	// Parent waiting
	if (waitpid (pid, &status, 0) == -1){ // wait failed
		return false;
	}

	else if (WEXITSTATUS(status) != 0){ // The command itself failed
		return false;
	}

    va_end(args);
	//closelog();
    return true;
}

/**
* @param outputfile - The full path to the file to write with command output.
*   This file will be closed at completion of the function call.
* All other parameters, see do_exec above
*/
bool do_exec_redirect(const char *outputfile, int count, ...)
{
	openlog("do_exec_redirect", 0, LOG_USER);
    va_list args;
    va_start(args, count);
    char * command[count+1];
    int i;
    for(i=0; i<count; i++)
    {
        command[i] = va_arg(args, char *);
    }
    command[count] = NULL;
    // this line is to avoid a compile warning before your implementation is complete
    // and may be removed
    command[count] = command[count];


/*
 * TODO
 *   Call execv, but first using https://stackoverflow.com/a/13784315/1446624 as a refernce,
 *   redirect standard out to a file specified by outputfile.
 *   The rest of the behaviour is same as do_exec()
 *
*/
	int kid_status;

	int kidpid;
	int fd = open(outputfile, O_WRONLY|O_TRUNC|O_CREAT, 0644);
	if (fd < 0) { syslog(LOG_ERR, "couldn't open file"); }
	switch (kidpid = fork()) {
  		case -1: return false; // Child creation failed
  		case 0: // Dealing with child process
    		if (dup2(fd, 1) < 0) { perror("dup2"); return false; }
    		close(fd);
			syslog(LOG_INFO, "redirect execv inputs: %s, %s", command[1], command[2]);
			int ex_ret = execv(command[0], command);
			if (ex_ret == -1){
				exit(-1);
			}
    		//execvp(cmd, args); perror("execvp"); abort();
  	default:
    	close(fd);

		// Parent: wait for child to finish
    	// Parent waiting
		if (waitpid (kidpid, &kid_status, 0) == -1){ // wait failed
			return false;
		}

		else if (WEXITSTATUS(kid_status) != 0){ // The command itself failed
			return false;
		}

	}

	closelog();
    va_end(args);
    return true;
}
 

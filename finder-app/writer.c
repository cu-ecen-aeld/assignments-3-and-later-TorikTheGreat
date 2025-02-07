#include <stdio.h>
#include <fcntl.h>
#include <stdlib.h>
#include <syslog.h>
#include <errno.h>
#include <unistd.h>
#include <string.h>

int main(int argc, char *argv[]){

    // Open syslog
    openlog("Assignment2", LOG_CONS, LOG_USER);

    // Return if amount of arguments is unexpected and log error
    if (argc != 3) {

	    printf("Received incorrect amount of parameters");
        syslog(LOG_ERR, "Invalid number of arguments: %d", argc);

        // Close log
        closelog();
	    return 1;
    }

    // Open file
    int fd = creat(argv[1], 0644);
    if (fd == -1) {
        perror("Failed opening file");
        syslog(LOG_ERR, "Failed opening file");

        // Close log
        closelog();
        return 1;
    }

    syslog(LOG_DEBUG, "Writing %s to %s", argv[2], argv[1]);

    // Write string to file
    size_t len = strlen(argv[2]);
    ssize_t nr = write(fd, argv[2], len);
    if(nr == -1){
        perror("Failed writing string to file");
        syslog(LOG_ERR, "Failed writing string to file");

        // Close log and file
        closelog();
        close(fd);
        return 1;
    }

    // If a different amount of chars was written than len,
    // then a partial write occured.
    else if(nr != len){
        printf("File partially written, try again");
    }

    // Close file
    if (close(fd) == -1){
        perror("Failed closing file");
        return 1;
    }

    return 0;

}

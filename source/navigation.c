#include <navigation.h>

/*
 * Returns the next valid character from the user. Blocks until there
 * is something to read.
 */
int fe_get_user_input()
{
    char input_char;
    int read_chars;

    /* Set up file descriptor set for the select syscall */
    fd_set read_fds;
    FD_ZERO(&read_fds);
    FD_SET(STDIN_FILENO, &read_fds);
    
    int ret = select(STDIN_FILENO+1, &read_fds, NULL, NULL, NULL);

    /* 
     * On success, select should return exactly one because we set only
     * one fd 
     */
    if(ret != 1)
    {
        perror("Waiting for user input");
        exit(EXIT_FAILURE);
    }

    if(!FD_ISSET(STDIN_FILENO, &read_fds))
    {
        printf("Wrong fd selected on waiting for user input\n");
        exit(EXIT_FAILURE);
    }

    read_chars=read(STDIN_FILENO, &input_char, 1);

    if(read_chars == -1)
    {
        printf("Error reading user input\n");
        exit(EXIT_FAILURE);
    }

    /*
     * This case shoudln't happen since the select told us that there is
     * something to read
     */
    if(read_chars == 0)
    {
        printf("Empty user input\n");
        exit(EXIT_FAILURE);
    }


    return input_char;
}

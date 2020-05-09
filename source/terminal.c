#include "terminal.h"

//
// TERMINAL - CURSOR POSITIONING
//

// Store the original termios in order to restore it on exit
static struct termios original_termios;

void fe_enable_raw_mode()
{
    /* 
     * Activate altrernate screen buffer to keep the original
     * terminal output
     */
    write( STDIN_FILENO, ALT_SCREEN, strlen( ALT_SCREEN ));
    struct termios raw;

    int ret = tcgetattr(STDIN_FILENO, &original_termios);
    if(ret == -1)
    {
        perror("Get terminal attributes");
        exit(EXIT_FAILURE);
    }

    atexit(fe_disable_raw_mode);
 
    raw = original_termios;

    // Input Modes
    raw.c_iflag &= ~(
            BRKINT | // ignore for example SIGINT
            INPCK  | // ignore parity errors
            ISTRIP | // don't strip 8th bit off
            INLCR  | // don't map CR to NL
            IXON     // disable input flow control
            );  

    // Output Modes
    raw.c_oflag &= ~(
            OPOST // disable any post processing
            );

    // Control Modes
    raw.c_cflag |= (
            CS8     // set char size to 8 bits
            );

    // Local Modes
    raw.c_lflag &= ~(
            ECHO    | // disable printing any character directly
            ICANON  | // disable line canonicalization
            IEXTEN  | // disable extensions
            ISIG      // disable signals
            );

    // Special Control Character
    raw.c_cc[VMIN] = 0;
    raw.c_cc[VTIME] = 0;

    if(tcsetattr(STDIN_FILENO, TCSAFLUSH, &raw) == -1)
    {
        perror("Enable raw mode in terminal");
        exit(EXIT_FAILURE);
    }
}

void fe_disable_raw_mode()
{
   int ret = tcsetattr(STDIN_FILENO, TCSAFLUSH, &original_termios);

    write( STDIN_FILENO, ORIG_SCREEN, strlen( ORIG_SCREEN ));
   if( ret == -1 )
   {
        perror("Disabling terminals raw mode");
        puts("Oops, we are still in raw mode.. Sorry!");
        exit(EXIT_FAILURE);
   }
}

TerminalSize fe_terminal_size()
{
    struct winsize ws;

    if(ioctl(STDIN_FILENO, TIOCGWINSZ, &ws) == -1)
    {
        perror("Get terminal size");
        exit(EXIT_FAILURE);
    }
    
    TerminalSize ts;
    ts.rows = ws.ws_row;
    ts.cols = ws.ws_col;

    return ts;
}

TerminalPosition fe_get_cursor_position()
{
    char buf[32] = {0};
    TerminalPosition p = {{0},{0}};

    // Query the current cursor position
    static const char *position_query = "\x1b[6n";

    if(write(STDIN_FILENO, position_query, strlen(position_query)) != 4)
    {
        perror("Querying cursor position");
        exit(EXIT_FAILURE);
    }

    
    unsigned i = 0;
    // Reading the response with the coordinates of the cursor
    while( i < sizeof(buf)-1)
    {
        if(read(STDOUT_FILENO, buf+i,1) != 1) break;
        if(buf[i] == 'R') break;
        i++;
    }

    buf[i] = '\0';
    
    if(sscanf(buf, "\x27[%u;%uR", &p.x, &p.y) == 0)
    {
        perror("Parsing cursor position");
        exit(EXIT_FAILURE);
    }

    return p;
}


static int fe_read_char()
{
    char input_char;
    int read_chars;
    read_chars=read(STDIN_FILENO, &input_char, 1);

    if(read_chars == -1)
    {
        printf("Error reading user input\n");
        exit(EXIT_FAILURE);
    }

    if(read_chars == 0)
        return 0;

    return input_char;
}

/*
 * Returns the next valid character from the user. Blocks until there
 * is something to read.
 *
 * Tries to parse an incoming escape sequence.
 * (http://man7.org/linux/man-pages/man4/console_codes.4.html)
 */
int fe_get_user_input()
{
    char input_char;


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

    
    input_char = fe_read_char();

    
    if(input_char == ESCAPE)
    {
        input_char = fe_read_char();

        if(input_char == 0)
            return ESCAPE;

        if(input_char != '[')
            return input_char;

        input_char = fe_read_char();

        switch(input_char)
        {
        case 'A':   return UP;
        case 'B':   return DOWN;
        case 'C':   return RIGHT;
        case 'D':   return LEFT;
        }
    }


    return input_char;
}

#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <termios.h>
#include <time.h>
#include <unistd.h>
#include <sys/ioctl.h>
#include <sys/select.h>

#include <terminal.h>


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

    const char* env_rows = getenv("ROWS");
    const char* env_cols = getenv("COLS");
    if( env_rows && env_cols ) 
    {
        ws.ws_row = strtol(env_rows, NULL, 10);
        ws.ws_col = strtol(env_cols, NULL, 10);
    } 
    else if(ioctl(STDIN_FILENO, TIOCGWINSZ, &ws) == -1)
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


static int fe_read_char(int timeout)
{
    char input_char;
    int read_chars;
    int ret;

    /* Set up file descriptor set for the select syscall */
    fd_set read_fds;
    FD_ZERO(&read_fds);
    FD_SET(STDIN_FILENO, &read_fds);
    
    /* In order to determmine if a char belongs to a key-combination
     * or not, differnt timeout setups are used.
     *
     * The first one should be always blocking and all further reads of
     * the parser should have a timeout. On timeout the parser knows that
     * the key-combination is done. This is necessary for reading the
     * buffer empty for unsupported combinations (e.g. F1-F4).
     */
    if(timeout){
        struct timeval timeout = {.tv_usec = 100};
        ret = select(STDIN_FILENO+1, &read_fds, NULL, NULL, &timeout);
    } else { 
        ret = select(STDIN_FILENO+1, &read_fds, NULL, NULL, NULL);
    }

    /* Early return on timeout */
    if(ret == 0) return 0;

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

    if(read_chars == 0)return 0;

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
    char input_char = fe_read_char(0);

    if(input_char != ESCAPE) return input_char;

    input_char = fe_read_char(1);

    if(input_char == 0)
        return ESCAPE;

    if(input_char != '[')
    {
        /* 
         * Read buffer empty 
         *
         * F1 to F4 generate nasty ESC+x+y sequences
         * where x and y are printable chars (P-S)
         */

        while(fe_read_char(1));
        return -1;
    }

    input_char = fe_read_char(1);

    switch(input_char)
    {
    case 'A':   return UP;
    case 'B':   return DOWN;
    case 'C':   return RIGHT;
    case 'D':   return LEFT;
    case 'H':   return HOME;
    case 'F':   return END;
    }

    if(input_char >= '0' && input_char <= '9')
    {
      char next_char = fe_read_char(1);
      if(next_char == 0) return ESCAPE;

      if(next_char == '~') {
          switch(input_char) {
            case '3': return DELETE;
            case '5': return PAGE_UP;
            case '6': return PAGE_DOWN;
          }
      }
      if(next_char == ';' && input_char == '1'){
        /* parse codes with prefix [1; */
        input_char = fe_read_char(1);

        if(input_char == '5') {
            next_char = fe_read_char(1);
            switch(next_char) {
            case 'A': return HOME;
            case 'B': return END;
            case 'C': return PAGE_DOWN;
            case 'D': return PAGE_UP;
            }
        }

      }
    }
  
    /* Read buffer empty */
    while(fe_read_char(1));
  
    return -1;
}

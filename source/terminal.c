#include "terminal.h"

//
// TERMINAL - CURSOR POSITIONING
//

// Store the original termios in order to restore it on exit
static struct termios original_termios;

void fe_enable_raw_mode()
{
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
   if(tcsetattr(STDIN_FILENO, TCSAFLUSH, &original_termios) == -1)
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

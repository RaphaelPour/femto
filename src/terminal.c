#include "terminal.h"

//
// TERMINAL - CURSOR POSITIONING
//

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

terminal_size fe_terminal_size()
{
    struct winsize ws;

    if(ioctl(STDIN_FILENO, TIOCGWINSZ, &ws) == -1)
    {
        perror("Get terminal size");
        exit(EXIT_FAILURE);
    }
    
    terminal_size ts;
    ts.rows = ws.ws_row;
    ts.cols = ws.ws_col;

    return ts;
}



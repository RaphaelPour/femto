#ifndef TERMINAL_H
#define TERMINAL_H

#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <termios.h>
#include <unistd.h>
#include <sys/ioctl.h>
#include <sys/select.h>

//
// terminos(2)
//
// Input Modes
//
// IGNBRK   /* ignore BREAK condition */
// BRKINT   /* map BREAK to SIGINTR */
// IBGNPAR   /* ignore (discard) parity errors */
// IPARMRK   /* mark parity and framing errors */
// INPCK    /* enable checking of parity errors */
// ISTRIP   /* strip 8th bit off chars */
// INLCR    /* map NL into CR */
// IGNCR    /* ignore CR */
// ICRNL    /* map CR to NL (ala CRMOD) */
// IXON     /* enable output flow control */
// IXOFF    /* enable input flow control */
// IXANY    /* any char will restart after stop */
// IMAXBEL  /* ring bell on input queue full */
// IUCLC    /* translate upper case to lower case */
//

// Output Modes
//
// OPOST   /* enable following output processing */
// ONLCR   /* map NL to CR-NL (ala CRMOD) */
// OXTABS  /* expand tabs to spaces */
// ONOEOT  /* discard EOT's `^D' on output) */
// OCRNL   /* map CR to NL */
// OLCUC   /* translate lower case to upper case */
// ONOCR   /* No CR output at column 0 */
// ONLRET  /* NL performs CR function */
//

// Control Modes
//
// CSIZE       /* character size mask */
// CS5         /* 5 bits (pseudo) */
// CS6         /* 6 bits */
// CS7         /* 7 bits */
// CS8         /* 8 bits */
// CSTOPB      /* send 2 stop bits */
// CREAD       /* enable receiver */
// PARENB      /* parity enable */
// PARODD      /* odd parity, else even */
// HUPCL       /* hang up on last close */
// CLOCAL      /* ignore modem status lines */
// CCTS_OFLOW  /* CTS flow control of output */
// CRTSCTS     /* same as CCTS_OFLOW */
// CRTS_IFLOW  /* RTS flow control of input */
// MDMBUF      /* flow control output via Carrier */
//

// Local Modes
// 
// ECHOKE      /* visual erase for line kill */
// ECHOE       /* visually erase chars */
// ECHO        /* enable echoing */
// ECHONL      /* echo NL even if ECHO is off */
// ECHOPRT     /* visual erase mode for hardcopy */
// ECHOCTL     /* echo control chars as ^(Char) */
// ISIG        /* enable signals INTR, QUIT, [D]SUSP */
// ICANON      /* canonicalize input lines */
// ALTWERASE   /* use alternate WERASE algorithm */
// IEXTEN      /* enable DISCARD and LNEXT */
// EXTPROC     /* external processing */
// TOSTOP      /* stop background jobs from output */
// FLUSHO      /* output being flushed (state) */
// NOKERNINFO  /* no kernel output from VSTATUS */
// PENDIN      /* XXX retype pending input (state) */
// NOFLSH      /* don't flush after interrupt */
//

// Special Control Characters
//
// Index Name    Special Character    Default Value
// VEOF          EOF                  ^D
// VEOL          EOL                  _POSIX_VDISABLE
// VEOL2         EOL2                 _POSIX_VDISABLE
// VERASE        ERASE                ^? `\177'
// VWERASE       WERASE               ^W
// VKILL         KILL                 ^U
// VREPRINT      REPRINT              ^R
// VINTR         INTR                 ^C
// VQUIT         QUIT                 ^\\ `\34'
// VSUSP         SUSP                 ^Z
// VDSUSP        DSUSP                ^Y
// VSTART        START                ^Q
// VSTOP         STOP                 ^S
// VLNEXT        LNEXT                ^V
// VDISCARD      DISCARD              ^O
// VMIN          ---                  1
// VTIME         ---                  0
// VSTATUS       STATUS               ^T


#define ALT_SCREEN "\033[?1049h"
#define ORIG_SCREEN "\033[?1049l"

// Terminal size struct

typedef struct {
   union { unsigned width, cols; };
   union { unsigned height, rows; };
} TerminalSize;

typedef struct {
    union { unsigned x, col; };
    union { unsigned y, row; };
} TerminalPosition;


void fe_enable_raw_mode();
void fe_disable_raw_mode();

TerminalSize fe_terminal_size();
TerminalPosition fe_get_cursor_position();

int fe_get_user_input();

enum KEYS {
    TAB = 9,
    ENTER_MAC = 10,
    ENTER = 13,
    CTRL_S = 19,
    ESCAPE = 27,
    BACKSPACE = 127,
    UP = 128,
    DOWN,
    LEFT,
    RIGHT,
    DELETE
};

#endif

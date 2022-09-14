#include "headers.h"

extern struct termios term;

void disableRawMode()
{
    if (tcsetattr(STDIN_FILENO, TCSAFLUSH, &term) == -1)
    {
        perror("tcsetattr");
        exit(0);
    }
}

void enableRawMode()
{
    if (tcgetattr(STDIN_FILENO, &term) == -1)
    {
        perror("tcgetattr");
        exit(0);
    }
    struct termios raw = term;
    raw.c_lflag &= ~(ICANON | ECHO);
    if (tcsetattr(STDIN_FILENO, TCSAFLUSH, &raw) == -1)
    {
        perror("tcsetattr");
        exit(0);
    }
}

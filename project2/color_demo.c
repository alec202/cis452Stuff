#include <stdio.h>

// ANSI color codes for output
#define COLOR_RED           "\033[4;38;5;52m"
#define COLOR_GREEN         "\033[4;38;5;22m"
#define COLOR_YELLOW        "\033[4;38;5;58m"
#define COLOR_BLUE          "\033[4;38;5;17m"
#define COLOR_MAGENTA       "\033[4;38;5;53m"
#define COLOR_CYAN          "\033[4;38;5;23m"
#define COLOR_UNDERLINED_ORANGE  "\033[4;38;5;130m"
#define COLOR_BRIGHT_RED     "\033[0;91m"
#define COLOR_BRIGHT_GREEN   "\033[0;92m"
#define COLOR_BRIGHT_YELLOW  "\033[0;93m"
#define COLOR_BRIGHT_BLUE    "\033[0;94m"
#define COLOR_BRIGHT_MAGENTA "\033[0;95m"
#define COLOR_BRIGHT_CYAN    "\033[0;96m"
#define COLOR_BRIGHT_ORANGE         "\033[38;5;208m"
#define COLOR_RESET          "\033[0m"  // Resets text color

void printColorDemo() {
    printf(COLOR_RED           "This is Red\n"            COLOR_RESET);
    printf(COLOR_GREEN         "This is Green\n"          COLOR_RESET);
    printf(COLOR_YELLOW        "This is Yellow\n"         COLOR_RESET);
    printf(COLOR_BLUE          "This is Blue\n"           COLOR_RESET);
    printf(COLOR_MAGENTA       "This is Magenta\n"        COLOR_RESET);
    printf(COLOR_CYAN          "This is Cyan\n"           COLOR_RESET);
    printf(COLOR_BRIGHT_RED     "This is Bright Red\n"     COLOR_RESET);
    printf(COLOR_BRIGHT_GREEN   "This is Bright Green\n"   COLOR_RESET);
    printf(COLOR_BRIGHT_YELLOW  "This is Bright Yellow\n"  COLOR_RESET);
    printf(COLOR_BRIGHT_BLUE    "This is Bright Blue\n"    COLOR_RESET);
    printf(COLOR_BRIGHT_MAGENTA "This is Bright Magenta\n" COLOR_RESET);
    printf(COLOR_BRIGHT_CYAN    "This is Bright Cyan\n"    COLOR_RESET);
    printf(COLOR_BRIGHT_ORANGE         "This is Orange\n"         COLOR_RESET);
    printf(COLOR_UNDERLINED_ORANGE  "This is UNDERLINED DARK Orange\n"  COLOR_RESET);
}

int main() {
    printColorDemo();
    return 0;
}

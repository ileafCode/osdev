#include "kbScancodeTranslation.h"

namespace QWERTYKeyboard
{

    const char ASCII_table_lower[] =
    {
         0 ,  0 , '1', '2',
        '3', '4', '5', '6',
        '7', '8', '9', '0',
        '-', '=', '\b','\t',
        'q', 'w', 'e', 'r',
        't', 'y', 'u', 'i',
        'o', 'p', '[', ']',
        '\n',  0 , 'a', 's',
        'd', 'f', 'g', 'h',
        'j', 'k', 'l', ';',
        '\'','`',  0 , '\\',
        'z', 'x', 'c', 'v',
        'b', 'n', 'm', ',',
        '.', '/',  0 , '*',
         0 , ' '
    };

    const char ASCII_table_cl[] =
    {
         0 ,  0 , '1', '2',
        '3', '4', '5', '6',
        '7', '8', '9', '0',
        '-', '=', '\b','\t',
        'Q', 'W', 'E', 'R',
        'T', 'Y', 'U', 'I',
        'O', 'P', '[', ']',
         0 ,  0 , 'A', 'S',
        'D', 'F', 'G', 'H',
        'J', 'K', 'L', ';',
        '\'','`',  0 , '\\',
        'Z', 'X', 'C', 'V',
        'B', 'N', 'M', ',',
        '.', '/',  0 , '*',
         0 , ' '
    };

    const char ASCII_table_shift[] =
    {
         0 ,  0 , '!', '@',
        '#', '$', '%', '^',
        '&', '*', '(', ')',
        '_', '+', '\b','\t',
        'Q', 'W', 'E', 'R',
        'T', 'Y', 'U', 'I',
        'O', 'P', '{', '}',
         0 ,  0 , 'A', 'S',
        'D', 'F', 'G', 'H',
        'J', 'K', 'L', ':',
        '\"', 0 ,  0 , '|',
        'Z', 'X', 'C', 'V',
        'B', 'N', 'M', '<',
        '>', '?',  0 , '*',
         0 , ' '
    };

    char Translate(uint8_t scancode, bool shift, bool caps)
    {
        if (scancode > 58)
            return 0;

        if (caps)
            return ASCII_table_cl[scancode];
        else if (shift)
            return ASCII_table_shift[scancode];
        else
            return ASCII_table_lower[scancode];
    }

}
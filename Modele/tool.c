#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include "tool.h"

void replace_underscore_with_space(char *str) {
    if (str == NULL) return;
    for (int i = 0; str[i] != '\0'; i++) {
        if (str[i] == '_') {
            str[i] = ' ';
        }
    }
}
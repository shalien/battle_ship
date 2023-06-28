#include <string.h>
#include <stdlib.h>
#include "point.h"

/*
 * Get a POINT from a string and assign it's state if needed
 */
POINT str_to_point(char *str, int has_direction) {
    POINT point;
    char strX[2 + 1];
    int i;

    // Protect has_direction range
    if (has_direction > 0) {
        has_direction = 1;
        point.state = str[strlen(str) - 1];
    } else {
        has_direction = 0;
    }

    // Split the str
    point.y = str[0] - 'a';

    for (i = 0; i < strlen(str) - (1 + has_direction); i++) {
        strX[i] = str[i + 1];
    }
    for (i + 1; i < strlen(strX); i++) {
        strX[i] = '\0';
    }


    point.x = strtol(strX, NULL, 10) - 1;

    return point;
}
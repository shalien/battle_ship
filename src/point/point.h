#ifndef BATTLESHIP_POINT_H
#define BATTLESHIP_POINT_H


typedef struct POINT {
    int x;
    int y;
    char state;
} POINT;

POINT str_to_point(char *str, int has_direction);

#endif //BATTLESHIP_POINT_H

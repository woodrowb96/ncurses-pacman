#ifndef COORD_H
#define COORD_H

struct Coord{
  int x;
  int y;
};

//operator overloads used for coord struct
bool operator==(const Coord& l, const Coord& r);
bool operator!=(const Coord& l, const Coord& r);

//addition and subtraction
Coord operator+(const Coord& l, const Coord& r);
Coord operator-(const Coord& l, const Coord& r);

//get distance between two coords
int distance(const Coord l, const Coord r);

#endif

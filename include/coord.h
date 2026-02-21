#ifndef COORD_H
#define COORD_H

struct Coord
{
  int x;
  int y;
};

//logical overloads
bool operator==(const Coord& l, const Coord& r);
bool operator!=(const Coord& l, const Coord& r);

//arithmetic overloads
Coord operator+(const Coord& l, const Coord& r);
Coord operator-(const Coord& l, const Coord& r);

int squared_dist(const Coord& l, const Coord& r);

#endif

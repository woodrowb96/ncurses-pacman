#include "coord.h"

bool operator==(const Coord& l, const Coord& r)
{
  return l.y == r.y && l.x == r.x;
}

bool operator!=(const Coord& l, const Coord& r)
{
  return l.y != r.y || l.x != r.x;
}

Coord operator+(const Coord& l, const Coord& r)
{
  return Coord{l.x + r.x, l.y + r.y};
}

Coord operator-(const Coord& l, const Coord& r)
{
  return Coord{l.x - r.x, l.y - r.y};
}

int distance(const Coord l, const Coord r)
{
  int x_diff = (l.x - r.x);
  int y_diff = l.y - r.y;
  return x_diff * x_diff + y_diff * y_diff;
}


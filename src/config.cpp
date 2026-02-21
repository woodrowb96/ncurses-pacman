#include "config.h"
#include "coord.h"

#include <list>
#include <string>
#include <fstream>

using std::ifstream;
using std::string;
using std::list;

constexpr const char* LOCATIONS_FILE {"assets/level_1_locations.txt"};
constexpr const char* SHAPES_FILE {"assets/level_1_shapes.txt"};

//calculate the coordinates of the shape formed by the symbol chars in the file
const list<Coord> gen_coordinates(const string& file, char symbol)
{
  ifstream ist{file};
  char c {'\0'};
  list<Coord> coords;
  Coord coord {0,0};   //the top left of the file will have coord (0,0)

  //dont skip the white space as we loop
  //we want it to calc the coords
  while(ist>> std::noskipws >> c) {
    if(c == symbol) {               //if we are at a symbol
      coords.push_back(coord);      //save the coord
      coord.x++;                    //move x to the right
    }
    else if(c == 'e') {             //else if we are at the end of the line
      coord.x = 0;                  //reset x
      coord.y++;                    //move y down a line
    }
    else {                          //else
      coord.x++;                    //just move x to the right
    }
  }

  return coords;
}

//Same as gen_coordinates, but returns the coord of the first 'symbol' it sees
const Coord gen_coordinate(const string& file, char symbol)
{
  ifstream ist{file};
  char c{' '};
  Coord coord {0,0};

  while(ist>> std::noskipws >> c) {
    if(c == symbol) {
      return coord;     //return as soon as we hit 'symbol'
    }
    else if(c == 'e') {
      coord.x = 0;
      coord.y++;
    }
    else {
      coord.x++;
    }
  }

  return Coord{-1,-1};    //return (-1,-1) if the symbols not in the file
}


/**************** PIECE LOCATIONS ********************************/
//pacman
const Coord Locations::PACMAN_START = gen_coordinate(LOCATIONS_FILE, '<');

//pinky
const Coord Locations::PINKY_START = gen_coordinate(LOCATIONS_FILE, 'P');
const Coord Locations::PINKY_SCATTER = gen_coordinate(LOCATIONS_FILE, 'p');

//blinky
const Coord Locations::BLINKY_START = gen_coordinate(LOCATIONS_FILE, 'B');
const Coord Locations::BLINKY_SCATTER = gen_coordinate(LOCATIONS_FILE, 'b');

//clyde
const Coord Locations::CLYDE_START = gen_coordinate(LOCATIONS_FILE, 'C');
const Coord Locations::CLYDE_SCATTER = gen_coordinate(LOCATIONS_FILE, 'c');

//inky
const Coord Locations::INKY_START = gen_coordinate(LOCATIONS_FILE, 'I');
const Coord Locations::INKY_SCATTER = gen_coordinate(LOCATIONS_FILE, 'i');

//warps
const Coord Locations::LEFT_WARP = gen_coordinate(LOCATIONS_FILE, 'l');
const Coord Locations::RIGHT_WARP = gen_coordinate(LOCATIONS_FILE, 'r');

/**************** PIECE SHAPES ********************************/
const list<Coord> Shapes::BORDER = gen_coordinates(SHAPES_FILE, '#');
const list<Coord> Shapes::INV_WALLS = gen_coordinates(SHAPES_FILE, 'x');
const list<Coord> Shapes::POINTS = gen_coordinates(SHAPES_FILE, '.');
const list<Coord> Shapes::POWER_UPS = gen_coordinates(SHAPES_FILE, '!');
const list<Coord> Shapes::GHOST_HOME = gen_coordinates(SHAPES_FILE, '$');

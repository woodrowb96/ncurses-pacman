#include "config.h"
#include "coord.h"

#include <list>
#include <string>
#include <fstream>

using std::ifstream;
using std::string;
using std::list;


//used to read in shape coordinates from a txt file and return them as a list<Cooord>
//function will look for charecters matching symbol in file and add their coordinates
//to the list
const list<Coord> gen_coordinates(string file, char symbol)
{

  list<Coord> coords;       //list to store coords

  ifstream ist{file};       //open file

  char c {'\0'};            //char to read file into

  int x {0};                //x and y coords, used to calc shapes coords as they are read in
  int y {0};

  while(ist>> std::noskipws >> c)     //read in file, dont skip white space
  {
    if(c == symbol)                   //if symbol matches shapes symbol
    {
      coords.push_back(Coord{x,y});   //add coord to list
      x++;                            //increment x
    }
    else if(c == 'e')   //look for 'e' to indicate end of line
    {
      x = 0;            //reset x coord
      y++;              //move y to next line
    } 
    else 
    {
      x++;        //increment x if we havent reached the end of line
    }
  }
  return coords;
}


//reads in a single coord from a text file, used to indicate a pieces location on the screen
const Coord gen_coordinate(string file, char symbol)
{

  ifstream ist{file};

  char c{' '};
  int x{0};
  int y{0};

  while(ist>> std::noskipws >> c)
  {
    if(c == symbol) 
    {
      return Coord{x,y};
    }
    else if(c == 'e') 
    {
      x = 0;
      y++;
    } 
    else
    {
      x++;
    }
  }
  return Coord{x,y};
}


//Piece locations
const Coord Locations::PACMAN_START = gen_coordinate("assets/level_1_locations.txt", '<');

const Coord Locations::PINKY_START = gen_coordinate("assets/level_1_locations.txt", 'P');
const Coord Locations::PINKY_SCATTER = gen_coordinate("assets/level_1_locations.txt", 'p');

const Coord Locations::BLINKY_START = gen_coordinate("assets/level_1_locations.txt", 'B');
const Coord Locations::BLINKY_SCATTER = gen_coordinate("assets/level_1_locations.txt", 'b');

const Coord Locations::CLYDE_START = gen_coordinate("assets/level_1_locations.txt", 'C');
const Coord Locations::CLYDE_SCATTER = gen_coordinate("assets/level_1_locations.txt", 'c');

const Coord Locations::INKY_START = gen_coordinate("assets/level_1_locations.txt", 'I');
const Coord Locations::INKY_SCATTER = gen_coordinate("assets/level_1_locations.txt", 'i');

const Coord Locations::LEFT_WARP = gen_coordinate("assets/level_1_locations.txt", 'l');
const Coord Locations::RIGHT_WARP = gen_coordinate("assets/level_1_locations.txt", 'r');

//Piece shapes
const list<Coord> Shapes::BOARDER = gen_coordinates("assets/level_1_shapes.txt", '#');
const list<Coord> Shapes::INV_WALLS = gen_coordinates("assets/level_1_shapes.txt", 'x');
const list<Coord> Shapes::POINTS = gen_coordinates("assets/level_1_shapes.txt", '.');
const list<Coord> Shapes::POWER_UPS = gen_coordinates("assets/level_1_shapes.txt", '!');
const list<Coord> Shapes::GHOST_HOME = gen_coordinates("assets/level_1_shapes.txt", '$');

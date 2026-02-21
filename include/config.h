#ifndef CONFIG_H
#define CONFIG_H

#include "coord.h"
#include <string>
#include <list>

namespace Symbols
{
  constexpr char PACMAN {'<'};
  constexpr char PINKY {'P'};
  constexpr char PINKY_FRIGHTENED {'p'};
  constexpr char BLINKY {'B'};
  constexpr char BLINKY_FRIGHTENED {'b'};
  constexpr char CLYDE {'C'};
  constexpr char CLYDE_FRIGHTENED {'c'};
  constexpr char INKY {'I'};
  constexpr char INKY_FRIGHTENED {'i'};
  constexpr char GHOST_EATEN {'X'};
  constexpr char BORDER {'#'};
  constexpr char POINTS {'.'};
  constexpr char POWER_UPS {'!'};
  constexpr char INVISIBLE {' '};
}

// shapes and location coords are generated in config.cpp
namespace Shapes
{
  const std::list<Coord> POINT { {0,0} };
  extern const std::list<Coord> BORDER;
  extern const std::list<Coord> INV_WALLS;
  extern const std::list<Coord> POINTS;
  extern const std::list<Coord> POWER_UPS;
  extern const std::list<Coord> GHOST_HOME;
}

namespace Locations
{
  const Coord TOP_LEFT {0,0};
  extern const Coord PACMAN_START;
  extern const Coord PINKY_START;
  extern const Coord PINKY_SCATTER;
  extern const Coord BLINKY_START;
  extern const Coord BLINKY_SCATTER;
  extern const Coord CLYDE_START;
  extern const Coord CLYDE_SCATTER;
  extern const Coord INKY_START;
  extern const Coord INKY_SCATTER;
  extern const Coord LEFT_WARP;
  extern const Coord RIGHT_WARP;
}

//Dimensions and positions of our windows
namespace Dimensions
{
  constexpr int GAME_SCR_H {25};
  constexpr int GAME_SCR_W {60};
  const Coord GAME_SCR_COORD {0,0};

  constexpr int STAT_SCR_H {5};
  constexpr int STAT_SCR_W {30};
  const Coord STAT_SCR_COORD {(GAME_SCR_COORD.y + GAME_SCR_H), 0};

  constexpr int MSG_SCR_H {9};
  constexpr int MSG_SCR_W {GAME_SCR_W};
  const Coord MSG_SCR_COORD = {5,0};
}

namespace Inputs
{
  constexpr int QUIT {'Q'};
  constexpr int PLAY {'p'};
  constexpr int UP {'w'};
  constexpr int DOWN {'s'};
  constexpr int LEFT {'a'};
  constexpr int RIGHT {'d'};
  constexpr int NO_INPUT {'\0'};
}

namespace Pause
{
  constexpr int SHORT {190};
  constexpr int MEDIUM {300};
  constexpr int LONG {600};
}

namespace GameConfig
{
  constexpr int STARTING_LEVEL {1};
  constexpr int PACMAN_START_LIVES {3};
  constexpr int PACMAN_START_POINTS {0};
  constexpr int GHOST_VALUE {20};
  constexpr int POINT_VALUE {5};
  constexpr int POWER_UP_VALUE {10};
  constexpr int CHASE_LENGTH {50};
  constexpr int SCATTER_LENGTH {20};
  constexpr int POWER_UP_LENGTH {40};
  constexpr int POWER_UP_BLINK_LENGTH {2};
}

namespace GameText
{
  constexpr const char* GAME_OVER_MSG {"\n\tGAME OVER"
                                   "\n\tPress p to play again"
                                   "\n\tPress Q to exit\n"};

  constexpr const char* START_MSG {"\n\t\t\t\tPACMAN"
                                "\n"
                                "\n\t Use w,s,a,d keys to move up,down,left and right."
                                "\n\t Press Q to quit at any time."
                                "\n"
                                "\n\t Press p to start game"
                                "\n\t Press Q to exit"
                                "\n\t Thank you, for playing!"};
}
#endif

#ifndef Config_H
#define Config_H

#include "coord.h"
#include <string>
#include <list>

namespace Symbols {
  const char PACMAN  {'<'};
  const char PINKY {'P'};
  const char PINKY_FRIGHTENED  {'p'};
  const char BLINKY  {'B'};
  const char BLINKY_FRIGHTENED {'b'};
  const char CLYDE  {'C'};
  const char CLYDE_FRIGHTENED {'c'};
  const char INKY  {'I'};
  const char INKY_FRIGHTENED {'i'};
  const char GHOST_EATEN {'X'};
  const char BOARDER {'#'};
  const char POINTS  {'.'};
  const char POWER_UPS {'!'};
  const char INVISIBLE {' '};
};



// shapes and location coords are generated in config.cpp
namespace Shapes{
  const std::list<Coord> POINT { {0,0} };
  extern const std::list<Coord> BOARDER;
  extern const std::list<Coord> INV_WALLS;
  extern const std::list<Coord> POINTS;
  extern const std::list<Coord> POWER_UPS;
  extern const std::list<Coord> GHOST_HOME;
}

namespace Locations{
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

namespace Dimensions{
  const int GAME_SCR_H {25};
  const int GAME_SCR_W {60};
  const Coord GAME_SCR_COORD {0,0};

  const int STAT_SCR_H {5};
  const int STAT_SCR_W {30};
  const Coord STAT_SCR_COORD = {(GAME_SCR_COORD.y + GAME_SCR_H), 0};

  const int MSG_SCR_H {9};
  const int MSG_SCR_W {GAME_SCR_W};
  const Coord MSG_SCR_COORD = {5,0};
};

namespace Inputs{
  const int QUIT {'Q'};
  const int PLAY {'p'};
  const int UP {'w'};
  const int DOWN {'s'};
  const int LEFT {'a'};
  const int RIGHT {'d'};
  const int NO_INPUT {'\0'};
}

namespace Pause{
  const int SHORT {190};
  const int MEDIUM {300};
  const int LONG {600};
}

namespace GameConfig{
  const int STARTING_LEVEL {1};
  const int PACMAN_START_LIVES {3};
  const int PACMAN_START_POINTS {0};
  const int GHOST_VALUE {20};
  const int POINT_VALUE {5};
  const int POWER_UP_VALUE {10};
  const int CHASE_LENGTH {50};
  const int SCATTER_LENGTH {20};
  const int POWER_UP_LENGTH {40};
  const int POWER_UP_BLINK_LENGTH {2};
};

namespace GameText{
  const std::string GAME_OVER_MSG {"\n\tGAME OVER"
                                   "\n\tPress p to play again"
                                   "\n\tPress Q to exit\n"};

  const std::string START_MSG {"\n\t\t\t\tPACMAN"
                                "\n"
                                "\n\t Use w,s,a,d keys to move up,down,left and right."
                                "\n\t Press Q to quit at any time."
                                "\n"
                                "\n\t Press p to start game"
                                "\n\t Press Q to exit"
                                "\n\t Thank you, for playing!"};
}
#endif

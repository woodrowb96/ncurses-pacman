#ifndef PIECES_H
#define PIECES_H

#include "config.h"
#include "coord.h"

#include <list>
#include <ncurses.h>

////////////  Piece //////////
/*
 * A piece is the most generic thing that can be printed onto the screen
 *
 * m_shape describes the pieces shape in the game window
 *
 * m_location described the pieces position in the game window, and can be added to each
 * coord in m_shape to print the piece onto the screen
 *
 * m_symbol is what char is used to represent the piece on screen
 *
 * m_symbol is accesed through a m_blinker, which is used to blink the piece on the screen
 * by switching between m_sybmol and an INVISIBLE symbol
 *
 */
class Piece
{
  public:
    Piece(Coord location, std::list<Coord> shape, char symbol);

    const std::list<Coord>& shape() const;

    Coord location() const;
    char symbol() const;      //will return the blinked symbol, not necessarily m_symbol

    void blink();            //blink the symbol returned by symbol()

    bool in(const Piece* p); //true if any of p's coords overlap with this piece
    bool in(Coord c);        //true if coord overlaps with this piece

    void print(WINDOW* w);        //print to window
    void print();                 //print to ncurses std screen

  protected:
    Coord m_location;             //pieces location on screen
    std::list<Coord> m_shape;          //coords describing pieces shape

    char m_symbol;

    //used to blink between invisible symbol and m_symbol
    const char* m_blinker[2]  {&m_symbol, &Symbols::INVISIBLE};

    void set_symbol(char symbol);     //set m_symbol

};

////////////  DynamicPiece  /////////////////////
/*
 * DynamicPieces are Pieces that can be moved around the screen
 *
 * each DynamicPiece has a m_momentum variable wich discribes the pieces current
 * direction its moving in
 *
 * each DynamicPiece also has a m_home_coord which it can jump back to
 *
 */

enum Momentum {up, down, left, right, still};   //used to indicate pieces current direction

class DynamicPiece : public Piece
{
  public:
    DynamicPiece(Coord location, std::list<Coord> shape, char symbol, Momentum start_m);

    Momentum momentum() const;

    void up(int n_spaces = 1);   //move in direction and set momentum in that dir
    void down(int n_spaces = 1);
    void left(int n_spaces = 1);
    void right(int n_spaces = 1);

    void jump(Coord coord);                   //jump to coord and keep current momentum
    void jump(Coord coord, Momentum new_m);   //jump and get new momentum

    void jump_home();                         //go to home coord with current momentum
    void jump_home(Momentum new_m);           //go home and set new momentum

    bool is_home();                       //see if we are at home coordinate
  protected:
    Momentum m_momentum;    //current momentum
    Coord m_home;           //home coordinate
};

////////////  PacMan  /////////////////////

class Ghost;    //forward decleration of Ghost

enum class EatenFlag{not_eaten, eaten};  //used to indicate if piece was eaten

class PacMan : public DynamicPiece
{
  public:
    PacMan();

    int points() const;
    void inc_points(int inc);       //increment points

    int lives() const;
    void dec_lives();               //decrement by 1 live

    bool check_eaten(Ghost* ghost); //check if pacman is eaten, also sets eaten flag
    bool eaten();                   //return true if eaten flag is set

    void reset();     //reset momentum, lives, points and location

    void reset_eaten_flag();   //reset eaten flag to not eaten
  private:
    int m_lives;
    int m_points;

    EatenFlag m_eaten_flag {EatenFlag::not_eaten};
};

////////////  Ghost  /////////////////////

enum class PursuitState;    //forward decleration from game.h

enum class GhostState{chase, scatter, turn_around, frightened, eaten};

/*
 * Ghosts have the following states, which determin behavior
 *
 * chase: go to chase target
 * Scatter: go to scatter target
 * turn_around: turn around
 * frightened: go in random direction
 * eaten: go to home coord
 *
 * the game class determines the current state, and calculates current targets
 * see the game class's update_ghost_state for next state logic
 *
 */

class Ghost : public DynamicPiece
{
  public:
    GhostState state() const;
    void set_state(GhostState new_state);

    //use games pursuit state to set current ghost state
    void set_state(PursuitState pursuit_state);

    int value();

    bool eats(PacMan* p);         //check if ghost eats pacman
    bool check_eaten(PacMan* p);  //check if ghost is eaten by pacman and set eaten flag

    bool eaten();          //return true if eaten flag is set

    void reset();         //reset location and momentum

    void reset_eaten_flag();

  protected:
    Ghost(Coord coord, char chase_symbol, char frigh_symbol, Coord scatter_coord);

  private:
    char m_chase_symbol;      //symbol in chase and scatter states
    char m_fright_symbol;     //symbol during turn_around and frightened states
    char m_eaten_symbol {Symbols::GHOST_EATEN}; //symbol during eaten state

    int m_value {GameConfig::GHOST_VALUE};   //points pacman gets when eating a ghost

    GhostState m_ghost_state  {GhostState::scatter};

    EatenFlag m_eaten_flag {EatenFlag::not_eaten};

    void update_symbol();         //update symbol based on current state
};

////////////  Pinky  /////////////////////

class Pinky : public Ghost 
{
  public:
    Pinky();
};

////////////  Blinky  /////////////////////

class Blinky : public Ghost 
{
  public:
    Blinky();
};

////////////  CLYDE  /////////////////////

class Clyde : public Ghost 
{
  public:
    Clyde();
};

////////////  INKY  /////////////////////

class Inky : public Ghost 
{
  public:
    Inky();
};

////////////  Boarders  /////////////////////

class Boarders : public Piece 
{
  public:
    Boarders();
};

////////////  InvWalls  /////////////////////

class InvWalls : public Piece 
{
  public:
    InvWalls();
};

//////////// Warp //////////////////////
class Warp : public Piece
{
  public:
    Warp(Coord location);
};

//////////// Left Warp //////////////////////
class LeftWarp : public Warp
{
  public:
    LeftWarp();
};

//////////// Right Warp //////////////////////
class RightWarp : public Warp
{
  public:
    RightWarp();
};

////////////  Scoring Piece  /////////////////////

/*
 * Whenever a point is scored, scoring piece will remove the scoring coordinate from
 * m_shape coordinates.
 *
 * m_shape can be reset using m_original_shape
 *
 */

enum class ScoreFlag {no_score, score};   //used to indicate if there was a score

class ScoringPiece : public Piece
{
  public:
    ScoringPiece(Coord location, std::list<Coord> shape, char symbol, int value);

    int value();

    //checks if there is a score, removes  scoring coord and sets score flag
    bool check_score(Piece* p);

    bool score();    //return true if score flag is set

    void reset();   //reset shape to original shape

    void reset_score_flag();
  private:
    std::list<Coord> m_original_shape;     //original shape of points
    int m_value;

    ScoreFlag m_score_flag {ScoreFlag::no_score};
};

////////////  Points  /////////////////////

class Points : public ScoringPiece{
  public:
    Points();

    bool all_eaten();     //return true if all points were eaten
};

////////////  Powerups  /////////////////////

enum class PowerUpState{off, active};   //power up state is either off or activated

class PowerUps : public ScoringPiece{
  public:
    PowerUps();

    PowerUpState state();
    void set_state(PowerUpState new_state);

  private:
    PowerUpState m_power_up_state {PowerUpState::off};
};


#endif

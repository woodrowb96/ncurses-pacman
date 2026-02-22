#ifndef PIECES_H
#define PIECES_H

#include "config.h"
#include "coord.h"

#include <list>
#include <ncurses.h>

/********************************** PIECE ***********************************/
// A piece is the most generic type of object that can be drawn on screen
//
// This class provides some common functionality used by all pieces:
//  -draw(window): draw the pieces shape onto the window
//  -blink(): blink the pieces symbol (i.e switch from 'x' to ' ' and vice versa)
//  -in(coord): check if this pieces coords overlap with a single coordinate
//  -in(piece): check if this pieces coords overlap with another pieces coords
/********************************************************************************/
class Piece
{
  public:
    Piece(Coord location, std::list<Coord> shape, char symbol);

    //getters
    const std::list<Coord>& shape() const;
    Coord location() const;
    char symbol() const;     //returns char in m_blinker[0], not neccesarily m_symbol

    void draw(WINDOW* w);    //draw m_shape at m_location on the window
    void blink();
    bool in(const Piece* other);
    bool in(Coord coord);

  protected:
    Coord m_location;           //coord relative to the windows coords
    std::list<Coord> m_shape;   //coords relative to m_location
    char m_symbol;
    const char* m_blinker[2] {&m_symbol, &Symbols::INVISIBLE};

    void set_symbol(char symbol);
};

/******************************** DYNAMIC PIECE ********************************/
// A dynamic piece is the most generic type of piece with movement
//
// Each dynamic piece has:
//  - a momentum to indicate its current direction of movement
//  - a home coord it can jump back to
//
// This class provides some common functionality used by all dynamic pieces
//  -movement functions: move piece in a certain dir
//  -jump: jump to an arbitrary coord
/********************************************************************************/

enum Momentum {up, down, left, right, still};

class DynamicPiece : public Piece
{
  public:
    DynamicPiece(Coord location, std::list<Coord> shape, char symbol, Momentum start_m);

    //getter
    Momentum momentum() const;

    //movement functions
    void up(int n_spaces = 1);
    void down(int n_spaces = 1);
    void left(int n_spaces = 1);
    void right(int n_spaces = 1);

    //jump and preserve momentum
    void jump(Coord coord);
    void jump_home();

    //jump and get new momentum
    void jump(Coord coord, Momentum new_m);
    void jump_home(Momentum new_m);

    //are we at the home coord
    bool is_home();

  protected:
    Momentum m_momentum;
    Coord m_home;
};

/*********************************** PACMAN ************************************/
//Our pacman game piece
//
//Pacman keeps track of:
//  -its current amount of lives
//  -how many points its eaten
//  -if it is currently being eaten
/********************************************************************************/
class Ghost;  //forward declaration

enum class EatenFlag {not_eaten, eaten};

class PacMan : public DynamicPiece
{
  public:
    PacMan();

    int points() const;
    void inc_points(int inc);

    int lives() const;
    void dec_lives();

    bool check_eaten(Ghost* ghost); //set the eaten flag, also returns true if we are being eaten
    bool eaten();                   //return true if we are being eaten
    void reset_eaten_flag();        //reset eaten flag to not eaten

    void reset();                   //reset momentum, lives, points and location

  private:
    int m_lives;
    int m_points;

    EatenFlag m_eaten_flag {EatenFlag::not_eaten};
};


/********************************** GHOST ***********************************/
//Our parent for all the ghost types.
//
//Ghosts have 5 states that determine their behavior
//  -chase: chase the target
//  -scatter: go to their scatter target
//  -turn_around: turn around and go in the opposite direction
//  -frightened: go in a random direction
//  -eaten: go back to their home coordinate
/********************************************************************************/
enum class PursuitState;    //forward declaration from game.h

enum class GhostState {chase, scatter, turn_around, frightened, eaten};

class Ghost : public DynamicPiece
{
  public:
    //getters
    GhostState state() const;
    int value();

    //setters
    void set_state(GhostState new_state);         //set state directly
    void set_state(PursuitState pursuit_state);   //use the games current PursuitState to calc and set the state

    bool eats(PacMan* p);         //check if ghost eats pacman

    bool check_eaten(PacMan* p);  //check if ghost is eaten by pacman and set the eaten state
    bool eaten();                 //return true if ghost is being eaten
    void reset_eaten_flag();

    void reset();                 //resets location and momentum

  protected:
    //protected constructor, we dont want to ever make a plain ghost
    Ghost(Coord coord, char chase_symbol, char fright_symbol, Coord scatter_coord);

  private:
    GhostState m_ghost_state  {GhostState::scatter};
    EatenFlag m_eaten_flag {EatenFlag::not_eaten};

    char m_chase_symbol;
    char m_fright_symbol;
    char m_eaten_symbol {Symbols::GHOST_EATEN};

    int m_value {GameConfig::GHOST_VALUE};  //points pacman gets when it eats the ghost

    void update_symbol();                   //update symbol based on current state
};

/************************ PINKY, BLINKY, INKY AND CLYDE ************************/

class Pinky : public Ghost
{
  public:
    Pinky();
};

class Blinky : public Ghost
{
  public:
    Blinky();
};

class Clyde : public Ghost
{
  public:
    Clyde();
};

class Inky : public Ghost
{
  public:
    Inky();
};

/***************************** BORDERS and INVWALLS *****************************/
//  All the border and invisible wall pieces for our game.
/********************************************************************************/
class Borders : public Piece
{
  public:
    Borders();
};

class InvWalls : public Piece 
{
  public:
    InvWalls();
};

/************************** WARP, LEFTWARP, RIGHTWARP ***************************/
//  A plain warp with generic coord, then our left and right warps
//  with their coords baked in
/********************************************************************************/

class Warp : public Piece
{
  public:
    Warp(Coord location);
};

class LeftWarp : public Warp
{
  public:
    LeftWarp();
};

class RightWarp : public Warp
{
  public:
    RightWarp();
};

/********************************** SCORINGPIECE ***********************************/
// A class for non-ghost scoring pieces.
//
// When a score happens the coord that was eaten gets removed from the shape
/*******************************************************************************/

enum class ScoreFlag {no_score, score};

class ScoringPiece : public Piece
{
  public:
    ScoringPiece(Coord location, std::list<Coord> shape, char symbol, int value);

    int value();

    bool check_score(Piece* p);   //if its a score: set the flag, remove the scoring coord, return true
    bool score();                 //return true if score flag is set
    void reset_score_flag();

    void reset();                 //reset shape to original shape

  private:
    ScoreFlag m_score_flag {ScoreFlag::no_score};
    std::list<Coord> m_original_shape;
    int m_value;

};

/******************************  POINTS *********************************/
// the point pellets pacman eats
/************************************************************************/

class Points : public ScoringPiece
{
  public:
    Points();

    bool all_eaten();   //return true if all points were eaten
};

/*********************************** POWERUPS ***********************************/
// Power ups are a special type of scoring piece, because they give pacman point 
// and a power-up when eaten.
//
// The power-up behavior needs special logic to be disabled when the power up is active.
/********************************************************************************/

enum class PowerUpState {off, active};

class PowerUps : public ScoringPiece
{
  public:
    PowerUps();

    PowerUpState state();

    void set_state(PowerUpState new_state);

  private:
    PowerUpState m_power_up_state {PowerUpState::off};
};

#endif

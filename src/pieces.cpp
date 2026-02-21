#include "pieces.h"
#include "coord.h"
#include "config.h"
#include "game.h"

#include <list>
#include <stdlib.h>
#include <ncurses.h>

using std::list;

/*****************    Piece   **********************************/
Piece::Piece(Coord location, list<Coord> shape, char symbol)
  :
  m_location {location},
  m_symbol {symbol}
{
  for(Coord coord : shape) {
    m_shape.push_back(coord);
  }
}

const list<Coord>& Piece::shape() const { return m_shape; }

Coord Piece::location() const { return m_location; }

char Piece::symbol() const 
{
  //print what is currently in index 0 of blinker to the screen
  return *m_blinker[0];
}

void Piece::blink()
{
  //swap pointers in blinker to change what is printed to screen
  const char* temp = m_blinker[0];
  m_blinker[0] = m_blinker[1];
  m_blinker[1] = temp;
}

bool Piece::in(const Piece* p)
{
  //compare each of our coords to each coord of p
  for(Coord my_coord : m_shape) {
    for(Coord their_coord : p->shape()) {
      //need to add location to each coord to get true location on screen
      if( (my_coord + m_location) == (their_coord + p->location()) )
        return true;
    }
  }
  return false;     //if no coords overlap return false
}

bool Piece::in(Coord c)
{
  for(Coord my_coord : m_shape) {
    if( (my_coord + m_location) == c) {//add m_location to m_coord to get true location on scrn
      return true;
    }
  }
  return false;
}

void Piece::draw(WINDOW* w)
{
  for(Coord coord : m_shape) {
    Coord location =  coord + m_location;
    mvwaddch(w, location.y, location.x, symbol());
  }
}

void Piece::set_symbol(char symbol) { m_symbol = symbol; }

/********************   DynamicPiece    ********************************/
DynamicPiece::DynamicPiece(Coord location, list<Coord> shape, char symbol, Momentum start_m)
  :Piece(location, shape, symbol),
    m_momentum  {start_m},
    m_home{location}
{}


Momentum DynamicPiece::momentum() const { return m_momentum; }

void DynamicPiece::jump(Coord coord) 
{
  m_location = coord;
}

void DynamicPiece::jump(Coord coord, Momentum new_m)
{
  m_location = coord;
  m_momentum = new_m;
}

void DynamicPiece::up(int n_spaces)
{
  m_location.y -= n_spaces;
  m_momentum = Momentum::up;

}

void DynamicPiece::down(int n_spaces)
{
  m_location.y += n_spaces;
  m_momentum = Momentum::down;
}

void DynamicPiece::right(int n_spaces)
{
  m_location.x += n_spaces;
  m_momentum = Momentum::right;
}

void DynamicPiece::left(int n_spaces)
{
  m_location.x -= n_spaces;
  m_momentum = Momentum::left;
}

void DynamicPiece::jump_home()
{
  m_location = m_home;
}

void DynamicPiece::jump_home(Momentum new_m)
{
  m_location = m_home;
  m_momentum = new_m;
}

bool DynamicPiece::is_home()
{
  return m_location == m_home;
}

/**************************** PACMAN ************************************/

PacMan::PacMan()
  :DynamicPiece(Locations::PACMAN_START, Shapes::POINT, Symbols::PACMAN, Momentum::left),
  m_lives {GameConfig::PACMAN_START_LIVES},
  m_points {GameConfig::PACMAN_START_POINTS}
{}

int PacMan::points() const { return m_points; }

void PacMan::inc_points(int inc)
{
  m_points += inc; 
}

int PacMan::lives() const { return m_lives; }

void PacMan::dec_lives()
{
  if(m_lives > 0) {     //cant have negative lives
    m_lives--;
  }
}

bool PacMan::check_eaten(Ghost* ghost)
{
  if(ghost->eats(this)) {             //if ghost eats pacman
    m_eaten_flag = EatenFlag::eaten;  //set eaten flag
    m_lives--;                        //dec lives
    return true;
  }
  m_eaten_flag = EatenFlag::not_eaten;  //if not eaten then set not_eaten flag
  return false;
}

bool PacMan::eaten()
{
  return m_eaten_flag == EatenFlag::eaten;
}

void PacMan::reset()
{
  jump_home(Momentum::left);                    //go home and set momentum left
  m_lives = GameConfig::PACMAN_START_LIVES;     //reset lives
  m_points = GameConfig::PACMAN_START_POINTS;   //reset points
  m_eaten_flag = EatenFlag::not_eaten;          //reset eaten flag
}

void PacMan::reset_eaten_flag()
{
  m_eaten_flag = EatenFlag::not_eaten;
}

/********************* Ghost ****************************************/

Ghost::Ghost(Coord location, char chase_symbol, char fright_symbol, Coord scatter_target)
  : DynamicPiece(location, Shapes::POINT, chase_symbol, Momentum::still), 
  m_chase_symbol  {chase_symbol},
  m_fright_symbol {fright_symbol}
{}

GhostState Ghost::state() const { return m_ghost_state; }

void Ghost::set_state(GhostState new_state)
{
  m_ghost_state = new_state;          //set new state
  update_symbol();                    //and update symbol
}

void Ghost::set_state(PursuitState pursuit_state)
{
  switch(pursuit_state) {
    case PursuitState::chase: {
      set_state(GhostState::chase);
      break;
    }
    case PursuitState::scatter: {
      set_state(GhostState::scatter);
      break;
    }
  }
}


void Ghost::update_symbol()
{
  switch(m_ghost_state) {
    case GhostState::chase:
    case GhostState::scatter: {
      set_symbol(m_chase_symbol);
      break;
    }
    case GhostState::frightened:
    case GhostState::turn_around: {
      set_symbol(m_fright_symbol);
      break;
    }
    case GhostState::eaten: {
      set_symbol(m_eaten_symbol);
      break;
    }
  }
}

int Ghost::value() { return m_value; }

bool Ghost::eats(PacMan* p)
{
  //can only eat pacman in chase and scatter states
  if(m_ghost_state == GhostState::chase || m_ghost_state == GhostState::scatter) {
    return in(p);
  }
  return false;
}

bool Ghost::check_eaten(PacMan* p)
{
  //can only be eaten in frightened and turn_around states
  if(m_ghost_state == GhostState::frightened || m_ghost_state == GhostState::turn_around) {
    if( in(p) ) {
      m_eaten_flag = EatenFlag::eaten;    //set eaten flag
      return true;
    }
  }
  m_eaten_flag = EatenFlag::not_eaten;    //set not eaten flag
  return false;
}

bool Ghost::eaten()
{
  return m_eaten_flag == EatenFlag::eaten;
}

void Ghost::reset()
{
  jump_home(Momentum::still);
  m_ghost_state = GhostState::scatter;
  m_eaten_flag = EatenFlag::not_eaten;
}

void Ghost::reset_eaten_flag()
{
  m_eaten_flag = EatenFlag::not_eaten;
}

/*************** Pinky **************************/

Pinky::Pinky()
  : Ghost(Locations::PINKY_START, 
          Symbols::PINKY,
          Symbols::PINKY_FRIGHTENED,
          Locations::PINKY_SCATTER) 
{}

/********  Blinky   *************/

Blinky::Blinky()
  : Ghost(Locations::BLINKY_START,
          Symbols::BLINKY, 
          Symbols::BLINKY_FRIGHTENED,
          Locations::BLINKY_SCATTER) 
{}

/********  CLyde   *************/

Clyde::Clyde()
  : Ghost(Locations::CLYDE_START,
          Symbols::CLYDE, 
          Symbols::CLYDE_FRIGHTENED,
          Locations::CLYDE_SCATTER) 
{}

/********  Inky   *************/

Inky::Inky()
  : Ghost(Locations::INKY_START,
          Symbols::INKY,
          Symbols::INKY_FRIGHTENED,
          Locations::INKY_SCATTER) 
{}

/*********  Borders  **********/

Borders::Borders()
  : Piece(Locations::TOP_LEFT, Shapes::BORDER, Symbols::BORDER) {}

/*******************  INVWALL   ******************/

InvWalls::InvWalls()
  :Piece(Locations::TOP_LEFT, Shapes::INV_WALLS, Symbols::INVISIBLE) {}

/******************* Warp *************/
Warp::Warp(Coord location)
  :Piece(location, Shapes::POINT, Symbols::INVISIBLE) {}

/******************* LeftWarp *************/
LeftWarp::LeftWarp()
  :Warp(Locations::LEFT_WARP) {}

/******************* LeftWarp *************/
RightWarp::RightWarp()
  :Warp(Locations::RIGHT_WARP) {}

  /*******  ScoringPiece   *********/

ScoringPiece::ScoringPiece(Coord location, list<Coord> shape, char symbol, int value)
  : Piece(location, shape, symbol), 
  m_original_shape {shape},
  m_value{value}
{}

int ScoringPiece::value() { return m_value; }

bool ScoringPiece::check_score(Piece* p)
{
  /*
   * there is a score if any coord in p overlaps with this piece
   *
   * if there is a score
   * remove overlaping coord from this piece
   * and set score flag
   *
   */
  for(Coord p_c: p->shape())
    for(Coord c : m_shape) {
      if( (c + m_location) == (p_c + p->location())) {   //add locations to get true screen location
        m_shape.remove(c);                  //remove coord
        m_score_flag = ScoreFlag::score;    //set score flag
        return true;
      }
    }
  m_score_flag = ScoreFlag::no_score;   //if no score set no_score flag
  return false;
}

bool ScoringPiece::score()
{
  return m_score_flag == ScoreFlag::score;
}

void ScoringPiece::reset()
{
  m_shape = m_original_shape;
  m_score_flag = ScoreFlag::no_score;
}
/************   Points   **************/

Points::Points()
  :ScoringPiece(Locations::TOP_LEFT, Shapes::POINTS, Symbols::POINTS, GameConfig::POINT_VALUE) {}

bool Points::all_eaten()
{
  return m_shape.empty();
}

void ScoringPiece::reset_score_flag()
{
  m_score_flag = ScoreFlag::no_score;
}

/************   Powerups   **************/

PowerUps::PowerUps()
  :ScoringPiece(Locations::TOP_LEFT, Shapes::POWER_UPS, Symbols::POWER_UPS, GameConfig::POWER_UP_VALUE) {}

PowerUpState PowerUps::state() { return m_power_up_state; }

void PowerUps::set_state(PowerUpState new_state) { m_power_up_state = new_state; }


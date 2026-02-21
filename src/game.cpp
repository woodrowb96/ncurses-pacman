#include "game.h"

#include <limits>
#include <thread>
#include <chrono>

using std::string;
using std::rand;
using std::to_string;
using std::vector;

Game::Game()
:
  m_game_win {Dimensions::GAME_SCR_H, Dimensions::GAME_SCR_W, Dimensions::GAME_SCR_COORD},
  m_stat_win {Dimensions::STAT_SCR_H, Dimensions::STAT_SCR_W, Dimensions::STAT_SCR_COORD},
  m_message_win {Dimensions::MSG_SCR_H, Dimensions::MSG_SCR_W, Dimensions::MSG_SCR_COORD}
{
  //add pieces to midground
  m_game_win.add(&m_pacman, WindowLayer::midground);
  m_game_win.add(&m_pinky, WindowLayer::midground);
  m_game_win.add(&m_blinky, WindowLayer::midground);
  m_game_win.add(&m_clyde, WindowLayer::midground);
  m_game_win.add(&m_inky, WindowLayer::midground);
  //add pieces to background
  m_game_win.add(&m_boarders, WindowLayer::background);
  m_game_win.add(&m_points, WindowLayer::background);
  m_game_win.add(&m_power_ups, WindowLayer::background);
}

void Game::run()
{
  //print starting message
  m_message_win.update_text(GameText::START_MSG);
  m_message_win.print();

  //loop and get valid input
  char input {'\0'};
  while( (input = m_scrn.get_ch(InputMode::block)) != Inputs::PLAY && input != Inputs::QUIT)
    continue;

  //if quit then exit game
  if(input == Inputs::QUIT)
    return;

  //else start game loop
  game_loop();
}

void Game::game_loop()
{
  char input {'\0'};

  m_game_win.print();
  print_stats();

  while( (input = m_scrn.get_ch(InputMode::non_block)) != Inputs::QUIT )  //get input exit if quit
  {

    //move pieces
    move_pacman(input);

    //check for eaten pieces
    check_pacman_eaten();
    check_ghosts_eaten();

    //print game
    m_game_win.print();

    if(!m_pacman.eaten())   //dont move ghost if pacman was eaten
    {
      move_ghosts();

      //check for eaten pieces
      check_pacman_eaten();
      check_ghosts_eaten();

      //print game
      m_game_win.print();
    }

    //check scores
    check_points_scored();
    check_power_ups_scored();

    //increment pacmans score
    calc_pacman_score();

    //update states
    update_power_ups_state();
    update_ghost_states();
    update_pursuit_state();

    //print stats
    print_stats();

    //check for game over
    if(m_pacman.lives() <= 0)
    {
      if(!play_again())
        return;
      reset_game();
    }

    //check for end of lever
    if(m_points.all_eaten())   //go to next level if all points are eaten
      reset_level();

    //reset piece positions if pacman was eaten
    if(m_pacman.eaten())
      reset_piece_positions();

    //reset score and eaten flags
    reset_piece_flags();

    //blink power ups
    blink_power_ups();

    //pause program to slow game loop
    pause(Pause::SHORT);
  }
}

void Game::move_pacman(char input)
{
  Coord current = m_pacman.location();
  Coord up = current + Coord{0,-1};
  Coord down = current + Coord{0,1};
  Coord right = current + Coord{2,0};
  Coord left = current + Coord{-2,0};

  switch(input)   //go to input direction
  {
    case Inputs::UP:
    {
      if(!m_boarders.in(up) && !m_inv_walls.in(up))   //check if direction is a collision
        m_pacman.up();                                //if not move
      else
        pacman_keep_moving();                         //else keep moving in momentum direction
      break;
    }
    case Inputs::DOWN:
    {
      if(!m_boarders.in(down) && !m_inv_walls.in(down))
        m_pacman.down();
      else 
        pacman_keep_moving();
      break;
    }
    case Inputs::RIGHT:
    {
      if(!m_boarders.in(right) && !m_inv_walls.in(right)) 
        m_pacman.right(2);
      else 
        pacman_keep_moving();
      break;
    }
    case Inputs::LEFT:
    {
      if(!m_boarders.in(left) && !m_inv_walls.in(left)) 
        m_pacman.left(2);
      else
        pacman_keep_moving();
      break;
    }
    default:
    {
      pacman_keep_moving();     //if no input direction then keep moving with momentum
    }
  }

  check_for_warp(&m_pacman);    //check for a warp
}

void Game::pacman_keep_moving()
{
  Coord current = m_pacman.location();
  Coord up = current + Coord{0,-1};
  Coord down = current + Coord{0,1};
  Coord right = current + Coord{2,0};
  Coord left = current + Coord{-2,0};

  switch(m_pacman.momentum())   //go to current momentum
  {
    case Momentum::up:
    {
      if(!m_boarders.in(up) && !m_inv_walls.in(up))   //see if momentum direct is a collision
        m_pacman.up();                                //if not then move
      break;                                          //else dont move
    }
    case Momentum::down:
    {
      if(!m_boarders.in(down) && !m_inv_walls.in(down))
        m_pacman.down();
      break;
    }
    case Momentum::left:
    {
      if(!m_boarders.in(left) && !m_inv_walls.in(left))
        m_pacman.left(2);
      break;
    }
    case Momentum::right:
    {
      if(!m_boarders.in(right) && !m_inv_walls.in(right))
        m_pacman.right(2);
      break;
    }
    default:
    {
      break;
    }
  }
}

void Game::move_ghosts()
{
  move_ghost(&m_blinky, blinky_target());   //move each ghost toward their target
  move_ghost(&m_pinky, pinky_target());
  move_ghost(&m_clyde, clyde_target());
  move_ghost(&m_inky, inky_target());
}

void Game::move_ghost(Ghost* ghost, Coord target)
{
  //use target to calculate ghosts destination
  Destination destination = calc_ghost_destination(ghost, target);

  switch(destination)   //go to destination and move
  {
    case Destination::go_up: 
    {
      ghost->up();
      break;
    }
    case Destination::go_down: 
    {
      ghost->down();
      break;
    }
    case Destination::go_left:
    {
      ghost->left(2);
      break;
    }
    case Destination::go_right:
    {
      ghost->right(2);
      break;
    }
    default:
    {
      break;    //if no destination stay still
    }
  }

  check_for_warp(ghost);
}

Game::Destination Game::calc_ghost_destination(Ghost* ghost, Coord target)
{
  /* 
   * Ghosts will move in the valid direction that minimizes linear distance to target
   *
   * Ghost can only turn around during the first turn after a power up has been activated
   * or if a ghost is trapped in a corner with only the space behind it being valid
   *
   */

  Coord current = ghost->location();
  Coord right = current + Coord{2,0};
  Coord down = current + Coord{0,1};
  Coord left = current + Coord{-2,0};
  Coord up = current + Coord{0,-1};

  Destination destination = Destination{Destination::stay_still};
  int min_distance = std::numeric_limits<int>::max();

  //go through each direction
  //first check if direction is valid
  //if it is valid then check if its the new minimum distance

  //can only turn around when in turn_around state
  if(ghost->momentum() != Momentum::left || ghost->state() == GhostState::turn_around)
  {
    if(!m_boarders.in(right))                             //cant go into boarders
    {
      if(!m_inv_walls.in(right))                          //cant go into an inv wall
      {
        if(scaled_distance(right,target) <= min_distance)  //check if min distance
        {
          destination = Destination::go_right;            //if it is update direction
          min_distance = scaled_distance(right,target);   //set new direction
        }
      }
    }
  }

  if(ghost->momentum() != Momentum::up || ghost->state() == GhostState::turn_around)
  {
    if( !m_boarders.in(down))
    {
      if(!m_inv_walls.in(down) || ghost->state() == GhostState::eaten)  //we can go down through inv wall if eaten
      {
        if(scaled_distance(down,target) <= min_distance)
        {
          destination = Destination::go_down;
          min_distance = scaled_distance(down,target);
        }
      }
    }
  }

  if(ghost->momentum() != Momentum::right || ghost->state() == GhostState::turn_around)
  {
    if(!m_boarders.in(left))
    {
      if(!m_inv_walls.in(left))
      {
        if(scaled_distance(left,target) <= min_distance)
        {
          destination = Destination::go_left;
          min_distance = scaled_distance(left,target);
        }
      }
    }
  }

  if(ghost->momentum() != Momentum::down || ghost->state() == GhostState::turn_around)
  {
    if(!m_boarders.in(up))
    {
      if(!m_inv_walls.in(up) || true)       //ghosts can always go up through inv walls
      {
        if(scaled_distance(up,target) <= min_distance)
        {
          destination = Destination::go_up;
          min_distance = scaled_distance(up,target);
        }
      }
    }
  }


  //now we can check if ghost is boxed in in three directions exept one
  if(destination == Destination::stay_still)
  {
    if(m_boarders.in(left) && m_boarders.in(right) && m_boarders.in(up))
    {
      if(!m_boarders.in(down))                //if only one valid direction ghost can turn around
        destination = Destination::go_down;
    }
    else if(m_boarders.in(left) && m_boarders.in(right) && m_boarders.in(down))
    {
      if(!m_boarders.in(up))
        destination = Destination::go_up;
    }
    else if(m_boarders.in(left) && m_boarders.in(up) && m_boarders.in(down))
    {
      if(!m_boarders.in(right))
        destination = Destination::go_right;
    }
    else if(m_boarders.in(right) && m_boarders.in(up) && m_boarders.in(down))
    {
      if(!m_boarders.in(left))
        destination = Destination::go_left;
    }
  }

  return destination;
}

void Game::check_for_warp(DynamicPiece* p)
{
  if(p->in(&m_left_warp))                 //if in left warp, jump to right warp
  {
    p->jump(m_right_warp.location());
  }
  else if(p->in(&m_right_warp))          // if in right warp, jump to left warp
  {
    p->jump(m_left_warp.location());
  }
}

Coord Game::random_target(Ghost* ghost) 
{
  enum class Direction{up,down,left,right};

  switch(static_cast<Direction>( rand() % 4) ) //choose random direction
  {
    case Direction::up:
    {
      return ghost->location() + Coord{0,-1};
    }
    case Direction::down:
    {
      return ghost->location() + Coord{0,1};
    }
    case Direction::right:
    {
      return ghost->location() + Coord{2,0};
    }
    case Direction::left:
    {
      return ghost->location() + Coord{-2,0};
    }
  };

  return ghost->location();
}

Coord Game::behind_target(Ghost* ghost)
{
  switch(ghost->momentum())   //look at momentum and go in oposite direction
  {
    case Momentum::up:
    {
      return ghost->location() + Coord{0,1};
    }
    case Momentum::down:
    {
      return ghost->location() + Coord{0,-1};
    }
    case Momentum::left:
    {
      return ghost->location() + Coord{2,0};
    }
    case Momentum::right:
    {
      return ghost->location() + Coord{-2,0};
    }
    case Momentum::still:
    {
      return ghost->location();
    }
  }
  return ghost->location();
}

Coord Game::blinky_target()
{
  Coord target = m_blinky.location();

  switch(m_blinky.state())    //look at state and determin target
  {
    case GhostState::chase:
    {
      target = m_pacman.location();   //blinkys chase target is just pacmans location
      break;
    }
    case GhostState::scatter:
    {
      target = Locations::BLINKY_SCATTER;
      break;
    }
    case GhostState::eaten:
    {
      target = Locations::BLINKY_START;
      break;
    }
    case GhostState::frightened:
    {
      target = random_target(&m_blinky);    //if frightened go in random direction
      break;
    }
    case GhostState::turn_around:
    {
      target = behind_target(&m_blinky);   //get coord behind ghost if turning around
      break;
    }
  };
  return target;
}

Coord Game::pinky_target()
{
  Coord target = m_pinky.location();

  switch(m_pinky.state())    //look at state and determin target
  {
    case GhostState::chase:
    {
      target = two_infront_of_pacman();     //pinky goes to a tile two infront of pac
      break;
    }
    case GhostState::scatter:
    {
      target = Locations::PINKY_SCATTER;
      break;
    }
    case GhostState::eaten:
    {
      target = Locations::PINKY_START;
      break;
    }
    case GhostState::frightened:
    {
      target = random_target(&m_pinky);    //if frightened go in random direction
      break;
    }
    case GhostState::turn_around:
    {
      target = behind_target(&m_pinky);   //get coord behind ghost if turning around
      break;
    }
  };
  return target;
}

Coord Game::two_infront_of_pacman()
{
  switch(m_pacman.momentum())
  {
    case Momentum::up:
    {
      return m_pacman.location() + Coord{0,-2};
    }
    case Momentum::down:
    {
      return m_pacman.location() + Coord{0,2};
    }
    case Momentum::left:
    {
      //ghosts and pacman move 2 spaces left or right, so go 4 coords over
      return m_pacman.location() + Coord{-4,0};
    }
    case Momentum::right:
    {
      return m_pacman.location() + Coord{4,0};
    }
    default:
    {
      return m_pacman.location();
    }
  }
}

Coord Game::clyde_target()
{
  Coord target = m_clyde.location();

  switch(m_clyde.state())    //look at state and determin target
  {
    case GhostState::chase:
    {
      //clydes target is pacman, unless they are less than 8 spaces apart
      //then clyde goes to his scatter target
      if( scaled_distance(m_clyde.location(), m_pacman.location()) > 8)
        target = m_pacman.location();
      else
        target = Locations::CLYDE_SCATTER;
      break;
    }
    case GhostState::scatter:
    {
      target = Locations::CLYDE_SCATTER;
      break;
    }
    case GhostState::eaten:
    {
      target = Locations::CLYDE_START;
      break;
    }
    case GhostState::frightened:
    {
      target = random_target(&m_clyde);    //if frightened go in random direction
      break;
    }
    case GhostState::turn_around:
    {
      target = behind_target(&m_clyde);   //get coord behind ghost if turning around
      break;
    }
  };
  return target;
}

Coord Game::inky_target()
{
  Coord target = m_inky.location();

  switch(m_inky.state())    //look at state and determin target
  {
    case GhostState::chase:
    {
      target = two_infront_of_pacman() - m_blinky.location();
      break;
    }
    case GhostState::scatter:
    {
      target = Locations::INKY_SCATTER;
      break;
    }
    case GhostState::eaten:
    {
      target = Locations::INKY_START;
      break;
    }
    case GhostState::frightened:
    {
      target = random_target(&m_inky);    //if frightened go in random direction
      break;
    }
    case GhostState::turn_around:
    {
      target = behind_target(&m_inky);   //get coord behind ghost if turning around
      break;
    }
  };
  return target;
}

void Game::update_pursuit_state()
{
  /*
   * game alternates between chase and scatter mode for the whole game
   */

  switch(m_pursuit_state)         // go to current state and calc next state
  {
    case PursuitState::chase:
      if(m_pursuit_state_timer == GameConfig::CHASE_LENGTH)   //if timer is up
      {
        m_pursuit_state = PursuitState::scatter;  //go to scatter state 
        m_pursuit_state_timer = 0;                //and reset timer
      }
      else
      {
        m_pursuit_state = PursuitState::chase;  //else stay in chase and keep countin up
        m_pursuit_state_timer++;
      }
      break;
    case PursuitState::scatter:
      if(m_pursuit_state_timer == GameConfig::SCATTER_LENGTH)   //if timer is up
      {
        m_pursuit_state = PursuitState::chase;      //go to chase
        m_pursuit_state_timer = 0;                  //reset timer
      }
      else
      {
        m_pursuit_state = PursuitState::scatter;  //else stay in scatter and keep countin up
        m_pursuit_state_timer++;
      }
      break;
  }
}

void Game::update_ghost_state(Ghost* ghost)
{
  /*
   * Ghost has the following states
   *
   * Chase:: pursue pacman until power up activated, or game switches pursuit_states
   * Scatter:: go to scatter chord until power up activated, of game switches pursuit states
   *
   * turn around:: if power up activated, turn around for one turn then go to frightened, unless eaten
   *
   * frightened:: go in random direction, until eaten or power up is turned off
   *
   * eaten:: go back to home coord, then go back to appropriate pursuit state
   *
   */

  switch(ghost->state())
  {
    case GhostState::chase:
    case GhostState::scatter:
    {
      if(m_power_ups.score())
      {
        ghost->set_state(GhostState::turn_around);  //if power up scores, turn around
      }
      else
      {
        ghost->set_state(m_pursuit_state);  //else look at pursuit_state and go to chase or scatter
      }
      break;
    }
    case GhostState::turn_around:
    {
      if(ghost->eaten())
      {
        ghost->set_state(GhostState::eaten);      //if eaten, go to eaten state
      }
      else
      {
        ghost->set_state(GhostState::frightened); //else go to frightened state
      }
      break;
    }
    case GhostState::frightened:
    {
      if(ghost->eaten())
      {
        ghost->set_state(GhostState::eaten);            //if eaten go to eaten
      }
      else if(m_power_ups.state() == PowerUpState::off)
      {
        ghost->set_state(m_pursuit_state);              //if power up turns off go to chase or scatter
      }
      else
      {
        ghost->set_state(GhostState::frightened);      //else stay frightened
      }
      break;
    }
    case GhostState::eaten:
    {
      if(ghost->is_home())
      {
        ghost->set_state(m_pursuit_state);    //if we are home go back to chase/scatter 
      }
      else
      {
        ghost->set_state(GhostState::eaten);    //else stay eaten
      }
      break;
    }
  };
}

void Game::update_ghost_states()
{
  update_ghost_state(&m_blinky);
  update_ghost_state(&m_pinky);
  update_ghost_state(&m_clyde);
  update_ghost_state(&m_inky);
}

void Game::update_power_ups_state()
{
  switch(m_power_ups.state())   //go to state, and calc next state
  {
    case PowerUpState::off:
    {
      if(m_power_ups.score())
      {
        m_power_up_timer = GameConfig::POWER_UP_LENGTH; //if we scored a powerup, start power up timer
        m_power_ups.set_state(PowerUpState::active);   //go to active state
      }
      else
      {
        m_power_ups.set_state(PowerUpState::off);     //else stay turned off
      }
      break;
    }
    case PowerUpState::active:
    {
      if(m_power_ups.score())
      {
        m_power_up_timer = GameConfig::POWER_UP_LENGTH; //if we scored another powerup, reset timer
        m_power_ups.set_state(PowerUpState::active);  //stay activated
      }
      else if(m_power_up_timer <= 0)
      {
        m_power_ups.set_state(PowerUpState::off);   //if timer is up, turn off power up
      }
      else
      {
        m_power_up_timer--;                           //else stay active, and dec timer
        m_power_ups.set_state(PowerUpState::active);
      }
      break;
    }
  }

}

void Game::calc_pacman_score()
{
  if(m_points.score())                        //if points score, inc points
    m_pacman.inc_points(m_points.value());

  if(m_power_ups.score())                     //if power up score, inc points
    m_pacman.inc_points(m_power_ups.value());

  if(m_blinky.eaten())                        //chech for eaten ghosts and inc points
    m_pacman.inc_points(m_blinky.value());

  if(m_pinky.eaten())
    m_pacman.inc_points(m_pinky.value());

  if(m_clyde.eaten())
    m_pacman.inc_points(m_clyde.value());

  if(m_inky.eaten())
    m_pacman.inc_points(m_inky.value());
}

bool Game::check_pacman_eaten()
{
  if(!m_pacman.eaten()) //pacman can only be eaten once
  {
    //can only be eaten by one ghost at a time, so check each ghost
    //individually and return if eaten
    if(m_pacman.check_eaten(&m_blinky))
      return true;
    if(m_pacman.check_eaten(&m_pinky))
      return true;
    if(m_pacman.check_eaten(&m_clyde))
      return true;
    if(m_pacman.check_eaten(&m_inky))
      return true;
  }
  return false;
}

bool Game::check_points_scored()
{
  //can only score points if pacman wasnt eaten
  if(!m_pacman.eaten())
    return m_points.check_score(&m_pacman);
  return false;
}

bool Game::check_power_ups_scored()
{
  //can only score a power up if pacman wasnt eaten
  if(!m_pacman.eaten())
    return m_power_ups.check_score(&m_pacman);
  return false;
}

bool Game::check_ghosts_eaten()
{
  bool ghost_eaten {false};

  if(!m_pacman.eaten())     //cant eat a ghost if pacman is eaten
    {
      if(!m_blinky.eaten())     //ghost cant be eaten twice
        ghost_eaten = m_blinky.check_eaten(&m_pacman);
      if(!m_pinky.eaten())
        ghost_eaten = ghost_eaten || m_pinky.check_eaten(&m_pacman);
      if(!m_inky.eaten())
        ghost_eaten = ghost_eaten || m_inky.check_eaten(&m_pacman);
      if(!m_clyde.eaten())
        ghost_eaten = ghost_eaten || m_clyde.check_eaten(&m_pacman);
    }
  return ghost_eaten;
}

void Game::reset_piece_positions()
{

  blink_pieces({&m_boarders,&m_points,&m_power_ups},2);

  m_pacman.jump_home(Momentum::left);     //send pacman home

  m_blinky.reset();   //reset ghosts
  m_pinky.reset();
  m_inky.reset();
  m_clyde.reset();

  m_game_win.print();

  pause(Pause::LONG);

  blink_pieces({&m_pacman,&m_blinky,&m_pinky,&m_inky,&m_clyde}, 2);

  pause(Pause::LONG);
}

void Game::reset_level()
{
  blink_pieces({&m_pacman,&m_blinky,&m_inky,&m_clyde,&m_pinky, &m_boarders}, 2);

  m_pacman.jump_home(Momentum::left);     //send pacman and ghosts home
  m_blinky.jump_home(Momentum::still);
  m_pinky.jump_home(Momentum::still);
  m_clyde.jump_home(Momentum::still);
  m_inky.jump_home(Momentum::still);

  m_points.reset();                       //reset points and power ups
  m_power_ups.reset();

  m_game_win.print();
  pause(Pause::LONG);

  blink_pieces({&m_pacman,&m_blinky,&m_inky,&m_clyde,&m_pinky,&m_boarders, &m_points, &m_power_ups}, 2);

  m_game_level++;                         //inc level number

  m_game_win.print();
  print_stats();
}

void Game::reset_game()
{
  m_pacman.reset();   //reset pacman

  m_blinky.reset();   //reset ghosts
  m_pinky.reset();
  m_clyde.reset();
  m_inky.reset();

  m_points.reset();   //reset points and power ups
  m_power_ups.reset();

  m_game_level = 1;   //go back to level 1

  m_power_up_timer = 0;      //reset timers
  m_pursuit_state_timer = 0;
  m_power_up_blink_timer = 0;

  m_pursuit_state = PursuitState::scatter;  //go to scatter state

  m_game_win.print();
  print_stats();
}

void Game::reset_piece_flags()
{
  m_pacman.reset_eaten_flag();

  m_blinky.reset_eaten_flag();
  m_pinky.reset_eaten_flag();
  m_inky.reset_eaten_flag();
  m_clyde.reset_eaten_flag();

  m_points.reset_score_flag();
  m_power_ups.reset_score_flag();
}

void Game::blink_pieces(const vector<Piece*>& pieces, int n_times)
{
  for(int i = 0; i < n_times; i++)    //blink each piece n_times
  {
    m_game_win.print();
    pause(Pause::MEDIUM);

    for(auto& p : pieces)       //go to blink symbol
      p->blink();

    m_game_win.print();
    pause(Pause::MEDIUM);

    for(auto& p : pieces)      //go back to normal symbol
      p->blink();

    m_game_win.print();
    pause(Pause::MEDIUM);
  }
}

void Game::blink_power_ups()
{
  //blink power ups every POWER_UP_BLINK_LENGTH turns
  if(m_power_up_blink_timer <= 0)
  {
    m_power_ups.blink();
    m_power_up_blink_timer = ::GameConfig::POWER_UP_BLINK_LENGTH;
  }
  else
  {
    m_power_up_blink_timer--;
  }
}

bool Game::play_again()
{
  blink_pieces({&m_boarders,&m_points,&m_power_ups},2);

  //print game over prompt
  m_message_win.update_text(GameText::GAME_OVER_MSG);
  m_message_win.print();

  //loop until valid input
  char input {'\0'};
  while( (input = m_scrn.get_ch(InputMode::block)) != Inputs::PLAY && input != Inputs::QUIT)
    continue;

  //return true to play again
  if(input == Inputs::PLAY)
    return true;

  //return false to quit game
  return false;
}

void Game::print_stats()
{
  //turn current stats into stirngs
  string level = "Level: " + to_string(m_game_level) + " \n";           //game level
  string score = "Score: " + to_string(m_pacman.points()) + " \n";      //points
  string lives = "Lives: " + to_string(m_pacman.lives()) + " \n";       //pacman lives

  string stats = level + score + lives;

  //print to stats window
  m_stat_win.update_text(stats);
  m_stat_win.print();
}

void Game::pause(int n_milliseconds)
{
  std::this_thread::sleep_for(std::chrono::milliseconds(n_milliseconds));
}

int Game::scaled_distance(const Coord l, const Coord r)
{
  /*
   * this finction accounts for the fact the pacman and ghosts 
   * move left and right by 2 spaces
   * and calculated the apporopriet distance between them
   *
   */

  //x distance should be halved
  int x_diff = (l.x - r.x)/2;
  //no need to adjust y distance
  int y_diff = l.y - r.y;
  //return the distance between peices
  return x_diff * x_diff + y_diff * y_diff;
}

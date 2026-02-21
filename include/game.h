#ifndef GAME_H
#define GAME_H

#include "screen.h"
#include "pieces.h"

#include <vector>

/*
 * The game class runs the pacman game and handles all state and movement logic.
 */

enum class PursuitState {chase, scatter};     //game alternates between chase and scatter modes

class Game
{
  public:
    Game();
    void run();

  private:
    //Game windows
    Screen m_scrn;              //main ncurses screen
    GameWindow m_game_win;      //game window, where game is played
    TextWindow m_stat_win;      //stats window, where stats are printed
    TextWindow m_message_win;   //message window, where start and game over messages are printed

    //Game pieces
    PacMan m_pacman;            //pacman

    Blinky m_blinky;            //ghosts
    Pinky m_pinky;
    Clyde m_clyde;
    Inky m_inky;

    Borders m_borders;           //borders
    InvWalls m_inv_walls;

    Points m_points;            //scoring pieces
    PowerUps m_power_ups;

    LeftWarp m_left_warp;       //warps
    RightWarp m_right_warp;

    //timers
    int m_power_up_timer {0};         //used to set power up length
    int m_pursuit_state_timer {0};    //used to set chase/scatter lengths
    int m_power_up_blink_timer {0};   //used to blink powerups every n_turns

    //pursuit state
    PursuitState m_pursuit_state {PursuitState::scatter};

    //current game level
    int m_game_level {GameConfig::STARTING_LEVEL};

    /*************** game methods **************/

    //main game loop
    void game_loop();

    //pacman move methods
    void move_pacman(int input);
    void pacman_keep_moving();

    //ghost move methods
    void move_ghosts();
    void move_ghost(Ghost* ghost, Coord target);
    enum class Destination {go_up, go_left, go_right, go_down, stay_still};
    Destination calc_ghost_destination(Ghost* ghost, Coord target);

    //check for a warp
    void check_for_warp(DynamicPiece* p);

    //calc ghost target methods
    Coord random_target(Ghost* ghost);
    Coord behind_target(Ghost* ghost);
    Coord blinky_target();
    Coord pinky_target();
    Coord two_infront_of_pacman();
    Coord clyde_target();
    Coord inky_target();

    //update game states
    void update_pursuit_state();
    void update_ghost_state(Ghost* ghost);
    void update_ghost_states();
    void update_power_ups_state();

    //calc pacmans score
    void calc_pacman_score();

    //check what peices were eaten
    bool check_pacman_eaten();
    bool check_points_scored();
    bool check_power_ups_scored();
    bool check_ghosts_eaten();

    //reset methods
    void reset_piece_positions();
    void reset_level();
    void reset_game();
    void reset_piece_flags();

    //blink methods
    void blink_pieces(const std::vector<Piece*>& pieces, int n_times);
    void blink_power_ups();

    //play again prompt
    bool play_again();

    //print game stats
    void print_stats();

    //pause game execution for n milliseconds
    void pause(int n_milliseconds);

    //calc scaled linear distance between two coords
    int scaled_distance(const Coord l, const Coord r);
};

#endif

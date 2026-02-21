#ifndef SCREEN_H
#define SCREEN_H

#include "pieces.h"
#include "coord.h"

#include <vector>
#include <string>
#include <ncurses.h>

/*********Screen *************/
// The screen class is a wrapper around ncurses that we use to:
//  - initialize the ncurses stdscrn
//  - get user input (blocking and non_blocking modes)
/****************************/
enum class InputMode {non_block, block};

class Screen{
  public:
    Screen();
    ~Screen();

    int get_ch(InputMode input_mode = InputMode::block);
};

/******************** Window ***************************************/
//The window class is used to:
//  -create and initialize an ncurses subwindow on the stdscrn
//  -provide a virtual print() interface for its children
//
//This class is the abstract base class for all window sub_types.
/******************************************************************/
class Window{
  public:
    ~Window();

    virtual void print() = 0;

  protected:
    Window(int height, int length, Coord stdscr_location);

    int   m_height;
    int   m_length;
    Coord m_stdscr_location;      //window's location on the stdscrn

    WINDOW* m_window  {nullptr};  //the windows ncurses handle
};

/************  GameWindow *************/

/*
 *  GameWindow is used to print Pieces onto the screen
 *
 *  GW has three layers pieces can be printed onto.
 *
 *  Pieces are added to the window and placed onto their layer using the add method
 *
 */

enum class WindowLayer {background, midground, foreground};

class GameWindow : public Window
{
  public:
    GameWindow(int height, int length, Coord stdscr_location);

    void print() override;      //print pieces to the screen

    void add(Piece* piece, WindowLayer layer);  //add piece to a layer on screen

  private:
    std::vector<Piece*> m_background;      //Pieces are stored in their layer vector
    std::vector<Piece*> m_midground;
    std::vector<Piece*> m_foreground;
};

/****** TextWindow ***********/

/*
 * textwindow is used to print strings of text to the screen
 */

class TextWindow : public Window{
  public:
    TextWindow(int height, int length, Coord stdscr_location);

    void print() override;          //print text

    void update_text(std::string text);  //update what text should be printed to screen

  private:
    std::string m_text;    //curent text being printed


};

#endif

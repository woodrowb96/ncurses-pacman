#ifndef SCREEN_H
#define SCREEN_H

#include "pieces.h"
#include "coord.h"

#include <vector>
#include <string>
#include <ncurses.h>

/************************************ Screen ************************************/
// The screen class is a wrapper around ncurses that we use to:
//  - initialize the ncurses stdscrn
//  - get user input (blocking and non_blocking modes)
/********************************************************************************/

enum class InputMode {non_block, block};

class Screen
{
  public:
    Screen();
    ~Screen();

    int get_ch(InputMode input_mode = InputMode::block);
};

/************************************ Window ************************************/
// The window class is used to:
//  -create and initialize an ncurses subwindow on the stdscrn
//  -provide a virtual print() interface for its children
//
// This class is the abstract base class for all window sub_types.
/********************************************************************************/

class Window
{
  public:
    ~Window();

    virtual void print() = 0;

  protected:
    Window(int height, int length, Coord stdscr_location);

    int   m_height;
    int   m_length;
    Coord m_stdscr_location;      //this window's location on the main stdscrn

    WINDOW* m_window  {nullptr};  //the windows ncurses handle
};

/********************************** GameWindow **********************************/
// The GameWindow is the subwindow we use to print the actual gameplay of our game.
//
// This class can:
//   -add pieces to background, midground or foreground layers
//   -print all three layers to the screen (background in the back, foreground on top)
/********************************************************************************/

enum class WindowLayer {background, midground, foreground};

class GameWindow : public Window
{
  public:
    GameWindow(int height, int length, Coord stdscr_location);

    void print() override;
    void add(Piece* piece, WindowLayer layer);

  private:
    std::vector<Piece*> m_background;
    std::vector<Piece*> m_midground;
    std::vector<Piece*> m_foreground;
};

/********************************** TextWindow **********************************/
// The TextWindow is a subwindow for printing text onto the screen
//
// This class can:
//   -update text we are printing
//   -print the text onto the screen
/********************************************************************************/

class TextWindow : public Window
{
  public:
    TextWindow(int height, int length, Coord stdscr_location);

    void print() override;
    void update_text(const std::string& text);

  private:
    std::string m_text;
};

#endif

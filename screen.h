#ifndef SCREEN_H
#define SCREEN_H

#include "pieces.h"
#include "coord.h"

#include <vector>
#include <string>
#include <ncurses.h>

using std::vector;
using std::string;

/*********Screen *************/

/*
 * the screen class is used to initialize the ncurses screen
 *
 * the screen class also has a destructor which ends the ncurses session
 *
 */

enum class InputMode {non_block, block};    //can switch between blocking and non blocking inpt

class Screen{
  public:
    Screen();       //initialize screen
    ~Screen();

    char get_ch(InputMode input_mode = InputMode::block); //wrapper around ncurses getch()
                                                          //function that lets us set
                                                          //blocking mode
};

/************** Window **************/

/*
 * Windows are ncurses subwindows which are used to print things onto the screen
 *
 * This base class just creats a window with given dimensions, and location on the 
 * ncurses stdscrn
 *
 */
class Window{
  public:
    ~Window();    //delete the window

    virtual void print() = 0;       //print window to screen

  protected:
    Window(int height, int length, Coord screen_location);

    int   m_height;
    int   m_length;
    Coord m_screen_location;      //location on ncurses stdscrn

    WINDOW* m_window  {nullptr};      //pointer to ncurses Window
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
    GameWindow(int height, int length, Coord screen_location);

    void print() override;      //print pieces to the screen

    void add(Piece* piece, WindowLayer layer);  //add piece to a layer on screen

  private:
    vector<Piece*> m_background;      //Pieces are stored in their layer vector
    vector<Piece*> m_midground;
    vector<Piece*> m_foreground;
};

/****** TextWindow ***********/

/*
 * textwindow is used to print strings of text to the screen
 */

class TextWindow : public Window{
  public:
    TextWindow(int height, int length, Coord screen_location);

    void print() override;          //print text

    void update_text(string text);  //update what text should be printed to screen

  private:
    string m_text;    //curent text being printed


};

#endif

#include "screen.h"
#include "pieces.h"
#include "coord.h"
#include "config.h"

#include <ncurses.h>
#include <vector>
#include <string>

using std::vector;
using std::string;

/************************************ Screen ************************************/

Screen::Screen()
{
  initscr();              //start ncurses stdscrn
  noecho();               //dont print keypresses to screen
  keypad(stdscr, TRUE);   //let ncurses read function keys
  curs_set(0);            //dont print cursor to screen
  refresh();              //refresh screen to start stdscrn
}

Screen::~Screen()
{
  endwin();    //end stdscrn
}

int Screen::get_ch(InputMode input_mode)
{
  int input {Inputs::NO_INPUT};

  nodelay(stdscr, input_mode == InputMode::non_block);  //set the proper blocking mode
  input = getch();                                      //get our input
  nodelay(stdscr,FALSE);                                //go back to blocking as default

  return input;
}

/************************************ Window ************************************/

Window::Window(int height, int length, Coord stdscr_location)
  :
  m_height          {height},
  m_length          {length},
  m_stdscr_location {stdscr_location}
{
  //create the window and place it on the stdscrn
  m_window = newwin(m_height, m_length, m_stdscr_location.x, m_stdscr_location.y);
}

Window::~Window()
{
  if(m_window) {
    delwin(m_window);
  }
}

/********************************** GameWindow **********************************/

GameWindow::GameWindow(int height, int length, Coord stdscr_location)
  : Window(height, length, stdscr_location) {}

void GameWindow::print()
{
  //clear the previous print
  werase(m_window);

  //draw each piece onto its layer
  for(Piece* piece : m_background) {
    piece->draw(m_window);
  }
  for(Piece* piece : m_midground) {
    piece->draw(m_window);
  }
  for(Piece* piece : m_foreground) {
    piece->draw(m_window);
  }

  //print the window onto the stdscrn
  wrefresh(m_window);
}

void GameWindow::add(Piece* piece, WindowLayer layer)
{
  switch(layer) {
    case WindowLayer::background: {
      m_background.push_back(piece);
      break;
    }
    case WindowLayer::midground: {
      m_midground.push_back(piece);
      break;
    }
    case WindowLayer::foreground: {
      m_foreground.push_back(piece);
    }
  }
}

/********************************** TextWindow **********************************/

TextWindow::TextWindow(int height, int length, Coord stdscr_location)
  : Window(height, length, stdscr_location) {}

void TextWindow::print()
{
  werase(m_window);                     //clear the previous prints
  waddstr(m_window, m_text.c_str());    //draw text onto window
  wrefresh(m_window);                   //print the window onto stdscrn
}

void TextWindow::update_text(const string& text)
{
  m_text = text;
}

#include "screen.h"

using std::vector;
using std::string;


/********* Screen *************/

Screen::Screen()
{
  initscr();            //start stdscrn

  noecho();             //dont print keypresses to screen

  keypad(stdscr, TRUE);   //let ncurses read funcion keys

  curs_set(0);        //dont print cursor to screen

  refresh();          //refresh screen to start stdscrn
}

Screen::~Screen()
{
  endwin();         //end stdscrn
}

char Screen::get_ch(InputMode input_mode)
{
  char input {Inputs::NO_INPUT};

  switch(input_mode)
  {
    case InputMode::non_block:
    {
      nodelay(stdscr,TRUE);   //getch() doesnt block
      input = getch();
      break;
    }
    case InputMode::block:
    {
      nodelay(stdscr,FALSE);   //getch() blocks
      input = getch();
      break;
    }
  }

  nodelay(stdscr,FALSE);  //go back to blocking by default
  return input;
}

/********* Window **********************/

Window::Window(int height, int length, Coord screen_location)
  :
  m_height          {height},
  m_length          {length},
  m_screen_location {screen_location}
{
  //initialize window
  m_window = newwin(m_height, m_length, m_screen_location.x, m_screen_location.y);
}

Window::~Window()
{
  if(m_window)
    delwin(m_window);  //delete the window
}

/********* GameWindow **********************/

GameWindow::GameWindow(int height, int length, Coord screen_location)
  : Window(height, length, screen_location) {}

void GameWindow::print()
{
  werase(m_window);                  //erase previous print

  for(Piece* piece : m_background)   //loop through shape on each layer and print
    piece->print(m_window);

  for(Piece* piece : m_midground)
    piece->print(m_window);

  for(Piece* piece : m_foreground)
    piece->print(m_window);

  wrefresh(m_window);                //refresh window to print to stdscrn
}

void GameWindow::add(Piece* piece, WindowLayer layer)
{
  switch(layer)                       //go to layer
  {
    case WindowLayer::background:     //and add piece to layers piece vector
      m_background.push_back(piece);
      break;
    case WindowLayer::midground:
      m_midground.push_back(piece);
      break;
    case WindowLayer::foreground: 
      m_foreground.push_back(piece);
  }
}

/********* TextWindow **********************/

TextWindow::TextWindow(int height, int length, Coord screen_location)
  : Window(height, length, screen_location) {}

void TextWindow::print()
{
  werase(m_window);
  waddstr(m_window, m_text.c_str());    //add string to window
  wrefresh(m_window);
}

void TextWindow::update_text(string text)
{
  m_text = text;
}

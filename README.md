# nCurses Pacman

A terminal-based PacMan game written in C++ using the ncurses TUI library.

https://github.com/user-attachments/assets/827631d1-a493-4d1c-ab2f-df1f35e3bce4

## Overview

An Object Oriented Programming implementation of Pacman with full ghost AI, power-ups, warp tunnels, and level progression. 

## Architecture

This project has three main class categories:
- A game pieces class hierarchy for all drawable and interactive objects.
- Screen and window classes used to print to the screen and get user input.
- A Game class that runs the game loop, ghost AI, and state management.

## Build & Run

Requires `g++` and `ncurses`.

```
make
./pacman
```

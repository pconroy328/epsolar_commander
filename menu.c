/*
 */

#include <stdlib.h>
#include <ctype.h>
#include <ncurses.h>
#include <locale.h>
#include <string.h>
#include <unistd.h>
#include <libmqttrv.h>
#include <log4c.h>
#include <libepsolar.h>

#include "epsolar_commander.h"


WINDOW      *menuWin;

// ---------------------------------------------------------------------------------
void    showMenu ()
{    
    //
    // WINDOW *newwin(int nlines, int ncols, int begin_y, int begin_x);
    //
    //  Calling newwin creates and returns a pointer to a new window with the given 
    //  number of lines and columns. The upper left-hand corner of the window is 
    //  at line begin_y, column begin_x. If either nlines or ncols is zero, 
    //  they default to LINES - begin_y and COLS - begin_x. 
    //  A new full-screen window is created by calling newwin(0,0,0,0). 
    
    //
    // WINDOW *newwin(int nlines, int ncols, int begin_y, int begin_x);
    //
    // Calling newwin creates and returns a pointer to a new window with the given 
    //  number of lines and columns. The upper left-hand corner of the window 
    //  is at line begin_y, column begin_x. If either nlines or ncols is zero, 
    //  they default to LINES - begin_y and COLS - begin_x. 
    //  A new full-screen window is created by calling newwin(0,0,0,0).
    int numCols = COLS;
    int numLINES = 3;
    int beginY = LINES - 3;
    int beginX = 0;
    
    menuWin = newwin( numLINES, numCols, beginY, beginX );
    box( menuWin, ACS_VLINE, ACS_HLINE );  
    wrefresh( menuWin );
    
    beginY = 0;
    beginX = 0;
    wmove( menuWin, beginY, beginX );
    wprintw( menuWin, "Test 0,0" );
    wrefresh( menuWin );
    wmove( menuWin, beginY + 1, beginX + 1 );
    wprintw( menuWin, "Test 1,1" );
    wrefresh( menuWin );
    
    wmove( menuWin, beginY, beginX );
    wattron( menuWin, A_REVERSE );
    wprintw( menuWin, "(H)ome    (B)attery    (L)oad    (D)evice   (S)ettings   (Q)uit");
    wattroff( menuWin, A_REVERSE );
    wrefresh( menuWin );
}

// -----------------------------------------------------------------------------
char    getMenuSelection ()
{
    return toupper( wgetch( menuWin ) );
}

void    menuDisplayMessage (const char *msg)
{
    wmove( menuWin, 0,0 );
    wattron( menuWin, A_REVERSE );
    wprintw( menuWin, msg );
    wattroff( menuWin, A_REVERSE );
    wrefresh( menuWin );
}

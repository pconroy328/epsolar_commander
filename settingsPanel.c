/*
 */

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
#include <pthread.h>
#include "epsolar_commander.h"

extern  int     MaxRows;
extern  int     MaxCols;
static  WINDOW  *panel;


// -----------------------------------------------------------------------------
static
void    paintSettingsData ()
{

}
// -----------------------------------------------------------------------------
void    showSettingsPanel()
{
    setActivePanel( SETTINGS_PANEL );

    int     startY = 0;
    int     startX = 0;
    int     nRows = MaxRows - 1;
    int     nCols = MaxCols;

    panel = grouping( &panel, startY, startX, nRows, nCols, "Solar Charge Controller" );
    
}

// -----------------------------------------------------------------------------
void    paintSettingsPanelData()
{
    paintSettingsData();
}

// -----------------------------------------------------------------------------
void    clearSettingsPanel()
{
    werase( panel );
    delwin( panel );
    werase( stdscr );
    refresh();
}

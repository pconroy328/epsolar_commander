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
void    paintLoadData ()
{
    //
    int beginRow = 2;
    int beginCol = 3;

    HaddTextField( panel, beginRow++, beginCol, "Manual            ", (loadControlMode == 0x00 ? "Yes" : "No ") );
    HaddTextField( panel, beginRow++, beginCol, "Night On/Day Off  ", (loadControlMode == 0x01 ? "Yes" : "No ") );
    HaddTextField( panel, beginRow++, beginCol, "Night On and Timer", (loadControlMode == 0x02 ? "Yes" : "No ") );
    HaddTextField( panel, beginRow++, beginCol, "Timer On and Off  ", (loadControlMode == 0x03 ? "Yes" : "No ") );
    
    beginRow ++;
    HaddTextField( panel, beginRow++, beginCol, "Timer One - On", timerOneOn );
    HaddTextField( panel, beginRow++, beginCol, "Timer Two - On", timerTwoOn ); 
            
    beginRow++;
    HfloatAddTextField( panel, beginRow++, beginCol, "Is Night Threshold <= V", nighttimeThresholdVoltage, 1, 4 );
    HfloatAddTextField( panel, beginRow++, beginCol, "Is Day Threshold >= V  ", daytimeThresholdVoltage, 1, 4 );

    beginRow++;
    HaddTextField( panel, beginRow++, beginCol, "Work Timer One", workOneTime );
    HaddTextField( panel, beginRow++, beginCol, "Work Timer Two", workTwoTime );
    
    beginRow++;
    HaddTextField( panel, beginRow++, beginCol, "Length of Night", workTwoTime );
            
    wrefresh( panel );
}

// -----------------------------------------------------------------------------
void    showLoadPanel()
{
    setActivePanel( LOAD_PANEL );

    int     startY = 0;
    int     startX = 0;
    int     nRows = MaxRows;
    int     nCols = MaxCols;

    panel = grouping( &panel, startY, startX, nRows, nCols, "Load Control" );
    paintLoadData();
    
}

// -----------------------------------------------------------------------------
void    paintLoadPanelData()
{
    paintLoadData();
}

// -----------------------------------------------------------------------------
void    clearLoadPanel()
{
    werase( panel );
    delwin( panel );
    werase( stdscr );
    refresh();
}

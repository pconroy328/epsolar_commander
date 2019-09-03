/*
 * To change this license header, choose License Headers in Project Properties.
 * To change this template file, choose Tools | Templates
 * and open the template in the editor.
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
static  WINDOW  *batteryPanel;
static  WINDOW  *batteryPanel2;

// -----------------------------------------------------------------------------
void    showBatteryPanel()
{
    int     battY = 0;
    int     battX = 0;
    int     battRows = 5;
    int     battCols = MaxCols;
    batteryPanel = grouping( &batteryPanel, battY, battX, battRows, battCols, "Battery Ratings" );
    
    
    int     battY2 = battRows;
    int     battX2 = 0;
    int     battRows2 = 18;
    int     battCols2 = MaxCols;
    batteryPanel2 = grouping( &batteryPanel2, battY2, battX2, battRows2, battCols2, "Battery Settings" );
    refresh();
}

// -----------------------------------------------------------------------------
void    cleanupBatteryPanel()
{
    werase( batteryPanel );
    werase( batteryPanel2 );
    delwin( batteryPanel );
    delwin( batteryPanel2 );
    werase( stdscr );
    refresh();
}

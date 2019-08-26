/*
 */

/* 
 * File:   ncurses-menu.h
 * Author: pconroy
 *
 * Created on July 29, 2019, 10:25 AM
 */

#ifndef NCURSES_MENU_H
#define NCURSES_MENU_H

#ifdef __cplusplus
extern "C" {
#endif


/************************************************************************/
/*                    Copyright � 2013 Max W�llstedt                    */
/*                                                                      */
/* This file is part of Ncurses-menu.                                   */
/*                                                                      */
/* Ncurses-menu is free software: you can redistribute it and/or modify */
/* it under the terms of the GNU General Public License as published by */
/* the Free Software Foundation, either version 3 of the License, or    */
/* (at your option) any later version.                                  */
/*                                                                      */
/* Ncurses-menu is distributed in the hope that it will be useful,      */
/* but WITHOUT ANY WARRANTY; without even the implied warranty of       */
/* MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the        */
/* GNU General Public License for more details.                         */
/*                                                                      */
/* You should have received a copy of the GNU General Public License    */
/* along with Ncurses-menu. If not, see <http://www.gnu.org/licenses/>. */
/*                                                                      */
/************************************************************************/

extern int print_menu (int sty, int x, int alts, int width,
                       char title[], char entries[][100], int start);


#ifdef __cplusplus
}
#endif

#endif /* NCURSES_MENU_H */


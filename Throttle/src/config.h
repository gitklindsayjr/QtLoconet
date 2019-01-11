/****************************************************************************
 *  Copyright (C) 2018 Ken Lindsay
 *
 *  This class is free software; you can redistribute it and/or
 *  modify it under the terms of the GNU Lesser General Public
 *  License as published by the Free Software Foundation; either
 *  version 2.1 of the License, or (at your option) any later version.
 *
 *  This library is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 *  Lesser General Public License for more details.
 *
 *  You should have received a copy of the GNU Lesser General Public
 *  License along with this library; if not, write to the Free Software
 *  Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
 *******************************************************************************/
/*******************************************************************************
 * This file needs to be changed to reflect locomotives being used
 * Todo: 1. Use comma seperated spread sheet in the future
 *       2. Make fix hard coded STEPS_PER_NOTCH in mainwindow.cpp to be part of loaded
 *          configuration file.
 ************************************************************************************/
#ifndef CONFIG_H
#define CONFIG_H

#define EMD_F7_TITLE              "Throttle EMD-F7"
#define LOCO_INDEX_EMD_F7         0
#define LOCO_INDEX_GE             1
#define EMD_F7_ADDRESS            2    // Atherns EMD F7 address
#define EMD_F7_SPEED_SCALE        .80  // Steps to mph scale
#define GE_ADDRESS                3    // GE Switcher address
#define GE_SPEED_SCALE            .78

#define GE_TITLE                  "Throttle GE-44 Ton"
#define EMD_F7_STEPS_PER_NOTCH    6 // Number of throttle steps per notch
#define GE_STEPS_PER_NOTCH        0 // Todo: Fix currently defaulting to EMD
#define INDEX_START               9
#define MAX_FUNCTIONS             24
#define MAX_INDEXS                15 // (MAX_FUNCTIONS - INDEX_START)
#define F3_INDEX                  3

#include <QVector>

typedef struct
{
    int     function;
    int     index[MAX_INDEXS];
    QString locoName;
    QString text[MAX_FUNCTIONS];
}functions_t;

QVector<functions_t> initializeFunctions(void);
void saveFunctions(QVector<functions_t> functions, std::string fname);
QVector<functions_t> loadFunctions(std::string fname);

#endif // CONFIG_H


/****************************************************************************
 *  Copyright (C) 2018 Ken Lindsay
 *
 *  This library is free software; you can redistribute it and/or
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
 * This file contains the functions for configuring your throttle for different
 * locomotives.  The string and integer arrays are used for intial setup to generate
 * respective configuration files. Refer to the mainwindow.cpp file for setting the #ifdef
 * switches for initilization settings.
 *****************************************************************************/
#include <fstream>
#include "config.h"

// Functions are for SoundTrax Tsunami2, Diesel Atherns F7
#define F7_DEFAULT    23
#define F7_ATHERNS    "AthernsF7"
QString f7Text[] = { "Headlight", "Bell", "Horn", "Short Horn", "Dynamic Brakes", "Lighting 1",
                     "Lighting 2", "Cab Chatter", "Mute","Half volume", "Sander valve", "Brake",
                     "Brake select", "Couple/Uncouple", "Half speed/override", "Hand brake",
                     "Hep MODE", "Fuel loading", "General service", "Steam generator", "All aboard", "-", "-", "-" };

// This vector translates combo box index's to functions.  Functions 19, 21, and 22 are not used
int f7Index[MAX_INDEXS] = { 9, 10, 11, 12, 13, 14, 15, 16, 17, 18, 20, 23, 0,  };

// Functions are for ESU LokSound V4.0, Diesel
#define GE_DEFAULT    9
#define GE_44TON_ESU_LOKSOUND "Ge 44 ton ESU LokSound"
QString geText[] = { "Headlight", "Bell", "Airhorn", "Coupler", "Dynamic Brakes", "Aux3",
                     "Aux1 + Aux2", "Switching Mode", "Mute", "Drive hold", "Independent brake",
                     "Radiator fan", "Dimmer", "AUX4", "Fast spitter", "Spitters", "Brake",
                     "Sander", "Air let off", "Compressor", "Slow spitter", "Air dryer", "-", "-" };

// This vector translates combo box index's to functions.  Functions 14, and 23 are not used
int geIndex[] = { 9, 10, 11, 12, 13, 15, 16, 17, 18, 19, 20, 21, 22, 0, };

using namespace std;
class MainWindow;

QVector<functions_t> initializeFunctions(void)
{   // Initializes vectors for pre-defined locomotives string and integer arrays above.
    functions_t funcs;
    QVector<functions_t> functions;
    funcs.locoName = F7_ATHERNS;
    funcs.function = F7_DEFAULT;
    for(int i = 0; i<MAX_INDEXS; i++)
        funcs.index[i] = f7Index[i];
    for(int i = 0; i<MAX_FUNCTIONS; i++)
        funcs.text[i] = f7Text[i];
    functions << funcs;

    funcs.locoName = GE_44TON_ESU_LOKSOUND;
    funcs.function = GE_DEFAULT;
    for(int i = 0; i<MAX_INDEXS; i++)
        funcs.index[i] = geIndex[i];
    for(int i = 0; i<MAX_FUNCTIONS; i++)
        funcs.text[i] = geText[i];
    functions << funcs;
    return functions;
}
void saveFunctions(QVector<functions_t> functions, string fname)
{   // Saves initialized vectors for pre-defined locomotives string and integer arrays above.
    ofstream f;
    functions_t funcs;
    f.open(fname);
    f << functions.size() << " # Number of locos" << endl;
    std::string text;
    for(int i=0; i<functions.size(); i++)
    {
        funcs = functions[i];
        f << funcs.locoName.toStdString() << endl;
        f << funcs.function << " # default function" << endl;
        f << "# index vector" << endl;
        for(int i=0; i<MAX_INDEXS; i++)
            f << funcs.index[i] << endl;
        for(int i=0; i<MAX_FUNCTIONS; i++)
        {
            text = funcs.text[i].toStdString();
            f << text << endl;
        }
        f << endl;
    }
    f.close();
}
QVector<functions_t> loadFunctions(string fname)
{   // Loads vect initialized vectors for pre-defined locomotives string and integer arrays above.
    ifstream f;
    functions_t funcs;
    QVector<functions_t> functions;
    std::string   text;

    f.open(fname);
    int locos;
    f >> locos; getline(f, text);
    for(int j=0; j<locos; j++)
    {
        getline(f, text); // number of locos, purge comment
        funcs.locoName = QString::fromStdString(text);
        f >> funcs.function;
        getline(f, text); // default function, purge comment
        getline(f, text); // purge comment
        for(int index, i=0; i<MAX_INDEXS; i++)
        {
            f >> index;
            funcs.index[i] = index;
        }
        getline(f, text);
        for(int i=0; i<MAX_FUNCTIONS; i++)
        {
            getline(f, text); // get text strings
            funcs.text[i] = QString::fromStdString(text);
        }
        functions << funcs; // Save results in class variable "functions"
        getline(f, text);
    }
    f.close();
    return functions;
}


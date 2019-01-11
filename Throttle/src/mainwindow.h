#ifndef MAINWINDOW_H
#define MAINWINDOW_H
/****************************************************************************
 *  Copyright (C) 2018 Ken Lindsay
 *
 *  This library is free software; you can redistribute it and/or
 *  modify it under the terms of the GNU Lesser General Public
 *  License as published by the Free Software Foundation; either
 *  version 2.1 of the License, or (at your option) any later version.
 *
 *  This class is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 *  Lesser General Public License for more details.
 *
 *  You should have received a copy of the GNU Lesser General Public
 *  License along with this library; if not, write to the Free Software
 *  Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
 *
 *****************************************************************************
 * MainWindow:
 * Interface between the Throttle UI/Form mainwindow.ui invoked by helper function, ui->setup()
 * part of class Ui_MainWindow.  This class is generated automatically by the Form editor.
 * Communication between the UI is done by signals and slots.  UI signals are connected to MainWindow slots.
 * These slots are then connected through signals to the Throttle class to Throttle slots as xxxCommand.
 * Responsed from the Throttle are relayed back to the UI through Throttle signals connected to MainWindow slots.
 * The slots are named xxxResponse and use the Ui_MainWindow class helper functions to register the throttle responses.
 * Switch requests are used by the Dispatcher if this throttle is a forked process of Dispatcher to initiate switch
 * action.  The switch requests are send via the stdin/stdout connections between the 2 processes.
 */

#include <QMainWindow>
#include <QtWidgets>
#include "throttle.h"
#include "config.h"

namespace Ui { class MainWindow; }

class MainWindow : public QMainWindow
{
    Q_OBJECT
public:
    explicit MainWindow(QWidget *parent = 0);
    ~MainWindow();
   Throttle *locoThrottle;
private:
    int    mySpeed;
    double mySpeedScale;
    int    myAddress;
    int    myNotch;
    bool   simulate;
    bool   myThrottle;
    int    myLoco;
    int    myFunction;
    int    myNumberOfLocos;
    bool   comboInit = false;
    QVector<functions_t> functions;
    bool dispatchChildProcess;
public slots:
    void muteResponse(bool state);
    void lightResponse(bool state);
    void bellResponse(bool state);
    void directionResponse(bool direction);
    void speedResponse(int speed);
    void addressResponse(int address);
    void slotResponse(int slot, bool ours);
    void powerResponse(bool state);
    void idResponse(int id, bool ours);
    void switchRequest(int sw, bool state);
    void switchState(int sw, bool state);
    void stateResponse(QString str);
    void statusResponse(QString str);
    void receiveMessage(QString msg);
signals: // Throttle signals
    void switchStateCommand(int sw, bool state);
    void powerCommand(bool state);
    void functionsCommand(int function);
    void bellCommand(bool state);
    void f3Command(bool state);
    void hornCommand(bool state);
    void dynamicBrakeCommand(bool state);
    void muteCommand(bool state);
    void lightCommand(bool state);
    void directionCommand(bool dir);
    void speedCommand(int speed);
    void stopCommand(void);
    void stealCommand(int address);
    void requestCommand(int address);
    void releaseCommand(bool force);
    void dispatchCommand(void);
    void recallCommand(void);
    void idChange(int id);
    void initCommand(int address);
private slots: // UI slots
    void on_comboBox_functions_currentIndexChanged(int index);
    void on_checkBox_bell_clicked(void);
    void on_checkBox_light_clicked(void);
    void on_checkBox_mute_clicked(void);
    void on_radioButton_pwrOn_clicked(void);
    void on_radioButton_pwrOff_clicked(void);
    void on_radioButton_forward_clicked(void);
    void on_radioButton_reverse_clicked(void);
    void on_pushButton_stop_clicked(void);
    void on_pushButton_dispatch_clicked(void);
    void on_pushButton_recall_clicked(void);
    void on_pushButton_steal_clicked(void);
    void on_pushButton_request_clicked(void);
    void on_pushButton_release_clicked(void);
    void on_pushButton_init_clicked(void);
    void on_pushButton_horn_pressed(void);
    void on_pushButton_horn_released(void);
    void on_pushButton_F3_pressed(void);
    void on_pushButton_brake_pressed(void);
    void on_pushButton_functions_clicked(void);
    void on_lineEdit_speed_steps_editingFinished(void);
    void on_lineEdit_id_editingFinished(void);
    void on_lineEdit_address_editingFinished(void);
    void on_dial_speed_sliderMoved(void);
    void on_dial_speed_sliderReleased(void);
    void on_horizontalSlider_notch_sliderReleased(void);
private:
    Ui::MainWindow *ui;
    int  getValue(QLineEdit *lnEdit);
    int  lineEditing(QLineEdit *lnEdit, int max, int min);
    void addFunctionsText(void);
};

#endif // MAINWINDOW_H

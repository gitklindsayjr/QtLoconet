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
 /*****************************************************************************
 * MainWindow:
 * Interface between the Throttle UI/Form mainwindow.ui invoked by helper function, ui->setup()
 * part of class Ui_MainWindow.  This class is generated automatically by the Form editor.
 * Communication between the UI is done by signals and slots.  UI signals are connected to MainWindow
 * slots. These slots are then connected through signals to the Throttle class to Throttle slots as
 * xxxCommand. Responsed from the Throttle are relayed back to the UI through Throttle signals
 * connected to MainWindow slots. The slots are named xxxResponse and use the Ui_MainWindow class
 * helper functions to register the throttle responses. Switch requests are used by the Dispatcher
 * if this throttle is a forked process of Dispatcher to initiate switch action.  The switch requests
 * are send via the stdin/stdout connections between the 2 processes.
 ****************************************************************************************************/

#include <unistd.h>
#include <stdio.h>
#include <fstream>
#include "mainwindow.h"
#include "config.h"
#include "ui_mainwindow.h"

#define STEPS_PER_NOTCH   EMD_F7_STEPS_PER_NOTCH
MainWindow::MainWindow(QWidget *parent) : QMainWindow(parent), ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    QStringList args = qApp->arguments();
    QString argsHelp;
    QString windowTitle;
    dispatchChildProcess = false;
    myLoco = LOCO_INDEX_EMD_F7;
    myAddress = EMD_F7_ADDRESS;
    mySpeedScale = EMD_F7_SPEED_SCALE;
    argsHelp = "Usage:\nthrotle [OPTION 1] [OPTION 2]\n\
         none - defaults to: EMD F7\n\
         [OPTION 1] < F7 | GE >\n\
         [OPTION 2] <child> #Child process used by dispatcher application";
     switch(args.size())
     {
        case 1:
            windowTitle = "Throttle EMD-F7";
            break;
        case 3:
            if(args[2] == "child") // This app is a forked child of the dispather application
                dispatchChildProcess = true;
            else
            {   // Argument 3 must be "child" else error
                QMessageBox::critical(this, "Argument ERROR:", argsHelp);
                break;
            }
        case 2:
            if(args[1].startsWith("-h")) // Help argument "-h"
                QMessageBox::critical(this, "Argument ERROR:", argsHelp);
            else if(args[1] == "GE")
            {   // Specify functions for ESU LockSound for a GE-44 Ton
                windowTitle = GE_TITLE;
                myLoco = LOCO_INDEX_GE;  // myLoco, used for disabled throttle notch functions
                myAddress = GE_ADDRESS;
                mySpeedScale = GE_SPEED_SCALE;
            }
            else if(args[1] == "F7")
            {   // Specify functions for Atherns Genesis, SoundTrax Tsunami II
                windowTitle = "EMD_F7_TITLE";
                myLoco = LOCO_INDEX_EMD_F7; // myLoco, allows throttle not functions, default loco
                myAddress = EMD_F7_ADDRESS;
                mySpeedScale = EMD_F7_SPEED_SCALE;
            }
            else if(args[1] == "child") // No loco specified, forked app usage
                dispatchChildProcess = true;
            else
                QMessageBox::critical(this, "Argument ERRPR:", argsHelp);
            break;
        default:
            QMessageBox::critical(this, "Argument ERROR:", argsHelp);
            break;
    }
    setWindowTitle(windowTitle);
    setWindowIcon(QIcon(":/Throttle.png"));
    myThrottle = false;
    mySpeed = 0; // Speed step
    myNotch = 0; // Notch step
    ui->lineEdit_address->setText(QString::number(myAddress));
#ifndef LOAD_LOCO_FUNCTIONS // Switch for initilizing locomotive functions
    functions = initializeFunctions();
    #ifdef SAVE_LOCO_FUNCTIONS // Switch for saving initialized locomotive functions
        saveFunctions(functions, "throttle.cfg");
    #endif
#else // Switch for loading previously saved locomotive functions
    functions = loadFunctions("throttle.cfg");
#endif

#ifdef SIMULATE // Switch for simulating loconet/locomotive behavior offline see Throttle.pro configuration
    simulate = true;
#else
    simulate = false;
#endif
    locoThrottle = new Throttle(dispatchChildProcess, this);
    // Throttle signals
    connect(this, SIGNAL(switchStateCommand(int, bool)), locoThrottle, SLOT(switchStateCommand(int, bool)));
    connect(this, SIGNAL(powerCommand(bool)), locoThrottle, SLOT(powerCommand(bool)));
    connect(this, SIGNAL(functionsCommand(int)), locoThrottle, SLOT(functionsCommand(int)));
    connect(this, SIGNAL(bellCommand(bool)), locoThrottle, SLOT(bellCommand(bool)));
    connect(this, SIGNAL(hornCommand(bool)), locoThrottle, SLOT(hornCommand(bool)));
    connect(this, SIGNAL(f3Command(bool)), locoThrottle, SLOT(f3Command(bool)));
    connect(this, SIGNAL(dynamicBrakeCommand(bool)), locoThrottle, SLOT(dynamicBrakeCommand(bool)));
    connect(this, SIGNAL(muteCommand(bool)), locoThrottle, SLOT(muteCommand(bool)));
    connect(this, SIGNAL(lightCommand(bool)), locoThrottle, SLOT(lightCommand(bool)));
    connect(this, SIGNAL(directionCommand(bool)), locoThrottle, SLOT(directionCommand(bool)));
    connect(this, SIGNAL(speedCommand(int)), locoThrottle, SLOT(speedCommand(int)));
    connect(this, SIGNAL(stopCommand()), locoThrottle, SLOT(stopCommand()));
    connect(this, SIGNAL(stealCommand(int)), locoThrottle, SLOT(stealCommand(int)));
    connect(this, SIGNAL(requestCommand(int)), locoThrottle, SLOT(requestCommand(int)));
    connect(this, SIGNAL(releaseCommand(bool)), locoThrottle, SLOT(releaseCommand(bool)));
    connect(this, SIGNAL(dispatchCommand()), locoThrottle, SLOT(dispatchCommand()));
    connect(this, SIGNAL(recallCommand()), locoThrottle, SLOT(recallCommand()));
    connect(this, SIGNAL(idChange(int)), locoThrottle, SLOT(idChange(int)));
    connect(this, SIGNAL(initCommand(int)), locoThrottle, SLOT(initCommand(int)));
}
MainWindow::~MainWindow()
{
    this->functions.clear();
    delete locoThrottle;
    delete ui;
}
void MainWindow::addFunctionsText(void)
{   // Add configuration loco functions to combo box
    comboInit = false;
    ui->comboBox_functions->clear();
    QString text;
    myNumberOfLocos = functions.size();
    if(myLoco >= myNumberOfLocos)
        return;
    functions_t funcs;
    funcs = this->functions[myLoco];
    myFunction = funcs.function;
    for(int i=INDEX_START; i<MAX_FUNCTIONS; i++)
    {
        text = funcs.text[i];
        if(text == "")
            break;
        ui->comboBox_functions->addItem(text);
    }
    for(int i = 0; i<MAX_INDEXS; i++)
    {
        if(myFunction == funcs.index[i])
            ui->comboBox_functions->setCurrentIndex(i);
    }
    if(myLoco == LOCO_INDEX_GE)
    {   // Disable the notch label and slider action for this loco
        ui->label_notch->setDisabled(true);
        ui->horizontalSlider_notch->setDisabled(true);
    }
    else
    {
        ui->label_notch->setEnabled(true);
        ui->horizontalSlider_notch->setEnabled(true);
    }
    comboInit = true;
    text = funcs.text[F3_INDEX];
    ui->pushButton_F3->setText(text);
}
void MainWindow::on_comboBox_functions_currentIndexChanged(int index)
{   // Set combo box index for use by "operate function slot"
    if(index < 0 || !comboInit)
        return;
    functions_t funcs;
    funcs = this->functions[myLoco];
    if(index >= MAX_INDEXS)
        return;
    myFunction = funcs.index[index];
}
// Throttle responses from LocoNet throttle class call back functions
void MainWindow::muteResponse(bool state)
{   // Check mute box, function F8
    if(myLoco)
        state^= 1;
    ui->checkBox_mute->setChecked(state);
}
void MainWindow::lightResponse(bool state)
{   // Enable/disable light, function F0
    ui->checkBox_light->setChecked(state);
}
void MainWindow::bellResponse(bool state)
{
    ui->checkBox_bell->setChecked(state);
}
void MainWindow::directionResponse(bool direction)
{
    if(dispatchChildProcess) // If this is a forked process dispatcher wants to know the direction
        { printf("<DI %d>\n", direction); fflush(stdout); }
    ui->radioButton_forward->setChecked(direction);
    ui->radioButton_reverse->setChecked(!direction);
}
void MainWindow::speedResponse(int speed)
{
    if(speed == EMERG_STOP)
        speed = 0;
    mySpeed = speed;
    double dSpeed = (double)speed;
    dSpeed*= mySpeedScale;
    speed = (int)dSpeed;
    if(dispatchChildProcess) // If this is a forked process dispatcher wants to know the speed
        { printf("<SP %d>\n", speed); fflush(stdout); }
    speed = mySpeed;
    speed*= 100; speed/= 128; // Scaled back for DigiTrax throttle displayts
    ui->lineEdit_speed_steps->setText(QString::number(speed));
    ui->dial_speed->setValue(speed);
    mySpeed = speed;
    // Adjust notch setting as a result of speed
    if(myLoco == LOCO_INDEX_GE)
        return;
    speed+= (STEPS_PER_NOTCH -1);
    speed/= STEPS_PER_NOTCH;
    if(myNotch < 0 && speed == 0)
        return;
    myNotch = speed;
    ui->horizontalSlider_notch->setValue(myNotch);
}
void MainWindow::switchRequest(int sw, bool state)
{   // This throttle app doesn't support switch requests instead forward
    if(dispatchChildProcess) // DT500 switch requests to Dispatcher
        { printf("<SW %d %d>\n", sw, state); fflush(stdout); }
}
void MainWindow::switchState(int sw, bool state)
{   // This throttle doesn't support switch states forward
    if(dispatchChildProcess) // to other thethered throttle from Dispatcher
        switchStateCommand(sw, state);
}
void MainWindow::addressResponse(int address)
{   // Update address
    myAddress = address;
    ui->lineEdit_address->setText(QString::number(address));
}
void MainWindow::idResponse(int id, bool ours)
{   // Update ID, set throttle ownership
    QPalette palette = ui->lineEdit_id->palette();
    if(ours)
    {   // green signifies it is our id
        palette.setColor(QPalette::Base, Qt::green);
        myThrottle = true;
        if(myAddress == EMD_F7_ADDRESS)
        {
            myLoco = LOCO_INDEX_EMD_F7;
            mySpeedScale = EMD_F7_SPEED_SCALE;
            setWindowTitle(EMD_F7_TITLE);
        }
        else if(myAddress == GE_ADDRESS)
        {
            myLoco = LOCO_INDEX_GE;
            mySpeedScale = GE_SPEED_SCALE;
            setWindowTitle(GE_TITLE);
        }
        else
            goto skip;
        addFunctionsText();
    }
    else
    {   // red signifies it is not our id
        palette.setColor(QPalette::Base, Qt::red);
        myThrottle = false;
    }
skip:
    ui->lineEdit_id->setText(QString::number(id));
    ui->lineEdit_id->setPalette(palette);
}
void MainWindow::slotResponse(int slot, bool ours)
{   // Update slot
    QPalette palette = ui->lineEdit_slot->palette();
    if(ours) // green signifies our slot
        palette.setColor(QPalette::Base, Qt::green);
    else     // red signifies some other throttles slot
        palette.setColor(QPalette::Base, Qt::red);
    ui->lineEdit_slot->setText(QString::number(slot));
    ui->lineEdit_slot->setPalette(palette);
}
void MainWindow::powerResponse(bool state)
{   // Set radio buttons on/off power state
    ui->radioButton_pwrOn->setChecked(state);
    ui->radioButton_pwrOff->setChecked(!state);
}
void MainWindow::stateResponse(QString str)
{   // Display current state
    ui->lineEdit_state->setText(str);
}
void MainWindow::statusResponse(QString str)
{   // Display current LACK status
    ui->lineEdit_status->setText(str);
}
void MainWindow::on_pushButton_horn_pressed(void)
{  // Operate horn, function F2
   hornCommand(true);
}
void MainWindow::on_pushButton_horn_released(void)
{  // Silence horn, function F2
       hornCommand(false);
}
void MainWindow::on_pushButton_F3_pressed(void)
{  // Operate F3 function, short horn for EMD-F7, coupler for GE-44 Ton
   f3Command(true);
}
void MainWindow::on_pushButton_brake_pressed(void)
{   // Enable dynamic brake, function F4
    dynamicBrakeCommand(true);
}
void MainWindow::on_pushButton_functions_clicked(void)
{   // Operate functions 9 - 23
    functionsCommand(myFunction);
}
void MainWindow::on_checkBox_bell_clicked(void)
{   // Enable/disable bell function F1
    bool state = ui->checkBox_bell->isChecked();
//    ui->checkBox_bell->setChecked(!state);
    if(state)
        bellCommand(true);
    else
        bellCommand(false);
}
void MainWindow::on_checkBox_light_clicked(void)
{   // Enable/disable light, function F0
    bool state = ui->checkBox_light->isChecked();
    if(state)
        lightCommand(true);
    else
        lightCommand(false);
}
void MainWindow::on_checkBox_mute_clicked(void)
{   // Enable/disable sound, function F8
    bool state = ui->checkBox_mute->isChecked();
    if(myLoco == LOCO_INDEX_GE)
        state^= 1;
    muteCommand(state);
}
void MainWindow::on_radioButton_pwrOn_clicked(void)
{   // turn on track power, clear error message
    statusBar()->clearMessage();
    powerCommand(true);
}
void MainWindow::on_radioButton_pwrOff_clicked(void)
{   // Remove track power
    powerCommand(false);
}
void MainWindow::on_radioButton_forward_clicked(void)
{   // Set loco to forward direction, clear error message
    if(myThrottle || simulate)
    {
        statusBar()->clearMessage();
        directionCommand(FORWARD);
    }
}
void MainWindow::on_radioButton_reverse_clicked(void)
{   // Set loco to reverse direction, clear error message
    if(myThrottle || simulate)
    {
        statusBar()->clearMessage();
        directionCommand(REVERSE);
    }
}
void MainWindow::on_pushButton_stop_clicked(void)
{   // Emergency stop, clear error message
    statusBar()->clearMessage();
    stopCommand();
}
void MainWindow::on_pushButton_steal_clicked(void)
{   // Steal loco address from other throttle, clear message
    int address = getValue(ui->lineEdit_address);
    if(address == 0)
    {
        statusBar()->showMessage("Error: No address");
        return;
    }
    statusBar()->clearMessage();
    stealCommand(address);
}
void MainWindow::on_pushButton_request_clicked(void)
{   // Request this address
    int address = getValue(ui->lineEdit_address);
    if(address == 0)
    {
        statusBar()->showMessage("Error: No address");
        return;
    }
    statusBar()->clearMessage();
    requestCommand(address);
}
void MainWindow::on_pushButton_release_clicked(void)
{   // Release slot
    statusBar()->clearMessage();
    releaseCommand(false);
}
void MainWindow::on_pushButton_dispatch_clicked(void)
{   // Dispatch slot, move slot to slot 0
    statusBar()->clearMessage();
    dispatchCommand();
}
void MainWindow::on_pushButton_recall_clicked(void)
{   // Recall previously dispatche address, slot 0 to mySlot
    statusBar()->clearMessage();
    recallCommand();
}
void MainWindow::on_pushButton_init_clicked(void)
{   // Initialize the UI, use address 0 if not previously set
    int address = getValue(ui->lineEdit_address);
    initCommand(address);
    statusBar()->clearMessage();
}
void MainWindow::receiveMessage(QString msg)
{   // Display error messages or other messages from the throttle
    statusBar()->showMessage(msg);
}
int  MainWindow::getValue(QLineEdit *lnEdit)
{
    bool ok;
    QString qStr;
    qStr = lnEdit->text();
    int value = qStr.toInt(&ok);
    return value;
}
int  MainWindow::lineEditing(QLineEdit *lnEdit, int max, int min)
{   // Message box and line editing function, address, id, slot, speed
    bool ok;
    QString qStr;
    qStr = lnEdit->text();
    int value = qStr.toInt(&ok);
    if(value > max)
    {
        value = max;
        qStr = QString::number(max);
        lnEdit->setText(qStr);
    }
    else if(value < min)
    {
        value = min;
        qStr = QString::number(min);
        lnEdit->setText(qStr);
    }
    return value;
}
void MainWindow::on_lineEdit_id_editingFinished(void)
{
    int id = lineEditing(ui->lineEdit_id, 11000, 1);
    idChange(id);
}
void MainWindow::on_lineEdit_address_editingFinished(void)
{
    lineEditing(ui->lineEdit_address, 120, 1);
}
void MainWindow::on_lineEdit_speed_steps_editingFinished(void)
{
    if(myThrottle || simulate)
    {   // Allow speed changes if simulating LocoNetThrottle or I own the throttle
        int speed = lineEditing(ui->lineEdit_speed_steps, 100, 0);
        if(speed == EMERG_STOP)
            speed = 0;
        speed*= 128;
        speed/= 100;
        speedCommand(speed);
    }
    else
    {
        ui->lineEdit_speed_steps->setText(QString::number(mySpeed));
        ui->dial_speed->setValue(mySpeed);
    }
}
void MainWindow::on_dial_speed_sliderReleased(void)
{
    if(myThrottle || simulate)
    {
        int speed = ui->dial_speed->value();
        ui->lineEdit_speed_steps->setText(QString::number(speed));
        if(speed == EMERG_STOP)
            speed = 0;
        speed*= 128;
        speed/= 100;
        speedCommand(speed);
    }
    else
    {
        ui->lineEdit_speed_steps->setText(QString::number(mySpeed));
        ui->dial_speed->setValue(mySpeed);
    }
}
void MainWindow::on_dial_speed_sliderMoved(void)
{
    int speed = ui->dial_speed->value();
    ui->lineEdit_speed_steps->setText(QString::number(speed));
    if(speed == EMERG_STOP)
        speed = 0;
    speed*= 128;
    speed/= 100;
    speedCommand(speed);
}
void MainWindow::on_horizontalSlider_notch_sliderReleased(void)
{
    int notch = ui->horizontalSlider_notch->value();
    int diff = (notch - myNotch);
    if(diff > 0)
    {  // Notch up function 26
        myNotch++;
        functionsCommand(F26_NOTCH_UP);
    }
    else if(diff < 0)
    {  // Notch down function 27
        myNotch--;
        functionsCommand(F27_NOTCH_DOWN);
    }
    ui->horizontalSlider_notch->setValue(myNotch);
}

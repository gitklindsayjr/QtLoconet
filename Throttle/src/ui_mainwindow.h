/********************************************************************************
** Form generated from reading UI file 'mainwindow.ui'
**
** Created by: Qt User Interface Compiler version 5.5.1
**
** WARNING! All changes made in this file will be lost when recompiling UI file!
********************************************************************************/

#ifndef UI_MAINWINDOW_H
#define UI_MAINWINDOW_H

#include <QtCore/QVariant>
#include <QtWidgets/QAction>
#include <QtWidgets/QApplication>
#include <QtWidgets/QButtonGroup>
#include <QtWidgets/QCheckBox>
#include <QtWidgets/QComboBox>
#include <QtWidgets/QDial>
#include <QtWidgets/QFrame>
#include <QtWidgets/QHeaderView>
#include <QtWidgets/QLabel>
#include <QtWidgets/QLineEdit>
#include <QtWidgets/QMainWindow>
#include <QtWidgets/QMenuBar>
#include <QtWidgets/QPushButton>
#include <QtWidgets/QRadioButton>
#include <QtWidgets/QSlider>
#include <QtWidgets/QStatusBar>
#include <QtWidgets/QToolBar>
#include <QtWidgets/QWidget>

QT_BEGIN_NAMESPACE

class Ui_MainWindow
{
public:
    QWidget *centralWidget;
    QRadioButton *radioButton_pwrOn;
    QRadioButton *radioButton_pwrOff;
    QFrame *line;
    QPushButton *pushButton_stop;
    QLineEdit *lineEdit_speed_steps;
    QLabel *label_speed;
    QDial *dial_speed;
    QRadioButton *radioButton_reverse;
    QRadioButton *radioButton_forward;
    QLineEdit *lineEdit_id;
    QLineEdit *lineEdit_slot;
    QLabel *label_addr;
    QLineEdit *lineEdit_address;
    QPushButton *pushButton_steal;
    QPushButton *pushButton_release;
    QPushButton *pushButton_dispatch;
    QPushButton *pushButton_request;
    QLineEdit *lineEdit_state;
    QLabel *label_state;
    QLabel *label_id;
    QLabel *label_slot;
    QLabel *label_status;
    QLineEdit *lineEdit_status;
    QPushButton *pushButton_init;
    QFrame *line_2;
    QPushButton *pushButton_recall;
    QLabel *label_notch;
    QSlider *horizontalSlider_notch;
    QComboBox *comboBox_functions;
    QLabel *label_functions;
    QPushButton *pushButton_functions;
    QCheckBox *checkBox_mute;
    QCheckBox *checkBox_light;
    QCheckBox *checkBox_bell;
    QPushButton *pushButton_horn;
    QPushButton *pushButton_F3;
    QPushButton *pushButton_brake;
    QLabel *label_mph;
    QMenuBar *menuBar;
    QToolBar *mainToolBar;
    QStatusBar *statusBar;

    void setupUi(QMainWindow *MainWindow)
    {
        if (MainWindow->objectName().isEmpty())
            MainWindow->setObjectName(QStringLiteral("MainWindow"));
        MainWindow->resize(386, 626);
        centralWidget = new QWidget(MainWindow);
        centralWidget->setObjectName(QStringLiteral("centralWidget"));
        radioButton_pwrOn = new QRadioButton(centralWidget);
        radioButton_pwrOn->setObjectName(QStringLiteral("radioButton_pwrOn"));
        radioButton_pwrOn->setGeometry(QRect(30, 0, 135, 30));
        radioButton_pwrOn->setAutoExclusive(false);
        radioButton_pwrOff = new QRadioButton(centralWidget);
        radioButton_pwrOff->setObjectName(QStringLiteral("radioButton_pwrOff"));
        radioButton_pwrOff->setGeometry(QRect(250, 0, 121, 30));
        radioButton_pwrOff->setChecked(false);
        radioButton_pwrOff->setAutoExclusive(false);
        line = new QFrame(centralWidget);
        line->setObjectName(QStringLiteral("line"));
        line->setGeometry(QRect(20, 20, 351, 30));
        QFont font;
        font.setBold(false);
        font.setWeight(50);
        line->setFont(font);
        line->setLineWidth(3);
        line->setFrameShape(QFrame::HLine);
        line->setFrameShadow(QFrame::Sunken);
        pushButton_stop = new QPushButton(centralWidget);
        pushButton_stop->setObjectName(QStringLiteral("pushButton_stop"));
        pushButton_stop->setGeometry(QRect(290, 50, 81, 30));
        lineEdit_speed_steps = new QLineEdit(centralWidget);
        lineEdit_speed_steps->setObjectName(QStringLiteral("lineEdit_speed_steps"));
        lineEdit_speed_steps->setGeometry(QRect(90, 50, 81, 30));
        lineEdit_speed_steps->setAlignment(Qt::AlignCenter);
        label_speed = new QLabel(centralWidget);
        label_speed->setObjectName(QStringLiteral("label_speed"));
        label_speed->setGeometry(QRect(30, 50, 61, 30));
        dial_speed = new QDial(centralWidget);
        dial_speed->setObjectName(QStringLiteral("dial_speed"));
        dial_speed->setGeometry(QRect(-10, 110, 191, 131));
        dial_speed->setMaximum(100);
        dial_speed->setPageStep(20);
        dial_speed->setTracking(false);
        dial_speed->setWrapping(false);
        dial_speed->setNotchesVisible(true);
        radioButton_reverse = new QRadioButton(centralWidget);
        radioButton_reverse->setObjectName(QStringLiteral("radioButton_reverse"));
        radioButton_reverse->setGeometry(QRect(120, 80, 91, 30));
        radioButton_forward = new QRadioButton(centralWidget);
        radioButton_forward->setObjectName(QStringLiteral("radioButton_forward"));
        radioButton_forward->setGeometry(QRect(20, 80, 101, 30));
        radioButton_forward->setChecked(false);
        radioButton_forward->setAutoExclusive(false);
        lineEdit_id = new QLineEdit(centralWidget);
        lineEdit_id->setObjectName(QStringLiteral("lineEdit_id"));
        lineEdit_id->setGeometry(QRect(100, 320, 61, 30));
        lineEdit_id->setAutoFillBackground(false);
        lineEdit_id->setAlignment(Qt::AlignCenter);
        lineEdit_slot = new QLineEdit(centralWidget);
        lineEdit_slot->setObjectName(QStringLiteral("lineEdit_slot"));
        lineEdit_slot->setGeometry(QRect(100, 400, 61, 30));
        lineEdit_slot->setAlignment(Qt::AlignCenter);
        label_addr = new QLabel(centralWidget);
        label_addr->setObjectName(QStringLiteral("label_addr"));
        label_addr->setGeometry(QRect(20, 360, 81, 30));
        label_addr->setFont(font);
        lineEdit_address = new QLineEdit(centralWidget);
        lineEdit_address->setObjectName(QStringLiteral("lineEdit_address"));
        lineEdit_address->setGeometry(QRect(100, 360, 61, 30));
        QFont font1;
        font1.setUnderline(false);
        lineEdit_address->setFont(font1);
        lineEdit_address->setAlignment(Qt::AlignCenter);
        pushButton_steal = new QPushButton(centralWidget);
        pushButton_steal->setObjectName(QStringLiteral("pushButton_steal"));
        pushButton_steal->setGeometry(QRect(260, 320, 111, 30));
        pushButton_steal->setFont(font1);
        pushButton_release = new QPushButton(centralWidget);
        pushButton_release->setObjectName(QStringLiteral("pushButton_release"));
        pushButton_release->setGeometry(QRect(170, 360, 81, 30));
        pushButton_dispatch = new QPushButton(centralWidget);
        pushButton_dispatch->setObjectName(QStringLiteral("pushButton_dispatch"));
        pushButton_dispatch->setGeometry(QRect(170, 400, 81, 30));
        pushButton_dispatch->setFont(font1);
        pushButton_request = new QPushButton(centralWidget);
        pushButton_request->setObjectName(QStringLiteral("pushButton_request"));
        pushButton_request->setGeometry(QRect(170, 320, 81, 30));
        pushButton_request->setFont(font1);
        lineEdit_state = new QLineEdit(centralWidget);
        lineEdit_state->setObjectName(QStringLiteral("lineEdit_state"));
        lineEdit_state->setGeometry(QRect(90, 480, 281, 30));
        label_state = new QLabel(centralWidget);
        label_state->setObjectName(QStringLiteral("label_state"));
        label_state->setGeometry(QRect(20, 480, 51, 30));
        label_id = new QLabel(centralWidget);
        label_id->setObjectName(QStringLiteral("label_id"));
        label_id->setGeometry(QRect(20, 320, 31, 30));
        label_slot = new QLabel(centralWidget);
        label_slot->setObjectName(QStringLiteral("label_slot"));
        label_slot->setGeometry(QRect(20, 400, 41, 30));
        label_status = new QLabel(centralWidget);
        label_status->setObjectName(QStringLiteral("label_status"));
        label_status->setGeometry(QRect(20, 520, 61, 30));
        lineEdit_status = new QLineEdit(centralWidget);
        lineEdit_status->setObjectName(QStringLiteral("lineEdit_status"));
        lineEdit_status->setGeometry(QRect(90, 520, 281, 30));
        pushButton_init = new QPushButton(centralWidget);
        pushButton_init->setObjectName(QStringLiteral("pushButton_init"));
        pushButton_init->setGeometry(QRect(260, 360, 111, 30));
        line_2 = new QFrame(centralWidget);
        line_2->setObjectName(QStringLiteral("line_2"));
        line_2->setGeometry(QRect(20, 450, 351, 30));
        line_2->setFont(font);
        line_2->setLineWidth(3);
        line_2->setFrameShape(QFrame::HLine);
        line_2->setFrameShadow(QFrame::Sunken);
        pushButton_recall = new QPushButton(centralWidget);
        pushButton_recall->setObjectName(QStringLiteral("pushButton_recall"));
        pushButton_recall->setGeometry(QRect(260, 400, 111, 51));
        label_notch = new QLabel(centralWidget);
        label_notch->setObjectName(QStringLiteral("label_notch"));
        label_notch->setEnabled(true);
        label_notch->setGeometry(QRect(250, 80, 67, 30));
        horizontalSlider_notch = new QSlider(centralWidget);
        horizontalSlider_notch->setObjectName(QStringLiteral("horizontalSlider_notch"));
        horizontalSlider_notch->setGeometry(QRect(180, 110, 191, 30));
        horizontalSlider_notch->setMinimum(-1);
        horizontalSlider_notch->setMaximum(7);
        horizontalSlider_notch->setPageStep(0);
        horizontalSlider_notch->setValue(0);
        horizontalSlider_notch->setOrientation(Qt::Horizontal);
        horizontalSlider_notch->setTickPosition(QSlider::TicksBothSides);
        horizontalSlider_notch->setTickInterval(1);
        comboBox_functions = new QComboBox(centralWidget);
        comboBox_functions->setObjectName(QStringLiteral("comboBox_functions"));
        comboBox_functions->setGeometry(QRect(20, 270, 151, 30));
        comboBox_functions->setMaxVisibleItems(14);
        label_functions = new QLabel(centralWidget);
        label_functions->setObjectName(QStringLiteral("label_functions"));
        label_functions->setGeometry(QRect(20, 240, 131, 30));
        pushButton_functions = new QPushButton(centralWidget);
        pushButton_functions->setObjectName(QStringLiteral("pushButton_functions"));
        pushButton_functions->setGeometry(QRect(200, 270, 171, 30));
        checkBox_mute = new QCheckBox(centralWidget);
        checkBox_mute->setObjectName(QStringLiteral("checkBox_mute"));
        checkBox_mute->setGeometry(QRect(180, 230, 71, 30));
        checkBox_mute->setCheckable(true);
        checkBox_light = new QCheckBox(centralWidget);
        checkBox_light->setObjectName(QStringLiteral("checkBox_light"));
        checkBox_light->setGeometry(QRect(180, 150, 71, 30));
        checkBox_light->setCheckable(true);
        checkBox_bell = new QCheckBox(centralWidget);
        checkBox_bell->setObjectName(QStringLiteral("checkBox_bell"));
        checkBox_bell->setGeometry(QRect(180, 190, 71, 30));
        pushButton_horn = new QPushButton(centralWidget);
        pushButton_horn->setObjectName(QStringLiteral("pushButton_horn"));
        pushButton_horn->setGeometry(QRect(270, 150, 101, 30));
        pushButton_horn->setCheckable(false);
        pushButton_F3 = new QPushButton(centralWidget);
        pushButton_F3->setObjectName(QStringLiteral("pushButton_F3"));
        pushButton_F3->setGeometry(QRect(270, 190, 101, 30));
        pushButton_F3->setCheckable(false);
        pushButton_brake = new QPushButton(centralWidget);
        pushButton_brake->setObjectName(QStringLiteral("pushButton_brake"));
        pushButton_brake->setGeometry(QRect(270, 230, 101, 30));
        pushButton_brake->setCheckable(false);
        label_mph = new QLabel(centralWidget);
        label_mph->setObjectName(QStringLiteral("label_mph"));
        label_mph->setGeometry(QRect(180, 50, 67, 30));
        MainWindow->setCentralWidget(centralWidget);
        menuBar = new QMenuBar(MainWindow);
        menuBar->setObjectName(QStringLiteral("menuBar"));
        menuBar->setGeometry(QRect(0, 0, 386, 25));
        MainWindow->setMenuBar(menuBar);
        mainToolBar = new QToolBar(MainWindow);
        mainToolBar->setObjectName(QStringLiteral("mainToolBar"));
        MainWindow->addToolBar(Qt::TopToolBarArea, mainToolBar);
        statusBar = new QStatusBar(MainWindow);
        statusBar->setObjectName(QStringLiteral("statusBar"));
        MainWindow->setStatusBar(statusBar);

        retranslateUi(MainWindow);

        QMetaObject::connectSlotsByName(MainWindow);
    } // setupUi

    void retranslateUi(QMainWindow *MainWindow)
    {
        MainWindow->setWindowTitle(QApplication::translate("MainWindow", "Throttle", 0));
        radioButton_pwrOn->setText(QApplication::translate("MainWindow", "Power On", 0));
        radioButton_pwrOff->setText(QApplication::translate("MainWindow", "Power Off", 0));
        pushButton_stop->setText(QApplication::translate("MainWindow", "Stop", 0));
        label_speed->setText(QApplication::translate("MainWindow", "Speed:", 0));
        radioButton_reverse->setText(QApplication::translate("MainWindow", "Reverse", 0));
        radioButton_forward->setText(QApplication::translate("MainWindow", "Forward", 0));
        label_addr->setText(QApplication::translate("MainWindow", "Address:", 0));
        lineEdit_address->setText(QString());
        pushButton_steal->setText(QApplication::translate("MainWindow", "Steal", 0));
        pushButton_release->setText(QApplication::translate("MainWindow", "Release", 0));
        pushButton_dispatch->setText(QApplication::translate("MainWindow", "Dispatch", 0));
        pushButton_request->setText(QApplication::translate("MainWindow", "Request", 0));
        label_state->setText(QApplication::translate("MainWindow", "State:", 0));
        label_id->setText(QApplication::translate("MainWindow", "Id:", 0));
        label_slot->setText(QApplication::translate("MainWindow", "Slot:", 0));
        label_status->setText(QApplication::translate("MainWindow", "Status:", 0));
        pushButton_init->setText(QApplication::translate("MainWindow", "Initialize", 0));
        pushButton_recall->setText(QApplication::translate("MainWindow", "Recall\n"
"Dispatched", 0));
        label_notch->setText(QApplication::translate("MainWindow", "Notch:", 0));
        label_functions->setText(QApplication::translate("MainWindow", "Functions 9-23:", 0));
        pushButton_functions->setText(QApplication::translate("MainWindow", "Operate Function", 0));
        checkBox_mute->setText(QApplication::translate("MainWindow", "Mute", 0));
        checkBox_light->setText(QApplication::translate("MainWindow", "Light", 0));
        checkBox_bell->setText(QApplication::translate("MainWindow", "Bell", 0));
        pushButton_horn->setText(QApplication::translate("MainWindow", "Horn", 0));
        pushButton_F3->setText(QApplication::translate("MainWindow", "F3", 0));
        pushButton_brake->setText(QApplication::translate("MainWindow", "Dyn. Brake", 0));
        label_mph->setText(QApplication::translate("MainWindow", "(Steps)", 0));
    } // retranslateUi

};

namespace Ui {
    class MainWindow: public Ui_MainWindow {};
} // namespace Ui

QT_END_NAMESPACE

#endif // UI_MAINWINDOW_H

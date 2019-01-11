#ifndef TRAIN_CTRL_H_
#define TRAIN_CTRL_H_

#include <QWidget>
#include <QTimer>
#include "panel.h"
#include "serial_io_class.h"
#include "ln_throttle.h"

#define THRU         true
#define DIVERGE      false
#define CROSSOVER    false
#define REVERSE      false
#define FORWARD      true
#define CW           1
#define CCW          -1
#define STOP         0

// All routes and lengths are based on clockwise movements
#define LOOP1_LENGTH            212  // Inches
#define LOOP1_SECTION_1         48   // Inches
#define LOOP1_SECTION_2         164  // Inches
#define BLOCK1_LENGTH           271  // Inches
#define BLOCK1_SWITCH1          263
#define BLOCK2_LENGTH           289  // Inches
#define BLOCK2_SWITCH3          72
#define BLOCK3_LENGTH           172  // Inches
#define BLOCK3_SWITCH7          9
#define BLOCK3_SWITCH5          27
#define BLOCK3_SWITCH2          145
#define BLOCK3_SWITCH4          145
#define BLOCK3_SWITCH6          154
#define BLOCK3_SWITCH8          163
#define BLOCK4_LENGTH           175  // Inches
#define BLOCK4_SWITCH11          70
#define BLOCK5_LENGTH           137  // Inches
#define BLOCK5_SWITCH12         107
#define BLOCK5_SWITCH13         128
#define LOOP2_LENGTH            192  // Inches
#define LOOP2_SECTION_1         156  // Inches
#define LOOP2_SECTION_2         36
#define STAGING_LENGTH          500  // Inches

#define NO_SWITCH             0
#define XOVER1_SW1_EN         0x00000001
#define XOVER1_SW1_THRU       0x00000001
#define XOVER1_SW1_BIT        0x00000002
#define XOVER1_SW1_CROSS      0x00000003
#define XOVER3_SW3_EN         0x00000004
#define XOVER3_SW3_THRU       0x00000004
#define XOVER3_SW3_BIT        0x00000008
#define XOVER3_SW3_CROSS      0x0000000c
#define XOVER2_SW2_EN         0x00000010
#define XOVER2_SW2_THRU       0x00000010
#define XOVER2_SW2_BIT        0x00000020
#define XOVER2_SW2_CROSS      0x00000030
#define LOOP1_SW4_EN          0x00000040
#define LOOP1_SW4_THRU        0x00000040
#define LOOP1_SW4_BIT         0x00000080
#define LOOP1_SW4_REVERSE     0x000000c0
#define LOOP2_SW5_EN          0x00000100
#define LOOP2_SW5_THRU        0x00000100
#define LOOP2_SW5_REVERSE     0x00000300
#define LOOP2_SW5_BIT         0x00000200
#define YARD1_SW7_EN          0x00000400
#define YARD1_SW7_THRU        0x00000400
#define YARD1_SW7_BIT         0x00000800
#define YARD1_SW7_DIVERGE     0x00000c00
#define YARD2_SW8_EN          0x00001000
#define YARD2_SW8_THRU        0x00001000
#define YARD2_SW8_BIT         0x00002000
#define YARD2_SW8_DIVERGE     0x00003000
#define YARD34_SW9_EN         0x00004000
#define YARD34_SW9_THRU       0x00004000
#define YARD34_SW9_BIT        0x00008000
#define YARD34_SW9_DIVERGE    0x0000c000
#define STAGING_SW6_EN        0x00010000
#define STAGING_SW6_THRU      0x00010000
#define STAGING_SW6_BIT       0x00020000
#define STAGING_SW6_DIVERGE   0x00030000
#define SPARE1_EN             0x00040000
#define SPARE1_THRU           0x00040000
#define SPARE1_BIT            0x00080000
#define SPARE1_DIVERGE        0x000c0000
#define SPARE2_EN             0x00100000
#define SPARE2_THRU           0x00100000
#define SPARE2_BIT            0x00200000
#define SPARE2_DIVERGE        0x00300000
#define SPARE3_EN             0x00400000
#define SPARE3_THRU           0x00400000
#define SPARE3_BIT            0x00800000
#define SPARE3_DIVERGE        0x00c00000
#define SW_MASK               0x00ffffff
#define ALL_THRU              0x00555555

enum _sw { SW1 = 1, SW2, SW3, SW4, SW5, SW6, SW7, SW8, SW9, SWS };

#define CHAR_BUFFER_SIZE        256
/********************* Routes structures ********************/
typedef struct
{   // Switch data reguired for routing and routes
    unsigned swId;
    unsigned swBitMask;
    bool     state;
}RtSwitch_t;
typedef struct
{   // Define the blocks properties for the route
    int direction;     // Train is travelling CW or CCW
    int block;         // Block we are working with
    int startJunction; // Starting junction location with in track objects, index
    int endJunction;   // Ending unctions location with in track objects, index
    int switchPt;      // Set switch at index count
    QVector<RtSwitch_t> switches; // Switches defined within the block
}RtBlock_t;
typedef struct
{   // A route is defined by a vector of blocks
    QVector<RtBlock_t> blocks;
}Route_t;
/*************************** Run time routing structures ************************/
typedef struct
{   // Section within the run time route, independent of blocks
    int      direction; // Sections direction of train travel CW or CCW
    unsigned row;       // The row location in the panel
    unsigned column;    // The column location in the panel
    unsigned samples;   // How many samples, at the sample rate (time) defined by the speed
    RtSwitch_t sw;      // Switches to operate in this section
}Section_t;
typedef struct
{   // Run time route defines the current block
    unsigned block;  // Block Id
    QVector<Section_t>  sections; // Vector of sections within the route
}Routing_t;

class Panel;
class AdcDialog;
class AnalyzeDialog;
class TrainSimulation;

class TrainControl : public QWidget
{
    Q_OBJECT
public:
    TrainControl(QWidget *parent);
    ~TrainControl(void);
    void blocks(void);
public:
    Panel      *panel;  // Control panel is child of train control
#ifdef SIMULATE
    TrainSimulation *trainSim;       // Engage MSP432 train simulator
#endif
    SerialIoClass   *port;  // Serial port for communication to MSP432
    char outStr[CHAR_BUFFER_SIZE];  // Output buffer to tx to MSP432
    char inStr[CHAR_BUFFER_SIZE];   // Input buffer from MSP432 tx
    uint16_t threshold[NUMBER_OF_BLOCKS]; // Adc block detection threshold
    uint16_t offset[NUMBER_OF_BLOCKS];    // Adc block detection offset
    QVector<Route_t> routes;   // Number of routes created by "create_routes.cpp"
    QVector<Routing_t> route;  // Run time route, Todo: associate with trainId
    bool run;                  // The train is runing, Todo: assoiate with trainId
    void switchReportCommand(unsigned swNum, unsigned direction);
private:
    QWidget *parent;          // Out parent is the MainWindow for signaling
    AdcDialog *adcDialog;     // Dialog for setting offsets and threholds for the MSP432
    AnalyzeDialog *analyzeDialog; // Dialog for analyzing the MSP432 block dectors
    QTimer *switchTimer;      // Time for sending disable command to the MSP432 switch controllers
    int      refreshCnt;      // 200 second update counter
    unsigned timer;           // Timer counter for runing trains along route
    unsigned sectionCnt;      // Runing route section counter
    unsigned nextTime;        // Number of samples to switch sections
    unsigned rtBlockIndex;
    unsigned currentBlock;    // The current block that a runing train is in
    unsigned switchesState;
    QVector<Section_t> section; // Sections within the current route
    void handleBlockStates(char *inStr);
    void setSwitches(int rt);
    void createRoutes(void);
    void createRouting(unsigned route, int speed);
    void displayRouteData(unsigned route);
    unsigned swNumToSwitch(unsigned swNum);
    void stdSwitch(unsigned swState);   // Handles standard switch signals from MainWindow
    void xoverSwitch(unsigned swState); // Handles crossover switch signals from MainWindow
public slots:
    void drawPanel(QPainter *qp);    // Draw the panel, signal from paintEvent from MainWindow
    void moveSwitch(unsigned swBitMask);  // Signal from MainWindow to operate a switch
private slots:
    void disableSwitches(void);      // Handle signal from switch timer
    void startTrainRoute(unsigned route, int speed); // Handle signal from MainWindow dialog
    void stopTrainRoute(void);        // Handle signal from MainWindow dialog
    void blockStatus(void);          // Handle signal from periodic timer to update status
    void startAdcAnalysis(void);     // Handle signal from MainWindow to start analyzeDialog
    void startAdcDialog(void);       // Handle signal from MainWindow to start adcDialog
    void updateAdcThresholds(void);  // Handle signal from adcDialog to update MSP432 adc thresholds
    void updateAdcOffsets(void);     // Handle signal from adcDialog to update MSP432 adc offsets
    void startAnalysis(void);        // Handle signal to transmit repetively MSP432 adc block data
    void stopAnalysis(void);         // Handle signal to stop the transmiting of MSP432 adc data
    void closeDialogs(void);         // Handle signal from MainWidow closeEvent to close open dialogs
signals:
    void setSwitchState(int swState);   // Tell the panel to update switch states showing thru or diverged
    void setUiSwitchState(int swState); // Feed back data from MSP432 to update MainWindow dialog
    void setUiStartBlock(unsigned block);
    void updateBlock(int block, bool occupied);
    void updateTrainPosition(unsigned swId);
    void updateTrainPosition(unsigned row, unsigned column); // Tell panel to display train position
    void adcData(char *inStr);  // Signal to analyzeDialog to send adc data
    void trainSimulationLoop(void);  // Signal simulation loop to time step
};
#endif

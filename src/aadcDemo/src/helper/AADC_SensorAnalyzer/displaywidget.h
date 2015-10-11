/**
Copyright (c) 
Audi Autonomous Driving Cup. All rights reserved.

Redistribution and use in source and binary forms, with or without modification, are permitted provided that the following conditions are met:

1. Redistributions of source code must retain the above copyright notice, this list of conditions and the following disclaimer.
2. Redistributions in binary form must reproduce the above copyright notice, this list of conditions and the following disclaimer in the documentation and/or other materials provided with the distribution.
3.  All advertising materials mentioning features or use of this software must display the following acknowledgement: “This product includes software developed by the Audi AG and its contributors for Audi Autonomous Driving Cup.”
4.  Neither the name of Audi nor the names of its contributors may be used to endorse or promote products derived from this software without specific prior written permission.

THIS SOFTWARE IS PROVIDED BY AUDI AG AND CONTRIBUTORS “AS IS” AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL AUDI AG OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.


**********************************************************************
* $Author:: spiesra $  $Date:: 2015-05-21 12:25:11#$ $Rev:: 35324   $
**********************************************************************/

#ifndef DISPWIDGET
#define DISPWIDGET
#define LIST_LENGTH 22
#define LIST_VOLTAGE_MEASUREMENT 0
#define LIST_VOLTAGE_SPEEDCTR 1
#define LIST_ULTRASONIC_FRONT_LEFT 2
#define LIST_ULTRASONIC_FRONT_CENTER_LEFT 3
#define LIST_ULTRASONIC_FRONT_CENTER 4
#define LIST_ULTRASONIC_FRONT_CENTER_RIGHT 5
#define LIST_ULTRASONIC_FRONT_RIGHT 6
#define LIST_ULTRASONIC_SIDE_LEFT 7
#define LIST_ULTRASONIC_SIDE_RIGHT 8
#define LIST_ULTRASONIC_REAR_LEFT 9
#define LIST_ULTRASONIC_REAR_CENTER 10
#define LIST_ULTRASONIC_REAR_RIGHT 11
#define LIST_GYROSCOPE_X_ACC 12
#define LIST_GYROSCOPE_Y_ACC 13
#define LIST_GYROSCOPE_Z_ACC 14
#define LIST_GYROSCOPE_YAW 15
#define LIST_GYROSCOPE_PITCH 16
#define LIST_GYROSCOPE_ROLL 17
#define LIST_WHEEL_TACH_RIGHT 18
#define LIST_WHEEL_TACH_LEFT 19
#define LIST_WHEEL_DIR_RIGHT 20
#define LIST_WHEEL_DIR_LEFT 21

struct tSensorPreset{
    cString sensorName;
    tFloat32 nominalValue;
    tFloat32 maxNegDeviation;
    tFloat32 maxPosDeviation;

    tSensorPreset():sensorName(""),nominalValue(0),maxNegDeviation(0),maxPosDeviation(0){}
};

#include "stdafx.h"

struct usSensorGeometrics{
    // coordinates from the car middle
    tInt xPos;
    tInt yPos;
    tInt startAngle;
    tFloat32 maxValue;
};

class cSensorAnalyzer;

namespace Ui {
    class DisplayGUI;
}

class DisplayWidget : public QWidget
{
    Q_OBJECT

public:
    DisplayWidget(QWidget* parent);
    ~DisplayWidget();

    /*!
    @param sensorPresets a vector with the sensor presets
    */
    void SetSensorPresets(vector<tSensorPreset> &sensorPresets);

    public slots:
        /*! slot for receiving data of ultrasonic sensors for gui 
        @param value value of ultrasonic data
        @param usSensor type of ultrasonic sensor
        */
        void SetSensorData(int senorListId, float value);

        /*! slot for setting the dir path
        @param absoute path to the dir
        */
        void SetDirectory(QString pathname);

public:
    /*! returns the widget pointer of the combo box
    */
    QComboBox* GetDropDownWidget(){return m_dropDownFileChooser;}

private:

    /*! the main widget */
    QWidget* m_pWidget;

    /*! the main font for the widget */
    QFont* m_mainFont;

    /*! the smaller main font for the tableviews etc */
    QFont* m_mainFontSmall;

    /*! the main layout of the widget */
    QVBoxLayout* m_mainLayout;

    /*! the dropdown filechooser */
    QComboBox* m_dropDownFileChooser;

    /*! the table view for the ultrasonic sensor data */
    QTableView* m_SensorTableView;

    /*! the item model for the ultrasonic sensor data */
    QStandardItemModel *m_SensorItemModel;

    /*! the presets loaded from xml file */
    vector<tSensorPreset> m_sensorPresets;
};

#endif

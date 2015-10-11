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


#include "stdafx.h"


class cCarControl;

namespace Ui {
class DisplayGUI;
}

class DisplayWidget : public QWidget
{
    Q_OBJECT

    public:
        DisplayWidget(QWidget* parent, bool enableFallback);
        ~DisplayWidget();

        /*! sets the zero values of the speed controller and the steering
        @param speedController zero value of the speed controller
        @param steering zero value of the steering
        */
        void setZeroValues(float speedController, float steering);
		QButtonGroup* getButtons();
    public slots:

       /*! this slot receives the commands for the speed controller and writes them to the labels
        @param value the value to be send to arduino (between 0 and 180, 90 is zero position
        */
        void OnSpeedController(int value);

        /*! this slot receives the commands for the speed controller and writes them to the labels
        @param value the value to be send to arduino (between 0 and 180, 90 is zero position
        */
        void OnSteeringController(int value);

        
        /*! slot to fall back steering (disabled) and speed to zero */
        void OnFallBack();

signals:
        /*! this signal sends the commands for the speed controller
        @param value the value to be send to arduino (between 0 and 180, 90 is zero position
        */
        void SendSpeedController(float value);

        /*! this signal sends the commands for the steering controller
        @param value the value to be send to arduino (between 0 and 180, 90 is zero position
        */
        void SendSteeringController(float value);
        

    private:

        /*! initialize the widgets for the speed controller data */
        void createSpeedContrWidgets();
        
        /*! initialize the widgets for the speed controller data */
        void createSteeringWidgets();

        /* catch the key events */
        void keyPressEvent(QKeyEvent* event);

        /*! the main widget */
        //QWidget* m_pWidget;
        
        /*! the main font for the widget */
        QFont* m_mainFont;

        /*! the smaller main font for the tableviews etc */
        QFont* m_mainFontSmall;

        /*! the main layout of the widget */
        QVBoxLayout* m_mainLayout;

        /*! the item model for the wheel sensor data */
        QStandardItemModel *m_wheelItemModel;

        /*! the table view for the gyro sensor data */
        QTableView* m_speedContrTableView;

        /*! the item model for the gyro sensor data */
        QStandardItemModel *m_speedContrItemModel;
        
        /*! the slider for the speed controller commands */
        QSlider* m_slidSpeedController;

        /*! the slider for the steering controller commands */
        QSlider* m_slidSteeringController;

        /*! the table view for the gyro sensor data */
        QTableView* m_steeringTableView;

        /*! the item model for the gyro sensor data */
        QStandardItemModel *m_steeringItemModel;

        /*! the zero value of the speed controller */
        int m_SpeedControllerZeroValue;

        /*! the zero value of the steering */
        int m_SteeringZeroValue;

		/*! the button group to toggle the lights*/
		QButtonGroup m_lightButtonGroup;

        /*! the timer to fall back steering and speed to zero */
        QTimer* m_timerFallBack;



};

#endif

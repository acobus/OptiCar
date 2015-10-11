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

#include "stdafx.h"
#include "displaywidget.h"
#include "cCarControl.h"
#define KEY_EVENT_STEER_STEPS 3
#define KEY_EVENT_SPEED_STEPS 3
#define FALL_BACK_STEPS 1

DisplayWidget::DisplayWidget(QWidget* parent, bool enableFallback) :
QWidget(parent)
{
// init the ultrasound geometrics

// the push buttons to toggle the lights

	m_lightButtonGroup.addButton(new QPushButton("HeadLight"), 0);
	m_lightButtonGroup.addButton(new QPushButton("ReverseLight"), 1);
	m_lightButtonGroup.addButton(new QPushButton("BrakeLight"), 2);
	m_lightButtonGroup.addButton(new QPushButton("TurnRight"), 3);
	m_lightButtonGroup.addButton(new QPushButton("TurnLeft"), 4);
    
// initialize the main widget
    //m_pWidget = new QWidget(this);
    //m_pWidget->setSizePolicy(QSizePolicy::, QSizePolicy::Fixed);		
    //m_pWidget->setFixedSize(500,200);
	//m_pWidget->setMinimumSize(QSize(500, 300));

//initialize the fonts
    m_mainFont = new QFont("Arial",12);
    m_mainFontSmall = new QFont("Arial",10);
    setFont(*m_mainFont);

    createSteeringWidgets();

    createSpeedContrWidgets();

//create the slider for the speed controller
    QGridLayout *slidSpeedContrLay = new QGridLayout();
    slidSpeedContrLay->addWidget(m_slidSpeedController,0,0,5,1);
    slidSpeedContrLay->addWidget(new QLabel(QString("180"),this),0,1,1,1,Qt::AlignTop);  
    slidSpeedContrLay->addWidget(new QLabel(QString("90"),this),2,1,1,1,Qt::AlignCenter);
    slidSpeedContrLay->addWidget(new QLabel(QString("0"),this),4,1,1,1,Qt::AlignBottom);
    slidSpeedContrLay->addWidget(m_speedContrTableView,0,3,5,1,Qt::AlignCenter);

    //create the groupbox for the slider
    QGroupBox *grSpeedSlider = new QGroupBox(this);
    grSpeedSlider->setLayout(slidSpeedContrLay);
    grSpeedSlider->setTitle("Speed Controller");  
    grSpeedSlider->setFixedSize(180,200);
    grSpeedSlider->setFont(*m_mainFontSmall);

//create the slider for the steering controller
    QGridLayout *slidSteeringContrLay = new QGridLayout();
    slidSteeringContrLay->addWidget(m_slidSteeringController,0,0,1,5);
    slidSteeringContrLay->addWidget(new QLabel(QString("60"),this),1,0,1,1,Qt::AlignLeft);
    slidSteeringContrLay->addWidget(new QLabel(QString("90"),this),1,2,1,1,Qt::AlignCenter);
    slidSteeringContrLay->addWidget(new QLabel(QString("120"),this),1,4,1,1,Qt::AlignRight);    
    slidSteeringContrLay->addWidget(m_steeringTableView,3,0,1,5,Qt::AlignCenter);
//create the groupbox for the slider
    QGroupBox *grSteeringSlider = new QGroupBox(this);
    grSteeringSlider->setLayout(slidSteeringContrLay);
    grSteeringSlider->setTitle("Steering Controller");  
    grSteeringSlider->setFixedSize(200,180);
    grSteeringSlider->setFont(*m_mainFontSmall);
    
// create title
    QLabel *titleLable = new QLabel("AADC Control");
    titleLable->setFont(QFont("Arial",25));
    titleLable->setWordWrap(true);
    titleLable->setAlignment(Qt::AlignCenter);

    QTableWidget *commandTableWidget = new QTableWidget(5,2,this);
    commandTableWidget->setItem(0,0,new QTableWidgetItem(QString("Forward")));
    commandTableWidget->setItem(1,0,new QTableWidgetItem(QString("Backward")));
    commandTableWidget->setItem(2,0,new QTableWidgetItem(QString("Left")));
    commandTableWidget->setItem(3,0,new QTableWidgetItem(QString("Right")));
    commandTableWidget->setItem(4,0,new QTableWidgetItem(QString("Break")));    
    commandTableWidget->setItem(0,1,new QTableWidgetItem(QString("Arrow Up")));
    commandTableWidget->setItem(1,1,new QTableWidgetItem(QString("Arrow Down")));
    commandTableWidget->setItem(2,1,new QTableWidgetItem(QString("Arrow Right")));
    commandTableWidget->setItem(3,1,new QTableWidgetItem(QString("Arrow Left")));
    commandTableWidget->setItem(4,1,new QTableWidgetItem(QString("Space")));    
    commandTableWidget->setFixedSize(commandTableWidget->columnWidth(0)*2,commandTableWidget->rowHeight(0)*5);
    commandTableWidget->verticalHeader()->setVisible(false);    
    commandTableWidget->horizontalHeader()->setVisible(false);
    commandTableWidget->setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    commandTableWidget->setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOff);  
// create layout with title and manual
    QVBoxLayout *description = new QVBoxLayout();
    description->addWidget(titleLable,0,Qt::AlignCenter);
    description->addWidget(commandTableWidget,0,Qt::AlignCenter);

// create main layout of widget
    m_mainLayout = new QVBoxLayout();
	m_mainLayout->setSpacing(5);


//create controller layout for widget
	QHBoxLayout* controllerLayout = new QHBoxLayout();
    controllerLayout->addWidget(grSteeringSlider,0,Qt::AlignCenter);
    controllerLayout->addWidget(grSpeedSlider,0,Qt::AlignCenter);
    controllerLayout->addLayout(description);

//create button layout for widget
	QHBoxLayout* buttonLayout = new QHBoxLayout();
	buttonLayout->addWidget(m_lightButtonGroup.button(0));
	buttonLayout->addWidget(m_lightButtonGroup.button(1));
	buttonLayout->addWidget(m_lightButtonGroup.button(2));
	buttonLayout->addWidget(m_lightButtonGroup.button(3));
	buttonLayout->addWidget(m_lightButtonGroup.button(4));

	m_mainLayout->addLayout(controllerLayout);
	m_mainLayout->addLayout(buttonLayout);
    setLayout(m_mainLayout);	
    

// make connections
    connect(m_slidSpeedController, SIGNAL(valueChanged(int)), this, SLOT(OnSpeedController(int)));
    connect(m_slidSteeringController, SIGNAL(valueChanged(int)), this, SLOT(OnSteeringController(int)));

// init slider values
    m_slidSteeringController->setSliderPosition(90);    
    m_slidSpeedController->setSliderPosition(90);
// set the focus to catch key events
    setFocusPolicy(Qt::ClickFocus);

// start and connect the timer
    m_timerFallBack = new QTimer(this);
    if (enableFallback) m_timerFallBack->start(30);
    connect(m_timerFallBack,SIGNAL(timeout()),this,SLOT(OnFallBack()));

}

DisplayWidget::~DisplayWidget()
{
}


void DisplayWidget::createSpeedContrWidgets()
{
    m_slidSpeedController = new QSlider(Qt::Vertical, this);
    m_slidSpeedController->setRange(0,180);
    m_slidSpeedController->setTickInterval(20);
    m_slidSpeedController->setTickPosition(QSlider::TicksAbove);    
// create the table view and the item model for the speed controller 
    m_speedContrItemModel = new QStandardItemModel(1,2,this);
    m_speedContrTableView = new QTableView(this);
    m_speedContrTableView->setModel(m_speedContrItemModel);
    m_speedContrTableView->resizeRowsToContents();
    m_speedContrTableView->verticalHeader()->setVisible(false);    
    m_speedContrTableView->horizontalHeader()->setVisible(false);
    m_speedContrTableView->setColumnWidth(0,60);
    m_speedContrTableView->setColumnWidth(1,40);
    m_speedContrTableView->setFixedWidth(m_speedContrTableView->columnWidth(0)+m_speedContrTableView->columnWidth(1));
    m_speedContrTableView->setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    m_speedContrTableView->setFixedHeight(m_speedContrTableView->rowHeight(1)*2+m_speedContrTableView->horizontalHeader()->height());
    m_speedContrTableView->setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOff);  
    
    m_speedContrItemModel->setData(m_speedContrItemModel->index(0,1), QString::fromUtf8("°"));
}


void DisplayWidget::createSteeringWidgets()
{
    m_slidSteeringController = new QSlider(Qt::Horizontal, this);
    m_slidSteeringController->setRange(60,120);
    m_slidSteeringController->setTickInterval(10);
    m_slidSteeringController->setTickPosition(QSlider::TicksAbove);

// create the table view and the item model for the speed controller 
    m_steeringItemModel = new QStandardItemModel(1,2,this);
    m_steeringTableView = new QTableView(this);
    m_steeringTableView->setModel(m_steeringItemModel);
    m_steeringTableView->resizeRowsToContents();
    m_steeringTableView->verticalHeader()->setVisible(false);    
    m_steeringTableView->setColumnWidth(1,70);    
    m_steeringTableView->setColumnWidth(2,40);
    m_steeringTableView->verticalHeader()->setVisible(false);    
    m_steeringTableView->horizontalHeader()->setVisible(false);
    m_steeringTableView->setFixedWidth(m_steeringTableView->columnWidth(0)+m_steeringTableView->columnWidth(1)+m_steeringTableView->columnWidth(2));
    m_steeringTableView->setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    m_steeringTableView->setFixedHeight(m_steeringTableView->rowHeight(1)*2+m_steeringTableView->horizontalHeader()->height());
    m_steeringTableView->setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOff);  

    m_steeringItemModel->setData(m_steeringItemModel->index(0,1), QString::fromUtf8("°"));
}


void DisplayWidget::OnSpeedController(int value)
{
    m_speedContrItemModel->setData(m_speedContrItemModel->index(0,0),QString::number(value,'f',2));
    emit SendSpeedController(float(value));
}


void DisplayWidget::OnSteeringController(int value)
{
    m_steeringItemModel->setData(m_steeringItemModel->index(0,0),QString::number(value,'f',2));
    emit SendSteeringController(float(value));
}


void DisplayWidget::keyPressEvent(QKeyEvent* event)
{    
    switch (event->key())
    {
    case Qt::Key_Up:
        {
            m_slidSpeedController->setSliderPosition(m_slidSpeedController->value()-KEY_EVENT_SPEED_STEPS);
            break;
        }
    case Qt::Key_Down:
        {
            m_slidSpeedController->setSliderPosition(m_slidSpeedController->value()+KEY_EVENT_SPEED_STEPS);
            break;
        }    
    case Qt::Key_Left:
        {
            m_slidSteeringController->setSliderPosition(m_slidSteeringController->value()-KEY_EVENT_STEER_STEPS);
            break;
        }    
    case Qt::Key_Right:
        {
            m_slidSteeringController->setSliderPosition(m_slidSteeringController->value()+KEY_EVENT_STEER_STEPS);
            break;
        }
    case Qt::Key_Space:
        {
            //set only speed to zero to break
            //m_slidSteeringController->setSliderPosition(90+m_SteeringZeroValue);
            m_slidSpeedController->setSliderPosition(90+m_SpeedControllerZeroValue);
            break;
        }
    }
}

void DisplayWidget::setZeroValues(float speedController, float steering)
{
    m_SpeedControllerZeroValue = int(speedController);
    m_SteeringZeroValue = int(steering);

    m_slidSteeringController->setSliderPosition(90+m_SteeringZeroValue);
    m_slidSpeedController->setSliderPosition(90+m_SpeedControllerZeroValue);
}

void DisplayWidget::OnFallBack()
 {
      //only the speedcontroller is set back to zero automatically
     /*if (abs(m_slidSteeringController->value())!=90+m_SteeringZeroValue)
          {
          if (m_slidSteeringController->value()>90+m_SteeringZeroValue)
              m_slidSteeringController->setSliderPosition(m_slidSteeringController->value()-FALL_BACK_STEPS);
          else if (m_slidSteeringController->value()<90+m_SteeringZeroValue)
              m_slidSteeringController->setSliderPosition(m_slidSteeringController->value()+FALL_BACK_STEPS);
          }*/
      if (abs(m_slidSpeedController->value())!=90+m_SpeedControllerZeroValue)
          {
          if (m_slidSpeedController->value()>90+m_SpeedControllerZeroValue)
              m_slidSpeedController->setSliderPosition(m_slidSpeedController->value()-FALL_BACK_STEPS);
          else if (m_slidSpeedController->value()<90+m_SpeedControllerZeroValue)
              m_slidSpeedController->setSliderPosition(m_slidSpeedController->value()+FALL_BACK_STEPS);
          }
 }


QButtonGroup* DisplayWidget::getButtons()
{
	return &m_lightButtonGroup;
}
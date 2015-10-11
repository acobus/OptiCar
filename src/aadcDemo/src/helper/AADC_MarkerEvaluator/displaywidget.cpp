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
* $Author:: spiesra $  $Date:: 2015-05-13 08:29:07#$ $Rev:: 35003   $
**********************************************************************/

#include "stdafx.h"
#include "aadc_roadSign_enums.h"
#include "cMarkerEvaluatorFilter.h"
#include "displaywidget.h"
#include "display.h"

DisplayWidget::DisplayWidget(QWidget* parent) : QWidget(parent), ui(new Ui::DisplayWidget)
{
    ui->setupUi(this);

    //load all image files from resource.qrc
    for(int i = 0; i < NUM_ROADSIGNS; i++)
        m_roadSigns[i] = new QImage();
    m_roadSigns[getListIndexOfSign(MARKER_ID_NOMATCH)]->load(":/resource/NoMatch.png");
    m_roadSigns[getListIndexOfSign(MARKER_ID_GIVEWAY)]->load(":/resource/Vorfahrt.png");
    m_roadSigns[getListIndexOfSign(MARKER_ID_STOPANDGIVEWAY)]->load(":/resource/Stop.png");
    m_roadSigns[getListIndexOfSign(MARKER_ID_HAVEWAY)]->load(":/resource/VorfahrtRichtung.png");
    m_roadSigns[getListIndexOfSign(MARKER_ID_AHEADONLY)]->load(":/resource/GeradeAus.png");
    m_roadSigns[getListIndexOfSign(MARKER_ID_PARKINGAREA)]->load(":/resource/Parken.png");
    m_roadSigns[getListIndexOfSign(MARKER_ID_UNMARKEDINTERSECTION)]->load(":/resource/Kreuzung.png");
    m_roadSigns[getListIndexOfSign(MARKER_ID_PEDESTRIANCROSSING)]->load(":/resource/Fussgaengerueberweg.png");
    m_roadSigns[getListIndexOfSign(MARKER_ID_ROUNDABOUT)]->load(":/resource/Kreisverkehr.png");
    m_roadSigns[getListIndexOfSign(MARKER_ID_NOOVERTAKING)]->load(":/resource/Ueberholverbot.png");
    m_roadSigns[getListIndexOfSign(MARKER_ID_NOENTRYVEHICULARTRAFFIC)]->load(":/resource/Einfahrtverboten.png");
    m_roadSigns[getListIndexOfSign(MARKER_ID_ONEWAYSTREET)]->load(":/resource/Einbahnstrasse.png");
    //add one graph
    m_graph_vorfahrtgew = ui->customPlot->addGraph();
    m_graph_vorfahrt = ui->customPlot->addGraph();
    m_graph_stop = ui->customPlot->addGraph();
    m_graph_parken = ui->customPlot->addGraph();
    m_graph_geradeaus = ui->customPlot->addGraph();
    m_graph_kreuzung = ui->customPlot->addGraph();
    m_graph_fussgaenger= ui->customPlot->addGraph();;    
    m_graph_kreisverkehr= ui->customPlot->addGraph();;    
    m_graph_ueberholverbot= ui->customPlot->addGraph();;    
    m_graph_verboteinfahrt= ui->customPlot->addGraph();;    
    m_graph_einbahnstrasse= ui->customPlot->addGraph();;
    //set sizepolicy
    ui->image->setSizePolicy(QSizePolicy::Ignored, QSizePolicy::Ignored);
    ui->image->setScaledContents(true);

    setupGraph();
    setRoadSign(getListIndexOfSign(MARKER_ID_NOMATCH));
}

DisplayWidget::~DisplayWidget()
{
    for(int i = 0; i < NUM_ROADSIGNS; i++)
        delete m_roadSigns[i];
    delete ui;
}

void DisplayWidget::setRoadSign(int sign)
{
    ui->image->setPixmap(QPixmap::fromImage(*m_roadSigns[sign]));
}

void DisplayWidget::pushData(tInt16 data)
{
    m_identiferBuffer.push_back(data);
}


void DisplayWidget::setupGraph()
{
    ui->customPlot->legend->setVisible(true);

    //graph vorfahrtgew
    QPen pen_vorfahrtgew;
    pen_vorfahrtgew.setColor(Qt::red);
    m_graph_vorfahrtgew->setPen(pen_vorfahrtgew);
    m_graph_vorfahrtgew->setName("Give Way");
    //graph vorfahrt
    QPen pen_vorfahrt;
    pen_vorfahrt.setColor(Qt::darkGray);
    m_graph_vorfahrt->setPen(pen_vorfahrt);
    m_graph_vorfahrt->setName("Have Way");
    //graph stop
    QPen pen_stop;
    pen_stop.setColor(Qt::green);
    m_graph_stop->setPen(pen_stop);
    m_graph_stop->setName("Stop");
    //graph parken
    QPen pen_parken;
    pen_parken.setColor(Qt::black);
    m_graph_parken->setPen(pen_parken);
    m_graph_parken->setName("Parking Area");
    //graph_geradeaus
    QPen pen_geradeaus;
    pen_geradeaus.setColor(Qt::blue);
    m_graph_geradeaus->setPen(pen_geradeaus);
    m_graph_geradeaus->setName("Ahead Only");
    //graph_kreuzung
    QPen pen_kreuzung;
    pen_kreuzung.setColor(Qt::cyan);
    m_graph_kreuzung->setPen(pen_kreuzung);
    m_graph_kreuzung->setName("Intersection");
    //m_graph_fussgaenger
    QPen pen_fussgaenger;
    pen_fussgaenger.setColor(Qt::magenta);
    m_graph_fussgaenger->setPen(pen_fussgaenger);
    m_graph_fussgaenger->setName("Pedestrian");
    //m_graph_kreisverkehr
    QPen pen_kreisverkehr;
    pen_kreisverkehr.setColor(Qt::darkGreen);
    m_graph_kreisverkehr->setPen(pen_kreisverkehr);
    m_graph_kreisverkehr->setName("Round About");
    //m_graph_uebeholverbot
    QPen pen_ueberholverbot;
    pen_ueberholverbot.setColor(Qt::darkBlue);
    m_graph_ueberholverbot->setPen(pen_ueberholverbot);
    m_graph_ueberholverbot->setName("No Overtaking");
    //m_graph_verboteinfahrt
    QPen pen_verboteinfahrt;
    pen_kreuzung.setColor(Qt::darkYellow);
    m_graph_verboteinfahrt->setPen(pen_verboteinfahrt);
    m_graph_verboteinfahrt->setName("No Entry");
    //m_graph_einbahnstrasse
    QPen pen_einbahnstrasse;
    pen_einbahnstrasse.setColor(Qt::lightGray);
    m_graph_einbahnstrasse->setPen(pen_einbahnstrasse);
    m_graph_einbahnstrasse->setName("One Way Street");
        
    m_graph_vorfahrtgew->setAntialiasedFill(false);
    m_graph_vorfahrt->setAntialiasedFill(false);
    m_graph_stop->setAntialiasedFill(false);
    m_graph_parken->setAntialiasedFill(false);
    m_graph_geradeaus->setAntialiasedFill(false);    
    m_graph_kreuzung->setAntialiasedFill(false);
    m_graph_fussgaenger->setAntialiasedFill(false);
    m_graph_kreisverkehr->setAntialiasedFill(false);
    m_graph_ueberholverbot->setAntialiasedFill(false);
    m_graph_verboteinfahrt->setAntialiasedFill(false);    
    m_graph_einbahnstrasse->setAntialiasedFill(false);
    
    ui->customPlot->xAxis->setTickLabelType(QCPAxis::ltDateTime);
    ui->customPlot->xAxis->setDateTimeFormat("hh:mm:ss");
    ui->customPlot->xAxis->setAutoTickStep(false);
    ui->customPlot->xAxis->setTickStep(2);
    ui->customPlot->yAxis->setRange(-1, RANGE_DATASET+2);

    // setup a timer that repeatedly calls realtimeData:
    connect(&dataTimer, SIGNAL(timeout()), this, SLOT(realTimeData()));
    dataTimer.start(0); // Interval 0 means to refresh as fast as possible
}

void DisplayWidget::realTimeData()
{
    double time = QDateTime::currentDateTime().toMSecsSinceEpoch()/1000.0;
    
    //as fast as possible calc the data
    calcData();
            
    // remove data of lines that's outside visible range:
    m_graph_vorfahrtgew->removeDataBefore(time - 12);
    m_graph_vorfahrt->removeDataBefore(time - 12);
    m_graph_stop->removeDataBefore(time - 12);
    m_graph_parken->removeDataBefore(time - 12);
    m_graph_geradeaus->removeDataBefore(time - 12);    
    m_graph_kreuzung->removeDataBefore(time - 12);
    m_graph_fussgaenger->removeDataBefore(time - 12);;
    m_graph_kreisverkehr->removeDataBefore(time - 12);
    m_graph_ueberholverbot->removeDataBefore(time - 12);
    m_graph_verboteinfahrt->removeDataBefore(time - 12);    
    m_graph_einbahnstrasse->removeDataBefore(time - 12);
    // make key axis range scroll with the data (at a constant range size of 8):
    ui->customPlot->xAxis->setRange(time + 0.25, 8, Qt::AlignRight);
    ui->customPlot->replot();
}

void DisplayWidget::calcData()
{
    tInt16 data;
    quint64 time = QDateTime::currentDateTime().toMSecsSinceEpoch()/1000.0;

    short int num_list[NUM_ROADSIGNS] = {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0};

    if(m_identiferBuffer.size() > RANGE_DATASET)
    {
        for(int i = 0; i < RANGE_DATASET; i++)
        {
            data = m_identiferBuffer.front();
            m_identiferBuffer.pop_front();

            num_list[getListIndexOfSign(data)]++;
            
        }

        int tmp = 0, tmpIndex = 0; 
        //get the max value
        for (int i = 0; i < NUM_ROADSIGNS; i++)
        {
            if(num_list[i] > tmp)
            {
                tmp = num_list[i];
                tmpIndex = i;
            }
        }

        setRoadSign(tmpIndex);
    
        m_graph_vorfahrtgew->addData(time, num_list[getListIndexOfSign(MARKER_ID_GIVEWAY)]);
        m_graph_vorfahrt->addData(time, num_list[getListIndexOfSign(MARKER_ID_HAVEWAY)]);
        m_graph_stop->addData(time, num_list[getListIndexOfSign(MARKER_ID_STOPANDGIVEWAY)]);
        m_graph_parken->addData(time, num_list[getListIndexOfSign(MARKER_ID_PARKINGAREA)]);
        m_graph_geradeaus->addData(time, num_list[getListIndexOfSign(MARKER_ID_AHEADONLY)]);        
        m_graph_kreuzung->addData(time, num_list[getListIndexOfSign(MARKER_ID_UNMARKEDINTERSECTION)]);
        m_graph_fussgaenger->addData(time, num_list[getListIndexOfSign(MARKER_ID_PEDESTRIANCROSSING)]);
        m_graph_kreisverkehr->addData(time, num_list[getListIndexOfSign(MARKER_ID_ROUNDABOUT)]);
        m_graph_ueberholverbot->addData(time, num_list[getListIndexOfSign(MARKER_ID_NOOVERTAKING)]);
        m_graph_verboteinfahrt->addData(time, num_list[getListIndexOfSign(MARKER_ID_NOENTRYVEHICULARTRAFFIC)]);    
        m_graph_einbahnstrasse->addData(time, num_list[getListIndexOfSign(MARKER_ID_ONEWAYSTREET)]);
    }
}


tInt16 DisplayWidget::getListIndexOfSign(tInt16 signId)
{
    switch(signId)
            {
            case MARKER_ID_NOMATCH:
                return 0;
                break;
            case MARKER_ID_GIVEWAY:
                return 1;
                break;
            case MARKER_ID_HAVEWAY:
                return 2;
                break;
            case MARKER_ID_STOPANDGIVEWAY:
                return 3;
                break;
            case MARKER_ID_PARKINGAREA:
                return 4;
                break;
            case MARKER_ID_AHEADONLY:
                return 5;
                break;
            case MARKER_ID_UNMARKEDINTERSECTION:
                return 6;
                break;
            case MARKER_ID_PEDESTRIANCROSSING:
                return 7;
                break;
            case MARKER_ID_ROUNDABOUT:
                return 8;
                break;
            case MARKER_ID_NOOVERTAKING:
                return 9;
                break;
            case MARKER_ID_NOENTRYVEHICULARTRAFFIC:
                return 10;
                break;
            case MARKER_ID_ONEWAYSTREET:
                return 11;
                break;
            default:
                return 0;
                break;
            }
}
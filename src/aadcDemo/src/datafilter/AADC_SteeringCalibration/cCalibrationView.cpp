/**
Copyright (c) 
Audi Autonomous Driving Cup. All rights reserved.
 
Redistribution and use in source and binary forms, with or without modification, are permitted provided that the following conditions are met:
 
1. Redistributions of source code must retain the above copyright notice, this list of conditions and the following disclaimer.
2. Redistributions in binary form must reproduce the above copyright notice, this list of conditions and the following disclaimer in the documentation and/or other materials provided with the distribution.
3.  All advertising materials mentioning features or use of this software must display the following acknowledgement: �This product includes software developed by the Audi AG and its contributors for Audi Autonomous Driving Cup.�
4.  Neither the name of Audi nor the names of its contributors may be used to endorse or promote products derived from this software without specific prior written permission.
 
THIS SOFTWARE IS PROVIDED BY AUDI AG AND CONTRIBUTORS �AS IS� AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL AUDI AG OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.


**********************************************************************
* $Author:: spiesra $  $Date:: 2015-09-28 17:09:16#$ $Rev:: 39510   $
**********************************************************************/
#include "stdafx.h"
#include "cCalibrationView.h"

cCalibrationView::cCalibrationView(QStandardItemModel *model,QWidget* parent) : QWidget(parent)
{ 

    m_pMainLayout = new QVBoxLayout (this);
	m_storeCalibrationXMLButton = new QPushButton("Store Calibration in XML File");
	m_tableView = new QTableView(this);

	m_model = model;

	m_tableView->setModel(model);

	//mapping needed to recv Signals from PushButton
	QSignalMapper *sigMap = new QSignalMapper(this);

	for(int i = 0; i < model->rowCount(); i++){
		QPushButton *pb = new QPushButton( QString("Go"),this);
		connect(pb, SIGNAL(clicked()), sigMap, SLOT(map()));
		sigMap->setMapping(pb, i);
		//add PushButton to TableView
		m_tableView->setIndexWidget(model->index(i,model->columnCount() -1), pb);
	}

	//map PushButtons to onPushButtonClicked()
	connect(sigMap, SIGNAL(mapped(int)), this, SLOT(onPushButtonClicked(int)));
	connect(m_storeCalibrationXMLButton, SIGNAL(clicked()),this,SLOT(onStoreXMLButtonClicked()));
	

	m_pMainLayout->addWidget(m_tableView);
	m_pMainLayout->addWidget(m_storeCalibrationXMLButton);
	setLayout(m_pMainLayout);
}

void cCalibrationView::onStoreXMLButtonClicked(){
	cString filename = QFileDialog::getSaveFileName().toUtf8().data();
}


void cCalibrationView::onPushButtonClicked(int index)
{
	m_model->data(m_model->index(index,0)).toFloat();
	emit startDrive(index);
}


cCalibrationView::~cCalibrationView(){

};

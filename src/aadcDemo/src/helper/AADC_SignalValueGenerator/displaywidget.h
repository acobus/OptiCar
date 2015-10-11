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

#ifndef DISPWIDGET_SIGNALGENERATOR_FILTER
#define DISPWIDGET_SIGNALGENERATOR_FILTER


#include "stdafx.h"

/*!
This class it the QWidget for the prototyp of the driver filter
*/
class DisplayWidget: public QWidget
{
    Q_OBJECT

    public:
        /*! constructor for the widget
        @param parent the parent widget
        */
        DisplayWidget(QString filename, QWidget* parent);
        ~DisplayWidget() {};

        /*! the button to send the response to the ready request in the given the section */
        QPushButton *m_btSendValue;
        
        /*! the button to save the file */
        QPushButton *m_btSaveToFile;

        /*! the button to save the file */
        QPushButton *m_btLoadDefaultFile;

        /*! the table view for the signal values data */
        QTableView* m_TableView;

        /*! the item model for the signal values data */
        QStandardItemModel *m_ItemModel;

        /*! the string for the file name with the default values*/
        QString m_defaultFileName;
        

signals:

        /*! signal for committing and starting the list
        */
        void sendCommit();


        /*! signal for sending the tuples to filter
        @param the timestamp in milliseconds
        @param the value 1
        @param the value 2
        */
        void sendTuple(int i8StateFlag, float flTime, float flValue1, float flValue2);
public slots:
        
        /*! called when the button is clicked */
        void OnButtonClicked();


       /*! called when the button Save is clicked */
       void OnButtonSaveClicked();

       /*! called when the button load defaults is clicked */
       void OnButtonLoadDefaultClicked();

    private:

        /*! the main widget */
        QWidget* m_pWidget;        
        
        /*! the main layout for the widget*/
        QVBoxLayout *m_mainLayout;

        /*! creates the table view*/
        void createTableModel();
};

#endif

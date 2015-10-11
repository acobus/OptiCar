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
* $Author:: spiesra $  $Date:: 2015-05-13 08:29:07#$ $Rev:: 35003   $
**********************************************************************/


#ifndef _BOOL_VALUE_HEADER
#define _BOOL_VALUE_HEADER

#define __guid "adtf.aadc.aadc_boolValueGenerator"

#include "stdafx.h"
#include "displaywidget.h"


/*!
With this small helper simple Media Samples of the tBoolSignalValue can be generated. When started a GUI with two buttons is shown. When clicking on �Send Value FALSE� a Media Sample with �False� in the media description element bValue is transmitted, when clicking on �Send Value TRUE� a Media Sample with �True� is transmitted.
*/
class cBoolValueGenerator : public QObject, public cBaseQtFilter
{
    ADTF_DECLARE_FILTER_VERSION(__guid, "AADC Bool Value Generator", OBJCAT_Auxiliary, "Bool Value Generator Filter", 1, 0, 0, "BFFT GmbH");    

    Q_OBJECT
    
signals:


public: // construction
    cBoolValueGenerator(const tChar *);
    virtual ~cBoolValueGenerator();

    /*! overrides cFilter */
    virtual tResult Init(tInitStage eStage, __exception = NULL);
    
    /*! overrides cFilter */
    virtual tResult Start(__exception = NULL);
    
    /*! overrides cFilter */
    virtual tResult Stop(__exception = NULL);
    
    /*! overrides cFilter */
    virtual tResult Shutdown(tInitStage eStage, __exception = NULL);
   

    /*! overrides cFilter */
    tResult Run(tInt nActivationCode, const tVoid* pvUserData, tInt szUserDataSize, ucom::IException** __exception_ptr=NULL);


protected: // Implement cBaseQtFilter

    /*! Creates the widget instance*/
    tHandle CreateView();

    /*! Destroys the widget instance*/
    tResult ReleaseView();
    

private:

    /*! The displayed widget*/    
    DisplayWidget *m_pWidget;

    /*! Coder Descriptor */
    cObjectPtr<IMediaTypeDescription> m_pDescriptionBool;
    /*! the id for the bool value output of the media description */
    tBufferID m_szIDBoolValueOutput;     
    /*! the id for the arduino timestamp output of the media description */
    tBufferID m_szIDArduinoTimestampOutput; 
    /*! indicates if bufferIDs were set */
    tBool m_bIDsBoolValueOutput;
    
    /*! input pin for the run command*/
    cOutputPin        m_oBoolValuePin;

    
public slots:
    /*! transmits a new mediasample with value false */
    void OnTransmitValueFalse();
    
    /*! transmits a new mediasample with value true */
    void OnTransmitValueTrue();



};

#endif

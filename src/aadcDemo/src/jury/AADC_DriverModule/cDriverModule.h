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


#ifndef _DRIVERMODULE_HEADER
#define _DRIVERMODULE_HEADER

#define __guid "adtf.aadc.driverModule"

#include "stdafx.h"

#include "juryEnums.h"

#include "displaywidget.h"



/*! /brief cDriverModule
This filter was developed as a prototype to explain the interaction with the Jury Module.
It receives the structs from the Jury module and shows them in the middle of the gui. The user can respond the the received messages with the four buttons “Ready to Start”, “Running”, “Error” and “Complete”. 
After clicking on the button the sample is transmitted on the output pin Driver_Struct an contains the following struct:
typedef struct
{
tInt8 i8StateID;
tInt16 i16ManeuverEntry;
} tDriverStruct;

The possible i8StateID are:
•	stateCar_Error: This is sent if some error occurred on the car.
•	stateCar_Ready: If the car is ready to start a maneuver ID this state is sent including the maneuver ID in i16ManeuverEntry.
•	stateCar_Running: Sent during running the maneuver contained in i16ManeuverEntry 
•	stateCar_Complete: Sent if the car finished the whole maneuver list.
•	stateCar_Startup: Sent at the initial phase to indicate that car is working properly

The struct tDriverStruct is defined in aadc_structs.h in src\aadcBase\include and the used enums are defined in juryEnums.h in src\aadcBase\include

The teams must not implement any filter containing a Qt GUI because there is no opportunity to control the car with a GUI in the competition. The only way to interact with the car is through the jury module which is controlled by the jury.

*/
class cDriverModule : public QObject, public cBaseQtFilter
{
    ADTF_DECLARE_FILTER_VERSION(__guid, "AADC Driver Module", OBJCAT_Tool, "Driver Module", 1, 1, 0, "BFFT");    

    Q_OBJECT

signals:


public: // construction
    cDriverModule(const tChar *);
    virtual ~cDriverModule();

    /*! overrides cFilter */
    virtual tResult Init(tInitStage eStage, __exception = NULL);

    /*! overrides cFilter */
    virtual tResult Start(__exception = NULL);

    /*! overrides cFilter */
    virtual tResult Stop(__exception = NULL);

    /*! overrides cFilter */
    virtual tResult Shutdown(tInitStage eStage, __exception = NULL);

    /*! overrides cFilter */
    tResult OnPinEvent(IPin* pSource, tInt nEventCode, tInt nParam1, tInt nParam2, IMediaSample* pMediaSample);

    /*! overrides cFilter */
    tResult Run(tInt nActivationCode, const tVoid* pvUserData, tInt szUserDataSize, ucom::IException** __exception_ptr=NULL);


protected: // Implement cBaseQtFilter

    /*! Creates the widget instance*/
    tHandle CreateView();

    /*! Destroys the widget instance*/
    tResult ReleaseView();

public slots:    
    /*! function which transmits the state      
    @param i8StateID state to be sent; -1: error, 0: Ready, 1: Running
    @param i16ManeuverEntry current entry to be sent
    */
    tResult OnSendState(stateCar stateID, tInt16 i16ManeuverEntry);
    /*! this functions loads the maneuver list given in the properties*/
    tResult LoadManeuverList();    

signals:
    /*! signal to the gui to show the command "run" from the jury
    @param entryId current entry to be sent 
    */
    void SendRun(int entryId); 

    /*! signal to the gui to show the command "stop" from the jury 
    @param entryId current entry to be sent 
    */
    void SendStop(int entryId);

    /*! signal to the gui to show the command "request ready" from the jury 
    @param entryId current entry to be sent 
    */
    void SendRequestReady(int entryId);

    void TriggerLoadManeuverList();
private:

    /*! The displayed widget*/    
    DisplayWidgetDriver *m_pWidget;

    /*! Coder Descriptor */
    cObjectPtr<IMediaTypeDescription> m_pDescJuryStruct;
    /*! the id for the i8StateID of the media description */
    tBufferID m_szIDJuryStructI8ActionID; 
    /*! the id for the i16ManeuverEntry of the media description data */
    tBufferID m_szIDJuryStructI16ManeuverEntry;
    /*! indicates if bufferIDs were set */
    tBool m_bIDsJuryStructSet;

    /*! Coder Descriptor */
    cObjectPtr<IMediaTypeDescription> m_pDescDriverStruct;
    /*! the id for the i8StateID of the media description */
    tBufferID m_szIDDriverStructI8StateID; 
    /*! the id for the i16ManeuverEntry of the media description data */
    tBufferID m_szIDDriverStructI16ManeuverEntry;
    /*! indicates if bufferIDs were set */
    tBool m_bIDsDriverStructSet;

    /* Coder description */
    cObjectPtr<IMediaTypeDescription> m_pDescManeuverList;


    /*! input pin for the run command*/
    cInputPin        m_JuryStructInputPin;
    /*! input pin for the maneuver list*/
    cInputPin        m_ManeuverListInputPin;
    /*! output pin for state from driver*/
    cOutputPin        m_DriverStructOutputPin;

    /*! whether output to console is enabled or not*/
    tBool m_bDebugModeEnabled;

    cString     m_strManeuverFileString;
    /*! this is the filename of the maneuver list*/
    cFilename m_maneuverListFile;
    /*! this is the list with all the loaded sections from the maneuver list*/
    std::vector<tSector> m_sectorList;

};

#endif

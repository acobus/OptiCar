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

/*! \brief cCarControl
*  With this filter the basic functions of the car can be controlled. You can set the steering and the speed controller with the two bars or use the keys described in the window. To use the keys the focus has to be set on this window by clicking in the window or the title bar. With the buttons at the bottom the lights of the car can be switched on or off.
*/

#ifndef _AADC_CARCONTROL
#define _AADC_CARCONTROL

#define OID_ADTF_CARCONTROL "adtf.aadc.aadc_carControl"

#include "stdafx.h"
#include "displaywidget.h"

class cCarControl : public QObject, public cBaseQtFilter
{
    ADTF_DECLARE_FILTER_VERSION(OID_ADTF_CARCONTROL, "AADC Car Control", OBJCAT_Application, "Car Control", 1, 0, 0, "BFFT GmbH");    

    Q_OBJECT

public: // construction
    cCarControl(const tChar *);
    virtual ~cCarControl();

    // overrides cFilter
    virtual tResult Init(tInitStage eStage, __exception = NULL);
    virtual tResult Start(__exception = NULL);
    virtual tResult Stop(__exception = NULL);
    virtual tResult Shutdown(tInitStage eStage, __exception = NULL);

    /*!Handles all the input from the arduino*/
    tResult OnPinEvent(IPin* pSource, tInt nEventCode, tInt nParam1, tInt nParam2, IMediaSample* pMediaSample);

    /*! Thread that sends a timed alive signal to the car*/
    tResult Run(tInt nActivationCode, const tVoid* pvUserData, tInt szUserDataSize, ucom::IException** __exception_ptr=NULL);


protected: // Implement cBaseQtFilter

    /*! Creates the widget instance*/
    tHandle CreateView();

    /*! Destroys the widget instance*/
    tResult ReleaseView();

    /*! The displayed widget*/
    DisplayWidget *m_pWidget;

private:

    /*! helper function to toggle a bool signal via the specified pin
    @param the destination pin to toggle
    */
    tResult TransmitBoolValue(cOutputPin* pin, tBool value);

    /*! the output pin to send the value for speed controller */
    cOutputPin     m_oOutputSpeedController;

    /*! the output pin to send the value for steering controller */
    cOutputPin     m_oOutputSteeringController;

    /*! the output pin to toggle the head light*/
    cOutputPin     m_oOutputHeadLight;

    /*! the output pin to toggle the reverse light*/
    cOutputPin     m_oOutputReverseLight;

    /*! the output pin to toggle the brake light*/
    cOutputPin     m_oOutputBrakeLight;

    /*! the output pin to toggle the turn right light*/
    cOutputPin     m_oOutputTurnRight;

    /*! the output pin to toggle the turn left light*/
    cOutputPin     m_oOutputTurnLeft;

    /*! Coder Descriptor */
    cObjectPtr<IMediaTypeDescription> m_pDescriptionBool;

    /*! the id for the bool value output of the media description */
    tBufferID m_szIDBoolValueOutput;     

    /*! the id for the arduino timestamp output of the media description */
    tBufferID m_szIDArduinoTimestampOutput; 

    /*! indicates if bufferIDs were set */
    tBool m_bIDsBoolValueOutput;

    /*! descriptor for actuator values data */        
    cObjectPtr<IMediaTypeDescription> m_pDescriptionActuatorOutput; 

    /*! the id for the f32value of the media description for the actuator output pins */
    tBufferID m_szIDActuatorOutputF32Value; 

    /*! the id for the arduino time stamp of the media description for the actuator output input pins */
    tBufferID m_szIDActuatorOutputArduinoTimestamp;         

    /*! indicates if bufferIDs were set */
    tBool m_bIDsActuatorOutputSet;

    /*! holds the current value for the speed controller (zero position is 90) */
    tFloat32 m_f32CurrentSpeedControllerValue;

    /*! holds the current value for the steering controller (zero position is 90) */
    tFloat32 m_f32CurrentSteeringControllerValue;

    /*! handle for timer for sending actuator values*/
    tHandle m_hTimerActuator;

    public slots:

        /*! this slot receives the commands for the speed controller and writes them to class members
        @param value the value to be send to arduino (between 0 and 180, 90 is zero position
        */
        void OnSpeedController(float fValue);

        /*! this slot receives the commands for the speed controller and writes them to class members
        @param value the value to be send to arduino (between 0 and 180, 90 is zero position
        */
        void OnSteeringController(float fValue);

        /*! this slot toggles the lights 
        @param id of the clicked button 0-4, head, reverse, brake, turn right, turn left
        */
        void ToggleLights(int id);

};

#endif

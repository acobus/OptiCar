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
#include "cCarControl.h"


ADTF_FILTER_PLUGIN("AADC Car Control", OID_ADTF_CARCONTROL, cCarControl);


cCarControl::cCarControl(const tChar* __info) : 
QObject(),
    cBaseQtFilter(__info),
    m_bIDsBoolValueOutput(false),
    m_f32CurrentSpeedControllerValue(90),
    m_f32CurrentSteeringControllerValue(90)
{
    // create the properties of this filter

    SetPropertyInt("Actuator Update Rate [Hz]",30);
    SetPropertyStr("Actuator Update Rate [Hz]" NSSUBPROP_DESCRIPTION, "defines how much updates for steering and speed controller are sent in one second (Range: 0 to 100 Hz)"); 
    SetPropertyInt("Actuator Update Rate [Hz]" NSSUBPROP_MIN, 0); 
    SetPropertyInt("Actuator Update Rate [Hz]" NSSUBPROP_MAX, 100);

    SetPropertyInt("Actuator Startup Time Delay [sec]",5);
    SetPropertyStr("Actuator Startup Time Delay [sec]" NSSUBPROP_DESCRIPTION, "defines at what delay the first actuator value should be send to arduino (Range: 0 to 10 sec)"); 
    SetPropertyInt("Actuator Startup Time Delay [sec]" NSSUBPROP_MIN, 0); 
    SetPropertyInt("Actuator Startup Time Delay [sec]" NSSUBPROP_MAX, 10); 

    SetPropertyFloat("Zero Position Steering Controller [deg]",0);
    SetPropertyStr("Zero Position Steering Controller [deg]" NSSUBPROP_DESCRIPTION, "defines the offset for the zero position of the steering controller (Range: -10° to 10°)"); 
    SetPropertyFloat("Zero Position Steering Controller [deg]" NSSUBPROP_MIN, -10); 
    SetPropertyFloat("Zero Position Steering Controller [deg]" NSSUBPROP_MAX, 10);     
    SetPropertyBool("Zero Position Steering Controller [deg]" NSSUBPROP_ISCHANGEABLE, tTrue); 

    SetPropertyFloat("Zero Position Speed Controller [deg]",0);
    SetPropertyStr("Zero Position Speed Controller [deg]" NSSUBPROP_DESCRIPTION, "defines the offset for the zero position of the speed controller (Range: -10° to 10°)"); 
    SetPropertyFloat("Zero Position Speed Controller [deg]" NSSUBPROP_MIN, -10); 
    SetPropertyFloat("Zero Position Speed Controller [deg]" NSSUBPROP_MAX, 10); 
    SetPropertyBool("Zero Position Speed Controller [deg]" NSSUBPROP_ISCHANGEABLE, tTrue); 

    SetPropertyBool("Enable Speed Controller Fallback",tTrue);
    SetPropertyStr("Enable Speed Controller Fallback" NSSUBPROP_DESCRIPTION, "When enabled the speed controller value falls back to zero automatically"); 

}

cCarControl::~cCarControl()
{
}

tHandle cCarControl::CreateView()
{
    QWidget* pWidget = (QWidget*)m_pViewport->VP_GetWindow();
    m_pWidget = new DisplayWidget(pWidget,GetPropertyBool("Enable Speed Controller Fallback"));    

    // making the connections between signals and slots
    connect(m_pWidget->getButtons(), SIGNAL(buttonClicked(int)), this, SLOT(ToggleLights(int)));
    connect(m_pWidget,SIGNAL(SendSpeedController(float)),this,SLOT(OnSpeedController(float)));
    connect(m_pWidget,SIGNAL(SendSteeringController(float)),this,SLOT(OnSteeringController(float)));
    return (tHandle)m_pWidget;
}

tResult cCarControl::ReleaseView()
{
    // delete the widget if exits
    if (m_pWidget != NULL)
    {
        delete m_pWidget;
        m_pWidget = NULL;
    }
    RETURN_NOERROR;
}

tResult cCarControl::Init(tInitStage eStage, __exception)
{
    RETURN_IF_FAILED(cBaseQtFilter::Init(eStage, __exception_ptr));

    if (eStage == StageFirst)
    {

        //get the description manager for this filter
        cObjectPtr<IMediaDescriptionManager> pDescManager;
        RETURN_IF_FAILED(_runtime->GetObject(OID_ADTF_MEDIA_DESCRIPTION_MANAGER,IID_ADTF_MEDIA_DESCRIPTION_MANAGER,(tVoid**)&pDescManager,__exception_ptr));

        //get description for sensor data pins
        tChar const * strDescSignalValue = pDescManager->GetMediaDescription("tSignalValue");	
        RETURN_IF_POINTER_NULL(strDescSignalValue);	
        //get mediatype for ultrasonic sensor data pins
        cObjectPtr<IMediaType> pTypeSignalValue = new cMediaType(0, 0, 0, "tSignalValue", strDescSignalValue, IMediaDescription::MDF_DDL_DEFAULT_VERSION);

        //get description for bool light sensors
        tChar const * strDescBoolSignalValue = pDescManager->GetMediaDescription("tBoolSignalValue");	
        RETURN_IF_POINTER_NULL(strDescSignalValue);	
        //get mediatype for ultrasonic sensor data pins
        cObjectPtr<IMediaType> pTypeBoolSignalValue = new cMediaType(0, 0, 0, "tBoolSignalValue", strDescBoolSignalValue, IMediaDescription::MDF_DDL_DEFAULT_VERSION);


        //get mediatype description for output data type
        RETURN_IF_FAILED(pTypeSignalValue->GetInterface(IID_ADTF_MEDIA_TYPE_DESCRIPTION, (tVoid**)&m_pDescriptionActuatorOutput));
        RETURN_IF_FAILED(pTypeBoolSignalValue->GetInterface(IID_ADTF_MEDIA_TYPE_DESCRIPTION, (tVoid**)&m_pDescriptionBool));

        //create pin for speed output data
        RETURN_IF_FAILED(m_oOutputSpeedController.Create("SpeedController", pTypeSignalValue, static_cast<IPinEventSink*> (this)));	
        RETURN_IF_FAILED(RegisterPin(&m_oOutputSpeedController));

        //create pin for steering output data
        RETURN_IF_FAILED(m_oOutputSteeringController.Create("SteeringController", pTypeSignalValue, static_cast<IPinEventSink*> (this)));	
        RETURN_IF_FAILED(RegisterPin(&m_oOutputSteeringController));

        //create pin for headlight bool data
        RETURN_IF_FAILED(m_oOutputHeadLight.Create("headLightEnabled", pTypeBoolSignalValue, static_cast<IPinEventSink*> (this)));	
        RETURN_IF_FAILED(RegisterPin(&m_oOutputHeadLight));

        //create pin for reverse bool data
        RETURN_IF_FAILED(m_oOutputReverseLight.Create("reverseLightEnabled", pTypeBoolSignalValue, static_cast<IPinEventSink*> (this)));	
        RETURN_IF_FAILED(RegisterPin(&m_oOutputReverseLight));

        //create pin for brake output data
        RETURN_IF_FAILED(m_oOutputBrakeLight.Create("brakeLightEnabled", pTypeBoolSignalValue, static_cast<IPinEventSink*> (this)));	
        RETURN_IF_FAILED(RegisterPin(&m_oOutputBrakeLight));

        //create pin for turn right output data
        RETURN_IF_FAILED(m_oOutputTurnRight.Create("turnSignalRightEnabled", pTypeBoolSignalValue, static_cast<IPinEventSink*> (this)));	
        RETURN_IF_FAILED(RegisterPin(&m_oOutputTurnRight));

        //create pin for turn left output data
        RETURN_IF_FAILED(m_oOutputTurnLeft.Create("turnSignalLeftEnabled", pTypeBoolSignalValue, static_cast<IPinEventSink*> (this)));	
        RETURN_IF_FAILED(RegisterPin(&m_oOutputTurnLeft));

    }
    else if(eStage == StageGraphReady)
    {
        // the ids are not set yet
        m_bIDsActuatorOutputSet = tFalse;
    }
    RETURN_NOERROR;
}

tResult cCarControl::Start(__exception)
{
    RETURN_IF_FAILED(cBaseQtFilter::Start(__exception_ptr));
    THROW_IF_POINTER_NULL(_kernel);

    // set the zero values to the widget
    m_pWidget->setZeroValues(GetPropertyFloat("Zero Position Speed Controller [deg]"), GetPropertyFloat("Zero Position Steering Controller [deg]"));    

    // get the value for the update rate
    tTimeStamp tmPeriod = tTimeStamp(1/float(GetPropertyInt("Actuator Update Rate [Hz]"))*1000000);

    //create the timer for the transmitting actuator values
    m_hTimerActuator = _kernel->TimerCreate(tmPeriod, GetPropertyInt("Actuator Startup Time Delay [sec]")*1000000, static_cast<IRunnable*>(this),
        NULL, &m_hTimerActuator, 0, 0, adtf_util::cString::Format("%s.timerActuator", OIGetInstanceName()));
    RETURN_NOERROR;
}

tResult cCarControl::Run(tInt nActivationCode, const tVoid* pvUserData, tInt szUserDataSize, ucom::IException** __exception_ptr/* =NULL */)
{
    if (nActivationCode == IRunnable::RUN_TIMER)
    {        
        // actuator timer was called, time to transmit actuator samples
        if (pvUserData==&m_hTimerActuator)
        {
            //create new media sample for steering controller	
            {                
                cObjectPtr<IMediaSample> pMediaSampleSteer;	
                RETURN_IF_FAILED(AllocMediaSample((tVoid**)&pMediaSampleSteer));

                //allocate memory with the size given by the descriptor	
                cObjectPtr<IMediaSerializer> pSerializerSteer;	
                m_pDescriptionActuatorOutput->GetMediaSampleSerializer(&pSerializerSteer);
                tInt nSize = pSerializerSteer->GetDeserializedSize();	
                RETURN_IF_FAILED(pMediaSampleSteer->AllocBuffer(nSize));	

                {   // focus for sample write lock	
                    //write date to the media sample with the coder of the descriptor
                    __adtf_sample_write_lock_mediadescription(m_pDescriptionActuatorOutput,pMediaSampleSteer,pCoder);

                    // set the ids if not already done
                    if(!m_bIDsActuatorOutputSet)
                    {
                        pCoder->GetID("f32Value", m_szIDActuatorOutputF32Value);
                        pCoder->GetID("ui32ArduinoTimestamp", m_szIDActuatorOutputArduinoTimestamp);
                        m_bIDsActuatorOutputSet = tTrue;
                    }    

                    pCoder->Set(m_szIDActuatorOutputF32Value, (tVoid*)&m_f32CurrentSteeringControllerValue);
                }

                //transmit media sample over output pin	
                RETURN_IF_FAILED(pMediaSampleSteer->SetTime(_clock->GetStreamTime()));
                RETURN_IF_FAILED(m_oOutputSteeringController.Transmit(pMediaSampleSteer));
            }
            //create new media sample for speed controller	
            {                
                cObjectPtr<IMediaSample> pMediaSampleSpeed;	
                RETURN_IF_FAILED(AllocMediaSample((tVoid**)&pMediaSampleSpeed));

                //allocate memory with the size given by the descriptor	
                cObjectPtr<IMediaSerializer> pSerializerSpeed;	
                m_pDescriptionActuatorOutput->GetMediaSampleSerializer(&pSerializerSpeed);
                tInt nSizeSpeed = pSerializerSpeed->GetDeserializedSize();	
                RETURN_IF_FAILED(pMediaSampleSpeed->AllocBuffer(nSizeSpeed));	

                {   // focus for sample write lock	
                    //write date to the media sample with the coder of the descriptor
                    __adtf_sample_write_lock_mediadescription(m_pDescriptionActuatorOutput,pMediaSampleSpeed,pCoder);

                    // set the ids if not already done
                    if(!m_bIDsActuatorOutputSet)
                    {
                        pCoder->GetID("f32Value", m_szIDActuatorOutputF32Value);
                        pCoder->GetID("ui32ArduinoTimestamp", m_szIDActuatorOutputArduinoTimestamp);
                        m_bIDsActuatorOutputSet = tTrue;
                    }  

                    // set the value
                    pCoder->Set(m_szIDActuatorOutputF32Value, (tVoid*)&m_f32CurrentSpeedControllerValue);
                }

                //transmit media sample over output pin	
                RETURN_IF_FAILED(pMediaSampleSpeed->SetTime(_clock->GetStreamTime()));
                RETURN_IF_FAILED(m_oOutputSpeedController.Transmit(pMediaSampleSpeed));
            }
        }

    }
    return cBaseQtFilter::Run(nActivationCode, pvUserData, szUserDataSize, __exception_ptr);
}

tResult cCarControl::Stop(__exception)
{
    // destroy the timer if exists
    if (m_hTimerActuator) 
    {
        _kernel->TimerDestroy(m_hTimerActuator);
        m_hTimerActuator = NULL;
    }

    RETURN_IF_FAILED(cBaseQtFilter::Stop(__exception_ptr));

    RETURN_NOERROR;
}

tResult cCarControl::Shutdown(tInitStage eStage, __exception)
{ 
    return cBaseQtFilter::Shutdown(eStage, __exception_ptr);
}


tResult cCarControl::OnPinEvent(IPin* pSource, tInt nEventCode, tInt nParam1, tInt nParam2, IMediaSample* pMediaSample)
{  
    RETURN_NOERROR;
}

tResult cCarControl::TransmitBoolValue(cOutputPin* oPin, bool value)
{
    cObjectPtr<IMediaSample> pMediaSample;
    AllocMediaSample((tVoid**)&pMediaSample);

    //allocate memory with the size given by the descriptor
    cObjectPtr<IMediaSerializer> pSerializer;
    m_pDescriptionBool->GetMediaSampleSerializer(&pSerializer);
    pMediaSample->AllocBuffer(pSerializer->GetDeserializedSize());

    tBool bValue = value;
    tUInt32 ui32TimeStamp = 0;

    //write date to the media sample with the coder of the descriptor
    {
        __adtf_sample_write_lock_mediadescription(m_pDescriptionBool, pMediaSample, pCoderOutput);    

        // set the id if not already done
        if(!m_bIDsBoolValueOutput)
        {
            pCoderOutput->GetID("bValue", m_szIDBoolValueOutput);
            pCoderOutput->GetID("ui32ArduinoTimestamp", m_szIDArduinoTimestampOutput);
            m_bIDsBoolValueOutput = tTrue;
        }      

        // set value from sample
        pCoderOutput->Set(m_szIDBoolValueOutput, (tVoid*)&bValue);     
        pCoderOutput->Set(m_szIDArduinoTimestampOutput, (tVoid*)&(ui32TimeStamp));     
    }

    pMediaSample->SetTime(_clock->GetStreamTime());

    //transmit media sample over output pin
    oPin->Transmit(pMediaSample);

    RETURN_NOERROR;
}


void cCarControl::OnSpeedController(float fValue)
{
    m_f32CurrentSpeedControllerValue = tFloat32(fValue);
}


void cCarControl::OnSteeringController(float fValue)
{
    m_f32CurrentSteeringControllerValue = tFloat32(fValue);
}

void cCarControl::ToggleLights(int id)
{
    static bool headToggle;
    static bool reverseToggle;
    static bool brakeToggle;
    static bool turnRightToggle;
    static bool turnLeftToggle;

    switch (id)
    {
    case 0: // Head
        TransmitBoolValue(&m_oOutputHeadLight, !headToggle);
        headToggle = !headToggle;
        LOG_INFO(cString::Format("Heads toggled: %d", headToggle));
        break;
    case 1: // Reverse
        TransmitBoolValue(&m_oOutputReverseLight, !reverseToggle);
        reverseToggle = !reverseToggle;
        LOG_INFO(cString::Format("Reverse toggled: %d", reverseToggle));
        break;
    case 2: // Brake
        TransmitBoolValue(&m_oOutputBrakeLight, !brakeToggle);
        brakeToggle = !brakeToggle;
        LOG_INFO(cString::Format("Brake toggled: %d", brakeToggle));
        break;
    case 3: // Right
        TransmitBoolValue(&m_oOutputTurnRight, !turnRightToggle);
        turnRightToggle = !turnRightToggle;
        LOG_INFO(cString::Format("Turn right toggled: %d", turnRightToggle));
        break;
    case 4: // Left
        TransmitBoolValue(&m_oOutputTurnLeft, !turnLeftToggle);
        turnLeftToggle = !turnLeftToggle;
        LOG_INFO(cString::Format("Turn Left toggled: %d", turnLeftToggle));
        break;
    default:
        break;
    }
}

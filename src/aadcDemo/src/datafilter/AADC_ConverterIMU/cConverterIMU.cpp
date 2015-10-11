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
#include "cConverterIMU.h"
#include "QuatTypes.h"
#include "EulerAngles.hpp"


ADTF_FILTER_PLUGIN("AADC Converter IMU", OID_ADTF_CONVERTER_IMU, cConverterIMU)

cConverterIMU::cConverterIMU(const tChar* __info) : cFilter(__info)
{

    m_bDebugModeEnabled = tFalse;
    SetPropertyBool("Debug Output to Console", m_bDebugModeEnabled);	
    SetPropertyStr("Debug Output to Console" NSSUBPROP_DESCRIPTION, "If enabled additional debug information is printed to the console (Warning: decreases performance)"); 

    m_ui32EulerAngleSystem = EulOrdXYZs;
    SetPropertyInt("Euler Angle System", 1);
    SetPropertyStr("Euler Angle System" NSSUBPROP_VALUELIST, "1@EulOrdXYZs|2@EulOrdXYXs|3@EulOrdXZYs|4@EulOrdXZXs|5@EulOrdYZXs|6@EulOrdYZYs|7@EulOrdYXZs|8@EulOrdYXYs|9@EulOrdZXYs|10@EulOrdZXZs|11@EulOrdZYXs|12@EulOrdZYZs|13@EulOrdZYXr|14@EulOrdXYXr|15@EulOrdYZXr|16@EulOrdXZXr|17@EulOrdXZYr|18@EulOrdYZYr|19@EulOrdZXYr|20@EulOrdYXYr|21@EulOrdYXZr|22@EulOrdZXZr|23@EulOrdXYZr|24@EulOrdZYZr"); 
    SetPropertyStr("Euler Angle System" NSSUBPROP_DESCRIPTION, 
        "Here you can set the euler system which should be used for transformation. It defines the order of rotation of the axes and whether static or rotating axes are used."); 
    
}

cConverterIMU::~cConverterIMU()
{
}


//****************************
tResult cConverterIMU::CreateInputPins(__exception)
{

    //get the description manager for this filter
    cObjectPtr<IMediaDescriptionManager> pDescManager;
    RETURN_IF_FAILED(_runtime->GetObject(OID_ADTF_MEDIA_DESCRIPTION_MANAGER,IID_ADTF_MEDIA_DESCRIPTION_MANAGER,(tVoid**)&pDescManager,__exception_ptr));
    
    //get description for inertial measurement sensor data pin
    tChar const * strDescInerMeasUnit = pDescManager->GetMediaDescription("tInerMeasUnitData");	
    RETURN_IF_POINTER_NULL(strDescInerMeasUnit);	

    //get mediatype for ultrasonic sensor data pins
    cObjectPtr<IMediaType> pTypeInerMeasUnit = new cMediaType(0, 0, 0, "tInerMeasUnitData", strDescInerMeasUnit, IMediaDescription::MDF_DDL_DEFAULT_VERSION);    	
    
    //get mediatype description for inertial measurement unit sensor data type
    RETURN_IF_FAILED(pTypeInerMeasUnit->GetInterface(IID_ADTF_MEDIA_TYPE_DESCRIPTION, (tVoid**)&m_pDescriptionInerMeasUnitData));
    
    //create pin for inertial measurement unit data
    RETURN_IF_FAILED(m_oInputInerMeasUnit.Create("InerMeasUnit_Struct", pTypeInerMeasUnit, static_cast<IPinEventSink*> (this)));	
    RETURN_IF_FAILED(RegisterPin(&m_oInputInerMeasUnit));
            
    RETURN_NOERROR;
}

//*****************************************************************************

tResult cConverterIMU::CreateOutputPins(__exception)
{
    //get the description manager for this filter
    cObjectPtr<IMediaDescriptionManager> pDescManager;
    RETURN_IF_FAILED(_runtime->GetObject(OID_ADTF_MEDIA_DESCRIPTION_MANAGER,IID_ADTF_MEDIA_DESCRIPTION_MANAGER,(tVoid**)&pDescManager,__exception_ptr));

    //get description for sensor data pins
    tChar const * strDescSignalValue = pDescManager->GetMediaDescription("tSignalValue");	
    RETURN_IF_POINTER_NULL(strDescSignalValue);	
    
    //get mediatype for ultrasonic sensor data pins
    cObjectPtr<IMediaType> pTypeSignalValue = new cMediaType(0, 0, 0, "tSignalValue", strDescSignalValue, IMediaDescription::MDF_DDL_DEFAULT_VERSION);    	

    //get mediatype description for signal  data type
    RETURN_IF_FAILED(pTypeSignalValue->GetInterface(IID_ADTF_MEDIA_TYPE_DESCRIPTION, (tVoid**)&m_pDescriptionSignalOutput));
  
    RETURN_IF_FAILED(m_oOutputYaw.Create("yaw", pTypeSignalValue, static_cast<IPinEventSink*> (this)));
    RETURN_IF_FAILED(RegisterPin(&m_oOutputYaw));

    RETURN_IF_FAILED(m_oOutputPitch.Create("pitch", pTypeSignalValue, static_cast<IPinEventSink*> (this)));
    RETURN_IF_FAILED(RegisterPin(&m_oOutputPitch));

    RETURN_IF_FAILED(m_oOutputRoll.Create("roll", pTypeSignalValue, static_cast<IPinEventSink*> (this)));
    RETURN_IF_FAILED(RegisterPin(&m_oOutputRoll));
    
    RETURN_IF_FAILED(m_oOutputAccX.Create("accX", pTypeSignalValue, static_cast<IPinEventSink*> (this)));
    RETURN_IF_FAILED(RegisterPin(&m_oOutputAccX));

    RETURN_IF_FAILED(m_oOutputAccY.Create("accY", pTypeSignalValue, static_cast<IPinEventSink*> (this)));
    RETURN_IF_FAILED(RegisterPin(&m_oOutputAccY));

    RETURN_IF_FAILED(m_oOutputAccZ.Create("accZ", pTypeSignalValue, static_cast<IPinEventSink*> (this)));
    RETURN_IF_FAILED(RegisterPin(&m_oOutputAccZ));
    
    RETURN_NOERROR;
}


//*****************************************************************************

tResult cConverterIMU::Init(tInitStage eStage, __exception)
{
    RETURN_IF_FAILED(cFilter::Init(eStage, __exception_ptr))

    if (eStage == StageFirst)
    {
        // create the input and output pins
        RETURN_IF_FAILED(CreateInputPins(__exception_ptr));
        RETURN_IF_FAILED(CreateOutputPins(__exception_ptr));
    }
    else if (eStage == StageNormal)
    {

        int e = GetPropertyInt("Euler Angle System");

        switch(e){
            case 1:
                m_ui32EulerAngleSystem = EulOrdXYZs;
                break;
            case 2:
                m_ui32EulerAngleSystem = EulOrdXYXs;
                break;
            case 3:
                m_ui32EulerAngleSystem = EulOrdXZYs;
                break;
            case 4:
                m_ui32EulerAngleSystem = EulOrdXZXs;
                break;
            case 5:
                m_ui32EulerAngleSystem = EulOrdYZXs;
                break;
            case 6:
                m_ui32EulerAngleSystem = EulOrdYZYs;
                break;
            case 7:
                m_ui32EulerAngleSystem = EulOrdYXZs;
                break;
            case 8:
                m_ui32EulerAngleSystem = EulOrdYXYs;
                break;
            case 9:
                m_ui32EulerAngleSystem = EulOrdZXYs;
                break;
            case 10:
                m_ui32EulerAngleSystem = EulOrdZXZs;
                break;
            case 11:
                m_ui32EulerAngleSystem = EulOrdZYXs;
                break;
            case 12:
                m_ui32EulerAngleSystem = EulOrdZYZs;
                break;
            case 13:
                m_ui32EulerAngleSystem = EulOrdZYXr;
                break;
            case 14:
                m_ui32EulerAngleSystem = EulOrdXYXr;
                break;
            case 15:
                m_ui32EulerAngleSystem = EulOrdYZXr;
                break;
            case 16:
                m_ui32EulerAngleSystem = EulOrdXZXr;
                break;
            case 17:
                m_ui32EulerAngleSystem = EulOrdXZYr;
                break;
            case 18:
                m_ui32EulerAngleSystem = EulOrdYZYr;
                break;
            case 19:
                m_ui32EulerAngleSystem = EulOrdZXYr;
                break;
            case 20:
                m_ui32EulerAngleSystem = EulOrdYXYr;
                break;
            case 21:
                m_ui32EulerAngleSystem = EulOrdYXZr;
                break;
            case 22:
                m_ui32EulerAngleSystem = EulOrdZXZr;
                break;
            case 23:
                m_ui32EulerAngleSystem = EulOrdXYZr;
                break;
            case 24:
                m_ui32EulerAngleSystem = EulOrdZYZr;
                break;
        }
    } 
    else if (eStage == StageGraphReady)
    {
        // no ids were set so far
        m_bIDsSignalOutputSet = tFalse;
        m_bIDsInerMeasUnitSet = tFalse;
    }
        
    RETURN_NOERROR;
}

tResult cConverterIMU::Start(__exception)
{
    return cFilter::Start(__exception_ptr);
}

tResult cConverterIMU::Stop(__exception)
{
    return cFilter::Stop(__exception_ptr);
}

tResult cConverterIMU::Shutdown(tInitStage eStage, __exception)
{
    return cFilter::Shutdown(eStage,__exception_ptr);
}

tResult cConverterIMU::OnPinEvent(IPin* pSource, tInt nEventCode, tInt nParam1, tInt nParam2, IMediaSample* pMediaSample)
{    
    RETURN_IF_POINTER_NULL(pMediaSample);
    RETURN_IF_POINTER_NULL(pSource);
    if (nEventCode == IPinEventSink::PE_MediaSampleReceived)
    {            
        // something was received
        if (pSource == &m_oInputInerMeasUnit)
        {
            RETURN_IF_FAILED(ProcessInerMeasUnitSample(pMediaSample));
        }        
    }
    RETURN_NOERROR;
}

tResult cConverterIMU::ProcessInerMeasUnitSample(IMediaSample* pMediaSampleIn)
{
    //write values with zero
    tFloat32 f32Q_w = 0;
    tFloat32 f32Q_x = 0;
    tFloat32 f32Q_y = 0;
    tFloat32 f32Q_z = 0;
    tFloat32 f32A_x = 0;
    tFloat32 f32A_y = 0;
    tFloat32 f32A_z = 0;
    tFloat32 f32Roll = 0;
    tFloat32 f32Pitch = 0;
    tFloat32 f32Yaw = 0;
    tUInt32 ui32ArduinoTimestamp = 0;
    
    {   // focus for sample read lock
        // read-out the incoming Media Sample
        __adtf_sample_read_lock_mediadescription(m_pDescriptionInerMeasUnitData,pMediaSampleIn,pCoderInput);
        
        // get the IDs for the items in the media sample 
        if(!m_bIDsInerMeasUnitSet)
        {
            pCoderInput->GetID("f32Q_w", m_szIDInerMeasUnitF32Q_w);
            pCoderInput->GetID("f32Q_x", m_szIDInerMeasUnitF32Q_x);
            pCoderInput->GetID("f32Q_y", m_szIDInerMeasUnitF32Q_y);
            pCoderInput->GetID("f32Q_z", m_szIDInerMeasUnitF32Q_z);
            pCoderInput->GetID("f32A_x", m_szIDInerMeasUnitF32A_x);
            pCoderInput->GetID("f32A_y", m_szIDInerMeasUnitF32A_y);
            pCoderInput->GetID("f32A_z", m_szIDInerMeasUnitF32A_z);
            pCoderInput->GetID("ui32ArduinoTimestamp", m_szIDInerMeasUnitArduinoTimestamp);
            m_bIDsInerMeasUnitSet = tTrue;
        }

        //write date to the media sample with the coder of the descriptor
        pCoderInput->Get(m_szIDInerMeasUnitF32Q_w, (tVoid*)&f32Q_w);
        pCoderInput->Get(m_szIDInerMeasUnitF32Q_x, (tVoid*)&f32Q_x);
        pCoderInput->Get(m_szIDInerMeasUnitF32Q_y, (tVoid*)&f32Q_y);
        pCoderInput->Get(m_szIDInerMeasUnitF32Q_z, (tVoid*)&f32Q_z);
        pCoderInput->Get(m_szIDInerMeasUnitF32A_x, (tVoid*)&f32A_x);
        pCoderInput->Get(m_szIDInerMeasUnitF32A_y, (tVoid*)&f32A_y);
        pCoderInput->Get(m_szIDInerMeasUnitF32A_z, (tVoid*)&f32A_z);
        pCoderInput->Get(m_szIDInerMeasUnitArduinoTimestamp, (tVoid*)&ui32ArduinoTimestamp);

    }
    // calculate the euler agnles from quaternions
    calulateEulerAngles(f32Q_w,f32Q_x,f32Q_y,f32Q_z,f32Yaw, f32Pitch,f32Roll);
    
    //get size of media sample signal value
    cObjectPtr<IMediaSerializer> pSerializerSignalOutput;	
    m_pDescriptionSignalOutput->GetMediaSampleSerializer(&pSerializerSignalOutput);
    tInt nSize = pSerializerSignalOutput->GetDeserializedSize();
    //create new media sample for yaw angle 	
    {        
        //allocate memory with the size given by the descriptor	
        cObjectPtr<IMediaSample> pMediaSampleYaw;	
        RETURN_IF_FAILED(AllocMediaSample((tVoid**)&pMediaSampleYaw));	
        RETURN_IF_FAILED(pMediaSampleYaw->AllocBuffer(nSize));	
        RETURN_IF_FAILED(pMediaSampleYaw->SetTime(pMediaSampleIn->GetTime()));
        {   // focus for sample write lock	
            //write date to the media sample with the coder of the descriptor
            __adtf_sample_write_lock_mediadescription(m_pDescriptionSignalOutput,pMediaSampleYaw,pCoder);
            
            // get the IDs for the items in the media sample 
            if(!m_bIDsSignalOutputSet)
            {
                pCoder->GetID("f32Value", m_szIDSignalOutputF32Value);
                pCoder->GetID("ui32ArduinoTimestamp", m_szIDSignalOutputArduinoTimestamp);
                m_bIDsSignalOutputSet = tTrue;
            }    
            
            pCoder->Set(m_szIDSignalOutputF32Value, (tVoid*)&f32Yaw);
            pCoder->Set(m_szIDSignalOutputArduinoTimestamp, (tVoid*)&ui32ArduinoTimestamp);
        }    
        //transmit media sample over output pin	
        RETURN_IF_FAILED(m_oOutputYaw.Transmit(pMediaSampleYaw));
    }

    //create new media sample for pitch angle 	
    {        
        cObjectPtr<IMediaSample> pMediaSamplePitch;	
        //allocate memory with the size given by the descriptor	
        RETURN_IF_FAILED(AllocMediaSample((tVoid**)&pMediaSamplePitch));	
        RETURN_IF_FAILED(pMediaSamplePitch->AllocBuffer(nSize));	
        RETURN_IF_FAILED(pMediaSamplePitch->SetTime(pMediaSampleIn->GetTime()));
        {   // focus for sample write lock	
            //write date to the media sample with the coder of the descriptor
            __adtf_sample_write_lock_mediadescription(m_pDescriptionSignalOutput,pMediaSamplePitch,pCoder);
            
            // get the IDs for the items in the media sample 
            if(!m_bIDsSignalOutputSet)
            {
                pCoder->GetID("f32Value", m_szIDSignalOutputF32Value);
                pCoder->GetID("ui32ArduinoTimestamp", m_szIDSignalOutputArduinoTimestamp);
                m_bIDsSignalOutputSet = tTrue;
            }   

            pCoder->Set(m_szIDSignalOutputF32Value, (tVoid*)&f32Pitch);
            pCoder->Set(m_szIDSignalOutputArduinoTimestamp, (tVoid*)&ui32ArduinoTimestamp);
        }    
        //transmit media sample over output pin	
        RETURN_IF_FAILED(m_oOutputPitch.Transmit(pMediaSamplePitch));
    }

     //create new media sample for roll angle 	
    {        
        cObjectPtr<IMediaSample> pMediaSampleRoll;	
        //allocate memory with the size given by the descriptor	
        RETURN_IF_FAILED(AllocMediaSample((tVoid**)&pMediaSampleRoll));	
        RETURN_IF_FAILED(pMediaSampleRoll->AllocBuffer(nSize));	
        RETURN_IF_FAILED(pMediaSampleRoll->SetTime(pMediaSampleIn->GetTime()));
        {   // focus for sample write lock	
            //write date to the media sample with the coder of the descriptor
            __adtf_sample_write_lock_mediadescription(m_pDescriptionSignalOutput,pMediaSampleRoll,pCoder);
            
            // get the IDs for the items in the media sample 
            if(!m_bIDsSignalOutputSet)
            {
                pCoder->GetID("f32Value", m_szIDSignalOutputF32Value);
                pCoder->GetID("ui32ArduinoTimestamp", m_szIDSignalOutputArduinoTimestamp);
                m_bIDsSignalOutputSet = tTrue;
            }   

            pCoder->Set(m_szIDSignalOutputF32Value, (tVoid*)&f32Roll);
            pCoder->Set(m_szIDSignalOutputArduinoTimestamp, (tVoid*)&ui32ArduinoTimestamp);
        }    
        //transmit media sample over output pin	
        RETURN_IF_FAILED(m_oOutputRoll.Transmit(pMediaSampleRoll));
    }

     //create new media sample for acceleration x-axis 	
    {        
        cObjectPtr<IMediaSample> pMediaSampleAccX;	
        //allocate memory with the size given by the descriptor	
        RETURN_IF_FAILED(AllocMediaSample((tVoid**)&pMediaSampleAccX));	
        RETURN_IF_FAILED(pMediaSampleAccX->AllocBuffer(nSize));	
        RETURN_IF_FAILED(pMediaSampleAccX->SetTime(pMediaSampleIn->GetTime()));
        {   // focus for sample write lock	
            //write date to the media sample with the coder of the descriptor
            __adtf_sample_write_lock_mediadescription(m_pDescriptionSignalOutput,pMediaSampleAccX,pCoder);
            
            // get the IDs for the items in the media sample 
            if(!m_bIDsSignalOutputSet)
            {
                pCoder->GetID("f32Value", m_szIDSignalOutputF32Value);
                pCoder->GetID("ui32ArduinoTimestamp", m_szIDSignalOutputArduinoTimestamp);
                m_bIDsSignalOutputSet = tTrue;
            }   
            
            pCoder->Set(m_szIDSignalOutputF32Value, (tVoid*)&f32A_x);
            pCoder->Set(m_szIDSignalOutputArduinoTimestamp, (tVoid*)&ui32ArduinoTimestamp);
        }    
        //transmit media sample over output pin	
        RETURN_IF_FAILED(m_oOutputAccX.Transmit(pMediaSampleAccX));
    }     
    
    //create new media sample for acceleration y-axis 	
    {        
        cObjectPtr<IMediaSample> pMediaSampleAccY;	
        //allocate memory with the size given by the descriptor	
        RETURN_IF_FAILED(AllocMediaSample((tVoid**)&pMediaSampleAccY));	
        RETURN_IF_FAILED(pMediaSampleAccY->AllocBuffer(nSize));	
        RETURN_IF_FAILED(pMediaSampleAccY->SetTime(pMediaSampleIn->GetTime()));
        {   // focus for sample write lock	
            //write date to the media sample with the coder of the descriptor
            __adtf_sample_write_lock_mediadescription(m_pDescriptionSignalOutput,pMediaSampleAccY,pCoder);
            
            // get the IDs for the items in the media sample 
            if(!m_bIDsSignalOutputSet)
            {
                pCoder->GetID("f32Value", m_szIDSignalOutputF32Value);
                pCoder->GetID("ui32ArduinoTimestamp", m_szIDSignalOutputArduinoTimestamp);
                m_bIDsSignalOutputSet = tTrue;
            } 

            pCoder->Set(m_szIDSignalOutputF32Value, (tVoid*)&f32A_y);
            pCoder->Set(m_szIDSignalOutputArduinoTimestamp, (tVoid*)&ui32ArduinoTimestamp);
        }    
        //transmit media sample over output pin	
        RETURN_IF_FAILED(m_oOutputAccY.Transmit(pMediaSampleAccY));
    }

     //create new media sample for acceleration z-axis 	
    {        
        cObjectPtr<IMediaSample> pMediaSampleAccZ;	
        //allocate memory with the size given by the descriptor	
        RETURN_IF_FAILED(AllocMediaSample((tVoid**)&pMediaSampleAccZ));	
        RETURN_IF_FAILED(pMediaSampleAccZ->AllocBuffer(nSize));	
        RETURN_IF_FAILED(pMediaSampleAccZ->SetTime(pMediaSampleIn->GetTime()));
        {   // focus for sample write lock	
            //write date to the media sample with the coder of the descriptor
            __adtf_sample_write_lock_mediadescription(m_pDescriptionSignalOutput,pMediaSampleAccZ,pCoder);
            
            // get the IDs for the items in the media sample 
            if(!m_bIDsSignalOutputSet)
            {
                pCoder->GetID("f32Value", m_szIDSignalOutputF32Value);
                pCoder->GetID("ui32ArduinoTimestamp", m_szIDSignalOutputArduinoTimestamp);
                m_bIDsSignalOutputSet = tTrue;
            } 

            pCoder->Set(m_szIDSignalOutputF32Value, (tVoid*)&f32A_z);
            pCoder->Set(m_szIDSignalOutputArduinoTimestamp, (tVoid*)&ui32ArduinoTimestamp);
        }    
        //transmit media sample over output pin	
        RETURN_IF_FAILED(m_oOutputAccZ.Transmit(pMediaSampleAccZ));
    }    
    RETURN_NOERROR;
}

tResult cConverterIMU::calulateEulerAngles(tFloat32 f32Qw, tFloat32 f32Qx, tFloat32 f32Qy, tFloat32 f32Qz, tFloat32 &f32Yaw_out, tFloat32 &f32Pitch_out, tFloat32 &f32Roll_out)
{
/*! total of 24 different euler systems. We support only one, yet. */
/*! For details please see "Graphic Germs IV", Ken Shoemake, 1993 */

    Quat q;
    q.w = f32Qw;
    q.x = f32Qx;
    q.y = f32Qy;
    q.z = f32Qz;

    EulerAngles e = Eul_FromQuat(q,m_ui32EulerAngleSystem);

    f32Roll_out = static_cast<tFloat32>(cStdMath::MATH_RAD2DEG * e.x);
    f32Pitch_out = static_cast<tFloat32>(cStdMath::MATH_RAD2DEG * e.y);
    f32Yaw_out = static_cast<tFloat32>(cStdMath::MATH_RAD2DEG *e.z);
        
    RETURN_NOERROR;
}
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


#ifndef _CONTROLLER_PID_H_
#define _CONTROLLER_PID_H_

#define OID_ADTF_PIDCONTROLLER "adtf.aadc.controller"

/*!
* This is filter is an prototyp for an PID Controller
*/
class cControllerPID : public adtf::cFilter
{
    ADTF_DECLARE_FILTER_VERSION(OID_ADTF_PIDCONTROLLER, "AADC PID Controller", OBJCAT_DataFilter, "Controller Filter", 1, 0,0, "Beta Version");    

    cInputPin m_oInputMeasured;                // the input pin for the measured value
    cInputPin m_oInputSetPoint;                // the input pin for the set point value
    cOutputPin m_oOutputManipulated;        // the output pin for the manipulated value

public:
    cControllerPID(const tChar* __info);
    virtual ~cControllerPID();

protected: // overwrites cFilter
    tResult Init(tInitStage eStage, __exception = NULL);
    tResult Start(__exception = NULL);
    tResult Stop(__exception = NULL);
    tResult Shutdown(tInitStage eStage, __exception = NULL);        
    tResult OnPinEvent(IPin* pSource, tInt nEventCode, tInt nParam1, tInt nParam2, IMediaSample* pMediaSample);    

private:
    /*! creates all the input Pins*/
    tResult CreateInputPins(__exception = NULL);
    /*! creates all the output Pins*/
    tResult CreateOutputPins(__exception = NULL);    

    /*! calculates the manipulated value for the given values, it uses the setpoint in m_f32SetPoint
    @param f32MeasuredValue    the measuredValue
    */
    tFloat32 getControllerValue(tFloat32 f32MeasuredValue);

    /*! returns the currentstreamtime*/
    tTimeStamp GetTime();

    /*! holds the last measuredValue */
    tFloat32 m_f32MeasuredVariable;
    /*! holds the last measured error */
    tFloat32 m_f32LastMeasuredError;
    /*! holds the last setpoint */
    tFloat32 m_f32SetPoint ;
    /*! holds the last sample time */
    tTimeStamp m_lastSampleTime;
    /*! holds the accumulatedVariable for the controller*/
    tFloat32 m_f32AccumulatedVariable;

    /*! Coder Descriptor for the pins*/
    cObjectPtr<IMediaTypeDescription> m_pDescriptionSignal;
    /*! the id for the f32value of the media description for the signal value input pins */
    tBufferID m_szIDSignalF32Value; 
    /*! the id for the arduino time stamp of the media description for the signal value input pins */
    tBufferID m_szIDSignalArduinoTimestamp;         
    /*! indicates if bufferIDs were set */
    tBool m_bIDsSignalSet;

};

#endif // _CONTROLLER_PID_H_


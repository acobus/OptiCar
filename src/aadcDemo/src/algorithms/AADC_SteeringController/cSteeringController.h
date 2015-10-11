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


#ifndef _STEERINGCONTROLLER_H_
#define _STEERINGCONTROLLER_H_

#include "stdafx.h"

#define OID_ADTF_STEERINGCONTROLLER "adtf.aadc.aadc_steeringController"

class cSteeringController : public adtf::cFilter
{
    ADTF_DECLARE_FILTER_VERSION(OID_ADTF_STEERINGCONTROLLER, "AADC Steering Controller", OBJCAT_DataFilter, "Steering Controller", 1, 0,0, "BFFT GmbH");    

        // the input pin for the set point value
        cInputPin m_oInputCurvature;                
        // the output pin for the manipulated value
        cOutputPin m_oOutputServoAngle;        

    public:
        cSteeringController(const tChar* __info);
        virtual ~cSteeringController();
    
    protected: // overwrites cFilter
        tResult Init(tInitStage eStage, __exception = NULL);
        tResult Start(__exception = NULL);
        tResult Stop(__exception = NULL);
        tResult Shutdown(tInitStage eStage, __exception = NULL);        
        tResult OnPinEvent(IPin* pSource, tInt nEventCode, tInt nParam1, tInt nParam2, IMediaSample* pMediaSample);    

    private:
        /*! creates all the input Pins
        @param __exception pointer for exception
        */
        tResult CreateInputPins(__exception = NULL);

        /*! creates all the output Pins
        @param __exception pointer for exception
        */
        tResult CreateOutputPins(__exception = NULL);    

        /*! calculates the manipulated value for the given values, it uses the setpoint in m_setPoint
        @param measuredValue    the measuredValue
        */
        tFloat64 getControllerValue(tFloat64 i_f64MeasuredValue);

        /*! returns the currentstreamtime*/
        tTimeStamp GetTime();
       
        /*! reads the xml file which is set in the filter properties 
        @param configFilename filename of file to load
        */
        tResult LoadConfigurationData(const cFilename& configFilename);

        /*! prints the configuration data to console*/
        tResult PrintConfigurationData();

        /*! holds the values for the negative supporting points */
        vector<std::pair<tFloat32,tFloat32> > m_f32ValuesNeg;
        /*! holds the values for the positive supporting points */
        vector<std::pair<tFloat32,tFloat32> > m_f32ValuesPos;

        /*! media description for the input pin set speed */
        cObjectPtr<IMediaTypeDescription> m_pDescriptionCurvature;        
        /*! the id for the f32value of the media description for input pin for the set speed */
        tBufferID m_buIDCurvatureF32Value; 
        /*! the id for the arduino time stamp of the media description for input pin for the set speed */
        tBufferID m_buIDCurvatureArduinoTimestamp;         
        /*! indicates of bufferIDs were set */
        tBool m_bCurvatureSet;

        /*! media description for the input pin set speed */
        cObjectPtr<IMediaTypeDescription> m_pDescriptionServoAngle;        
        /*! the id for the f32value of the media description for input pin for the set speed */
        tBufferID m_buIDServoAngleF32Value; 
        /*! the id for the arduino time stamp of the media description for input pin for the set speed */
        tBufferID m_buIDServoAngleArduinoTimestamp;         
        /*! indicates of bufferIDs were set */
        tBool m_bServoAngleSet;
        
        /*! if the debug mode is enabled */ 
        tBool m_bDebugModeEnabled;

        /*! getting the cosine interpolation value
        @param f32Value the value which should be interpolated
        */
        tFloat32 getCosineInterpolation(tFloat32 f32Value);
        
        /*! doing the cosine interpolation
        @param f32y1 the preceding value
        @param f32y2 the next value
        @param f32mu the percentage of distance between f32y1 and f32y2 for that the interpolation should be done
        */
        tFloat32 doCosineInterpolation(tFloat32 f32y1, tFloat32 f32y2, tFloat32 f32mu);
        
};

#endif // _STEERINGCONTROLLER_H_


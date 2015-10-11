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


#ifndef _CONVERTER_IMU_H_
#define _CONVERTER_IMU_H_

#define OID_ADTF_CONVERTER_IMU "adtf.aadc.aadc_converterIMU"

using namespace adtf;
/*! 
* This filter takes the struct given by the Arduino Sensors Filter with the data from the IMU and converts the data to Euler Angles and the acceleration splitted to the three axes. The IMU data directly from Sensor is in Quaternions which is not easy to understand. The unit of the acceleration data is in m/(sec)^2 , the yaw, pitch and roll angles are in radiant. With the property “Euler Angle System” the Euler System to be used has to be set. The different systems varies in their order of rotation of the X-, Y- and Z-Axis and if static or rotating axis are used.
Static Axes:
EulOrdXYZs, EulOrdXYXs, EulOrdXZYs, EulOrdXZXs,
 EulOrdYZXs, EulOrdYZYs, EulOrdYXZs, EulOrdYXYs,
 EulOrdZXYs, EulOrdZXZs, EulOrdZYXs, EulOrdZYZs
Rotating axes:
EulOrdZYXr, EulOrdXYXr, EulOrdYZXr, EulOrdXZXr,  
EulOrdXZYr, EulOrdYZYr, EulOrdZXYr,  EulOrdYXYr, 
EulOrdYXZr, EulOrdZXZr, EulOrdXYZr, EulOrdZYZr
For further information refer to "Graphic Germs IV" from Ken Shoemake or other appropriate literature.
*/
class cConverterIMU : public adtf::cFilter
{
    ADTF_DECLARE_FILTER_VERSION(OID_ADTF_CONVERTER_IMU, "AADC Converter IMU", OBJCAT_DataFilter, "Converter IMU Struct", 1, 0, 0, "BFFT GmbH");    
    
    public:
        cConverterIMU(const tChar* __info);
        virtual ~cConverterIMU();
    
    protected: // overwrites cFilter
        tResult Init(tInitStage eStage, __exception = NULL);
        tResult Start(__exception = NULL);
        tResult Stop(__exception = NULL);
        tResult Shutdown(tInitStage eStage, __exception = NULL);
        tResult OnPinEvent(IPin* pSource, tInt nEventCode, tInt nParam1, tInt nParam2, IMediaSample* pMediaSample);
    
    private:
        /*! creates all the output pins 
        @param __exception the exception pointer
        */
        tResult CreateOutputPins(__exception = NULL);
        
        /*! creates all the input pins 
        @param __exception the exception pointer
        */
        tResult CreateInputPins(__exception = NULL);
        
        /*! calculates the euler angles from the quaternion
        @param f32Qw the w part of the quatuernion
        @param qf32Qx the x part of the quatuernion
        @param f32Qy the y part of the quatuernion
        @param f32Qz the z part of the quatuernion
        @param f32Yaw_out pointer to the yaw angle
        @param f32Pitch_out pointer to the pitch angle
        @param f32Roll_out pointer ot the roll angle
        */
        tResult calulateEulerAngles(tFloat32 f32Qw, tFloat32 f32Qx, tFloat32 f32Qy, tFloat32 f32Qz, tFloat32 &f32Yaw_out, tFloat32 &f32Pitch_out, tFloat32 &f32Roll_out);

        /*! this function processes the incoming mediasample
        @param pMediaSample the incoming sample
        */
        tResult ProcessInerMeasUnitSample(IMediaSample* pMediaSample);
        
        /*! Input pin for the inertial measurement unit data */
        cInputPin m_oInputInerMeasUnit;
        
        /*! output pin for pitch angle */
        cOutputPin m_oOutputPitch;
        
        /*! output pin for yaw angle */
        cOutputPin m_oOutputYaw;
        
        /*! output pin for roll angle */
        cOutputPin m_oOutputRoll;
        
        /*! output pin for the acceleration value in x-direction */
        cOutputPin m_oOutputAccX;
        
        /*! output pin for the acceleration value in y-direction */
        cOutputPin m_oOutputAccY;
        
        /*! output pin for the acceleration value in z-direction */
        cOutputPin m_oOutputAccZ;
                
        /*!.whether prints has to made to the console */
        tBool m_bDebugModeEnabled;
        
        /*! Coder Descriptor for the pins*/
        cObjectPtr<IMediaTypeDescription> m_pDescriptionSignalOutput;
        /*! the id for the f32value of the media description for output pin of the voltage data */
        tBufferID m_szIDSignalOutputF32Value; 
        /*! the id for the arduino time stamp of the media description for output pin of the voltage data */
        tBufferID m_szIDSignalOutputArduinoTimestamp;         
        /*! indicates if bufferIDs were set */
        tBool m_bIDsSignalOutputSet;

        /*! descriptor for intertial measurement unit sensor data */        
        cObjectPtr<IMediaTypeDescription> m_pDescriptionInerMeasUnitData;          
        /*! the id for the f32Q_w of the media description for output pin of the imu data */
        tBufferID m_szIDInerMeasUnitF32Q_w; 
        /*! the id for the f32Q_x of the media description for output pin of the imu data */
        tBufferID m_szIDInerMeasUnitF32Q_x; 
        /*! the id for the f32Q_y of the media description for output pin of the imu data */
        tBufferID m_szIDInerMeasUnitF32Q_y; 
        /*! the id for the f32Q_z of the media description for output pin of the imu data */
        tBufferID m_szIDInerMeasUnitF32Q_z; 
        /*! the id for the f32A_x of the media description for output pin of the imu data */
        tBufferID m_szIDInerMeasUnitF32A_x; 
        /*! the id for the f32A_y of the media description for output pin of the imu data */
        tBufferID m_szIDInerMeasUnitF32A_y; 
        /*! the id for the f32A_z of the media description for output pin of the imu data */
        tBufferID m_szIDInerMeasUnitF32A_z; 
        /*! the id for the arduino time stamp of the media description for output pin of the imu data */
        tBufferID m_szIDInerMeasUnitArduinoTimestamp;         
        /*! indicates if bufferIDs were set */
        tBool m_bIDsInerMeasUnitSet;

        /*! the used euler in this filter */
        tUInt32 m_ui32EulerAngleSystem;
};



//*************************************************************************************************

#endif // _CONVERTER_IMU_H_

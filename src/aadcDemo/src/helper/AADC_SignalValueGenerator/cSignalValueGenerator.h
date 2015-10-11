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


#ifndef _SIGNAL_VALUE_HEADER
#define _SIGNAL_VALUE_HEADER

struct tValueStruct {
    tFloat32 timeStamp;
    tFloat32 f32Value1;
    tFloat32 f32Value2;
};

#define __guid "adtf.aadc.aadc_signalValueGenerator"

#include "stdafx.h"
#include "displaywidget.h"
/*!
This filter can be used to generate two waveforms in Media Samples of the type tSignalValue.
The waveforms can be specified in table in the GUI of the filter by specify a timestamp and corresponding two values which are transmitted on the output pins. The filter does a linear interpolation between the given points and the sample rate of the output samples must be set in the property Actuator Update Rate [Hz] .
The filter can also work with a default file with predefined values. The file can be selected in the properties and the values can be loaded with the button Load Defaults. After editing values the button Save can be used to save the values to a new file or also back to the file with the default values used in the properties.
The values in the properties Default Value 1 and Default Value 2 are transmitted always except when the defined waveform is transmitted, i.e. before the waveform starts and after the waveform ends.
The file must be a simple file containing in one line first the timestamp, followed by the value 1 and value 2. A file looks like:
1000 0 80 
8000 3 110 
15000 3 90

*/
class cSignalValueGenerator : public QObject, public cBaseQtFilter
{
    ADTF_DECLARE_FILTER_VERSION(__guid, "AADC Signal Value Generator", OBJCAT_Auxiliary, "Signal value Generator filter", 1, 0, 0, "BFFT GmbH");    

    Q_OBJECT

signals:


public: // construction
    cSignalValueGenerator(const tChar *);
    virtual ~cSignalValueGenerator();

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

    /*! overrides cFilter */
    tResult PropertyChanged(const tChar* strName);

protected: // Implement cBaseQtFilter

    /*! Creates the widget instance*/
    tHandle CreateView();

    /*! Destroys the widget instance*/
    tResult ReleaseView();

    /*! the output pin to send the value for speed controller */
    cOutputPin     m_oOutputValue1;

    /*! the output pin to send the value for steering controller */
    cOutputPin     m_oOutputValue2;

    /*! descriptor for actuator values data */        
    cObjectPtr<IMediaTypeDescription> m_pDescriptionOutput; 
    /*! the id for the f32value of the media description for the pins */
    tBufferID m_szIDOutputF32Value; 
    /*! the id for the arduino time stamp of the media description for the pins */
    tBufferID m_szIDOutputArduinoTimestamp;         
    /*! indicates if bufferIDs were set */
    tBool m_bIDsOutputSet;

    /*! descriptor for actuator values data */        
    cObjectPtr<IMediaTypeDescription> m_pDescriptionOutput2; 
    /*! the id for the f32value of the media description for the pins */
    tBufferID m_szIDOutput2F32Value; 
    /*! the id for the arduino time stamp of the media description for the pins */
    tBufferID m_szIDOutput2ArduinoTimestamp;         
    /*! indicates if bufferIDs were set */
    tBool m_bIDsOutput2Set;

    /*! handle for timer for sending actuator values*/
    tHandle m_hTimerOutput;


private:

    /*! The displayed widget*/    
    DisplayWidget *m_pWidget;
    /*! if set to true the outputs are generated from the list otherwise the default values are send*/
    tBool m_bEnableOutput;

    /*! the default output for value 1 */
    tFloat32 m_f32DefaultValue1;

    /*! the default output for value 2 */
    tFloat32 m_f32DefaultValue2;

    /*! holds the timestamp when the list was started*/
    tFloat32 m_f32StartTimeStamp;

    /*! the array with the timestamps and their corresponding signal values */ 
    vector<tValueStruct> m_signalValues;

    /*! gets the current value from the vector for the given timestamp 
    @param timestamp the current timestamp of which the value has to be get
    @param valueId the id of which the value has to be get (only 1 or 2 are valid by now: columns in vector)
    */
    tFloat32 getCurrentValue(tFloat32 f32Timestamp, tInt8 i8ValueID);

    /*! gets the default value for the given id
    @param valueId the id of which the value has to be get (only 1 or 2 are valid by now: columns in vector)
    */
    tFloat32 getDefaultValue(tInt8 i8ValueID);
    public slots:

        /*! signal for receiving the tuples of the gui
        @param the timestamp in milliseconds
        @param the value 1
        @param the value 2
        */
        void OnSendTuple(int iStateFlag, float fTime, float fValue1, float fValue2);


};

#endif

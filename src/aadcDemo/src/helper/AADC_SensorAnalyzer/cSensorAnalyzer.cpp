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
#include "cSensorAnalyzer.h"


ADTF_FILTER_PLUGIN("AADC Sensor Analyzer", __guid, cSensorAnalyzer);


cSensorAnalyzer::cSensorAnalyzer(const tChar* __info) : 
QObject(),
    cBaseQtFilter(__info)
{	
    SetPropertyStr("Directory for Sensorpresets" ,"");
    SetPropertyBool("Directory for Sensorpresets" NSSUBPROP_REQUIRED, tTrue);
    SetPropertyBool("Directory for Sensorpresets" NSSUBPROP_DIRECTORY, tTrue);
    SetPropertyStr("Directory for Sensorpresets" NSSUBPROP_DESCRIPTION, "Here you have to select the folder which contains the sensor preset files"); 
}


cSensorAnalyzer::~cSensorAnalyzer()
{
}

tHandle cSensorAnalyzer::CreateView()
{
    QWidget* pWidget = (QWidget*)m_pViewport->VP_GetWindow();
    m_pWidget = new DisplayWidget(pWidget);

    connect(this, SIGNAL(SensorDataChanged(int,float)), m_pWidget, SLOT(SetSensorData(int,float)));
    connect(this, SIGNAL(DirectoryReceived(QString)), m_pWidget, SLOT(SetDirectory(QString)));
    connect(m_pWidget->GetDropDownWidget(), SIGNAL(currentIndexChanged(const QString&)), this, SLOT(SetConfiguration(const QString&)));

    return (tHandle)m_pWidget;
}

tResult cSensorAnalyzer::ReleaseView()
{
    if (m_pWidget != NULL)
    {
        delete m_pWidget;
        m_pWidget = NULL;
    }
    RETURN_NOERROR;
}

tResult cSensorAnalyzer::Init(tInitStage eStage, __exception)
{
    RETURN_IF_FAILED(cBaseQtFilter::Init(eStage, __exception_ptr));

    if (eStage == StageFirst)
    {
        THROW_IF_FAILED(CreateInputPins(__exception_ptr));
    }  
    else if (StageNormal == eStage)
    {
        m_sensorPresets.resize(LIST_LENGTH);
        //THROW_IF_FAILED(LoadConfigurationData("default"));        
    }
    else if(eStage == StageGraphReady)
    {                
        //m_pWidget->SetSensorPresets(m_sensorPresets);
        cFilename filepath = GetPropertyStr("Directory for Sensorpresets");
        ADTF_GET_CONFIG_FILENAME(filepath);
        cString path = filepath.CreateAbsolutePath(".");
        emit DirectoryReceived(QString(path.GetPtr()));
        
        // no ids were set so far
        m_bIDsVoltageSet= tFalse;
        m_bIDsInerMeasUnitSet= tFalse;
        m_bIDsWheelDataSet= tFalse;
        m_bIDsUltrasonicSet = tFalse;
    }
    RETURN_NOERROR;
}

tResult cSensorAnalyzer::CreateInputPins(__exception)
{    
    //get the description manager for this filter
    cObjectPtr<IMediaDescriptionManager> pDescManager;
    RETURN_IF_FAILED(_runtime->GetObject(OID_ADTF_MEDIA_DESCRIPTION_MANAGER,IID_ADTF_MEDIA_DESCRIPTION_MANAGER,(tVoid**)&pDescManager,__exception_ptr));

    //get description for sensor data pins
    tChar const * strDescSignalValue = pDescManager->GetMediaDescription("tSignalValue");	
    RETURN_IF_POINTER_NULL(strDescSignalValue);	
    //get mediatype for ultrasonic sensor data pins
    cObjectPtr<IMediaType> pTypeSignalValue = new cMediaType(0, 0, 0, "tSignalValue", strDescSignalValue, IMediaDescription::MDF_DDL_DEFAULT_VERSION);    	

    //get description for inertial measurement sensor data pin
    tChar const * strDescInerMeasUnit = pDescManager->GetMediaDescription("tInerMeasUnitData");	
    RETURN_IF_POINTER_NULL(strDescInerMeasUnit);	

    //get mediatype for ultrasonic sensor data pins
    cObjectPtr<IMediaType> pTypeInerMeasUnit = new cMediaType(0, 0, 0, "tInerMeasUnitData", strDescInerMeasUnit, IMediaDescription::MDF_DDL_DEFAULT_VERSION);    	

    //get description for wheel sensors data pins
    tChar const * strDescWheelData = pDescManager->GetMediaDescription("tWheelData");	
    RETURN_IF_POINTER_NULL(strDescWheelData);
    //get mediatype for wheeldata sensor data pins
    cObjectPtr<IMediaType> pTypeWheelData = new cMediaType(0, 0, 0, "tWheelData", strDescWheelData, IMediaDescription::MDF_DDL_DEFAULT_VERSION);    	

    //get mediatype description for ultrasonic sensor data type
    RETURN_IF_FAILED(pTypeSignalValue->GetInterface(IID_ADTF_MEDIA_TYPE_DESCRIPTION, (tVoid**)&m_pDescriptionUsData));

    //get mediatype description for voltage sensor data type
    RETURN_IF_FAILED(pTypeSignalValue->GetInterface(IID_ADTF_MEDIA_TYPE_DESCRIPTION, (tVoid**)&m_pDescriptionVoltData));

    //get mediatype description for inertial measurement unit sensor data type
    RETURN_IF_FAILED(pTypeInerMeasUnit->GetInterface(IID_ADTF_MEDIA_TYPE_DESCRIPTION, (tVoid**)&m_pDescriptionInerMeasUnitData));

    //get mediatype description for wheel sensor data type
    RETURN_IF_FAILED(pTypeWheelData->GetInterface(IID_ADTF_MEDIA_TYPE_DESCRIPTION, (tVoid**)&m_pDescriptionWheelData));

    //create pins for ultrasonic sensor data
    RETURN_IF_FAILED(m_oInputUsFrontLeft.Create("Ultrasonic_Front_Left", pTypeSignalValue, static_cast<IPinEventSink*> (this)));	
    RETURN_IF_FAILED(RegisterPin(&m_oInputUsFrontLeft));

    RETURN_IF_FAILED(m_oInputUsFrontCenterLeft.Create("Ultrasonic_Front_Center_Left", pTypeSignalValue, static_cast<IPinEventSink*> (this)));	
    RETURN_IF_FAILED(RegisterPin(&m_oInputUsFrontCenterLeft));

    RETURN_IF_FAILED(m_oInputUsFrontCenter.Create("Ultrasonic_Front_Center", pTypeSignalValue, static_cast<IPinEventSink*> (this)));	
    RETURN_IF_FAILED(RegisterPin(&m_oInputUsFrontCenter));

    RETURN_IF_FAILED(m_oInputUsFrontCenterRight.Create("Ultrasonic_Front_Center_Right", pTypeSignalValue, static_cast<IPinEventSink*> (this)));	
    RETURN_IF_FAILED(RegisterPin(&m_oInputUsFrontCenterRight));

    RETURN_IF_FAILED(m_oInputUsFrontRight.Create("Ultrasonic_Front_Right", pTypeSignalValue, static_cast<IPinEventSink*> (this)));	
    RETURN_IF_FAILED(RegisterPin(&m_oInputUsFrontRight));

    RETURN_IF_FAILED(m_oInputUsSideLeft.Create("Ultrasonic_Side_Left", pTypeSignalValue, static_cast<IPinEventSink*> (this)));	
    RETURN_IF_FAILED(RegisterPin(&m_oInputUsSideLeft));

    RETURN_IF_FAILED(m_oInputUsSideRight.Create("Ultrasonic_Side_Right", pTypeSignalValue, static_cast<IPinEventSink*> (this)));	
    RETURN_IF_FAILED(RegisterPin(&m_oInputUsSideRight));

    RETURN_IF_FAILED(m_oInputUsRearLeft.Create("Ultrasonic_Rear_Left", pTypeSignalValue, static_cast<IPinEventSink*> (this)));	
    RETURN_IF_FAILED(RegisterPin(&m_oInputUsRearLeft));

    RETURN_IF_FAILED(m_oInputUsRearCenter.Create("Ultrasonic_Rear_Center", pTypeSignalValue, static_cast<IPinEventSink*> (this)));	
    RETURN_IF_FAILED(RegisterPin(&m_oInputUsRearCenter));

    RETURN_IF_FAILED(m_oInputUsRearRight.Create("Ultrasonic_Rear_Right", pTypeSignalValue, static_cast<IPinEventSink*> (this)));	
    RETURN_IF_FAILED(RegisterPin(&m_oInputUsRearRight));

    //create pins for voltage sensor data
    RETURN_IF_FAILED(m_oInputVoltMeas.Create("Voltage_Measurement", pTypeSignalValue, static_cast<IPinEventSink*> (this)));	
    RETURN_IF_FAILED(RegisterPin(&m_oInputVoltMeas));

    RETURN_IF_FAILED(m_oInputVoltSpeedCtr.Create("Voltage_SpeedCntrl", pTypeSignalValue, static_cast<IPinEventSink*> (this)));	
    RETURN_IF_FAILED(RegisterPin(&m_oInputVoltSpeedCtr));

    //create pin for inertial measurement unit data
    RETURN_IF_FAILED(m_oInputInerMeasUnit.Create("InerMeasUnit_Struct", pTypeInerMeasUnit, static_cast<IPinEventSink*> (this)));	
    RETURN_IF_FAILED(RegisterPin(&m_oInputInerMeasUnit));

    //create pin for wheel left sensor data
    RETURN_IF_FAILED(m_oInputWheelLeft.Create("WheelLeft_Struct", pTypeWheelData, static_cast<IPinEventSink*> (this)));	
    RETURN_IF_FAILED(RegisterPin(&m_oInputWheelLeft));

    //create pin for wheel right data
    RETURN_IF_FAILED(m_oInputWheelRight.Create("WheelRight_Struct", pTypeWheelData, static_cast<IPinEventSink*> (this)));	
    RETURN_IF_FAILED(RegisterPin(&m_oInputWheelRight));
    RETURN_NOERROR;
}

tResult cSensorAnalyzer::Start(__exception)
{
    RETURN_IF_FAILED(cBaseQtFilter::Start(__exception_ptr));
    THROW_IF_POINTER_NULL(_kernel);


    //create the timer for the transmitting actuator values    
    RETURN_NOERROR;
}

tResult cSensorAnalyzer::Stop(__exception)
{
    RETURN_IF_FAILED(cBaseQtFilter::Stop(__exception_ptr));

    RETURN_NOERROR;
}

tResult cSensorAnalyzer::Shutdown(tInitStage eStage, __exception)
{ 
    return cBaseQtFilter::Shutdown(eStage, __exception_ptr);
}

tResult cSensorAnalyzer::OnPinEvent(IPin* pSource, tInt nEventCode, tInt nParam1, tInt nParam2, IMediaSample* pMediaSample)
{
    RETURN_IF_POINTER_NULL(pMediaSample);
    RETURN_IF_POINTER_NULL(pSource);
    if (nEventCode == IPinEventSink::PE_MediaSampleReceived)
    {            
        if (pSource == &m_oInputUsFrontLeft)
        {
            RETURN_IF_FAILED(ProcessUltrasonicSample(pMediaSample, SensorDefinition::usFrontLeft));
        }
        else if (pSource == &m_oInputUsFrontCenterLeft)
        {
            RETURN_IF_FAILED(ProcessUltrasonicSample(pMediaSample, SensorDefinition::usFrontCenterLeft));
        }
        else if (pSource == &m_oInputUsFrontCenter)
        {
            RETURN_IF_FAILED(ProcessUltrasonicSample(pMediaSample, SensorDefinition::usFrontCenter));
        }
        else if (pSource == &m_oInputUsFrontCenterRight)
        {
            RETURN_IF_FAILED(ProcessUltrasonicSample(pMediaSample, SensorDefinition::usFrontCenterRight));
        }
        else if (pSource == &m_oInputUsFrontRight)
        {
            RETURN_IF_FAILED(ProcessUltrasonicSample(pMediaSample, SensorDefinition::usFrontRight));
        }
        else if (pSource == &m_oInputUsSideLeft)
        {
            RETURN_IF_FAILED(ProcessUltrasonicSample(pMediaSample, SensorDefinition::usSideLeft));
        }
        else if (pSource == &m_oInputUsSideRight)
        {
            RETURN_IF_FAILED(ProcessUltrasonicSample(pMediaSample, SensorDefinition::usSideRight));
        }
        else if (pSource == &m_oInputUsRearLeft)
        {
            RETURN_IF_FAILED(ProcessUltrasonicSample(pMediaSample, SensorDefinition::usRearLeft));
        }
        else if (pSource == &m_oInputUsRearCenter)
        {
            RETURN_IF_FAILED(ProcessUltrasonicSample(pMediaSample, SensorDefinition::usRearCenter));
        }
        else if (pSource == &m_oInputUsRearRight)
        {
            RETURN_IF_FAILED(ProcessUltrasonicSample(pMediaSample, SensorDefinition::usRearRight));
        }
        else if (pSource == &m_oInputVoltMeas)
        {
            RETURN_IF_FAILED(ProcessVoltageSample(pMediaSample, SensorDefinition::measurementCircuit));
        }
        else if (pSource == &m_oInputVoltSpeedCtr)
        {
            RETURN_IF_FAILED(ProcessVoltageSample(pMediaSample, SensorDefinition::speedCntrlCircuit));
        }
        else if (pSource == &m_oInputInerMeasUnit)
        {
            RETURN_IF_FAILED(ProcessInerMeasUnitSample(pMediaSample));
        }
        else if (pSource == &m_oInputWheelLeft)
        {
            RETURN_IF_FAILED(ProcessWheelSample(pMediaSample, SensorDefinition::wheelLeft));
        }
        else if (pSource == &m_oInputWheelRight)
        {
            RETURN_IF_FAILED(ProcessWheelSample(pMediaSample, SensorDefinition::wheelRight));
        }
    }
    RETURN_NOERROR;
}

tResult cSensorAnalyzer::ProcessUltrasonicSample(IMediaSample* pMediaSample, SensorDefinition::ultrasonicSensor usSensor)
{
    __synchronized_obj(m_oProcessUsDataCritSection);
    //write values with zero
    tFloat32 f32value = 0;
    {   // focus for sample read lock
        // read-out the incoming Media Sample
        __adtf_sample_read_lock_mediadescription(m_pDescriptionUsData,pMediaSample,pCoderInput);

        // get the IDs for the items in the media sample 
        if(!m_bIDsUltrasonicSet)
        {
            pCoderInput->GetID("f32Value", m_szIDUltrasonicF32Value);
            pCoderInput->GetID("ui32ArduinoTimestamp", m_szIDUltrasonicArduinoTimestamp);
            m_bIDsUltrasonicSet = tTrue;
        }     

        //get values from media sample        
        pCoderInput->Get(m_szIDUltrasonicF32Value, (tVoid*)&f32value);

        // convert distance from meter to centimeter
        f32value = f32value * 100.0f;
        //emit signal to gui
        switch (usSensor)
        {
        case SensorDefinition::usFrontLeft:
            emit SensorDataChanged(LIST_ULTRASONIC_FRONT_LEFT, static_cast<float>(f32value)); 
            break;
        case SensorDefinition::usFrontCenterLeft:
            emit SensorDataChanged(LIST_ULTRASONIC_FRONT_CENTER_LEFT, static_cast<float>(f32value)); 
            break;
        case SensorDefinition::usFrontCenter:
            emit SensorDataChanged(LIST_ULTRASONIC_FRONT_CENTER, static_cast<float>(f32value)); 
            break;
        case SensorDefinition::usFrontCenterRight:
            emit SensorDataChanged(LIST_ULTRASONIC_FRONT_CENTER_RIGHT, static_cast<float>(f32value)); 
            break;
        case SensorDefinition::usFrontRight:
            emit SensorDataChanged(LIST_ULTRASONIC_FRONT_RIGHT, static_cast<float>(f32value)); 
            break;
        case SensorDefinition::usSideLeft:
            emit SensorDataChanged(LIST_ULTRASONIC_SIDE_LEFT, static_cast<float>(f32value)); 
            break;
        case SensorDefinition::usSideRight:
            emit SensorDataChanged(LIST_ULTRASONIC_SIDE_RIGHT, static_cast<float>(f32value)); 
            break;
        case SensorDefinition::usRearLeft:
            emit SensorDataChanged(LIST_ULTRASONIC_REAR_LEFT, static_cast<float>(f32value)); 
            break;
        case SensorDefinition::usRearCenter:
            emit SensorDataChanged(LIST_ULTRASONIC_REAR_CENTER, static_cast<float>(f32value)); 
            break;
        case SensorDefinition::usRearRight:
            emit SensorDataChanged(LIST_ULTRASONIC_REAR_RIGHT, static_cast<float>(f32value)); 
            break;
        }
    }
    RETURN_NOERROR;
}

tResult cSensorAnalyzer::ProcessVoltageSample(IMediaSample* pMediaSample, SensorDefinition::voltageSensor voltageSensor)
{
    __synchronized_obj(m_oProcessVoltageDataCritSection);
    //write values with zero
    tFloat32 f32value = 0;
    {   // focus for sample read lock
        // read-out the incoming Media Sample
        __adtf_sample_read_lock_mediadescription(m_pDescriptionVoltData,pMediaSample,pCoderInput);

         // get the IDs for the items in the media sample        
        if(!m_bIDsVoltageSet)
        {
            pCoderInput->GetID("f32Value", m_szIDVoltageF32Value);
            pCoderInput->GetID("ui32ArduinoTimestamp", m_szIDVoltageArduinoTimestamp);
            m_bIDsVoltageSet = tTrue;
        }     

        //get values from media sample        
        pCoderInput->Get(m_szIDVoltageF32Value, (tVoid*)&f32value);

        //emit signal to gui
        switch (voltageSensor)
        {
        case SensorDefinition::measurementCircuit:
            emit SensorDataChanged(LIST_VOLTAGE_MEASUREMENT, static_cast<float>(f32value));
            break;
        case SensorDefinition::speedCntrlCircuit:
            emit SensorDataChanged(LIST_VOLTAGE_SPEEDCTR, static_cast<float>(f32value));
            break;
        default:
            break;
        }

    }
    RETURN_NOERROR;
}

tResult cSensorAnalyzer::ProcessInerMeasUnitSample(IMediaSample* pMediaSample)
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

    {   // focus for sample read lock
        // read-out the incoming Media Sample
        __adtf_sample_read_lock_mediadescription(m_pDescriptionInerMeasUnitData,pMediaSample,pCoderInput);

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

        //get values from media sample        
        pCoderInput->Get(m_szIDInerMeasUnitF32Q_w, (tVoid*)&f32Q_w);
        pCoderInput->Get(m_szIDInerMeasUnitF32Q_x, (tVoid*)&f32Q_x);
        pCoderInput->Get(m_szIDInerMeasUnitF32Q_y, (tVoid*)&f32Q_y);
        pCoderInput->Get(m_szIDInerMeasUnitF32Q_z, (tVoid*)&f32Q_z);
        pCoderInput->Get(m_szIDInerMeasUnitF32A_x, (tVoid*)&f32A_x);
        pCoderInput->Get(m_szIDInerMeasUnitF32A_y, (tVoid*)&f32A_y);
        pCoderInput->Get(m_szIDInerMeasUnitF32A_z, (tVoid*)&f32A_z);

        //emit signal to gui
        calulateEulerAngles(f32Q_w,f32Q_x,f32Q_y,f32Q_z,f32Yaw, f32Pitch,f32Roll);

        // multiplicate with 180/pi (*180/M_PI) to get angle in degree
        emit SensorDataChanged(LIST_GYROSCOPE_X_ACC, static_cast<tFloat>(f32A_x));  
        emit SensorDataChanged(LIST_GYROSCOPE_Y_ACC, static_cast<tFloat>(f32A_y));  
        emit SensorDataChanged(LIST_GYROSCOPE_Z_ACC, static_cast<tFloat>(f32A_z));  
        emit SensorDataChanged(LIST_GYROSCOPE_YAW, static_cast<tFloat>(f32Yaw*180/M_PI));
        emit SensorDataChanged(LIST_GYROSCOPE_PITCH, static_cast<tFloat>(f32Pitch*180/M_PI));
        emit SensorDataChanged(LIST_GYROSCOPE_ROLL, static_cast<tFloat>(f32Roll*180/M_PI));  	

    }
    RETURN_NOERROR;
}

tResult cSensorAnalyzer::ProcessWheelSample(IMediaSample* pMediaSample, SensorDefinition::wheelSensor wheelSensorType)
{
    __synchronized_obj(m_oProcessWheelDataCritSection);
    //write values with zero
    tUInt32 ui32Tach = 0;
    tInt8 i8Direction = 0;
    {   // focus for sample read lock
        // read-out the incoming Media Sample
        __adtf_sample_read_lock_mediadescription(m_pDescriptionWheelData,pMediaSample,pCoderInput);

        // get the IDs for the items in the media sample 
        if(!m_bIDsWheelDataSet)
        {
            pCoderInput->GetID("i8WheelDir", m_szIDWheelDataI8WheelDir);
            pCoderInput->GetID("ui32WheelTach", m_szIDWheelDataUi32WheelTach);
            pCoderInput->GetID("ui32ArduinoTimestamp", m_szIDWheelDataArduinoTimestamp);
            m_bIDsWheelDataSet = tTrue;
        }        
        
        //get values from media sample        
        pCoderInput->Get(m_szIDWheelDataUi32WheelTach, (tVoid*)&ui32Tach);        
        pCoderInput->Get(m_szIDWheelDataI8WheelDir, (tVoid*)&i8Direction);

        //LOG_INFO(cString::Format("test %f, %d",ui32Tach,i8Direction));
        //emit signal to gui
        switch (wheelSensorType)
        {
        case SensorDefinition::wheelLeft:
            emit SensorDataChanged(LIST_WHEEL_TACH_LEFT, static_cast<tFloat>(ui32Tach));                
            emit SensorDataChanged(LIST_WHEEL_DIR_LEFT, static_cast<tFloat>(i8Direction));
            break;
        case SensorDefinition::wheelRight:
            emit SensorDataChanged(LIST_WHEEL_TACH_LEFT, static_cast<tFloat>(ui32Tach));                
            emit SensorDataChanged(LIST_WHEEL_DIR_LEFT, static_cast<tFloat>(i8Direction));
            break;
        }
    }
    RETURN_NOERROR;
}

tResult cSensorAnalyzer::calulateEulerAngles(tFloat32 f32QW, tFloat32 f32Qx, tFloat32 f32Qy, tFloat32 f32Qz, tFloat32 &f32Yaw_out, tFloat32 &f32Pitch_out, tFloat32 &f32Roll_out)
{

/*! total of 24 different euler systems. We support only one, yet. */
/*! For details please see "Graphic Germs IV", Ken Shoemake, 1993 */
    f32Roll_out = atan2(2*f32Qy*f32QW - 2*f32Qx*f32Qz, 1 - 2*f32Qy*f32Qy - 2*f32Qz*f32Qz);
    f32Pitch_out = atan2(2*f32Qx*f32QW - 2*f32Qy*f32Qz, 1 - 2*f32Qx*f32Qx - 2*f32Qz*f32Qz);
    f32Yaw_out = asin(2*f32Qx*f32Qy + 2*f32Qz*f32QW); 

    /*
    f32Roll_out = atan2(2*qW*f32Qx + 2*f32Qy*f32Qz, 1 - 2*f32Qx*f32Qx - 2*f32Qy*f32Qy);
    f32Yaw_out = atan2(2*qW*f32Qz + 2*f32Qx*f32Qy, 1 - 2*f32Qy*f32Qy - 2*f32Qz*f32Qz);
    f32Pitch_out = asin(2*qW*f32Qy - 2*f32Qz*f32Qx); 
    */
        
    RETURN_NOERROR;
}

tResult cSensorAnalyzer::LoadConfigurationData(cString filename)
{
    //Get path of configuration file
    //EDS macro is resolved automatically because it is a file property
    m_fileConfig = filename;//GetPropertyStr("Filename for Sensorpresets");

    if (m_fileConfig.IsEmpty())
        LOG_WARNING("Configuration file not found for Analyzer");    

    ADTF_GET_CONFIG_FILENAME(m_fileConfig);
    m_fileConfig = m_fileConfig.CreateAbsolutePath(".");

    if (cFileSystem::Exists(m_fileConfig))
    {
        cDOM oDOM;
        oDOM.Load(m_fileConfig);
        cDOMElementRefList oElems;
        if(IS_OK(oDOM.FindNodes("presets/sensorPreset", oElems)))
        {                
            for (cDOMElementRefList::iterator itElem = oElems.begin(); itElem != oElems.end(); ++itElem)
            {
                cDOMElement* pConfigElement;
                tSensorPreset newSensorPreset;
                if (IS_OK((*itElem)->FindNode("sensor", pConfigElement)))
                {
                    newSensorPreset.sensorName = cString(pConfigElement->GetData());
                    if (IS_OK((*itElem)->FindNode("nominalValue", pConfigElement)))
                        newSensorPreset.nominalValue = static_cast<tFloat32>(cString(pConfigElement->GetData()).AsFloat64());
                    if (IS_OK((*itElem)->FindNode("maxPosDeviation", pConfigElement)))
                        newSensorPreset.maxPosDeviation = static_cast<tFloat32>(cString(pConfigElement->GetData()).AsFloat64());
                    if (IS_OK((*itElem)->FindNode("maxNegDeviation", pConfigElement)))
                        newSensorPreset.maxNegDeviation = static_cast<tFloat32>(cString(pConfigElement->GetData()).AsFloat64());
                }          
                /*LOG_INFO(cString::Format("Name %s, Nominal: %f, NegDev: %f, PosDev: %f",
                newSensorPreset.sensorName.GetPtr(),
                newSensorPreset.nominalValue,
                newSensorPreset.maxNegDeviation,
                newSensorPreset.maxPosDeviation
                ));*/
                addParsedElement(newSensorPreset);
            }        
        }
        else
        {
            LOG_WARNING("Configured configuration file does not contain valid xml scheme");
        }
    }
    else
    {
        LOG_WARNING("Configured configuration file not found (yet).Will appear after extracting the extended data");
    }

    RETURN_NOERROR;
}

tResult cSensorAnalyzer::addParsedElement(tSensorPreset newSensorStruct)
{
    if (newSensorStruct.sensorName.Compare("VOLTAGE_MEASUREMENT")==0)
        m_sensorPresets.at(LIST_VOLTAGE_MEASUREMENT)=newSensorStruct;
    else if (newSensorStruct.sensorName.Compare("VOLTAGE_SPEEDCTR")==0)
        m_sensorPresets.at(LIST_VOLTAGE_SPEEDCTR)=newSensorStruct;
    else if (newSensorStruct.sensorName.Compare("ULTRASONIC_FRONT_LEFT")==0)
        m_sensorPresets.at(LIST_ULTRASONIC_FRONT_LEFT)=newSensorStruct;
    else if (newSensorStruct.sensorName.Compare("ULTRASONIC_FRONT_CENTER_LEFT")==0)
        m_sensorPresets.at(LIST_ULTRASONIC_FRONT_CENTER_LEFT)=newSensorStruct;
    else if (newSensorStruct.sensorName.Compare("ULTRASONIC_FRONT_CENTER")==0)
        m_sensorPresets.at(LIST_ULTRASONIC_FRONT_CENTER)=newSensorStruct;
    else if (newSensorStruct.sensorName.Compare("ULTRASONIC_FRONT_CENTER_RIGHT")==0)
        m_sensorPresets.at(LIST_ULTRASONIC_FRONT_CENTER_RIGHT)=newSensorStruct;
    else if (newSensorStruct.sensorName.Compare("ULTRASONIC_FRONT_RIGHT")==0)
        m_sensorPresets.at(LIST_ULTRASONIC_FRONT_RIGHT)=newSensorStruct;
    else if (newSensorStruct.sensorName.Compare("ULTRASONIC_SIDE_LEFT")==0)
        m_sensorPresets.at(LIST_ULTRASONIC_SIDE_LEFT)=newSensorStruct;
    else if (newSensorStruct.sensorName.Compare("ULTRASONIC_SIDE_RIGHT")==0)
        m_sensorPresets.at(LIST_ULTRASONIC_SIDE_RIGHT)=newSensorStruct;
    else if (newSensorStruct.sensorName.Compare("ULTRASONIC_REAR_LEFT")==0)
        m_sensorPresets.at(LIST_ULTRASONIC_REAR_LEFT)=newSensorStruct;
    else if (newSensorStruct.sensorName.Compare("ULTRASONIC_REAR_CENTER")==0)
        m_sensorPresets.at(LIST_ULTRASONIC_REAR_CENTER)=newSensorStruct;
    else if (newSensorStruct.sensorName.Compare("ULTRASONIC_REAR_RIGHT")==0)
        m_sensorPresets.at(LIST_ULTRASONIC_REAR_RIGHT)=newSensorStruct;
    else if (newSensorStruct.sensorName.Compare("GYROSCOPE_X_ACC")==0)
        m_sensorPresets.at(LIST_GYROSCOPE_X_ACC)=newSensorStruct;
    else if (newSensorStruct.sensorName.Compare("GYROSCOPE_Y_ACC")==0)
        m_sensorPresets.at(LIST_GYROSCOPE_Y_ACC)=newSensorStruct;
    else if (newSensorStruct.sensorName.Compare("GYROSCOPE_Z_ACC")==0)
        m_sensorPresets.at(LIST_GYROSCOPE_Z_ACC)=newSensorStruct;
    else if (newSensorStruct.sensorName.Compare("GYROSCOPE_YAW")==0)
        m_sensorPresets.at(LIST_GYROSCOPE_YAW)=newSensorStruct;
    else if (newSensorStruct.sensorName.Compare("GYROSCOPE_PITCH")==0)
        m_sensorPresets.at(LIST_GYROSCOPE_PITCH)=newSensorStruct;
    else if (newSensorStruct.sensorName.Compare("GYROSCOPE_ROLL")==0)
        m_sensorPresets.at(LIST_GYROSCOPE_ROLL)=newSensorStruct;
    else if (newSensorStruct.sensorName.Compare("WHEEL_TACH_RIGHT")==0)
        m_sensorPresets.at(LIST_WHEEL_TACH_RIGHT)=newSensorStruct;
    else if (newSensorStruct.sensorName.Compare("WHEEL_TACH_LEFT")==0)
        m_sensorPresets.at(LIST_WHEEL_TACH_LEFT)=newSensorStruct;
    else if (newSensorStruct.sensorName.Compare("WHEEL_DIR_RIGHT")==0)
        m_sensorPresets.at(LIST_WHEEL_DIR_RIGHT)=newSensorStruct;
    else if (newSensorStruct.sensorName.Compare("WHEEL_DIR_LEFT")==0)
        m_sensorPresets.at(LIST_WHEEL_DIR_LEFT)=newSensorStruct;

    RETURN_NOERROR;
}

void cSensorAnalyzer::SetConfiguration(const QString& filename)
{
    std::string stdString = filename.toStdString();
    const tChar* string = stdString.c_str();
    cFilename path = GetPropertyStr("Directory for Sensorpresets");
    path.Append("/");
    path.Append(string);
    LoadConfigurationData(path);
    m_pWidget->SetSensorPresets(m_sensorPresets);
}

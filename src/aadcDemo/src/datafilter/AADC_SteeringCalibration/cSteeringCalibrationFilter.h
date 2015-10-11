/**
Copyright (c) 
Audi Autonomous Driving Cup. All rights reserved.
 
Redistribution and use in source and binary forms, with or without modification, are permitted provided that the following conditions are met:
 
1. Redistributions of source code must retain the above copyright notice, this list of conditions and the following disclaimer.
2. Redistributions in binary form must reproduce the above copyright notice, this list of conditions and the following disclaimer in the documentation and/or other materials provided with the distribution.
3. All advertising materials mentioning features or use of this software must display the following acknowledgement: “This product includes software developed by the Audi AG and its contributors for Audi Autonomous Driving Cup.”
4. Neither the name of Audi nor the names of its contributors may be used to endorse or promote products derived from this software without specific prior written permission.
 
THIS SOFTWARE IS PROVIDED BY AUDI AG AND CONTRIBUTORS “AS IS” AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL AUDI AG OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.


**********************************************************************
* $Author:: spiesra $  $Date:: 2015-09-24 11:06:37#$ $Rev:: 39420   $
**********************************************************************/

/*!
 *         This filter can be used to obtain a calibration table for the Steering Controller which can be used to steer the car with given curvature and not only by setting the servo angle of the steering controller. The servo angle is an abstract value and does not describe the car behavior explicitly. The curvature is the more useable to control the car on the street.
 
The calibration is done by series of driven curvature defined by different servo steering angles as the GUI shows. If the button Go is clicked the car starts to drive a curvature with the servo steering angle in the line of the table. If the maximum arc distance or the maximum angle from the properties is reached the filter calculates the radius of the driven curvature by using the driven distance and the yaw angle obtained by the IMU. The calculated radius is shown in the column Radius.
With the button Store Calibration in XML File  the results can be saved to an XML File.
The output pin SpeedController must not be connected to the Arduino Actuators only by using the Wheel Speed Controller.

 */

#ifndef _STEERING_CALIBRATION_FILTER_H_
#define _STEERING_CALIBRATION_FILTER_H_

#define __guid "adtf.aadc.aadc_steeringCalibration"
#include "cCalibrationView.h"
#include "cCalibrationModel.h"

//Prof of concept
//ToDo: StateMaschine einfuegen fuer Messungen. Momentan Logik in Filter / GUI 


//*************************************************************************************************
class cSteeringCalibrationFilter : public cBaseQtFilter
{
     ADTF_DECLARE_FILTER_VERSION(__guid, "AADC Steering Calibration" , adtf::OBJCAT_DataFilter, "Steering Calibration Filter", 1, 0,0, "BFFT GmbH");   
	// Q_OBJECT

protected:
    cInputPin    m_oYawInput;
	cInputPin    m_oDistanceInput;
    cOutputPin    m_oSpeedOutput;
	cOutputPin    m_oSteeringOutput;
	tResult Start(__exception = NULL);
    tResult Stop(__exception = NULL);

public:
    cSteeringCalibrationFilter(const tChar* __info);
    virtual ~cSteeringCalibrationFilter();


protected:
    tResult Init(tInitStage eStage, __exception);
    tResult Shutdown(tInitStage eStage, __exception);

    // implements IPinEventSink
    tResult OnPinEvent(IPin* pSource,
                       tInt nEventCode,
                       tInt nParam1,
                       tInt nParam2,
                       IMediaSample* pMediaSample);
	

	//ToDo: move to State Maschine, 
	tFloat32 m_prop_maxArcDistance;
	tFloat32 m_prop_maxAngle;
	tFloat32 m_prop_averageSpeed;
	
	/*! Coder Descriptor for the pins*/
    cObjectPtr<IMediaTypeDescription> m_pDescriptionSignal;     
    /*! the id for the f32value of the media description for the pins */
    tBufferID m_szIDSignalF32Value; 
    /*! the id for the arduino time stamp of the media description for the pins */
    tBufferID m_szIDSignalArduinoTimestamp;         
    /*! indicates if bufferIDs were set */
    tBool m_bIDsSignalSet; 

	tResult transmitSpeed(tFloat32 speed);
	tResult transmitSteerAngle(tFloat32 angle);
	tResult processDistance(tFloat32 distance);
	tResult processYaw(tFloat32 Yaw);

	cCalibrationModel m_model;

	tResult doDriving();



	void DriveFinished();


protected: 
	// Implement cBaseQtFilter

    /*! Creates the widget instance*/
    virtual tHandle CreateView();

    /*! Destroys the widget instance*/
    virtual tResult ReleaseView();

	cCalibrationView *m_calibrationWidget;

};



//*************************************************************************************************
#endif // _TEMPLATE_PROJECT_FILTER_H_

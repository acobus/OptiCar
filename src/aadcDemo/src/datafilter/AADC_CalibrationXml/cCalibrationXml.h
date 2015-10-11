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

/*! \brief Calibration Scaling Extended
 *         
 *  Dieser Filter dient zur Kalibrierung und Skalierung von Daten. Im Gegensatz zum Calibration Scaling Filter wird hier eine abschnittsweise definierte Funktion f(x) mit Stützstellen zur Interpolation der Messdaten verwendet.
 */

#ifndef _CALIBRATION_XML_H_
#define _CALIBRATION_XML_H_

#include "stdafx.h"

#define OID_ADTF_SENSOR_CALIBRATION "adtf.aadc.aadc_calibrationXML"

/*!
This filter does in comparison to the Calibration Filter an extended calibration. The user has to generate a XML-File which has to be set in the property. This XML-File must include a calibration table with x- and y-values and a mode which has to be used for interpolation.
When the filter receives an input value it looks on the x-Axis of the calibration table and gets the corresponding value on the y-Axis using the set interpolation. This result is transmitted on the pin output_value. If the value on the input pin is greater than the maximum value in the table the maximum value is used, if it is smaller than the minimum value the minimum value is used.
The x-Values in the table must be in increasing order otherwise the calibration does not work and prints an error.
The structure of the XML has to be like this:
<?xml version="1.0" encoding="iso-8859-1" standalone="yes"?>
<calibration xmlns:xsi="http://www.w3.org/2001/XMLSchema-instance">
	<settings>
		<mode>linear</mode>
	</settings>
	<supportingPoints>
		<point>
			<xValue>307</xValue>
			<yValue>-45</yValue>
		</point>
		<point>
			<xValue>572</xValue>
			<yValue>45</yValue>
		</point>
	</supportingPoints>
</calibration>
If no interpolation mode is set in the XML – File the interpolation mode is chosen from the filter properties (i.e. the mode in the XML-File always overwrite the filter property). 

*/
class cCalibrationXml : public adtf::cFilter
{
    ADTF_DECLARE_FILTER_VERSION(OID_ADTF_SENSOR_CALIBRATION, "AADC Calibration XML", OBJCAT_DataFilter, "Calibration XML", 1, 0, 0, "BFFT GmbH");    

         /*! input pin for the raw value */
        cInputPin m_oInput;               
        /*! output pin for the calibrated value */
        cOutputPin m_oOutput;            

    public:
        cCalibrationXml(const tChar* __info);
        virtual ~cCalibrationXml();
    
    protected: // overwrites cFilter
        tResult Init(tInitStage eStage, __exception = NULL);
        tResult Start(__exception = NULL);
        tResult Stop(__exception = NULL);
        tResult Shutdown(tInitStage eStage, __exception = NULL);
        
        tResult OnPinEvent(IPin* pSource, tInt nEventCode, tInt nParam1, tInt nParam2, IMediaSample* pMediaSample);
    
    private:
        /*! creates all the input Pins
        @param __exception the exception pointer
        */
        tResult CreateInputPins(__exception = NULL);
        /*! creates all the output Pins        
        @param __exception the exception pointer
        */
        tResult CreateOutputPins(__exception = NULL);
        /*! reads the xml file which is set in the filter properties */
        tResult LoadConfigurationData();
        /*! checks the loaded configuration data; checks if the xvalues are in increasing order*/
        tResult CheckConfigurationData();

        /*! doing the linear interpolation
        @param fl32InputValue the value which should be interpolated
        */
        tFloat32 getLinearInterpolatedValue(tFloat32 fl32InputValue);

        /*! doing the cubic spline interpolation
        @param fl32InputValue the value which should be interpolated
        */
        tFloat32 getCubicSplineInterpolatedValue(tFloat32 fl32InputValue);
        
        /*! holds the xml file for the supporting points*/
        cFilename m_fileConfig;
       
        /*! holds the yValues for the supporting points*/
        vector<tFloat32> m_yValues;
        /*! holds the xValues for the supporting points*/
        vector<tFloat32> m_xValues;
        
        /*! the class for cubic interpolation*/
        Cubic *m_cubicInterpolation;
        
        /*! if debug console output is enabled */
        tBool m_bDebugModeEnabled;
        
        /*! indicates which calibration mode is used: 1: linear, 2: cubic, 3: none*/
        tInt m_iCalibrationMode;
        
        /*! Coder Descriptor for the pins*/
        cObjectPtr<IMediaTypeDescription> m_pCoderDescSignal;   
        /*! the id for the f32value of the media description for the pins */
        tBufferID m_szIDSignalF32Value; 
        /*! the id for the arduino time stamp of the media description for the pins */
        tBufferID m_szIDSignalArduinoTimestamp;         
        /*! indicates if bufferIDs were set */
        tBool m_bIDsSignalSet; 
    
};



//*************************************************************************************************

#endif // _CALIBRATION_XML_H_


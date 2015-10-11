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


#ifndef _CAMERACALIBRATIONFILTER_HEADER
#define _CAMERACALIBRATIONFILTER_HEADER
#define OID_ADTF_CAMERACALIBRATION "adtf.aadc.aadc_cameraCalibration"

#include "stdafx.h"
#include "displaywidget.h"


/*! 
This filter can be used for doing the intrinsic calibration of the used camera. After starting a GUI is shown which displays the video stream and has two buttons for starting the calibration or saving the file.
The calibration is done with the standard opencv function for intrinsic calibration described in the opencv documentation. The main functions are used from opencv samples in (opencv/samples/cpp/calibration.cpp). For further information have look in this documentation.
To perform the calibration with this ADTF Filter a calibration pattern has to be printed first. A standard chessboard pattern is located in src\aadcDemo\src\xtion\AADC_CameraCalibration\ pattern.png  and has to be printed on an A3 paper. It is better to use a thick paper or stick the paper to solid background.
After that the RGB Output of the Xtion Filter has to be connected to the input pin of this filter and the configuration has to be started. Do not forget to set the setup file for the Xtion Filter.
The side length of one square of the printed chessboard pattern has to be measured and the length be set in the property Square Size in the Filter (unit is meter).
 
Figure 19 GUI Camera Calibration
When the configuration is started you can click “Start Calibration” in the GUI of this filter. After that the algorithms searches for the chessboard in the image. If a chessboard with the given number of squares is detected it is marked with some diagonal lines. During this procedure the chessboard has to be held in different views and angles the get a better calibration. In the console is logged how many datasets have been collected and how many are still needed.
 
Figure 20 Chessboard Detection in Calibration
When all the calibration dataset have been collected the button „Save Calibration File“ can be clicked and the parameters are saved in the standard opencv-yml-File.
This file contains the following parameters:
•	Camera Matrix
•	Distortion Coefficients
•	Reprojection Errors
•	Extrinsic Parameters
In the filter properties the number of squares can be set and especially the square size of the printed out chessboard hast o be set.
 */
class cCameraCalibration : public QObject, public cBaseQtFilter
{
     ADTF_DECLARE_FILTER_VERSION(OID_ADTF_CAMERACALIBRATION, "AADC Camera Calibration", adtf::OBJCAT_Tool, "AADC Camera Calibration Filter", 0, 5, 0, "Beta Version");    

     Q_OBJECT

public:
        
    cCameraCalibration(const tChar* __info);
    virtual ~cCameraCalibration();
    tResult Init(tInitStage eStage, __exception);    
    tResult OnPinEvent(adtf::IPin* pSource, tInt nEventCode,tInt nParam1,tInt nParam2,adtf::IMediaSample* pMediaSample);    
    tResult Shutdown(tInitStage eStage, __exception = NULL);

protected:
    /*! input Pin for video */
    cVideoPin m_oPinInputVideo;                

    /*! Creates the widget instance*/
    tHandle CreateView();

    /*! Destroys the widget instance*/
    tResult ReleaseView();
    
signals:
    /*! sends the image of the current media sample to the gui
    @param QImage the image
    */
    void newImage(const QImage &); 

    /*! send the state to the gui
    @param state the state with the enum
    */
    void sendState(int state);

public slots:
    /*! slot for starting the calibration */
    void OnStartCalibration();
    
    /*! slot for saving the file
    @param qFilename the filename including path where to save
    */
    void OnSaveAs(QString qFilename);
private:

    /*! The displayed widget*/    
    DisplayWidget *m_pWidget;

    /*! the pointer which holds the image data shown in the gui */    
    Mat m_matInputImage;

    /*! the pattern which is used for calibration */
    Pattern m_pattern;
    /*! holds the number of squares given in the filter properties */
    Size m_boardSize;        
    /*! the delay which must be between frames used for calibration (to secure that there is a difference in the images)*/ 
    tInt32 m_delay;
    /*! the timestamp of the last sample to measure the delay*/
    clock_t m_prevTimestamp;
    /*! the image points (i.e. results) of the measurements */
    vector<vector<Point2f> > m_imagePoints;
    /*! the minimum number of frames which are used for calibration */
    tInt8 m_nFrames;
    /*! the current state of filter */
    tInt8 m_calibrationState;
    /*! the size of the frames*/ 
    Size m_matrixSize;
    /*! the camera matrix of the calibration */
    Mat m_cameraMatrix;
    /*! the distortion coefficients of the camera*/
    Mat m_distCoeffs;
    
    /*! bitmapformat of input image */
    tBitmapFormat      m_sInputFormat;

    /*! indicates wheter information is printed to the console or not */
    tBool m_bDebugModeEnabled;

    /*! function process the video data
    @param pSample the new media sample to be processed
    */    
    tResult ProcessVideo(IMediaSample* pSample);
    
    /*! function to set the m_sProcessFormat and the  m_sInputFormat variables
    @param pFormat the new format for the input and input pin
    */
    tResult UpdateInputImageFormat(const tBitmapFormat* pFormat);
    
    /*! returns the current streamtime*/
    tTimeStamp GetTime();

    /*! calls the calculation of the matrixes and saving them to the given file
    (original Source is at opencv/samples/cpp/calibration.cpp)
    @param outputFilename refer to opencv/samples/cpp/calibration.cpp
    @param imagePoints refer to opencv/samples/cpp/calibration.cpp
    @param imageSize refer to opencv/samples/cpp/calibration.cpp
    @param boardSize refer to opencv/samples/cpp/calibration.cpp
    @param patternType refer to opencv/samples/cpp/calibration.cpp
    @param squareSize refer to opencv/samples/cpp/calibration.cpp
    @param aspectRatio refer to opencv/samples/cpp/calibration.cpp
    @param flags refer to opencv/samples/cpp/calibration.cpp
    @param cameraMatrix refer to opencv/samples/cpp/calibration.cpp
    @param distCoeffs refer to opencv/samples/cpp/calibration.cpp
    @param writeExtrinsics refer to opencv/samples/cpp/calibration.cpp
    @param writePoints refer to opencv/samples/cpp/calibration.cpp
    */
    static tResult runAndSave(const string& outputFilename,
                const vector<vector<Point2f> >& imagePoints,
                Size imageSize, Size boardSize, Pattern patternType, float squareSize,
                float aspectRatio, int flags, Mat& cameraMatrix,
                Mat& distCoeffs, bool writeExtrinsics, bool writePoints );
    
    /*! does the calculation
    (original Source is at opencv/samples/cpp/calibration.cpp)
    @param imagePoints refer to opencv/samples/cpp/calibration.cpp
    @param imageSize refer to opencv/samples/cpp/calibration.cpp
    @param boardSize refer to opencv/samples/cpp/calibration.cpp
    @param patternType refer to opencv/samples/cpp/calibration.cpp
    @param squareSize refer to opencv/samples/cpp/calibration.cpp
    @param aspectRatio refer to opencv/samples/cpp/calibration.cpp
    @param flags refer to opencv/samples/cpp/calibration.cpp
    @param cameraMatrix refer to opencv/samples/cpp/calibration.cpp
    @param distCoeffs refer to opencv/samples/cpp/calibration.cpp
    @param rvecs refer to opencv/samples/cpp/calibration.cpp
    @param tvecs refer to opencv/samples/cpp/calibration.cpp
    @param reprojErrs refer to opencv/samples/cpp/calibration.cpp
    @param totalAvgErr refer to opencv/samples/cpp/calibration.cpp
    */
    static tResult runCalibration( vector<vector<Point2f> > imagePoints,
                    Size imageSize, Size boardSize, Pattern patternType,
                    float squareSize, float aspectRatio,
                    int flags, Mat& cameraMatrix, Mat& distCoeffs,
                    vector<Mat>& rvecs, vector<Mat>& tvecs,
                    vector<float>& reprojErrs,
                    double& totalAvgErr);

    /*! saves the camera params
    (original Source is at opencv/samples/cpp/calibration.cpp)
    @param filename refer to opencv/samples/cpp/calibration.cpp
    @param imageSize refer to opencv/samples/cpp/calibration.cpp
    @param boardSize refer to opencv/samples/cpp/calibration.cpp
    @param squareSize refer to opencv/samples/cpp/calibration.cpp
    @param aspectRatio refer to opencv/samples/cpp/calibration.cpp
    @param flags refer to opencv/samples/cpp/calibration.cpp
    @param cameraMatrix refer to opencv/samples/cpp/calibration.cpp
    @param distCoeffs refer to opencv/samples/cpp/calibration.cpp
    @param rvecs refer to opencv/samples/cpp/calibration.cpp
    @param tvecs refer to opencv/samples/cpp/calibration.cpp
    @param reprojErrs refer to opencv/samples/cpp/calibration.cpp
    @param imagePoints refer to opencv/samples/cpp/calibration.cpp
    @param totalAvgErr refer to opencv/samples/cpp/calibration.cpp
    */
    static void saveCameraParams( const string& filename,
                           Size imageSize, Size boardSize,
                           float squareSize, float aspectRatio, int flags,
                           const Mat& cameraMatrix, const Mat& distCoeffs,
                           const vector<Mat>& rvecs, const vector<Mat>& tvecs,
                           const vector<float>& reprojErrs,
                           const vector<vector<Point2f> >& imagePoints,
                           double totalAvgErr );
    
    /*! computes the reprojection errors
    (original Source is at opencv/samples/cpp/calibration.cpp)
    @param objectPoints refer to opencv/samples/cpp/calibration.cpp
    @param imagePoints refer to opencv/samples/cpp/calibration.cpp
    @param tvecs refer to opencv/samples/cpp/calibration.cpp
    @param cameraMatrix refer to opencv/samples/cpp/calibration.cpp
    @param distCoeffs refer to opencv/samples/cpp/calibration.cpp
    @param perViewErrors refer to opencv/samples/cpp/calibration.cpp
    */
    static double computeReprojectionErrors(
        const vector<vector<Point3f> >& objectPoints,
        const vector<vector<Point2f> >& imagePoints,
        const vector<Mat>& rvecs, const vector<Mat>& tvecs,
        const Mat& cameraMatrix, const Mat& distCoeffs,
        vector<float>& perViewErrors ); 
    
    /*! calculates the chessboard corners
    (original Source is at opencv/samples/cpp/calibration.cpp)
    @param boardSize size of board
    @param squareSize size of square
    @param corners vector with corners
    @param patternType type of used pattern
    */
    static void calcChessboardCorners(Size boardSize, float squareSize, vector<Point3f>& corners, Pattern patternType = CHESSBOARD);

};
#endif //_CAMERACALIBRATIONFILTER_HEADER

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
#include "cCameraCalibration.h"

#define DELAYBETWEENFRAMES 1000
#define NUMBEROFFRAMESFORCALIBRATION 10


ADTF_FILTER_PLUGIN("Camera Calibration Filter Plugin", OID_ADTF_CAMERACALIBRATION, cCameraCalibration)



cCameraCalibration::cCameraCalibration(const tChar* __info):QObject(),
cBaseQtFilter(__info)
{
    // create alle the properties
    SetPropertyInt("Width [number of squares]",8);
    SetPropertyStr("Width [number of squares]" NSSUBPROP_DESCRIPTION, "The number of squares in horizontal axis (Range: 5 to 15)"); 
    SetPropertyInt("Width [number of squares]" NSSUBPROP_MIN, 5); 
    SetPropertyInt("Width [number of squares]" NSSUBPROP_MAX, 15);

    SetPropertyInt("Height [number of squares]",6);
    SetPropertyStr("Height [number of squares]" NSSUBPROP_DESCRIPTION, "The number of squares in vertical axis (Range: 5 to 15)"); 
    SetPropertyInt("Height [number of squares]" NSSUBPROP_MIN, 5); 
    SetPropertyInt("Height [number of squares]" NSSUBPROP_MAX, 15);

    SetPropertyFloat("Square Size",0.036f);
    SetPropertyStr("Square Size" NSSUBPROP_DESCRIPTION, "Square size (length of one side) in meters (0.036 by default)"); 
    SetPropertyFloat("Square Size" NSSUBPROP_MIN, 0); 
    SetPropertyFloat("Square Size" NSSUBPROP_MAX, 20);
    
    SetPropertyFloat("Aspect Ratio",1.0f);
    SetPropertyStr("Aspect Ratio" NSSUBPROP_DESCRIPTION, "Fix aspect ratio (fx/fy) (1 by default)"); 
    SetPropertyFloat("Aspect Ratio" NSSUBPROP_MIN, 0); 
    SetPropertyFloat("Aspect Ratio" NSSUBPROP_MAX, 20);

    SetPropertyInt("Calibration Pattern", 1);
    SetPropertyStr("Calibration Pattern" NSSUBPROP_VALUELIST, "1@Chessboard"); 
    SetPropertyStr("Calibration Pattern" NSSUBPROP_DESCRIPTION, "Defines the pattern which is used for calibration");     
    
}

cCameraCalibration::~cCameraCalibration()
{
}

tResult cCameraCalibration::Init(tInitStage eStage, __exception)
{
    RETURN_IF_FAILED(cBaseQtFilter::Init(eStage, __exception_ptr));
    if (eStage == StageFirst)
        {
        //create the video pin for RGB input video
        RETURN_IF_FAILED(m_oPinInputVideo.Create("Video_RGB_input",IPin::PD_Input, static_cast<IPinEventSink*>(this))); 
        RETURN_IF_FAILED(RegisterPin(&m_oPinInputVideo));
        }
    else if (eStage == StageNormal)
    {
        // set the member variables from the properties
        // debug mode or not
        m_bDebugModeEnabled = GetPropertyBool("Debug Output to Console");
        // get calibration pattern
        if (GetPropertyInt("Calibration Pattern")==1) m_pattern = CHESSBOARD;
        // get width and height of board
        m_boardSize= Size(GetPropertyInt("Width [number of squares]"),GetPropertyInt("Height [number of squares]"));
        
        // set the member variables for the calibration (see header)
        m_delay = DELAYBETWEENFRAMES;
        m_nFrames = NUMBEROFFRAMESFORCALIBRATION;
        
        // set the state and other necessary variables
        m_prevTimestamp = 0;
        m_imagePoints.clear();
        m_calibrationState = WAITING;        
    }
    else if (eStage == StageGraphReady)
        {
        // get the image format of the input video pin
        cObjectPtr<IMediaType> pType;
        RETURN_IF_FAILED(m_oPinInputVideo.GetMediaType(&pType));
        cObjectPtr<IMediaTypeVideo> pTypeVideo;
        RETURN_IF_FAILED(pType->GetInterface(IID_ADTF_MEDIA_TYPE_VIDEO, (tVoid**)&pTypeVideo));

        // set the image format of the input video pin
        UpdateInputImageFormat(pTypeVideo->GetFormat());

        }
    RETURN_NOERROR;
}

tResult cCameraCalibration::Shutdown(tInitStage eStage, __exception)
{
    return cBaseQtFilter::Shutdown(eStage, __exception_ptr);
}

tResult cCameraCalibration::OnPinEvent(IPin* pSource,
                                            tInt nEventCode,
                                            tInt nParam1,
                                            tInt nParam2,
                                            IMediaSample* pMediaSample)
{
    switch (nEventCode)
            {
            case IPinEventSink::PE_MediaSampleReceived:
                // a new image was received so the processing is started
                if (pSource == &m_oPinInputVideo)
                    ProcessVideo(pMediaSample);
                break;
        case IPinEventSink::PE_MediaTypeChanged:
            if (pSource == &m_oPinInputVideo)
            {
                //the input format was changed, so the imageformat has to changed in this filter also
                cObjectPtr<IMediaType> pType;
                RETURN_IF_FAILED(m_oPinInputVideo.GetMediaType(&pType));

                cObjectPtr<IMediaTypeVideo> pTypeVideo;
                RETURN_IF_FAILED(pType->GetInterface(IID_ADTF_MEDIA_TYPE_VIDEO, (tVoid**)&pTypeVideo));

                UpdateInputImageFormat(m_oPinInputVideo.GetFormat());    
            }            
            break;
        default:
            break;
        }    
    RETURN_NOERROR;
}

tResult cCameraCalibration::ProcessVideo(adtf::IMediaSample* pISample)
{
    RETURN_IF_POINTER_NULL(pISample);
    
    //creating new pointer for input data
    const tVoid* l_pSrcBuffer;

    //receiving data from input sample, and saving to inputFrame
    if (IS_OK(pISample->Lock(&l_pSrcBuffer)))
        {
        //creating the matrix with the data
        m_matInputImage  = Mat(m_sInputFormat.nHeight,m_sInputFormat.nWidth,CV_8UC3,(tVoid*)l_pSrcBuffer,m_sInputFormat.nBytesPerLine);    
        
        //note: input image is BGR not RGB
        //cvtColor(frame, frame, COLOR_BGR2RGB);
        
        //copy to new mat and convert to grayscale otherwise the image in original media sample is modified
        Mat frame_gray;
        cvtColor(m_matInputImage, frame_gray, COLOR_BGR2GRAY);
               
        //variables for detection
        tBool bFoundPattern=tFalse;
        vector<Point2f> pointbuf;
        
        // is not calibrated and is not waiting
        if (m_calibrationState!=CALIBRATED && m_calibrationState!=WAITING)
            {
            //find pattern in input image, result is written to pointbuf
            switch(m_pattern)
                {
                    case CHESSBOARD:
                        bFoundPattern = findChessboardCorners( m_matInputImage, m_boardSize, pointbuf,
                            CALIB_CB_ADAPTIVE_THRESH | CALIB_CB_FAST_CHECK | CALIB_CB_NORMALIZE_IMAGE);
                        break;
                    default:
                        LOG_ERROR("Unknown Calibration Pattern chosen");
                        break;
                }
            
            // improve the found corners' coordinate accuracy
            if( m_pattern == CHESSBOARD && bFoundPattern) cornerSubPix( frame_gray, pointbuf, Size(11,11),
                Size(-1,-1), TermCriteria( TermCriteria::EPS+TermCriteria::COUNT, 30, 0.1 ));
            
            //if the delay time is passed and the pattern is found create a valid calibration dataset in m_imagePoints
            if(bFoundPattern && (abs(m_prevTimestamp - GetTime()) > m_delay*1000) )
                {
                    m_imagePoints.push_back(pointbuf);
                    m_prevTimestamp = GetTime();
                    LOG_INFO(cString::Format("Processed %d of %d needed datasets",m_imagePoints.size(),m_nFrames));                
                }

            //draw the corners in the gray image
            if(bFoundPattern)
               drawChessboardCorners( frame_gray, m_boardSize, Mat(pointbuf), bFoundPattern );
            
            //if more image points than the defined numbers are found switch to state CALIBRATED
            if(m_imagePoints.size() >= (unsigned)m_nFrames )
                {
                    m_matrixSize = frame_gray.size();
                    m_calibrationState  = CALIBRATED;
                    emit sendState(CALIBRATED);
                }        
            }

        //create an qimage from the data to print to qt gui
        QImage image(frame_gray.data, frame_gray.cols, frame_gray.rows,static_cast<int>(frame_gray.step),QImage::Format_Indexed8);
        
        // for a grayscale image there is no suitable definition in qt so we have to create our own color table
        QVector<QRgb> grayscaleTable;
        for (int i = 0; i<256;i++) grayscaleTable.push_back(qRgb(i,i,i));
        image.setColorTable(grayscaleTable);
        
        //send the image to the gui
        emit newImage(image.scaled(320,240,Qt::KeepAspectRatio));
        
        }
    pISample->Unlock(l_pSrcBuffer); 
    
    RETURN_NOERROR;
}

tResult cCameraCalibration::UpdateInputImageFormat(const tBitmapFormat* pFormat)
{
    // set the input format to given format in argument
    if (pFormat != NULL)
        m_sInputFormat = (*pFormat);
    
    RETURN_NOERROR;
}

tHandle cCameraCalibration::CreateView()
{
    QWidget* pWidget = (QWidget*)m_pViewport->VP_GetWindow();
    m_pWidget = new DisplayWidget(pWidget);        
    
    //doing the connection between gui and filter
    connect(this,SIGNAL(newImage(const QImage &)),m_pWidget,SLOT(OnNewImage(const QImage &)));
    connect(m_pWidget->m_btStart,SIGNAL(clicked()),this,SLOT(OnStartCalibration()));
    connect(m_pWidget,SIGNAL(SendSaveAs(QString)),this,SLOT(OnSaveAs(QString)));
    connect(this,SIGNAL(sendState(int)),m_pWidget,SLOT(OnSetState(int)));
    
    return (tHandle)m_pWidget;
}

tResult cCameraCalibration::ReleaseView()
{
    if (m_pWidget != NULL)
    {
        delete m_pWidget;
        m_pWidget = NULL;
    }
    RETURN_NOERROR;
}

tResult cCameraCalibration::runAndSave(const string& outputFilename,
                const vector<vector<Point2f> >& imagePoints,
                Size imageSize, Size boardSize, Pattern patternType, float squareSize,
                float aspectRatio, int flags, Mat& cameraMatrix,
                Mat& distCoeffs, bool writeExtrinsics, bool writePoints )
{
    // for more details and comments look to original Source at opencv/samples/cpp/calibration.cpp
    vector<Mat> rvecs, tvecs;
    vector<tFloat32> reprojErrs;
    tFloat64 totalAvgErr = 0;

    tBool ok = runCalibration(imagePoints, imageSize, boardSize, patternType, squareSize,
                   aspectRatio, flags, cameraMatrix, distCoeffs,
                   rvecs, tvecs, reprojErrs, totalAvgErr);
    if (ok)
        LOG_INFO(cString::Format("Calibration succeeded, avg reprojection error = %.2f", totalAvgErr));
    else
        LOG_INFO(cString::Format("Calibration failed, avg reprojection error = %.2f", totalAvgErr));
    
    if( ok )
        saveCameraParams( outputFilename, imageSize,
                         boardSize, squareSize, aspectRatio,
                         flags, cameraMatrix, distCoeffs,
                         writeExtrinsics ? rvecs : vector<Mat>(),
                         writeExtrinsics ? tvecs : vector<Mat>(),
                         writeExtrinsics ? reprojErrs : vector<float>(),
                         writePoints ? imagePoints : vector<vector<Point2f> >(),
                         totalAvgErr );
    return ok;
}

tResult cCameraCalibration::runCalibration( vector<vector<Point2f> > imagePoints,
                    Size imageSize, Size boardSize, Pattern patternType,
                    float squareSize, float aspectRatio,
                    int flags, Mat& cameraMatrix, Mat& distCoeffs,
                    vector<Mat>& rvecs, vector<Mat>& tvecs,
                    vector<float>& reprojErrs,
                    double& totalAvgErr)
{
    // for more details and comments look to original Source at opencv/samples/cpp/calibration.cpp
    cameraMatrix = Mat::eye(3, 3, CV_64F);
    if( flags & CALIB_FIX_ASPECT_RATIO )
        cameraMatrix.at<double>(0,0) = aspectRatio;

    distCoeffs = Mat::zeros(8, 1, CV_64F);

    vector<vector<Point3f> > objectPoints(1);
    calcChessboardCorners(boardSize, squareSize, objectPoints[0], patternType);

    objectPoints.resize(imagePoints.size(),objectPoints[0]);

    double rms = calibrateCamera(objectPoints, imagePoints, imageSize, cameraMatrix,
                    distCoeffs, rvecs, tvecs, flags|CALIB_FIX_K4|CALIB_FIX_K5);
                    ///*|CALIB_FIX_K3*/|CALIB_FIX_K4|CALIB_FIX_K5);
    LOG_INFO(cString::Format("RMS error reported by calibrateCamera: %g", rms));

    bool ok = checkRange(cameraMatrix) && checkRange(distCoeffs);

    totalAvgErr = computeReprojectionErrors(objectPoints, imagePoints,
                rvecs, tvecs, cameraMatrix, distCoeffs, reprojErrs);

    return ok;
}


void cCameraCalibration::saveCameraParams( const string& filename,
                       Size imageSize, Size boardSize,
                       float squareSize, float aspectRatio, int flags,
                       const Mat& cameraMatrix, const Mat& distCoeffs,
                       const vector<Mat>& rvecs, const vector<Mat>& tvecs,
                       const vector<float>& reprojErrs,
                       const vector<vector<Point2f> >& imagePoints,
                       double totalAvgErr )
{
    // for more details and comments look to original Source at opencv/samples/cpp/calibration.cpp
    FileStorage fs( filename, FileStorage::WRITE );

    time_t tt;
    time( &tt );
    struct tm *t2 = localtime( &tt );
    char buf[1024];
    strftime( buf, sizeof(buf)-1, "%c", t2 );

    fs << "calibration_time" << buf;

    if( !rvecs.empty() || !reprojErrs.empty() )
        fs << "nframes" << (int)std::max(rvecs.size(), reprojErrs.size());
    fs << "image_width" << imageSize.width;
    fs << "image_height" << imageSize.height;
    fs << "board_width" << boardSize.width;
    fs << "board_height" << boardSize.height;
    fs << "square_size" << squareSize;

    if( flags & CALIB_FIX_ASPECT_RATIO )
        fs << "aspectRatio" << aspectRatio;

    if( flags != 0 )
    {
        sprintf( buf, "flags: %s%s%s%s",
            flags & CALIB_USE_INTRINSIC_GUESS ? "+use_intrinsic_guess" : "",
            flags & CALIB_FIX_ASPECT_RATIO ? "+fix_aspectRatio" : "",
            flags & CALIB_FIX_PRINCIPAL_POINT ? "+fix_principal_point" : "",
            flags & CALIB_ZERO_TANGENT_DIST ? "+zero_tangent_dist" : "" );
        //cvWriteComment( *fs, buf, 0 );
    }

    fs << "flags" << flags;

    fs << "camera_matrix" << cameraMatrix;
    fs << "distortion_coefficients" << distCoeffs;

    fs << "avg_reprojection_error" << totalAvgErr;
    if( !reprojErrs.empty() )
        fs << "per_view_reprojection_errors" << Mat(reprojErrs);

    if( !rvecs.empty() && !tvecs.empty() )
    {
        CV_Assert(rvecs[0].type() == tvecs[0].type());
        Mat bigmat((int)rvecs.size(), 6, rvecs[0].type());
        for( int i = 0; i < (int)rvecs.size(); i++ )
        {
            Mat r = bigmat(Range(i, i+1), Range(0,3));
            Mat t = bigmat(Range(i, i+1), Range(3,6));

            CV_Assert(rvecs[i].rows == 3 && rvecs[i].cols == 1);
            CV_Assert(tvecs[i].rows == 3 && tvecs[i].cols == 1);
            //*.t() is MatExpr (not Mat) so we can use assignment operator
            r = rvecs[i].t();
            t = tvecs[i].t();
        }
        //cvWriteComment( *fs, "a set of 6-tuples (rotation vector + translation vector) for each view", 0 );
        fs << "extrinsic_parameters" << bigmat;
    }

    if( !imagePoints.empty() )
    {
        Mat imagePtMat((int)imagePoints.size(), (int)imagePoints[0].size(), CV_32FC2);
        for( int i = 0; i < (int)imagePoints.size(); i++ )
        {
            Mat r = imagePtMat.row(i).reshape(2, imagePtMat.cols);
            Mat imgpti(imagePoints[i]);
            imgpti.copyTo(r);
        }
        fs << "image_points" << imagePtMat;
    }
}

double cCameraCalibration::computeReprojectionErrors(
        const vector<vector<Point3f> >& objectPoints,
        const vector<vector<Point2f> >& imagePoints,
        const vector<Mat>& rvecs, const vector<Mat>& tvecs,
        const Mat& cameraMatrix, const Mat& distCoeffs,
        vector<float>& perViewErrors )
{
    // for more details and comments look to original Source at opencv/samples/cpp/calibration.cpp
    vector<Point2f> imagePoints2;
    int i, totalPoints = 0;
    double totalErr = 0, err;
    perViewErrors.resize(objectPoints.size());

    for( i = 0; i < (int)objectPoints.size(); i++ )
    {
        projectPoints(Mat(objectPoints[i]), rvecs[i], tvecs[i],
                      cameraMatrix, distCoeffs, imagePoints2);
        err = norm(Mat(imagePoints[i]), Mat(imagePoints2), NORM_L2);
        int n = (int)objectPoints[i].size();
        perViewErrors[i] = (float)std::sqrt(err*err/n);
        totalErr += err*err;
        totalPoints += n;
    }

    return std::sqrt(totalErr/totalPoints);
}

void cCameraCalibration::calcChessboardCorners(Size boardSize, float squareSize, vector<Point3f>& corners, Pattern patternType)
{
    // for more details and comments look to original Source at opencv/samples/cpp/calibration.cpp
    corners.resize(0);

    switch(patternType)
    {
      case CHESSBOARD:
      case CIRCLES_GRID:
        for( int i = 0; i < boardSize.height; i++ )
            for( int j = 0; j < boardSize.width; j++ )
                corners.push_back(Point3f(float(j*squareSize),
                                          float(i*squareSize), 0));
        break;

      case ASYMMETRIC_CIRCLES_GRID:
        for( int i = 0; i < boardSize.height; i++ )
            for( int j = 0; j < boardSize.width; j++ )
                corners.push_back(Point3f(float((2*j + i % 2)*squareSize),
                                          float(i*squareSize), 0));
        break;

      default:
        CV_Error(Error::StsBadArg, "Unknown pattern type\n");
    }
}

tTimeStamp cCameraCalibration::GetTime()
{
    return (_clock != NULL) ? _clock->GetTime () : cSystem::GetTime();
}

void cCameraCalibration::OnStartCalibration()
{
    // switch state to CAPTURING
    m_calibrationState = CAPTURING; 
    emit sendState(CAPTURING);
    LOG_INFO("Starting Calibration");
}

void cCameraCalibration::OnSaveAs(QString qFilename)
{
    // save the calibration results to the given file in argument
    if (m_calibrationState  == CALIBRATED)
        {
        // set some params for save method
        tInt iFlags = 0;
        tBool bWriteExtrinsics = true, bWritePoints = false;
        
        // convert file name to absolute file
        cFilename filename = qFilename.toStdString().c_str();
        ADTF_GET_CONFIG_FILENAME(filename);
        filename = filename.CreateAbsolutePath(".");
                
        if( runAndSave(filename.GetPtr(), m_imagePoints, m_matrixSize,
                m_boardSize, m_pattern, GetPropertyFloat("Square Size",1.0f), GetPropertyFloat("Aspect Ratio",1.0f),
                iFlags, m_cameraMatrix, m_distCoeffs,
                bWriteExtrinsics, bWritePoints))
            {
            LOG_INFO(cString::Format("Saved calibration to %s",filename.GetPtr()));
            // switch to state WAITING again
            m_calibrationState = WAITING;
            }
        }
}


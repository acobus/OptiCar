#include "stdafx.h"

#include "cCalibrationModel.h"
#include "angleDiff.h"

cCalibrationModel::cCalibrationModel(QObject *parent): QObject(parent), tableViewModel(9,5), m_distance(0), m_yaw(0), m_distanceInital(0), m_yawInital(0), m_distanceLimit(0), m_yawLimit(0), m_isDrivingIndex(-1)
{
	QStringList header;	
	header.append("SteerAngle [°]");
	header.append("Distance [m]");
	header.append("Yaw [°]");
	header.append("Radius [m]");
	header.append("");
	
	tableViewModel.setHorizontalHeaderLabels(header);

	int i = 0;
	tableViewModel.setData(tableViewModel.index(i++,0),QVariant(60));
	tableViewModel.setData(tableViewModel.index(i++,0),QVariant(65));
	tableViewModel.setData(tableViewModel.index(i++,0),QVariant(70));
	tableViewModel.setData(tableViewModel.index(i++,0),QVariant(80));
	tableViewModel.setData(tableViewModel.index(i++,0),QVariant(90));
	tableViewModel.setData(tableViewModel.index(i++,0),QVariant(100));
	tableViewModel.setData(tableViewModel.index(i++,0),QVariant(110));
	tableViewModel.setData(tableViewModel.index(i++,0),QVariant(115));
	tableViewModel.setData(tableViewModel.index(i++,0),QVariant(120));	
}

cCalibrationModel::~cCalibrationModel(){

	
}

float cCalibrationModel::getSteerAngle(){
	if(!isDriving()) return 90;

	float angle = tableViewModel.data(tableViewModel.index(getDriveViewIndex(),0)).toFloat();	

	return angle;
}

bool cCalibrationModel::isDriving(){
	int ret;
	m_driveMutex.LockRead();
	{
		ret = m_isDrivingIndex;
	}
	m_driveMutex.UnlockRead();
	if(ret > -1 )
		return true;
	else
		return false;
}

void cCalibrationModel::setDrivingIndex(int index){
	m_driveMutex.LockWrite();
	{
		m_isDrivingIndex = index;
	}
	m_driveMutex.UnlockRead();
}

void cCalibrationModel::onStartDrive(int driveViewIndex){
	
	if(isDriving()){
		LOG_INFO("I'm already driving");
		return;
	}else{
			LOG_INFO("start driving");
	}
	m_distanceInital = getDistance();
	m_yawInital = getYaw();
	setDrivingIndex(driveViewIndex);
}

int cCalibrationModel::getDriveViewIndex(){
	int ret = -1;
	m_driveMutex.LockRead();
	{
		ret = m_isDrivingIndex;
	}
	m_driveMutex.UnlockRead();
	return ret;
}

float cCalibrationModel::getDeltaYaw(){
	float delta = angleDiff(m_yawInital,getYaw());
	return delta;
}

float cCalibrationModel::getDeltaDistance(){
	return getDistance() - m_distanceInital; 
}


void cCalibrationModel::updateYaw(const float yaw){
	m_yawMutex.LockWrite();
	{
		m_yaw = yaw;
	}
	m_yawMutex.UnlockWrite();
	checkLimits();

	if(isDriving()){
		tableViewModel.setData(tableViewModel.index(getDriveViewIndex(),2),QVariant(getDeltaYaw() * cStdMath::MATH_RAD2DEG));	
		updateRadius();	
	}

}


float cCalibrationModel::getRadius(){
	return arcRadius(getDeltaDistance(),getDeltaYaw());
}

void cCalibrationModel::updateRadius(){
	double radius = arcRadius(getDeltaDistance(),getDeltaYaw());

	tableViewModel.setData(tableViewModel.index(getDriveViewIndex(),3),QVariant(radius));	
}


void cCalibrationModel::updateDistance(const float distance){
	m_distanceMutex.LockWrite();
	{
		m_distance = distance;
	}
	m_distanceMutex.UnlockWrite();

	checkLimits();

	if(isDriving()){
		tableViewModel.setData(tableViewModel.index(getDriveViewIndex(),1),QVariant(getDeltaDistance()));
		updateRadius();	
	}
}

void cCalibrationModel::checkLimits(){
	
	if(isDriving() && m_distanceLimit > 0 && m_distanceLimit < getDeltaDistance()){
		//Finished
		setDrivingIndex(-1);
		emit driveFinished(getDeltaDistance(),getDeltaYaw());
		return;
	}else if(isDriving() && m_yawLimit > 0 && m_yawLimit < fabs(getDeltaYaw()) /** fabs() don't care for left or right turn**/){
		//Finished
		setDrivingIndex(-1);
		emit driveFinished(getDeltaDistance(),getDeltaYaw());
		return;
	}

}

void cCalibrationModel::setLimits(const float max_distance,const float max_yaw){
	m_distanceLimit = max_distance;
	m_yawLimit = max_yaw;
}



float cCalibrationModel::getYaw(){
	float ret = 0;
	m_yawMutex.LockRead();
	ret = m_yaw;
	m_yawMutex.UnlockRead();
	return ret;
}



float cCalibrationModel::getDistance(){
	float ret = 0;
	m_distanceMutex.LockRead();
	ret = m_distance;
	m_distanceMutex.UnlockRead();
	return ret;

}



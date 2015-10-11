#ifndef C_CALIBRATION_SM_H
#define C_CALIBRATION_SM_H
#include <qobject.h>

class cCalibrationModel : public QObject
{
	 Q_OBJECT

	public:
		cCalibrationModel(QObject *parent = NULL);
		virtual ~cCalibrationModel();
		void updateYaw(const float yaw);
		void updateDistance(const float distance);
		
		float getYaw();
		float getSteerAngle();
		float getDistance();
		float getDeltaYaw();
		float getDeltaDistance();
		float getRadius();
		void setLimits(const float max_distance,const float max_yaw);
		bool isDriving();
		void setDrivingIndex(int index);
		//returns VIEW Index
		int getDriveViewIndex();

		QStandardItemModel tableViewModel;
	public slots:
		void onStartDrive(int driveViewIndex);
	signals:
		void driveFinished(float max_distance, float max_yaw);

	private:
		//emit driveFinished() SIGNAL if limit reached
		void checkLimits();

		void updateRadius();
		
		float m_distance;
		cReadWriteMutex m_distanceMutex;

		float m_yaw;
		cReadWriteMutex m_yawMutex;

		float m_distanceInital;
		float m_yawInital;
		float m_distanceLimit;
		float m_yawLimit;

		int m_isDrivingIndex;

		cReadWriteMutex m_driveMutex;

};



#endif
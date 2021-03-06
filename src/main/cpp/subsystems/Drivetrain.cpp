#include "Robot.h"
#include "subsystems/Drivetrain.h"
#include "GamepadMap.h"
#include "commands/CmdDriveWithGamepad.h"

//Drivetrain Constants
//Encoder TICKS PER INCH Calibration
const int Drivetrain::LEFT_ENCODER_TPI  = 1153;			//*** Calibrated 3/3/2020
const int Drivetrain::RIGHT_ENCODER_TPI = 1153;			//*** Calibrated 3/3/2020

const double Drivetrain::RAMP_FACTOR    = 0.03;         //Auto Ramp Factor


//Drive limiter
#define MAX_DRIVE_POWER		0.85
#define MAX_TURN_POWER		0.65



Drivetrain::Drivetrain() : Subsystem("Drivetrain") 
{
    std::cout<<"InDrivetrain"<<std::endl;
}


void Drivetrain::InitDefaultCommand() 
{
    SetDefaultCommand( new CmdDriveWithGamepad() );    
}


void Drivetrain::InitFalcons(void)
{
    std::cout<<"Drivetrain: Falcon Init"<<std::endl;

    //reset
    m_leftMotorFront.ConfigFactoryDefault();
    m_leftMotorBack.ConfigFactoryDefault();
    m_rightMotorFront.ConfigFactoryDefault();
    m_rightMotorBack.ConfigFactoryDefault();

    //Setup up Back motors as followers
    m_leftMotorBack.Follow( m_leftMotorFront );
    m_rightMotorBack.Follow( m_rightMotorFront );

    //Set Inverted
    m_leftMotorFront.SetInverted( true );
    m_leftMotorBack.SetInverted( true );
    m_rightMotorFront.SetInverted( true );
    m_rightMotorBack.SetInverted( true );

    //Set Brake Mode
    m_leftMotorFront.SetNeutralMode(NeutralMode::Brake);
    m_leftMotorBack.SetNeutralMode(NeutralMode::Brake);
    m_rightMotorFront.SetNeutralMode(NeutralMode::Brake);
    m_rightMotorBack.SetNeutralMode(NeutralMode::Brake);        

    //Setup Encoders
    m_leftMotorFront.ConfigSelectedFeedbackSensor(FeedbackDevice::IntegratedSensor,0,10);
    m_leftMotorBack.ConfigSelectedFeedbackSensor(FeedbackDevice::IntegratedSensor,0,10);
    m_rightMotorFront.ConfigSelectedFeedbackSensor(FeedbackDevice::IntegratedSensor,0,10);
    m_rightMotorBack.ConfigSelectedFeedbackSensor(FeedbackDevice::IntegratedSensor,0,10);

    ctre::phoenix::motorcontrol::SupplyCurrentLimitConfiguration falconConfig;
    falconConfig.enable = true;
    falconConfig.currentLimit = 40;
    falconConfig.triggerThresholdCurrent = 60;
    falconConfig.triggerThresholdTime = 0;

    // m_rightMotorBack.ConfigSupplyCurrentLimit(falconConfig,0);
    // m_rightMotorFront.ConfigSupplyCurrentLimit(falconConfig,0);
    // m_leftMotorBack.ConfigSupplyCurrentLimit(falconConfig,0);
    // m_leftMotorFront.ConfigSupplyCurrentLimit(falconConfig,0);
}


void Drivetrain::Drive( double left, double right )
{
    //m_leftMotorFront.Set(ControlMode::PercentOutput, left );
    //m_leftMotorBack.Set(ControlMode::PercentOutput, left );
    //m_rightMotorFront.Set(ControlMode::PercentOutput, right );
    //m_rightMotorBack.Set(ControlMode::PercentOutput, right );
    
    m_differentialDrive.TankDrive(left,right,false);

}


void Drivetrain::Stop(void)
{
    std::cout<<"Drivetrain Stop!"<<std::endl;
    m_differentialDrive.TankDrive(0,0,false);
}


double Drivetrain::GetRightMotor(void)
{
    return -m_rightMotorFront.Get();
}


double Drivetrain::GetLeftMotor(void)
{
	return m_leftMotorFront.Get();
}


void Drivetrain::WriteFalconTemps(void)
{
    frc::SmartDashboard::PutNumber("FalconTemp LF", m_leftMotorFront.GetTemperature() );
    frc::SmartDashboard::PutNumber("FalconTemp LR", m_leftMotorBack.GetTemperature() );
    frc::SmartDashboard::PutNumber("FalconTemp RF", m_rightMotorFront.GetTemperature() );
    frc::SmartDashboard::PutNumber("FalconTemp RR", m_rightMotorBack.GetTemperature() );
}

void Drivetrain::DriveWithGamepad( void )
{
	const double DEADBAND = 0.05;
	
	double yL = Robot::m_oi.GetDriverGamepad()->GetRawAxis(GAMEPADMAP_AXIS_L_Y);
	double xL = Robot::m_oi.GetDriverGamepad()->GetRawAxis(GAMEPADMAP_AXIS_L_X);
	double yR = Robot::m_oi.GetDriverGamepad()->GetRawAxis(GAMEPADMAP_AXIS_R_Y);
	double xR = Robot::m_oi.GetDriverGamepad()->GetRawAxis(GAMEPADMAP_AXIS_R_X); 
	//double tL = Robot::m_oi.GetDriverGamepad()->GetRawAxis(GAMEPADMAP_AXIS_L_TRIG);
	//double tR = Robot::m_oi.GetDriverGamepad()->GetRawAxis(GAMEPADMAP_AXIS_R_TRIG);

	//Control DeadBand
	if (fabs(yL)<= DEADBAND) yL = 0;
	if (fabs(xL)<= DEADBAND) xL = 0;
	if (fabs(yR)<= DEADBAND) yR = 0;
	if (fabs(xR)<= DEADBAND) xR = 0;

	//Subtract off deadband
	if (fabs(yL)>= DEADBAND) yL +=  (yL>0)? -DEADBAND : +DEADBAND;
	if (fabs(xL)>= DEADBAND) xL +=  (xL>0)? -DEADBAND : +DEADBAND;
	if (fabs(yR)>= DEADBAND) yR +=  (yR>0)? -DEADBAND : +DEADBAND;
	if (fabs(xR)>= DEADBAND) xR +=  (xR>0)? -DEADBAND : +DEADBAND;

	// //TankDrive
	// yL *= MAX_DRIVE_POWER;	
	// yR *= MAX_DRIVE_POWER;	
  	// m_differentialDrive.TankDrive( -yL,  -yR,  false);

	//Arcade Drive
	yL *= MAX_DRIVE_POWER;
	xR *= MAX_TURN_POWER;
	m_differentialDrive.ArcadeDrive(-yL,xR,  true);
}


//**************** ENCODERS *********************
int Drivetrain::GetLeftEncoder(void)
{

    return (m_leftMotorFront.GetSelectedSensorPosition(0)  - m_l1_enc_zero);
    //return -(m_leftMotorFront.GetSensorCollection().GetIntegratedSensorPosition()  - m_l1_enc_zero);
	//return -m_leftMotorFront.GetSensorCollection().GetIntegratedSensorPosition();
}

int Drivetrain::GetLeftEncoder2(void)
{
    return (m_leftMotorBack.GetSelectedSensorPosition(0) - m_l2_enc_zero);
    //return -(m_leftMotorBack.GetSensorCollection().GetIntegratedSensorPosition() - m_l2_enc_zero);
	//return -m_leftMotorBack.GetSensorCollection().GetIntegratedSensorPosition();
}

int Drivetrain::GetRightEncoder(void)
{
    return -(m_rightMotorFront.GetSelectedSensorPosition(0) - m_r1_enc_zero);
    //return m_rightMotorFront.GetSensorCollection().GetIntegratedSensorPosition() - m_r1_enc_zero;
	//return m_rightMotorFront.GetSensorCollection().GetIntegratedSensorPosition();
}

int Drivetrain::GetRightEncoder2(void)
{
    return -(m_rightMotorBack.GetSelectedSensorPosition(0) - m_r2_enc_zero);
    //return m_rightMotorBack.GetSensorCollection().GetIntegratedSensorPosition() - m_r2_enc_zero;
	//return m_rightMotorBack.GetSensorCollection().GetIntegratedSensorPosition();
}

void Drivetrain::HardResetEncoders(void)
{

    //** WARNING ***
    //It takes several cycles to perform a hard reset on the Falcons
    //Do not use encoders immediately after a reset
    std::cout<< "Hard Encoder Reset" << std::endl;
    // m_leftMotorFront.GetSensorCollection().SetIntegratedSensorPosition(0);
    // m_leftMotorBack.GetSensorCollection().SetIntegratedSensorPosition(0);
    // m_rightMotorFront.GetSensorCollection().SetIntegratedSensorPosition(0);
    // m_rightMotorBack.GetSensorCollection().SetIntegratedSensorPosition(0);

    m_leftMotorFront.SetSelectedSensorPosition(0);
    m_leftMotorBack.SetSelectedSensorPosition(0);
    m_rightMotorFront.SetSelectedSensorPosition(0);
    m_rightMotorBack.SetSelectedSensorPosition(0);
    //
    m_l1_enc_zero = 0;
    m_l2_enc_zero = 0;
    m_r1_enc_zero = 0;
    m_r2_enc_zero = 0;
}



void Drivetrain::ResetEncoders(void)
{
    std::cout<< "Soft Encoder Reset" << std::endl;
    // m_l1_enc_zero = m_leftMotorFront.GetSensorCollection().GetIntegratedSensorPosition();
    // m_l2_enc_zero = m_leftMotorBack.GetSensorCollection().GetIntegratedSensorPosition();
    // m_r1_enc_zero = m_rightMotorFront.GetSensorCollection().GetIntegratedSensorPosition();
    // m_r2_enc_zero = m_rightMotorBack.GetSensorCollection().GetIntegratedSensorPosition();

    m_l1_enc_zero = m_leftMotorFront.GetSelectedSensorPosition(0);
    m_l2_enc_zero = m_leftMotorBack.GetSelectedSensorPosition(0);
    m_r1_enc_zero = m_rightMotorFront.GetSelectedSensorPosition(0);
    m_r2_enc_zero = m_rightMotorBack.GetSelectedSensorPosition(0);


}

//**************** AHRS (NavX) *********************
bool Drivetrain::IsGyroConnected(void)
{
	return m_ahrs.IsConnected();
}

double Drivetrain::GetGyroYaw(void)
{
    //Returns Relative Yaw:  -180 to +180
	return (double) m_ahrs.GetYaw();  
}

double Drivetrain::GetGyroAngle(void)
{
    //returns total accumulated angle -inf to +inf  (continuous through 360deg)
	return (double)m_ahrs.GetAngle();
}

double Drivetrain::GetGyroRate(void)
{
	return m_ahrs.GetRate();
}
//ed was here
void Drivetrain::ZeroGyro(void)
{
  	std::cout<<"ZeroGyro"<<std::endl;
	m_ahrs.ZeroYaw();
}
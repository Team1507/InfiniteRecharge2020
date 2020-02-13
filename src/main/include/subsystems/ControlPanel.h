#pragma once

#include <frc/commands/Subsystem.h>
#include "ctre/Phoenix.h"
#include "RobotMap.h"
#include <frc/DigitalInput.h>

class ControlPanel : public frc::Subsystem {
    private:
        bool m_isDeployed;

        frc::DigitalInput m_bottomSwitch{0};
        frc::DigitalInput m_topSwitch{1};

        //servo
        WPI_VictorSPX m_deployMotor{CP_DEPLOY_CAN_ID};
        WPI_VictorSPX m_spinnerMotor{CP_SPINNER_CAN_ID};
        //color sensor idek what this is yet
    public:
        ControlPanel();
        void InitDefaultCommand() override;
        bool isTopSwitchPress( void );
        bool isBottomSwitchPress( void );
        bool GetIsDeployed( void );
        void DeployControl(void);
        void RetractControl(void);
        void StopControl(void);     

        int GetColor(void); //return type not determined (most likely 1 red, 2 green etc...)
        void SpinControl(void);
        void StopSpinControl(void);

};

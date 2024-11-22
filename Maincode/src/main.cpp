#include "main.h"
#include "pros/distance.hpp"
#include "pros/imu.hpp"
#include "pros/optical.hpp"

pros::MotorGroup driveleft({-11, -12, -13});
pros::MotorGroup driveright({18, 19, 20});
pros::Controller controller(pros::E_CONTROLLER_MASTER);
pros::IMU imu(3);
pros::Optical optical(5);

//will be too incovenient if these were not global

void on_center_button()
{

}

void initialize()
{
	
}

void disabled()
{

}

void competition_initialize()
{

}

void drive(pros::Distance sensor, int distance)
{
	const int target = sensor.get() - distance; //gets target distance compared to initial distance
	int power = 0;

	while(distance > 2) //drives until error is less than 2mm
	{
		if (distance > 127) //max power
		{
			power = 127;
		}
		else if (distance > 0) //power has a linear relation to distance left
		{
			power = distance / 2 + 64;
		}
		else
		{
			power = distance / 2 - 64;
		}

		driveleft.move(power);
		driveright.move(power);

		distance = sensor.get() - target; //updates distance

		pros::delay(15);
	}
}

void turn(pros::IMU imu, const int heading)
{
	imu.set_rotation(0); //resets rotation to make calculations easier

	int error = heading;
	int power = 0;

	while (std::abs(error) > 1)
	{
		if (error > 127) //max power
		{
			power = 127;
		}
		else if (error < -127)
		{
			power = -127;
		}
		else if (error > 0) //power has a linear relation to degrees left
		{
			power = error / 2 + 64;
		}
		else
		{
			power = error / 2 - 64;
		}

		driveleft.move(power);
		driveright.move(-power);

		error = heading - imu.get_heading(); //updates error

		pros::delay(15);
	}
}

void autonomous ()
{
	
}

class Intake
{
	public: 
		pros::Motor intake;
		short intakeSave;
		short intakeCurrent;

	public:
		Intake (const int port): intake(port), intakeSave(0), intakeCurrent(0) {}

		void registerClick (const short intakeNew)
		{
			this -> intakeSave = this -> intakeCurrent;
			this -> intakeCurrent = intakeNew;
		}

		short direction ()
		{
			if (this -> intakeSave == this -> intakeCurrent)
			{
				this -> intakeCurrent = 0;
			}

			return this -> intakeCurrent;
		}

		void run (const float ladyBrownVelocity)
		{
			if (ladyBrownVelocity < 0)
			{
				this -> intake.move_velocity(-63);
			}
			else
			{
				const short direction = this -> direction();
				this -> intake.move_velocity (direction * 127);
			}
		}
};

void opcontrol()
{
	Intake intake(2);
	pros::Motor ladyBrown(1);
	pros::adi::DigitalOut clamp (8);
	pros::adi::DigitalIn limitSwitch(7);
	int color = 0;
	int speed = 0;
	int turning = 0;
	int intakespinforward = 1;
	float ladyBrownVelocity = 0.0;
	bool intakeSave = true;
	bool intakeCurrent = false;
	bool L1pressed = false;
	bool L2pressed = false;
	bool clampIn = false;
	bool loadLadyBrown = false;
	//definitions for everything else

	ladyBrown.set_brake_mode(pros::E_MOTOR_BRAKE_HOLD);

	while (true)
	{
		if (controller.get_digital_new_press(pros::E_CONTROLLER_DIGITAL_L1)) //intake system
		{
			intake.registerClick(1);
		}
		else if (controller.get_digital_new_press(pros::E_CONTROLLER_DIGITAL_L2))
		{
			intake.registerClick(-1);
		}

		if (controller.get_digital_new_press(pros::E_CONTROLLER_DIGITAL_R1)) //clamp toggle
		{
			clampIn = !clampIn;
		}

		if (controller.get_digital_new_press(pros::E_CONTROLLER_DIGITAL_Y))
		{
			intake.intake.move_relative(-100, -100);
			loadLadyBrown = false;
		}
		else if (controller.get_digital(pros::E_CONTROLLER_DIGITAL_Y))
		{
			ladyBrownVelocity -= 1;
		}
		else if (controller.get_digital(pros::E_CONTROLLER_DIGITAL_RIGHT))
		{
			ladyBrownVelocity += 1;
			loadLadyBrown = false;
		}
		else if (controller.get_digital_new_press(pros::E_CONTROLLER_DIGITAL_R2))
		{
			loadLadyBrown = true;
		}
		
		if (!limitSwitch.get_value() && loadLadyBrown)
		{
			ladyBrownVelocity = -0.5;
		}
		else if ((limitSwitch.get_value() || ladyBrown.get_position() >= 100) && loadLadyBrown)
		{
			ladyBrownVelocity = 0;
			loadLadyBrown = false;
		}

		speed = pow(controller.get_analog(pros::E_CONTROLLER_ANALOG_LEFT_Y), 3) / 16129;
		turning = pow(controller.get_analog(pros::E_CONTROLLER_ANALOG_RIGHT_X), 3) / 16129;
		color = optical.get_hue();
		ladyBrownVelocity = 0;

		ladyBrown.move_velocity(ladyBrownVelocity * 127);
		clamp.set_value(clampIn);
		driveleft.move(speed + turning);
		driveright.move(speed - turning);

		if (color > 330 || color < 30) // sensing red
		{
			intake.run(0);
			pros::delay(10);
		}

		intake.run(ladyBrownVelocity);
		//moves everything
		
		pros::delay(15);
	}
};
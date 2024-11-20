#include "main.h"
#include "pros/distance.hpp"
#include "pros/imu.hpp"

pros::MotorGroup driveleft ({-11, -12, -13});
pros::MotorGroup driveright ({18, 19, 20});
pros::Controller controller(pros::E_CONTROLLER_MASTER);

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

void drive(pros::Distance sensor, const int distance)
{
	const int target = sensor.get() - distance;
	int error = sensor.get() - target;
	int power = 0;

	while(error > 2)
	{
		if (error > 127)
		{
			power = 127;
		}
		else if (error < -127)
		{
			power = -127;
		}
		else if (error > 0)
		{
			power = error / 2 + 64;
		}
		else
		{
			power = error / 2 - 64;
		}

		driveleft.move(power);
		driveright.move(power);

		error = sensor.get() - target;;

		pros::delay(15);
	}
}

void turn(pros::IMU imu, const int heading)
{
	int error = heading - imu.get_heading();
	int power = 0;

	imu.set_rotation(0);

	while (std::abs(error) > 1)
	{
		if (error > 127)
		{
			power = 127;
		}
		else if (error < -127)
		{
			power = -127;
		}
		else if (error > 0)
		{
			power = error / 2 + 64;
		}
		else
		{
			power = error / 2 - 64;
		}

		driveleft.move(power);
		driveright.move(-power);

		error = heading - imu.get_heading();

		pros::delay(15);
	}
}

void autonomous ()
{
	pros::IMU imu (3);
	imu.reset();
}

/**
 * Runs the operator control code. This function will be started in its own task
 * with the default priority and stack size whenever the robot is enabled via
 * the Field Management System or the VEX Competition Switch in the operator
 * control mode.
 *
 * If no competition control is connected, this function will run immediately
 * following initialize().
 *
 * If the robot is disabled or communications is lost, the
 * operator control task will be stopped. Re-enabling the robot will restart the
 * task, not resume it from where it left off.
 */

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
	pros::ADIDigitalOut clamp (8);
	pros::ADIDigitalIn limitSwitch(7);
	int speed = 0;
	int turning = 0;
	int intakespinforward = 1;
	float ladyBrownVelocity = 0.0;
	bool intakeSave = true; //true = L1/forward, false = L2/backward
	bool intakeCurrent = false;
	bool L1pressed = false;
	bool L2pressed = false;
	bool clampIn = false;
	bool loadLadyBrown = false;

	ladyBrown.set_brake_mode(pros::E_MOTOR_BRAKE_HOLD);

	while (true)
	{
		if (controller.get_digital_new_press(pros::E_CONTROLLER_DIGITAL_L1))
		{
			intake.registerClick(1);
		}
		else if (controller.get_digital_new_press(pros::E_CONTROLLER_DIGITAL_L2))
		{
			intake.registerClick(-1);
		}

		if (controller.get_digital_new_press(pros::E_CONTROLLER_DIGITAL_R1))
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

		ladyBrown.move_velocity(ladyBrownVelocity * 127);
		intake.run(ladyBrownVelocity);
		ladyBrownVelocity = 0;
		clamp.set_value(clampIn);
		speed = pow(controller.get_analog(pros::E_CONTROLLER_ANALOG_LEFT_Y), 3) / 16129;
		turning = pow(controller.get_analog(pros::E_CONTROLLER_ANALOG_RIGHT_X), 3) / 16129;
		driveleft.move(speed + turning);
		driveright.move(speed - turning);
		
		pros::delay(15);
	}
};	
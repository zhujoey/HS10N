#include "main.h"

pros::MotorGroup driveleft ({-11, -12, -13});
pros::MotorGroup driveright ({18, 19, 20});

/**
 * A callback function for LLEMU's center button.
 *
 * When this callback is fired, it will toggle line 2 of the LCD text between
 * "I was pressed!" and nothing.
 */
void on_center_button()
{

}

/**
 * Runs initialization code. This occurs as soon as the program is started.
 *
 * All other competition modes are blocked by initialize; it is recommended
 * to keep execution time for this mode under a few seconds.
 */
void initialize()
{

}

/**
 * Runs while the robot is in the disabled state of Field Management System or
 * the VEX Competition Switch, following either autonomous or opcontrol. When
 * the robot is enabled, this task will exit.
 */
void disabled()
{

}

/**
 * Runs after initialize(), and before autonomous when connected to the Field
 * Management System or the VEX Competition Switch. This is intended for
 * competition-specific initialization routines, such as an autonomous selector
 * on the LCD.
 *
 * This task will exit when the robot is enabled and autonomous or opcontrol
 * starts.
 */
void competition_initialize()
{

}

void autonomous()
{

}

void opcontrol()
{
	pros::Controller controller(pros::E_CONTROLLER_MASTER);
	pros::Motor ladyBrown (1);
	pros::Motor intake(2);
	pros::adi::DigitalOut clamp (8);
	int speed = 0;
	int turning = 0;
	bool L1pressed = false;
	bool L2pressed = false;
	bool clampIn = false;
	bool moving = false;
	float ladyBrownVelocity = 0;
	float intakespeed = 0;
	
	ladyBrown.set_brake_mode(pros::E_MOTOR_BRAKE_HOLD);

	while (true)
	{
		if (controller.get_digital_new_press(pros::E_CONTROLLER_DIGITAL_L1))
		{
			if (intakespeed == 1)
			{
				intakespeed = 0;
			}
			else
			{
				intakespeed = 1;
			}
		}

		if (controller.get_digital_new_press(pros::E_CONTROLLER_DIGITAL_L2))
		{
			if (intakespeed == -1)
			{
				intakespeed = 0;
			}
			else
			{
				intakespeed = -1;
			}
		}

		if (controller.get_digital_new_press(pros::E_CONTROLLER_DIGITAL_R1))
		{
			clampIn = !clampIn;
		}

		if (controller.get_digital(pros::E_CONTROLLER_DIGITAL_Y))
		{
			moving = false;
			ladyBrownVelocity = -1;
		}
		else if (controller.get_digital(pros::E_CONTROLLER_DIGITAL_RIGHT))
		{
			moving = false;
			ladyBrownVelocity = 1;
		}
		else if (controller.get_digital(pros::E_CONTROLLER_DIGITAL_R2))
		{
			moving = true;
			intakespeed = 0.5;
			if (ladyBrown.get_position() < 29)
			{
				ladyBrownVelocity = -1;
			}
			else if (ladyBrown.get_position() > 31)
			{
				ladyBrownVelocity = 1;
			}
			else
			{
				ladyBrownVelocity = 0;
				moving = false;
			}
		}
		
		if (!moving)
		{
			ladyBrownVelocity = 0;
		}

		speed = pow(controller.get_analog(pros::E_CONTROLLER_ANALOG_LEFT_Y), 3) / 16129;
		turning = pow(controller.get_analog(pros::E_CONTROLLER_ANALOG_RIGHT_X), 3) / 16129;

		clamp.set_value(clampIn);
		ladyBrown.move_velocity(ladyBrownVelocity * 200);
		intake.move(127 * intakespeed);
		driveleft.move(speed + turning);
		driveright.move(speed - turning);
		
		pros::delay(20);
	}
};
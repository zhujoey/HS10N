#include "main.h"
#include "lemlib/api.hpp"

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
{}

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
{}

/**
 * Runs the user autonomous code. This function will be started in its own task
 * with the default priority and stack size whenever the robot is enabled via
 * the Field Management System or the VEX Competition Switch in the autonomous
 * mode. Alternatively, this function may be called in initialize or opcontrol
 * for non-competition testing purposes.
 *
 * If the robot is disabled or communications is lost, the autonomous task
 * will be stopped. Re-enabling the robot will restart the task, not re-start it
 * from where it left off.
 */

/*
void autonomous() {
	pros::Motor leftFront  ( 13, pros::E_MOTOR_GEAR_BLUE);
	pros::Motor leftMiddle ( 12, pros::E_MOTOR_GEAR_BLUE);
	pros::Motor leftBack   (-11, pros::E_MOTOR_GEAR_BLUE);
	
	pros::Motor rightFront  (-18, pros::E_MOTOR_GEAR_BLUE);
	pros::Motor rightMiddle (-19, pros::E_MOTOR_GEAR_BLUE);
	pros::Motor rightBack   ( 20, pros::E_MOTOR_GEAR_BLUE);

	pros::MotorGroup leftDrive ({leftFront, leftMiddle, leftBack});
	pros::MotorGroup rightDrive ({rightFront, rightMiddle, rightBack});

	// New 3.25” | 3.25 | lemlib::Omniwheel::NEW_325 

	lemlib::Drivetrain drivetrain (&leftDrive, &rightDrive, 12, lemlib::Omniwheel::NEW_325, 600, 2);
	lemlib::ControllerSettings lateralController (10, 0, 3, 3, 1, 100, 3, 500, 20);
	lemlib::ControllerSettings angularController(2, 0, 10, 3, 1, 100, 3, 500, 0);

	pros::IMU imu(1);
	
	lemlib::Chassis chassis(drivetrain, lateralController, angularController, NULL);
	chassis.setPose(0, 0, 0);
	chassis.turnToHeading(90, 100000);
};
*/


void autonomous () {
	driveleft.move(25);
	driveright.move(25);
	pros::delay(1000);
	driveleft.move(0);
	driveright.move(0);
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

class Intake {
	public: 
		pros::Motor intake;
		short intakeSave;
		short intakeCurrent;
	

	public:
		Intake (const int port): intake(port), intakeSave(0), intakeCurrent(0) {}

		void registerClick (const short intakeNew) {
			this -> intakeSave = this -> intakeCurrent;
			this -> intakeCurrent = intakeNew;
		}

		short direction () {
			if (this -> intakeSave == this -> intakeCurrent) {
				this -> intakeCurrent = 0;
			}
			return this -> intakeCurrent;
		}

		void run (const float ladyBrownVelocity) {
			if (ladyBrownVelocity < 0) {
				this -> intake.move_velocity(-63);
			} else {
				const short direction = this -> direction();
				this -> intake.move_velocity (direction * 127);
			}
		}
};

void opcontrol() {
	pros::Controller controller(pros::E_CONTROLLER_MASTER);
	pros::Motor ladyBrown (1);
	ladyBrown.set_brake_mode(pros::E_MOTOR_BRAKE_HOLD);
	//pros::Motor intake(2);
	Intake intake(2);
	pros::IMU imu (10);

	pros::ADIDigitalOut clamp (8);
	pros::ADIDigitalIn limitSwitch(7);

	int speed = 0;
	int turning = 0;
	int intakespinforward = 1;

	bool intakeSave = NULL; //true = L1/forward, false = L2/backward
	bool intakeCurrent = NULL;

	bool L1pressed = false;
	bool L2pressed = false;

	bool clampIn = false;
	float ladyBrownVelocity = 0;
	bool loadLadyBrown = false;

	while (true) {
		// Arcade Drive
		speed = pow(controller.get_analog(pros::E_CONTROLLER_ANALOG_LEFT_Y), 3) / 16129;
		turning = pow(controller.get_analog(pros::E_CONTROLLER_ANALOG_RIGHT_X), 3) / 16129;

		if (controller.get_digital_new_press(pros::E_CONTROLLER_DIGITAL_L1)) {
			intake.registerClick(1);
		}
		if (controller.get_digital_new_press(pros::E_CONTROLLER_DIGITAL_L2)) {
			intake.registerClick(-1);
		}
		if (controller.get_digital_new_press(pros::E_CONTROLLER_DIGITAL_R1)) {
			clampIn = !clampIn;
		}
		if (controller.get_digital(pros::E_CONTROLLER_DIGITAL_Y)) {
			int x = 0;
		}
		if (controller.get_digital_new_press(pros::E_CONTROLLER_DIGITAL_Y)) {
			intake.intake.move_relative(-100, -100);
			loadLadyBrown = false;
		}
		if (controller.get_digital(pros::E_CONTROLLER_DIGITAL_Y)) {
			ladyBrownVelocity -= 1;
		}
		if (controller.get_digital(pros::E_CONTROLLER_DIGITAL_RIGHT)) {
			ladyBrownVelocity += 1;
			loadLadyBrown = false;
		}
		if (controller.get_digital_new_press(pros::E_CONTROLLER_DIGITAL_R2)) {
			loadLadyBrown = true;

		}
		if (!limitSwitch.get_value() && loadLadyBrown) {
			ladyBrownVelocity = -0.5;
		} else if ((limitSwitch.get_value() || ladyBrown.get_position() >= 100) && loadLadyBrown) {
			ladyBrownVelocity = 0;
			loadLadyBrown = false;
		}
		ladyBrown.move_velocity(ladyBrownVelocity * 127);
		intake.run(ladyBrownVelocity);
		ladyBrownVelocity = 0;
		clamp.set_value(clampIn);

		//intake.move(intakespinforward * 127);
		driveleft.move(speed + turning);
		driveright.move(speed - turning);
		
		pros::delay(20);
	}
};
#include "main.h"
#include "lemlib/api.hpp"
#include <cmath>

pros::MotorGroup driveleft ({-11, -12, -13});
pros::MotorGroup driveright ({18, 19, 20});
pros::Controller controller(pros::E_CONTROLLER_MASTER);
pros::ADIDigitalOut clamp (8);
pros::Motor ladyBrown (1);

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

Intake intake(2);
pros::IMU imu(3);

void on_center_button()
{}
void initialize()
{
}

void disabled()
{} 

void competition_initialize()
{}


void autonomous () {

}

void opcontrol() {
	
	ladyBrown.set_brake_mode(pros::E_MOTOR_BRAKE_HOLD);
	//pros::Motor intake(2);
	pros::IMU imu (3);

	pros::ADIDigitalIn limitSwitch(7);

	int speed = 0;
	int turning = 0;
	int intakespinforward = 1;

	bool intakeSave = NULL; //true = L1/forward, false = L2/backward
	bool intakeCurrent = NULL;

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
		if (controller.get_digital_new_press(pros::E_CONTROLLER_DIGITAL_R2)) {
			ladyBrown.move_absolute(0, 200);
		}
		if (controller.get_digital_new_press(pros::E_CONTROLLER_DIGITAL_RIGHT)) {
			ladyBrown.move_absolute(-120, 200);
		}
		if (controller.get_digital_new_press(pros::E_CONTROLLER_DIGITAL_Y)) {
			ladyBrown.move_absolute(-2.5 * 360, 200);
			intake.intake.move_absolute(-360, 200);
		}

		intake.run(ladyBrownVelocity);
		ladyBrownVelocity = 0;
		clamp.set_value(clampIn);

		//intake.move(intakespinforward * 127);
		driveleft.move(speed + turning);
		driveright.move(speed - turning);
		
		pros::delay(20);
	}
}
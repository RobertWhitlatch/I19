#ifndef __MOTOR_DEPOT_H__
#define __MOTOR_DEPOT_H__ 1

#include "Master.h"

#define FULL_OFF 0
#define FULL_ON 4096

//>>>>>>>>>>>>>>>>General<<<<<<<<<<<<<<<<<<<//

void MotorDepot_Init(void);

void suspend_all(void);

void resume_all(void);

void print_all(void);

//>>>>>>>>>>>>>>>>Brushed<<<<<<<<<<<<<<<<<<<//

unsigned int suspend_brushed(unsigned int group, unsigned int channel);

unsigned int resume_brushed(unsigned int group, unsigned int channel);

unsigned int set_decay_brushed(unsigned int group, unsigned int channel, unsigned int decay, bool update);

unsigned int set_speed_brushed(unsigned int group, unsigned int channel, unsigned int speed, bool update);

unsigned int set_direction_brushed(unsigned int group, unsigned int channel, int direction, bool update);

unsigned int set_velocity_brushed(unsigned int group, unsigned int channel, int velocity, bool update);

unsigned int set_all_brushed(unsigned int group, unsigned int channel, unsigned int decay, int direction, unsigned int speed, bool update);

unsigned int update_brushed(unsigned int group, unsigned int channel);

//>>>>>>>>>>>>>>>>Bridged<<<<<<<<<<<<<<<<<<<//

unsigned int suspend_bridged(unsigned int group);

unsigned int resume_bridged(unsigned int group);

unsigned int set_decay_bridged(unsigned int group, unsigned int decay, bool update);

unsigned int set_speed_bridged(unsigned int group, unsigned int speed, bool update);

unsigned int set_direction_bridged(unsigned int group, int direction, bool update);

unsigned int set_velocity_bridged(unsigned int group, int velocity, bool update);

unsigned int set_all_bridged(unsigned int group, unsigned int decay, int direction, unsigned int speed, bool update);

unsigned int update_bridged(unsigned int group);

//>>>>>>>>>>>>>>>>>Servo<<<<<<<<<<<<<<<<<<<<//

unsigned int suspend_servo(unsigned int group, unsigned int line);

unsigned int resume_servo(unsigned int group, unsigned int line);

unsigned int set_bounds_servo(unsigned int group, unsigned int line, unsigned int upper, unsigned int lower);

unsigned int set_position_servo(unsigned int group, unsigned int line, unsigned int position);

//>>>>>>>>>>>>>>>>Stepper<<<<<<<<<<<<<<<<<<<//

unsigned int suspend_stepper(unsigned int group);

unsigned int resume_stepper(unsigned int group);

unsigned int set_num_steps_stepper(unsigned int group, unsigned int num_steps);

unsigned int select_method_stepper(unsigned int group, unsigned int method);

unsigned int move_one_step_stepper(unsigned int group, int direction, bool wait);

unsigned int move_num_steps_blocking_stepper(unsigned int group, unsigned int num_steps, int direction);

// WARNING: While this is nonblocking, the interrupt has a heavy time requirement
unsigned int move_num_steps_nonblocking_stepper(unsigned int group, unsigned int num_steps, int direction);

// WARNING: While this is nonblocking, the interrupt has a heavy time requirement
unsigned int move_continuous_stepper(unsigned int group, unsigned int stepsPerSecond, int direction);

unsigned int stop_continuous_stepper(unsigned int group);

#endif // __MOTOR_DEPOT_H__

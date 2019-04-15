#include "Master.h"

//#define BOARD_0 0
//#define BOARD_1 1
//#define GROUP_0 0
//#define GROUP_1 1
//#define GROUP_2 2
//#define GROUP_3 3
//#define GROUP_4 4
//#define GROUP_5 5
//#define GROUP_6 6
//#define GROUP_7 7

#define BRSH(x,y) depot.group[(x)].config.brushed[(y)]
#define SRV(x,y) depot.group[(x)].config.servo[(y)]
#define STEP(x) depot.group[(x)].config.stepper

#define CHECK(x) \
    if(depot.group[group].status != (x)){ \
        return(ERROR); \
    }

typedef struct Brushed_Config{
    unsigned int speed;
             int direction;
    unsigned int decay;
}Brushed_t;
#define BRUSHED_SIZE sizeof(Brushed_t)*2

typedef struct Servo_Config{
    unsigned int position;
    unsigned int upperBound;
    unsigned int lowerBound;
}Servo_t;
#define SERVO_SIZE sizeof(Servo_t)*4

typedef struct Stepper_Config{
    unsigned int method;
    unsigned int method_size;
    unsigned int state;
    unsigned int numSteps;
    unsigned int stepsPerSecond;
             int direction;
}Stepper_t;
#define STEPPER_SIZE sizeof(Stepper_t)

typedef union Group_Config{
    unsigned int  buff[SERVO_SIZE];
    Brushed_t brushed[2];
    Servo_t   servo[4];
    Stepper_t stepper;
}GroupConfig_t;

typedef struct Group_Master{
    unsigned int status;
    GroupConfig_t config;
}Group_t;

typedef struct MotorDepot_Config{
    unsigned int board_status[2];
    Group_t  group[8];
}MotorDepot_t;

MotorDepot_t depot = {
    .board_status = {
        BOARD_ZERO_STATUS,
        BOARD_ONE_STATUS,
    },
    .group = {
        {.status = GROUP_ZERO_STATUS, },
        {.status = GROUP_ONE_STATUS, },
        {.status = GROUP_TWO_STATUS, },
        {.status = GROUP_THREE_STATUS, },
        {.status = GROUP_FOUR_STATUS, },
        {.status = GROUP_FIVE_STATUS, },
        {.status = GROUP_SIX_STATUS, },
        {.status = GROUP_SEVEN_STATUS, },
    }
};

extern uint32_t SystemCoreClock;

//>>>>>>>>>>>>>>>>General<<<<<<<<<<<<<<<<<<<//

void MotorDepot_Init(void){
    initI2C();
    initGPIO(depot.board_status);
    initPWM(depot.board_status);
    for(int i = 0; i < 8; i++){
        if(depot.group[i].status == BRUSHED){
            for(int j = 0; j < 2; i++){
                depot.group[i].config.brushed[j].direction = DIRECTION_FORWARD;
                depot.group[i].config.brushed[j].decay = DECAY_FAST;
                depot.group[i].config.brushed[j].speed = 0;
            }
        }
        if(depot.group[i].status == SERVO){
            for(int j = 0; j < 4; j++){
                depot.group[i].config.servo[j].lowerBound = 410;
                depot.group[i].config.servo[j].position = 1230;
                depot.group[i].config.servo[j].upperBound = 2048;
            }
        }
        if(depot.group[i].status == STEPPER){
            depot.group[i].config.stepper.method = STANDARD;
            depot.group[i].config.stepper.method_size = 8;
            depot.group[i].config.stepper.state = 0;
            depot.group[i].config.stepper.direction = DIRECTION_FORWARD;
            depot.group[i].config.stepper.stepsPerSecond = 0;
            depot.group[i].config.stepper.numSteps = 400;
        }
    }
}

void suspend_all(void){
    if(depot.board_status[0]){
        disablePWM(0);
    }
    if(depot.board_status[1]){
        disablePWM(1);
    }
}

void resume_all(void){
    if(depot.board_status[0]){
        while(enablePWM(0));
    }
    if(depot.board_status[1]){
        while(enablePWM(1));
    }
}

void print_all(void){
    if(depot.board_status[0]){
        print_gpio_registers(0);
    }
    if(depot.board_status[1]){
        print_gpio_registers(1);
    }
    if(depot.board_status[0]){
        print_pwm_registers(0);
    }
    if(depot.board_status[1]){
        print_pwm_registers(1);
    }
}

//>>>>>>>>>>>>>>>>Brushed<<<<<<<<<<<<<<<<<<<//

unsigned int suspend_brushed(unsigned int group, unsigned int channel){
    CHECK(BRUSHED)
    setChannel(group, channel, FULL_OFF, FULL_OFF);
    return(NO_ERROR);
}

unsigned int resume_brushed(unsigned int group, unsigned int channel){
    CHECK(BRUSHED)
    update_brushed(group, channel);
    return(NO_ERROR);
}

unsigned int set_decay_brushed(unsigned int group, unsigned int channel, unsigned int decay, bool update){
    CHECK(BRUSHED)
    if(decay != DECAY_FAST || decay != DECAY_SLOW){
        return(ERROR);
    }
    BRSH(group, channel).decay = decay;
    if(update){
        update_brushed(group, channel);
    }
    return(NO_ERROR);
}

unsigned int set_speed_brushed(unsigned int group, unsigned int channel, unsigned int speed, bool update){
    CHECK(BRUSHED)
    if(speed > FULL_ON){
        return(ERROR);
    }
    BRSH(group, channel).speed = speed;
    if(update){
        update_brushed(group, channel);
    }
    return(NO_ERROR);
}

unsigned int set_direction_brushed(unsigned int group, unsigned int channel, int direction, bool update){
    CHECK(BRUSHED)
    if(direction != DIRECTION_FORWARD || direction != DIRECTION_BACKWARD){
        return(ERROR);
    }
    BRSH(group, channel).direction = direction;
    if(update){
        update_brushed(group, channel);
    }
    return(NO_ERROR);
}

unsigned int set_velocity_brushed(unsigned int group, unsigned int channel, int velocity, bool update){
    CHECK(BRUSHED)
    int direction = (direction < 0) ? DIRECTION_BACKWARD : DIRECTION_FORWARD;
    velocity *= direction;
    if(velocity > FULL_ON){
        return(ERROR);
    }
    BRSH(group, channel).direction = direction;
    BRSH(group, channel).speed = velocity;
    if(update){
        update_brushed(group, channel);
    }
    return(NO_ERROR);
}

unsigned int set_all_brushed(unsigned int group, unsigned int channel, unsigned int decay, int direction, unsigned int speed, bool update){
    CHECK(BRUSHED)
    int check = 0;
    check += set_decay_brushed(group, channel, decay, false);
    check += set_direction_brushed(group, channel, direction, false);
    check += set_speed_brushed(group, channel, speed, false);
    if(update){
        check += update_brushed(group, channel);
    }
    return((check == 0) ? NO_ERROR : ERROR);
}

unsigned int update_brushed(unsigned int group, unsigned int channel){
    CHECK(BRUSHED)
    if(BRSH(group,channel).speed == 0){
        setChannel(group,channel,0,0);
    }
    if(BRSH(group,channel).direction == DIRECTION_FORWARD){
        if(BRSH(group,channel).decay == DECAY_FAST){
            setChannel(group, channel, BRSH(group,channel).speed, FULL_OFF);
        }else{ // DECAY_SLOW
            int speed_n = 4096 - BRSH(group,channel).speed;
            setChannel(group, channel, FULL_ON, speed_n);
        }
    }else{ // DIRECTION_BACKWARD
        if(BRSH(group,channel).decay == DECAY_FAST){
            setChannel(group, channel, FULL_OFF, BRSH(group, channel).speed);
        }else{ // DECAY_SLOW
            int speed_n = 4096 - BRSH(group,channel).speed;
            setChannel(group, channel, speed_n, FULL_ON);
        }
    }
    return(NO_ERROR);
}

//>>>>>>>>>>>>>>>>>Servo<<<<<<<<<<<<<<<<<<<<//

unsigned int suspend_servo(unsigned int group, unsigned int line){
    CHECK(SERVO)
    setLine(group, line, FULL_OFF);
    return(NO_ERROR);
}

unsigned int resume_servo(unsigned int group, unsigned int line){
    CHECK(SERVO)
    setLine(group, line, SRV(group,line).position);
    return(NO_ERROR);
}

unsigned int set_bounds_servo(unsigned int group, unsigned int line, unsigned int lower, unsigned int upper){
    CHECK(SERVO)
    if(lower < 410){
        return(ERROR);
    }
    if(upper > 2048){
        return(ERROR);
    }
    SRV(group,line).upperBound = upper;
    SRV(group,line).lowerBound = lower;
    return(NO_ERROR);
}

unsigned int set_position(unsigned int group, unsigned int line, unsigned int position){
    CHECK(SERVO)
    if(SRV(group,line).lowerBound <= position && position <= SRV(group,line).upperBound){
        SRV(group,line).position = position;
        return(NO_ERROR);
    }
    return(ERROR);
}

unsigned int move_num_steps_servo(unsigned int group, unsigned int line, unsigned int num_steps, int direction){
    CHECK(SERVO)
    unsigned int position = SRV(group,line).position + (num_steps*direction);
    if(SRV(group,line).lowerBound <= position && position <= SRV(group,line).upperBound){
        SRV(group,line).position = position;
        return(NO_ERROR);
    }
    return(ERROR);
}

//>>>>>>>>>>>>>>>>Stepper<<<<<<<<<<<<<<<<<<<//

const unsigned int states[3][8][4] = {
    { // STANDARD
        {FULL_ON,  FULL_OFF, FULL_OFF, FULL_OFF},
        {FULL_OFF, FULL_OFF, FULL_ON,  FULL_OFF},
        {FULL_OFF, FULL_ON,  FULL_OFF, FULL_OFF},
        {FULL_OFF, FULL_OFF, FULL_OFF,  FULL_ON},
    },
    { // HI_TORQUE
        {FULL_ON,  FULL_OFF, FULL_OFF, FULL_ON },
        {FULL_ON,  FULL_OFF, FULL_ON,  FULL_OFF},
        {FULL_OFF, FULL_ON,  FULL_ON,  FULL_OFF},
        {FULL_OFF, FULL_ON,  FULL_OFF, FULL_ON },
    },
    { // HALF_STEPS
        {FULL_ON,  FULL_OFF, FULL_OFF, FULL_OFF},
        {FULL_ON,  FULL_OFF, FULL_ON,  FULL_OFF},
        {FULL_OFF, FULL_OFF, FULL_ON,  FULL_OFF},
        {FULL_OFF, FULL_ON,  FULL_ON,  FULL_OFF},
        {FULL_OFF, FULL_ON,  FULL_OFF, FULL_OFF},
        {FULL_OFF, FULL_ON,  FULL_OFF, FULL_ON },
        {FULL_OFF, FULL_OFF, FULL_OFF, FULL_ON },
        {FULL_ON,  FULL_OFF, FULL_OFF, FULL_ON },
    }
};

#define STEPPER_STATE states[STEP(group).method][STEP(group).state]
#define ADVANCE_STATE STEP(group).state = (STEP(group).state + direction) % STEP(group).method_size;

unsigned int suspend_stepper(unsigned int group){
    CHECK(STEPPER)
    disableDriver(group);
    return(NO_ERROR);
}

unsigned int resume_stepper(unsigned int group){
    CHECK(STEPPER)
    enableDriver(group);
    return(NO_ERROR);
}

unsigned int set_num_steps_stepper(unsigned int group, unsigned int num_steps){
    CHECK(STEPPER)
    STEP(group).numSteps = num_steps;
    return(NO_ERROR);
}

unsigned int select_stepping_method(unsigned int group, unsigned int method){
    CHECK(STEPPER)
    switch(method){
        case STANDARD:
        case HI_TORQUE:
            STEP(group).method_size = 4;
            break;
        case HALF_STEPS:
            STEP(group).method_size = 8;
            break;
        default:
            return(ERROR);
    }
    STEP(group).method = method;
    return(NO_ERROR);
}

unsigned int move_one_step_stepper(unsigned int group, int direction, bool wait){
    CHECK(STEPPER)
    setGroup(group,STEPPER_STATE);
    ADVANCE_STATE
    if (wait){
        unsigned int j = SystemCoreClock/2000;
        while(j--);
    }
    return(NO_ERROR);
}

unsigned int move_num_steps_blocking(unsigned int group, unsigned int num_steps, int direction){
    CHECK(STEPPER)
    for(int i = 0; i < num_steps; i++){
        move_one_step_stepper(group,direction, true);
    }
    return(NO_ERROR);
}

unsigned int move_num_steps_nonblocking(unsigned int group, unsigned int num_steps, int direction){
    CHECK(STEPPER)
    
    return(NO_ERROR);
}

unsigned int move_continuous_stepper(unsigned int group, unsigned int speed, int direction){
    CHECK(STEPPER)
    
    return(NO_ERROR);
}

#include "Master.h"

#define BRSH(x,y) depot.group[(x)].config.brushed[(y)]
#define BRDG(x) depot.group[(x)].config.bridged
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

typedef struct Bridged_Config{
    unsigned int speed;
             int direction;
    unsigned int decay;
}Bridged_t;
#define BRIDGED_SIZE sizeof(Bridged_t)

typedef struct Servo_Config{
    unsigned int position;
    unsigned int upperBound;
    unsigned int lowerBound;
}Servo_t;
#define SERVO_SIZE sizeof(Servo_t)*4

typedef struct Stepper_Config{
    unsigned int method;
    unsigned int methodSize;
    unsigned int state;
    unsigned int stepsPerSecond;
    unsigned int stepsRemaining;
             int direction;
}Stepper_t;
#define STEPPER_SIZE sizeof(Stepper_t)

typedef union Group_Config{
    unsigned int  buff[SERVO_SIZE];
    Brushed_t brushed[2];
    Bridged_t bridged;
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
    unsigned int freqUpdate[2];
}MotorDepot_t;

MotorDepot_t depot = {
    .board_status = {
        BOARD_ZERO_STATUS,
        BOARD_ONE_STATUS,
    },
    .freqUpdate = {
        BOARD_ZERO_FREQ,
        BOARD_ONE_FREQ,
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
    },
};

unsigned int CS_getMCLK(void);
unsigned int clockHz;

//>>>>>>>>>>>>>>>>General<<<<<<<<<<<<<<<<<<<//

void MotorDepot_Init(void){
    clockHz = CS_getMCLK();
    initI2C();
    initGPIO(depot.board_status);
    initPWM(depot.board_status, depot.freqUpdate);
    for(int i = 0; i < 8; i++){
        if(depot.group[i].status == BRUSHED){
            enableDriver(i);
            for(int j = 0; j < 2; i++){
                BRSH(i,j).direction = DIRECTION_FORWARD;
                BRSH(i,j).decay = DECAY_FAST;
                BRSH(i,j).speed = 0;
            }
        }
        if(depot.group[i].status == BRIDGED){
            enableDriver(i);
            BRDG(i).direction = DIRECTION_FORWARD;
            BRDG(i).decay = DECAY_FAST;
            BRDG(i).speed = 0;
        }
        if(depot.group[i].status == SERVO){
            for(int j = 0; j < 4; j++){
                SRV(i,j).lowerBound = 410;
                SRV(i,j).position = 1230;
                SRV(i,j).upperBound = 2048;
            }
        }
        if(depot.group[i].status == STEPPER){
            enableDriver(i);
            STEP(i).method = STANDARD;
            STEP(i).methodSize = 4;
            STEP(i).state = 0;
            STEP(i).direction = DIRECTION_FORWARD;
            STEP(i).stepsPerSecond = 0;
            STEP(i).stepsRemaining = 0;
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

void printMotorDepot(void){
    for(int i = 0; i < 2; i++){
        fprintf(uart, "Board installed = %d\n", depot.board_status[i]);
        fprintf(uart, "Frequency selected = %d\n", depot.freqUpdate[i]);
        for(int j = 0; j < 4; j++){
            fprintf(uart, "Group %d = %d\n", j, depot.group[j+i*4].status);
        }
    }
}

//>>>>>>>>>>>>>>>>Brushed<<<<<<<<<<<<<<<<<<<//

unsigned int suspend_brushed(unsigned int group, unsigned int channel){
    CHECK(BRUSHED)
    setBrushed(group, channel, FULL_OFF, FULL_OFF);
    return(NO_ERROR);
}

unsigned int resume_brushed(unsigned int group, unsigned int channel){
    CHECK(BRUSHED)
    update_brushed(group, channel);
    return(NO_ERROR);
}

unsigned int set_decay_brushed(unsigned int group, unsigned int channel, unsigned int decay, bool update){
    CHECK(BRUSHED)
    if(decay != DECAY_FAST && decay != DECAY_SLOW){
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
    if(direction != DIRECTION_FORWARD && direction != DIRECTION_BACKWARD){
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
        setBrushed(group, channel, FULL_OFF, FULL_OFF);
    }
    if(BRSH(group,channel).direction == DIRECTION_FORWARD){
        if(BRSH(group,channel).decay == DECAY_FAST){
            setBrushed(group, channel, BRSH(group,channel).speed, FULL_OFF);
        }else{ // DECAY_SLOW
            int speed_n = 4096 - BRSH(group,channel).speed;
            setBrushed(group, channel, FULL_ON, speed_n);
        }
    }else{ // DIRECTION_BACKWARD
        if(BRSH(group,channel).decay == DECAY_FAST){
            setBrushed(group, channel, FULL_OFF, BRSH(group, channel).speed);
        }else{ // DECAY_SLOW
            int speed_n = 4096 - BRSH(group,channel).speed;
            setBrushed(group, channel, speed_n, FULL_ON);
        }
    }
    return(NO_ERROR);
}

//>>>>>>>>>>>>>>>>Bridged<<<<<<<<<<<<<<<<<<<//

unsigned int suspend_bridged(unsigned int group){
    CHECK(BRIDGED)
    disableDriver(group);
    return(NO_ERROR);
}

unsigned int resume_bridged(unsigned int group){
    CHECK(BRIDGED)
    enableDriver(group);
    return(NO_ERROR);
}

unsigned int set_decay_bridged(unsigned int group, unsigned int decay, bool update){
    CHECK(BRIDGED)
    if(decay != DECAY_FAST && decay != DECAY_SLOW){
        return(ERROR);
    }
    BRDG(group).decay = decay;
    if(update){
        update_bridged(group);
    }
    return(NO_ERROR);
}

unsigned int set_speed_bridged(unsigned int group, unsigned int speed, bool update){
    CHECK(BRIDGED)
    if(speed > FULL_ON){
        return(ERROR);
    }
    BRDG(group).speed = speed;
    if(update){
        update_bridged(group);
    }
    return(NO_ERROR);
}

unsigned int set_direction_bridged(unsigned int group, int direction, bool update){
    CHECK(BRIDGED)
    if(direction != DIRECTION_FORWARD && direction != DIRECTION_BACKWARD){
        return(ERROR);
    }
    BRDG(group).direction = direction;
    if(update){
        update_bridged(group);
    }
    return(NO_ERROR);
}

unsigned int set_velocity_bridged(unsigned int group, int velocity, bool update){
    CHECK(BRIDGED)
    int direction = (direction < 0) ? DIRECTION_BACKWARD : DIRECTION_FORWARD;
    velocity *= direction;
    if(velocity > FULL_ON){
        return(ERROR);
    }
    BRDG(group).direction = direction;
    BRDG(group).speed = velocity;
    if(update){
        update_bridged(group);
    }
    return(NO_ERROR);
}

unsigned int set_all_bridged(unsigned int group, unsigned int decay, int direction, unsigned int speed, bool update){
    CHECK(BRIDGED)
    int check = 0;
    check += set_decay_bridged(group, decay, false);
    check += set_direction_bridged(group, direction, false);
    check += set_speed_bridged(group, speed, false);
    if(update){
        check += update_bridged(group);
    }
    return((check == 0) ? NO_ERROR : ERROR);
}

unsigned int update_bridged(unsigned int group){
    CHECK(BRIDGED)
    if(BRDG(group).speed == 0){
        setBridged(group, FULL_OFF, FULL_OFF);
    }
    if(BRDG(group).direction == DIRECTION_FORWARD){
        if(BRDG(group).decay == DECAY_FAST){
            setBridged(group, BRDG(group).speed, FULL_OFF);
        }else{ // DECAY_SLOW
            int speed_n = 4096 - BRDG(group).speed;
            setBridged(group, FULL_ON, speed_n);
        }
    }else{ // DIRECTION_BACKWARD
        if(BRDG(group).decay == DECAY_FAST){
            setBridged(group, FULL_OFF, BRDG(group).speed);
        }else{ // DECAY_SLOW
            int speed_n = 4096 - BRDG(group).speed;
            setBridged(group, speed_n, FULL_ON);
        }
    }
    return(NO_ERROR);
}

//>>>>>>>>>>>>>>>>>Servo<<<<<<<<<<<<<<<<<<<<//

unsigned int suspend_servo(unsigned int group, unsigned int line){
    CHECK(SERVO)
    setServo(group, line, FULL_OFF);
    return(NO_ERROR);
}

unsigned int resume_servo(unsigned int group, unsigned int line){
    CHECK(SERVO)
    setServo(group, line, SRV(group,line).position);
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

unsigned int set_position_servo(unsigned int group, unsigned int line, unsigned int position){
    CHECK(SERVO)
    if(SRV(group,line).lowerBound <= position && position <= SRV(group,line).upperBound){
        SRV(group,line).position = position;
        setServo(group, line, SRV(group,line).position);
        return(NO_ERROR);
    }
    return(ERROR);
}

//>>>>>>>>>>>>>>>>Stepper<<<<<<<<<<<<<<<<<<<//

const unsigned int states[3][8][4] = {
    { // STANDARD
        {0x10, 0x00, 0x00, 0x00},
        {0x00, 0x00, 0x10, 0x00},
        {0x00, 0x10, 0x00, 0x00},
        {0x00, 0x00, 0x00, 0x10},
    },
    { // HI_TORQUE
        {0x10, 0x00, 0x00, 0x10},
        {0x10, 0x00, 0x10, 0x00},
        {0x00, 0x10, 0x10, 0x00},
        {0x00, 0x10, 0x00, 0x10},
    },
    { // HALF_STEPS
        {0x10, 0x00, 0x00, 0x00},
        {0x10, 0x00, 0x10, 0x00},
        {0x00, 0x00, 0x10, 0x00},
        {0x00, 0x10, 0x10, 0x00},
        {0x00, 0x10, 0x00, 0x00},
        {0x00, 0x10, 0x00, 0x10},
        {0x00, 0x00, 0x00, 0x10},
        {0x10, 0x00, 0x00, 0x10},
    }
};

#define STEPPER_STATE states[STEP(group).method][STEP(group).state]
#define ADVANCE_STATE STEP(group).state = (STEP(group).state + direction) % STEP(group).methodSize;

unsigned int timersInUse[4] = {8, 8, 8, 8};

volatile unsigned short* TA[4][4] = {
    {((volatile uint16_t *)0x40000000), ((volatile uint16_t *)0x40000002), ((volatile uint16_t *)0x40000012), ((volatile uint16_t *)0x40000020)},
    {((volatile uint16_t *)0x40000400), ((volatile uint16_t *)0x40000402), ((volatile uint16_t *)0x40000412), ((volatile uint16_t *)0x40000420)},
    {((volatile uint16_t *)0x40000800), ((volatile uint16_t *)0x40000802), ((volatile uint16_t *)0x40000812), ((volatile uint16_t *)0x40000820)},
    {((volatile uint16_t *)0x40000C00), ((volatile uint16_t *)0x40000C02), ((volatile uint16_t *)0x40000C12), ((volatile uint16_t *)0x40000C20)},
};

#define CTL 0
#define CCTL0 1
#define CCR0 2
#define EX0 3

#define NVIC_PRI2_R             (*(volatile uint32_t *)0xE000E408)  // Interrupt 8-11 Priority
#define NVIC_PRI3_R             (*(volatile uint32_t *)0xE000E40C)  // Interrupt 12-15 Priority
#define NVIC_PRI6_R             (*(volatile uint32_t *)0xE000E418)  // Interrupt 24-27 Priority
#define NVIC_PRI7_R             (*(volatile uint32_t *)0xE000E41C)  // Interrupt 28-31 Priority
#define NVIC_EN0_R              (*(volatile uint32_t *)0xE000E100)  // Interrupt 0-31 Set Enable

void (*Task[4])(unsigned int);

unsigned int Timer_Init(void (*task)(unsigned int), unsigned short period, unsigned int timer_num){

    if(timer_num+1 > TIMERS_AVAILABLE_FOR_STEPPERS) {
        return(ERROR);
    }
    Task[timer_num] = task;
    *(TA[timer_num][CTL]) &= ~0x0030;
    *(TA[timer_num][CTL]) = 0x0242;
    *(TA[timer_num][CCTL0]) = 0x0010;
    *(TA[timer_num][CCR0]) = period - 1;
    *(TA[timer_num][EX0]) &= ~0x0007;
    switch(timer_num){
        case 0:
            NVIC_PRI2_R = (NVIC_PRI2_R&0xFFFFFF00)|0x00000040; // priority 2
            NVIC_EN0_R |= 0x00000100;         // interrupt 24/8
            break;
        case 1:
            NVIC_PRI2_R = (NVIC_PRI2_R&0xFF00FFFF)|0x00400000; // priority 2
            NVIC_EN0_R |= 0x00000400;         // interrupt 26/10
            break;
        case 2:
            NVIC_PRI3_R = (NVIC_PRI3_R&0xFFFFFF00)|0x00000040; // priority 2
            NVIC_EN0_R |= 0x00001000;         // interrupt 28/12
            break;
        case 3:
            NVIC_PRI3_R = (NVIC_PRI3_R&0xFF00FFFF)|0x00400000; // priority 2
            NVIC_EN0_R |= 0x00004000;         // interrupt 30/14
            break;
    }
    *(TA[timer_num][CTL]) |= 0x0014;
    return(NO_ERROR);

}

void TA0_0_IRQHandler(void){
    *(TA[0][CCTL0]) &= ~0x0001;
    (*Task[0])(0);
}

void TA1_0_IRQHandler(void){
    *(TA[1][CCTL0]) &= ~0x0001;
    (*Task[1])(1);
}

void TA2_0_IRQHandler(void){
    *(TA[2][CCTL0]) &= ~0x0001;
    (*Task[2])(2);
}

void TA3_0_IRQHandler(void){
    *(TA[3][CCTL0]) &= ~0x0001;
    (*Task[3])(3);
}

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

unsigned int select_method_stepper(unsigned int group, unsigned int method){
    CHECK(STEPPER)
    switch(method){
        case STANDARD:
        case HI_TORQUE:
            STEP(group).methodSize = 4;
            break;
        case HALF_STEPS:
            STEP(group).methodSize = 8;
            break;
        default:
            return(ERROR);
    }
    STEP(group).method = method;
    return(NO_ERROR);
}

unsigned int move_one_step_stepper(unsigned int group, int direction, bool wait){
    CHECK(STEPPER)
    setStepper(group,STEPPER_STATE);
    ADVANCE_STATE
    if (wait){
        unsigned int j = clockHz/500;
        while(j--);
    }
    return(NO_ERROR);
}

void move_one_step_timer(unsigned int timer){
    unsigned int group = timersInUse[timer];
    int direction = STEP(group).direction;
    setStepper(group,STEPPER_STATE);
    ADVANCE_STATE
    STEP(group).stepsRemaining--;
    if(STEP(group).stepsRemaining == 0){
        timersInUse[timer] = 8;
        (*TA[timer][CTL]) &= ~0x0302;
    }
}

unsigned int move_num_steps_blocking_stepper(unsigned int group, unsigned int num_steps, int direction){
    CHECK(STEPPER)
    for(int i = 0; i < num_steps; i++){
        move_one_step_stepper(group, direction, true);
    }
    return(NO_ERROR);
}

unsigned int move_num_steps_nonblocking_stepper(unsigned int group, unsigned int num_steps, int direction){
    CHECK(STEPPER)
    unsigned int i;
    for(i = 0; i < TIMERS_AVAILABLE_FOR_STEPPERS; i++){
        if(timersInUse[i] == group){
            return(ERROR);
        }
    }
    for(i = 0; i < TIMERS_AVAILABLE_FOR_STEPPERS; i++){
        if(timersInUse[i] == 8){
            break;
        }
    }
    if(i == TIMERS_AVAILABLE_FOR_STEPPERS){
        return(ERROR);
    }
    STEP(group).stepsRemaining = num_steps;
    STEP(group).direction = direction;
    timersInUse[i] = group;
    Timer_Init(move_one_step_timer, clockHz/(500*4), i);
    return(NO_ERROR);
}


void move_one_step_continuous(unsigned int timer){
    unsigned int group = timersInUse[timer];
    int direction = STEP(group).direction;
    setStepper(group,STEPPER_STATE);
    ADVANCE_STATE
}

unsigned int move_continuous_stepper(unsigned int group, unsigned int stepsPerSecond, int direction){
    CHECK(STEPPER)
    unsigned int i;
    for(i = 0; i < TIMERS_AVAILABLE_FOR_STEPPERS; i++){
        if(timersInUse[i] == group){
            return(ERROR);
        }
    }
    for(i = 0; i < TIMERS_AVAILABLE_FOR_STEPPERS; i++){
        if(timersInUse[i] == 8){
            break;
        }
    }
    if(i == TIMERS_AVAILABLE_FOR_STEPPERS){
        return(ERROR);
    }
    STEP(group).stepsPerSecond = stepsPerSecond;
    STEP(group).direction = direction;
    timersInUse[i] = group;
    Timer_Init(move_one_step_continuous, clockHz/(stepsPerSecond*2), i);
    return(NO_ERROR);
}

unsigned int stop_continuous_stepper(unsigned int group){
    CHECK(STEPPER)
    int i;
    for(i = 0; i < TIMERS_AVAILABLE_FOR_STEPPERS; i++){
        if(timersInUse[i] == group){
            break;
        }
    }
    if(i == TIMERS_AVAILABLE_FOR_STEPPERS){
        return(ERROR);
    }
    timersInUse[i] = 8;
    (*TA[i][CTL]) &= ~0x0302;
    return(NO_ERROR);
}


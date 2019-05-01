// CarMain.c

#include "Master.h"
#include "interrupt.h"

uint8_t leftMessage;
uint8_t rightMessage;
uint16_t leftSpeed;
uint16_t rightSpeed;
uint16_t translationTable[32] = {
       0, 1796, 1876, 1956,
    2036, 2116, 2196, 2276,
    2356, 2436, 2516, 2596,
    2676, 2656, 2736, 2816,
    2896, 2976, 3056, 3136,
    3216, 3296, 3376, 3456,
    3536, 3616, 3696, 3776,
    3856, 3936, 4016, 4096,
};

void CarMain(void){

    MotorDepot_Init();
    xBee_Init();
    Interrupt_enableMaster();

    while(1){

        xBee_InCmd(&leftMessage, &rightMessage);

        leftSpeed = translationTable[SPEED(leftMessage)];

        rightSpeed = translationTable[SPEED(rightMessage)];

        set_all_bridged(3, DECAY_FAST, SIGN(leftMessage),  leftSpeed,  UPDATE_BUILD);
        set_all_bridged(2, DECAY_FAST, SIGN(leftMessage),  leftSpeed,  UPDATE_BUILD);
        set_all_bridged(1, DECAY_FAST, SIGN(rightMessage), rightSpeed, UPDATE_BUILD);
        set_all_bridged(0, DECAY_FAST, SIGN(rightMessage), rightSpeed, UPDATE_BUILD);
        write_board_buffer(0);

//        uint8_t leftSign = (SIGN(leftMessage) == PLUS) ? 'F' : 'R';
//        uint8_t rightSign = (SIGN(rightMessage) == MINUS) ? 'F' : 'R';
//        fprintf(uart, "L %c 0x%04X\tR %c 0x%04X\n", leftSign, leftSpeed, rightSign, rightSpeed);

    }

}

// CarMain.c

#include "Master.h"
#include "interrupt.h"

void CarMain(void){

    uint8_t leftMessage;
    uint8_t rightMessage;
    uint16_t leftSpeed;
    uint16_t rightSpeed;

    MotorDepot_Init();
    xBee_Init();
    Interrupt_enableMaster();

    while(1){

        xBee_InCmd(&leftMessage, &rightMessage);

        // leftSpeed =  ( (SPEED(leftMessage)<<6) + ( (SPEED(leftMessage) == 0) ? 0x00 : 0x3F)) & 0x0FFF;
        leftSpeed = SPEED(leftMessage) << 6;
        if(leftSpeed != 0){
            leftSpeed += 0x3F;
        }
        if(leftSpeed >= 0x0FFF){
            leftSpeed = FULL_ON;
        }

        // rightSpeed = ((SPEED(rightMessage)<<6) + ((SPEED(rightMessage) == 0) ? 0x00 : 0x3F)) & 0x0FFF;
        rightSpeed = SPEED(rightMessage) << 6;
        if(rightSpeed != 0){
            rightSpeed += 0x3F;
        }
        if(rightSpeed >= 0x0FFF){
            rightSpeed = FULL_ON;
        }

        set_all_bridged(3, DECAY_SLOW, SIGN(leftMessage),  leftSpeed,  UPDATE_BUILD);
        set_all_bridged(2, DECAY_SLOW, SIGN(leftMessage),  leftSpeed,  UPDATE_BUILD);
        set_all_bridged(1, DECAY_SLOW, SIGN(rightMessage), rightSpeed, UPDATE_BUILD);
        set_all_bridged(0, DECAY_SLOW, SIGN(rightMessage), rightSpeed, UPDATE_BUILD);
        write_board_buffer(0);

//        fprintf(uart, "0x%02X\t0x%02X\n", leftMessage, rightMessage);

//        uint8_t leftSign = (SIGN(leftMessage) == PLUS) ? 'F' : 'R';
//        uint8_t rightSign = (SIGN(rightMessage) == MINUS) ? 'F' : 'R';
//        fprintf(uart, "L %c 0x%04X\tR %c 0x%04X\n", leftSign, leftSpeed, rightSign, rightSpeed);

//        set_all_bridged(3, DECAY_SLOW, DIRECTION_FORWARD, FULL_ON, UPDATE_BUILD);
//        set_all_bridged(2, DECAY_SLOW, DIRECTION_FORWARD, FULL_ON, UPDATE_BUILD);
//        set_all_bridged(1, DECAY_SLOW, DIRECTION_FORWARD, FULL_ON, UPDATE_BUILD);
//        set_all_bridged(0, DECAY_SLOW, DIRECTION_FORWARD, FULL_ON, UPDATE_BUILD);
//        write_board_buffer(0);

    }

}

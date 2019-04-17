#ifndef __MOTORDEPOTCONFIG_H__
#define __MOTORDEPOTCONFIG_H__ 1

#define NOT_IN_USE  0x0000
#define INSTALLED   0x0001
#define UNUSED      0x0000
#define BRUSHED     0x0001
#define BRIDGED     0x0002
#define SERVO       0x0003
#define STEPPER     0x0004

#define NO_ERROR 0x0
#define ERROR 0x1

#define DECAY_FAST 0
#define DECAY_SLOW 1

#define DIRECTION_FORWARD 1
#define DIRECTION_BACKWARD -1

#define STANDARD 0
#define HI_TORQUE 1
#define HALF_STEPS 2

#define LOWEST_FREQ 24 // Hz,
#define STANDARD_FREQ 200 // Hz, Stepper speed severally hampered
#define STEPPER_FOCUSED 1000 // Hz, Does not support Servos
#define FASTEST_UPDATE 1526 // Hz, Use when only Brushed or Bridged

#define BOARD_ZERO_STATUS   INSTALLED
#define BOARD_ZERO_FREQ     FASTEST_UPDATE

#define GROUP_ZERO_STATUS   BRIDGED
#define GROUP_ONE_STATUS    BRIDGED
#define GROUP_TWO_STATUS    BRIDGED
#define GROUP_THREE_STATUS  BRIDGED

#define BOARD_ONE_STATUS    NOT_IN_USE
#define BOARD_ONE_FREQ      NOT_IN_USE

#define GROUP_FOUR_STATUS   NOT_IN_USE
#define GROUP_FIVE_STATUS   NOT_IN_USE
#define GROUP_SIX_STATUS    NOT_IN_USE
#define GROUP_SEVEN_STATUS  NOT_IN_USE

#endif

from MotorLibrary import *
from time import sleep

depot = MotorDepot(1000, 200)

bridged = depot.get_group(0, 'Bridged')
bridged_0 = bridged[0]
bridged_0.set_speed(4096, update=True)

brushed = depot.get_group(1, 'Brushed')
brushed_0 = brushed[0]
brushed_1 = brushed[1]
brushed_0.set_speed(2048, update=True)
brushed_1.set_speed(4096, update=True)

servo = depot.get_group(2, 'Servo')
servo_0 = servo[0]
servo_1 = servo[1]
servo_2 = servo[2]

stepper = depot.get_group(3, 'Stepper')
stepper_0 = stepper[0]

flag = True
i = 412
j = 2048
while True:
    try:
        servo_0.set_position(i)
        servo_1.set_position(i)
        servo_2.set_position(i)
        sleep(1)
        servo_0.set_position(j)
        servo_1.set_position(j)
        servo_2.set_position(j)
        sleep(1)
        stepper_0.move_num_steps(200, 'CW')
        sleep(1)
        if flag:
            i = i + 1
            j = j - 1
        else:
            i = i - 1
            j = j - 1
        if i == 1230:
            flag = False
        if i == 410:
            flag = True
    except KeyboardInterrupt:
        depot.close_depot()
        exit()







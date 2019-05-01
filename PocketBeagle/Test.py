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
servo_1 = servo[1]

stepper = depot.get_group(3, 'Stepper')
stepper_0 = stepper[0]

flag = True
i = 500
j = 2000
while True:
    try:
        servo_1.set_position(i)
        sleep(3)
        servo_1.set_position(j)
        sleep(3)
        stepper_0.move_num_steps(200, 'CW')
        sleep(1)
    except KeyboardInterrupt:
        depot.close_depot()
        exit()







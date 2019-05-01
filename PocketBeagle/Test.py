from MotorLibrary import *
from time import sleep

depot = MotorDepot(200, 200)

bridged = depot.get_group(0, 'Bridged')
bridged_0 = bridged[0]
bridged_0.set_speed(4096, update=True)

brushed = depot.get_group(1, 'Brushed')
brushed_0 = brushed[0]
brushed_1 = brushed[1]
brushed_0.set_speed(2048, update=True)
brushed_1.set_speed(4096, update=True)

servo_0, servo_1, servo_2, servo_3 = depot.get_group(2, 'Servo')
servo_0.set_position(1230)
servo_1.set_position(1230)
servo_2.set_position(1230)
servo_3.set_position(1230)

stepper = depot.get_group(3, 'Stepper')
stepper_0 = stepper[0]
stepper_0.spin_standard('CW')

while True:
    try:
        servo_0.set_position(500)
        servo_1.set_position(500)
        servo_2.set_position(500)
        sleep(2)
        servo_0.set_position(2000)
        servo_1.set_position(2000)
        servo_2.set_position(2000)
        sleep(2)
    except KeyboardInterrupt:
        depot.close_depot()
        exit()







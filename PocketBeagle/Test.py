from MotorLibrary import *

depot = MotorDepot(1000, 200)

bridged = depot.get_group(0, 'Bridged')
bridged_0 = bridged[0]
bridged_0.set_speed(2048, update=True)

bridged = depot.get_group(1, 'Bridged')
bridged_1 = bridged[0]
bridged_1.set_speed(4096, update=True)

brushed_0, brushed_1 = depot.get_group(7, 'Brushed')
brushed_0.set_speed(2048, update=True)
brushed_1.set_speed(4096, update=True)

servo = depot.get_group(4, 'Servo')
servo_0 = servo[0]

servo = depot.get_group(5, 'Servo')
servo_1 = servo[0]

servo = depot.get_group(6, 'Servo')
servo_2 = servo[0]

stepper = depot.get_group(3, 'Stepper')
stepper_0 = stepper[0]

while True:
    servo_0.set_position(410)
    servo_1.set_position(410)
    servo_2.set_position(410)
    stepper_0.move_num_stepper(200, 'CW')
    sleep(3)
    servo_0.set_position(2048)
    servo_1.set_position(2048)
    servo_2.set_position(2048)
    stepper_0.move_num_stepper(200, 'CW')
    sleep(3)






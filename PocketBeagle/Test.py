from MotorLibrary import *

depot = MotorDepot()

brushed = depot.get_group(0, 'Brushed')
zero = brushed[0]
one = brushed[1]

while True:

    input('Press any key to start group 0...')

    zero.set_all(2048, 'forward', 'fast')
    one.set_all(2048, 'forward', 'fast')

    input('Press any key to stop group 0...')

    zero.set_speed(0)
    one.set_speed(0)


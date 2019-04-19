from MotorLibrary import *
import Adafruit_GPIO.I2C as I2C
from time import sleep


def value_valid(x, low, high):
    while low > x > high:
        x = int(input('Value invalid, specify a value within [' + str(low) + '-' + str(high) + ']\n>'))
    return x


def value_valid_escape(x, low, high, escape):
    while low[0] > x > high[0] & low[1] > x > high[1]:
        if x >= escape:
            return -1
        x = int(input('Value invalid, specify a value within ['
                      + str(low) + '-' + str(high) + '] (' + str(escape) + ' or greater to cancel)\n>'))
    return x


def brushed_menu(brushed_depot):
    print('Welcome to the Brushed Menu')
    brushed_group = value_valid(int(input('Please specify a Group to target. [0-7]\n>')), 0, 7)
    brushed_channel = value_valid(int(input('Please specify a Channel to target. [0-1]\n>')), 0, 1)
    brushed_list = brushed_depot.get_group(brushed_group, 'Brushed')
    while True:
        command = input('Brushed>')
        if command == '1':
            x = value_valid(int(input('Please specify a Power Value. [0-4096]\n>')), 0, 4096)
            brushed_list[brushed_channel].set_all(x, 'forward', 'fast', True)
        elif command == '2':
            x = value_valid(int(input('Please specify a Power Value. [0-4096]\n>')), 0, 4096)
            brushed_list[brushed_channel].set_all(x, 'forward', 'slow', True)
        elif command == '3':
            x = value_valid(int(input('Please specify a Power Value. [0-4096]\n>')), 0, 4096)
            brushed_list[brushed_channel].set_all(x, 'reverse', 'fast', True)
        elif command == '4':
            x = value_valid(int(input('Please specify a Power Value. [0-4096]\n>')), 0, 4096)
            brushed_list[brushed_channel].set_all(x, 'reverse', 'slow', True)
        elif command == '0':
            brushed_list[brushed_channel].set_speed(0, True)
        elif command == 'x' or command == 'X':
            x = value_valid(int(input('Please specify a Group to target. [0-7]\n>')), 0, 7)
            y = value_valid(int(input('Please specify a Channel to target. [0-1]\n>')), 0, 1)
            brushed_depot.release_group(brushed_group, 'Brushed')
            brushed_group = x
            brushed_channel = y
            brushed_list = brushed_depot.get_group(brushed_group, 'Brushed')
        elif command == 'h' or command == 'H':
            print('1 - Fire Motor Forward with Fast Decay')
            print('2 - Fire Motor Forward with Slow Decay')
            print('3 - Fire Motor Backward with Fast Decay')
            print('4 - Fire Motor Backward with Slow Decay')
            print('0 - Stop Motor')
            print('x - Retarget')
            print('h - Help')
            print('q - Return to Main Menu')
        elif command == 'q' or command == 'Q':
            brushed_depot.release_group(brushed_group, 'Brushed')
            break


def bridged_menu(bridged_depot):
    print('Welcome to the Bridged Menu')
    bridged_group = value_valid(int(input('Please specify a Group to target. [0-7]\n>')), 0, 7)
    bridged_list = bridged_depot.get_group(bridged_group, 'Bridged')
    while True:
        command = input('Bridged>')
        if command == '1':
            x = value_valid(int(input('Please specify a Power Value. [0-4096]\n>')), 0, 4096)
            bridged_list[0].set_all(x, 'forward', 'fast', True)
        elif command == '2':
            x = value_valid(int(input('Please specify a Power Value. [0-4096]\n>')), 0, 4096)
            bridged_list[0].set_all(x, 'forward', 'slow', True)
        elif command == '3':
            x = value_valid(int(input('Please specify a Power Value. [0-4096]\n>')), 0, 4096)
            bridged_list[0].set_all(x, 'reverse', 'fast', True)
        elif command == '4':
            x = value_valid(int(input('Please specify a Power Value. [0-4096]\n>')), 0, 4096)
            bridged_list[0].set_all(x, 'reverse', 'slow', True)
        elif command == '0':
            bridged_list[0].set_speed(0, True)
        elif command == 'x' or command == 'X':
            x = value_valid(int(input('Please specify a Group to target. [0-7]\n>')), 0, 7)
            bridged_depot.release_group(bridged_group, 'Bridged')
            bridged_group = x
            bridged_list = bridged_depot.get_group(bridged_group, 'Bridged')
        elif command == 'h' or command == 'H':
            print('1 - Fire Motor Forward with Fast Decay')
            print('2 - Fire Motor Forward with Slow Decay')
            print('3 - Fire Motor Backward with Fast Decay')
            print('4 - Fire Motor Backward with Slow Decay')
            print('0 - Stop Motor')
            print('x - Retarget')
            print('h - Help')
            print('q - Return to Main Menu')
        elif command == 'q' or command == 'Q':
            bridged_depot.release_group(bridged_group, 'Bridged')
            break


def servo_menu(servo_depot):
    print('Welcome to the Servo Menu')
    servo_group = value_valid(int(input('Please specify a Group to target. [0-7]\n>')), 0, 7)
    servo_line = value_valid(int(input('Please specify a Line to target. [0-3]\n>')), 0, 3)
    servo_list = servo_depot.get_group(servo_group, 'Servo')
    while True:
        command = input('Servo>')
        if command == '1':
            x = value_valid(int(input('Specify the lower limit of the Servo\'s range. [410-2048]\n>')), 410, 2048)
            y = value_valid(int(input('Specify the upper limit of the Servo\'s range. [' + str(x) + '-2048]\n>')), x, 2048)
            servo_list[servo_line].set_bounds(x, y)
        elif command == '2':
            a = servo_list[servo_line].get_lower()
            b = servo_list[servo_line].get_upper()
            x = value_valid(int(input('Specify new position for Servo. (' + str(a) + '-' + str(b) + ')\n>')), a, b)
            servo_list[servo_line].set_position(x)
        elif command == '3':
            a = servo_list[servo_line].get_lower()
            b = servo_list[servo_line].get_upper()
            servo_list[servo_line].set_position(a)
            sleep(1)
            servo_list[servo_line].set_position(b)
            sleep(1)
            servo_list[servo_line].set_position(a)
        elif command == 'x' or command == 'X':
            x = value_valid(int(input('Please specify a Group to target. [0-7]\n>')), 0, 7)
            y = value_valid(int(input('Please specify a Line to target. [0-3]\n>')), 0, 3)
            servo_depot.release_group(servo_group, 'Servo')
            servo_group = x
            servo_line = y
            servo_list = servo_depot.get_group(servo_group, 'Servo')
        elif command == 'h' or command == 'H':
            print('1 - Set Servo Bounds')
            print('2 - Set Servo Position')
            print('3 - Sweep Servo')
            print('x - Retarget')
            print('h - Help')
            print('q - Return to Main Menu')
        elif command == 'q' or command == 'Q':
            servo_depot.release_group(servo_group, 'Servo')
            return


def stepper_menu(stepper_depot):
    print('Welcome to the Stepper Menu')
    stepper_group = value_valid(int(input('Please specify a Group to target. [0-7]\n>')), 0, 7)
    stepper_list = stepper_depot.get_group(stepper_group, 'Stepper')
    while True:
        command = input('Stepper>')
        if command == '1':
            x = value_valid(int(input('Specify number of steps to move Stepper. [1-65535]\n>')), 1, 65535)
            stepper_list[0].move_num_steps(x, 'CW')
        elif command == '2':
            x = value_valid(int(input('Specify number of steps to move Stepper. [1-65535]\n>')), 1, 65535)
            stepper_list[0].move_num_steps(x, 'CCW')
        elif command == '3':
            x = stepper_list[0].get_stepping_method()
            if x == 'standard':
                stepper_list[0].spin_standard('CW')
            elif x == 'torque':
                stepper_list[0].spin_torque('CW')
            elif x == 'half':
                stepper_list[0].spin_half('CW')
        elif command == '4':
            x = stepper_list[0].get_stepping_method()
            if x == 'standard':
                stepper_list[0].spin_standard('CCW')
            elif x == 'torque':
                stepper_list[0].spin_torque('CCW')
            elif x == 'half':
                stepper_list[0].spin_half('CCW')
        elif command == '7':
            stepper_list[0].select_stepping_method('standard')
        elif command == '8':
            stepper_list[0].select_stepping_method('torque')
        elif command == '9':
            stepper_list[0].select_stepping_method('half')
        elif command == '0':
            stepper_list[0].stop_spinning()
        elif command == 'x' or command == 'X':
            x = value_valid(int(input('Please specify a Group to target. [0-7]\n>')), 0, 7)
            stepper_depot.release_group(stepper_group, 'Stepper')
            stepper_group = x
            stepper_list = stepper_depot.get_group(stepper_group, 'Stepper')
        elif command == 'h' or command == 'H':
            print('1 - Move X steps CW')
            print('2 - Move X steps CCw')
            print('3 - Spin CW')
            print('4 - Spin CCW')
            print('7 - Select Standard Stepping Method')
            print('8 - Select Hi-Torque Stepping Method')
            print('9 - Select Half-Step Stepping Method')
            print('0 - Stop Motor')
            print('x - Retarget')
            print('h - Help')
            print('q - Return to Main Menu')
        elif command == 'q' or command == 'Q':
            stepper_depot.release_group(stepper_group, 'Stepper')
            return


def register_menu():
    gpio = [I2C.get_i2c_device(0x20, 0x02), I2C.get_i2c_device(0x21, 0x02)]
    pwm = [I2C.get_i2c_device(0x40, 0x02), I2C.get_i2c_device(0x41, 0x02)]
    print('Welcome to the Register Menu')
    board = value_valid(int(input('Please specify a Board to target. [0-1]\n>')), 0, 1)
    while True:
        command = input('Register>')
        if command == '1':
            print('GPIO Contents Board ' + str(board))
            for x in range(4):
                print("0x", end="")
                print("%.2X" % x, "=", end=" ")
                print("0x", end="")
                print("%.2X" % gpio[board].readU8(x))
        elif command == '2':
            print('PWM Contents Board ' + str(board))
            for x in range(255):
                if x < 0x46 or x > 0xF9:
                    print("0x", end="")
                    print("%.2X" % x, "=", end=" ")
                    print("0x", end="")
                    print("%.2X" % pwm[board].readU8(x))
        elif command == '3':
            x = value_valid_escape(int(input('Enter GPIO register number to set. [0-3] (256 or greater to cancel)\n>')),
                                   [0, 0], [3, 3], 256)
            if x == -1:
                break
            y = value_valid_escape(int(input('Enter value to set. [0-255] (256 or greater to cancel)\n>')),
                                   [0, 0], [255, 255], 256)
            if y == -1:
                break
            gpio[board].write8(x, y)
            print('Register 0x', end='')
            print('%.2X' % x, '=', end=' ')
            print('0x', end='')
            print('%.2X' % gpio[board].readU8(x))
        elif command == '4':
            x = value_valid_escape(
                int(input('Enter PWM register number to set. [0-45]U[250-254] (256 or greater to cancel)\n>')),
                [0, 250], [45, 254], 256)
            if x == -1:
                break
            y = value_valid_escape(int(input('Enter value to set. [0-255] (256 or greater to cancel)\n>')),
                                   [0, 0], [255, 255], 256)
            if y == -1:
                break
            pwm[board].write8(x, y)
            print('Register 0x', end='')
            print('%.2X' % x, '=', end=' ')
            print('0x', end='')
            print('%.2X' % pwm[board].readU8(x))
        elif command == 'x' or command == 'X':
            board = value_valid(int(input('Please specify a board to target. [0-1]\n>')), 0, 1)
        elif command == 'h' or command == 'H':
            print('1 - Read GPIO Registers')
            print('2 - Read PWM Registers')
            print('3 - Write GPIO Registers')
            print('4 - Write PWM Registers')
            print('x - Retarget')
            print('h - Help')
            print('q - Return to Main Menu')
        elif command == 'q' or command == 'Q':
            return


if __name__ == '__main__':

    depot = MotorDepot(1000, 200)

    print('Welcome to MotorDepot Console')

    while True:
        cmd = input('>')
        if cmd == 'b' or cmd == 'B':
            brushed_menu(depot)
        elif cmd == 'd' or cmd == 'D':
            bridged_menu(depot)
        elif cmd == 's' or cmd == 'S':
            servo_menu(depot)
        elif cmd == 't' or cmd == 't':
            stepper_menu(depot)
        elif cmd == 'z' or cmd == 'z':
            register_menu()
        elif cmd == 'h' or cmd == 'H':
            print('b - Brushed Menu')
            print('d - Bridged Menu')
            print('s - Servo Menu')
            print('t - Stepper Menu')
            print('z - Register Menu')
            print('h - Help')
            print('q - Quit')
        elif cmd == 'q' or cmd == 'Q':
            exit(0)

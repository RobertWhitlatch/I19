import Adafruit_GPIO.I2C as I2C

cmd = ''
escape = False
reg, reg_num, value, value_num = None, None, None, None
gpio = I2C.get_i2c_device(0x20, 0x02)
pwm = I2C.get_i2c_device(0x40, 0x02)

while True:

    cmd = input('>')

    if cmd == 'd' or cmd == 'D':
        print("PWM Contents\tGPIO Contents")
        for x in range(255):
            if x < 0x46 or x > 0xF9:
                print("0x", end="")
                print("%.2X" % x, "=", end=" ")
                print("0x", end="")
                if x < 4:
                    print("%.2X" % pwm.readU8(x), end='\t')
                    print("0x", end="")
                    print("%.2X" % x, "=", end=" ")
                    print("0x", end="")
                    print("%.2X" % gpio.readU8(x))
                else:
                    print("%.2X" % pwm.readU8(x))

    elif cmd == 'g' or cmd == 'G':

        while True:

            g = input('>>')

            if g == 'd' or g == 'D':
                print("GPIO Contents")
                for x in range(4):
                    print("0x", end="")
                    print("%.2X" % x, "=", end=" ")
                    print("0x", end="")
                    print("%.2X" % gpio.readU8(x))

            elif g == 'r' or g == 'R':

                while True:
                    reg = input('Enter GPIO register number to read. (Hex, 0x00-0x03, q to break)\n')
                    if reg == '' or reg == 'q' or reg == 'Q':
                        escape = True
                        break
                    reg_num = int(reg, 16)
                    if reg_num < 0x00 or reg_num > 0x03:
                        print('Invalid Register')
                        continue
                    break

                if escape:
                    escape = False
                    continue

                print('Register 0x', end='')
                print('%.2X' % reg_num, '=', end=' ')
                print('0x', end='')
                print('%.2X' % gpio.readU8(reg_num))

            elif g == 's' or g == 'S':

                while True:
                    reg = input('Enter GPIO register number to set. (Hex, 0x00-0x03, q to break)\n')
                    if reg == '' or reg == 'q' or reg == 'Q':
                        escape = True
                        break
                    reg_num = int(reg, 16)
                    if reg_num < 0x00 or reg_num > 0x03:
                        print('Invalid Register')
                        continue
                    break

                if escape:
                    escape = False
                    continue

                while True:
                    value = input('Enter value to set. (Hex, 0x00-0xFF, q to break)\n')
                    if value == '' or value == 'q' or value == 'Q':
                        escape = True
                        break
                    value_num = int(value, 16)
                    if value_num < 0x00 or value_num > 0xFF:
                        print('Invalid Value')
                        continue
                    break

                if escape:
                    escape = False
                    continue

                gpio.write8(reg_num, value_num)
                print('Register 0x', end='')
                print('%.2X' % reg_num, '=', end=' ')
                print('0x', end='')
                print('%.2X' % gpio.readU8(reg_num))

            elif g == 'h' or g == 'H':
                print('h - Help Menu')
                print('d - Dump GPIO Registers')
                print('r - Read GPIO Registers')
                print('s - Set GPIO Registers')
                print('q - Back')

            elif g == 'q' or g == 'Q':
                break

            else:
                print('Invalid Selection')
                continue

    elif cmd == 'h' or cmd == 'H':
        print('h - Help Menu')
        print('i - Info')
        print('d - Dump All Registers')
        print('g - GPIO Menu')
        print('p - PWM Menu')
        print('q - Quit')

    elif cmd == 'i' or cmd == 'I':
        x = gpio.readList(0x00, 4)
        for i in range(4):
            y = (x[0] >> i) & 0x1
            status = 'off\n'
            if y == 1:
                status = 'on\n'
            print('Motor Driver', i, 'is', status, end='')

    elif cmd == 'p' or cmd == 'P':

        while True:

            p = input('>>')

            if p == 'd' or p == 'D':
                print("PWM Contents")
                for x in range(255):
                    if x < 0x46 or x > 0xF9:
                        print("0x", end="")
                        print("%.2X" % x, "=", end=" ")
                        print("0x", end="")
                        print("%.2X" % pwm.readU8(x))

            elif p == 'r' or p == 'R':

                while True:
                    reg = input('Enter PWM register number to read. (Hex, 0x00-0x45, 0xFA-0xFF, q to break)\n')
                    if reg == '' or reg == 'q' or reg == 'Q':
                        escape = True
                        break
                    reg_num = int(reg, 16)
                    if reg_num < 0x00 or reg_num > 0x45:
                        if reg_num < 0xFA or reg_num > 0xFE:
                            print('Invalid Register')
                            continue
                    break

                if escape:
                    escape = False
                    continue

                print('Register 0x', end='')
                print('%.2X' % reg_num, '=', end=' ')
                print('0x', end='')
                print('%.2X' % pwm.readU8(reg_num))

            elif p == 's' or p == 'S':

                while True:
                    reg = input('Enter PWM register number to set. (Hex, 0x00-0x45, 0xFA-0xFF, q to break)\n')
                    if reg == '' or reg == 'q' or reg == 'Q':
                        escape = True
                        break
                    reg_num = int(reg, 16)
                    if reg_num < 0x00 or reg_num > 0x45:
                        if reg_num < 0xFA or reg_num > 0xFE:
                            print('Invalid Register')
                            continue
                    break

                if escape:
                    escape = False
                    continue

                while True:
                    value = input('Enter value to set. (Hex, 0x00-0xFF, q to break)\n')
                    if value == '' or value == 'q' or value == 'Q':
                        escape = True
                        break
                    value_num = int(value, 16)
                    if value_num < 0x00 or value_num > 0xFF:
                        print('Invalid Value')
                        continue
                    break

                if escape:
                    escape = False
                    continue

                pwm.write8(reg_num, value_num)
                print('Register 0x', end='')
                print('%.2X' % reg_num, '=', end=' ')
                print('0x', end='')
                print('%.2X' % pwm.readU8(reg_num))

            elif p == 'h' or p == 'H':
                print('h - Help Menu')
                print('d - Dump PWM Registers')
                print('r - Read PWM Registers')
                print('s - Set PWM Registers')
                print('q - Back')

            elif p == 'q' or p == 'Q':
                break

            else:
                print('Invalid Selection')
                continue

    elif cmd == 'q' or cmd == 'Q':
        break

    else:
        print('Invalid Selection')
        continue

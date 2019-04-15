import sys
from time import sleep

import Adafruit_GPIO.I2C as I2C

_pwm_on_l = 0
_pwm_on_h = 1
_pwm_off_l = 2
_pwm_off_h = 3


def _print_gpio_registers(gpio):
    print('GPIO Contents')
    for x in range(4):
        print('0x', end='')
        print('%.2X' % x, '=', end=' ')
        print('0x', end='')
        print('%.2X' % gpio.readU8(x))


def _print_pwm_registers(pwm):
    print('PWM Contents')
    for x in range(255):
        if x < 0x46 or x > 0xF9:
            print('0x', end='')
            print('%.2X' % x, '=', end=' ')
            print('0x', end='')
            print('%.2X' % pwm.readU8(x))


class MotorDepot(object):

    def __init__(self, bus=2):
        self._bus = bus
        self._board_status = [0, 0]
        self._gpio_addresses = [0x20, 0x21]
        self._pwm_addresses = [0x40, 0x41]
        self._gpio = [None, None]
        self._pwm = [None, None]
        self._groups = [None, None, None, None, None, None, None, None]
        self._group_status = [0, 0, 0, 0, 0, 0, 0, 0]
        for i in range(2):
            self._check_active_addresses(i, bus)

    def _check_active_addresses(self, i, bus):
        x, y = None, None
        try:
            self._gpio[i] = I2C.get_i2c_device(self._gpio_addresses[i], bus)
            self._pwm[i] = I2C.get_i2c_device(self._pwm_addresses[i], bus)
            x = self._gpio[i].readU8(0x00)
            y = self._pwm[i].readU8(0x00)
        except OSError:
            self._gpio[i] = None
            self._pwm[i] = None
            self._board_status[i] = 0
        finally:
            if x is not None and y is not None:
                self._board_status[i] = 1
                self._gpio[i].write8(0x03, 0xE0)
                self._gpio[i].write8(0x01, 0xE0)
                self._pwm[i].write8(0x00, 0x00)
                for z in range(2, 6):
                    self._pwm[i].write8(z, 0x00)

    def get_group(self, group_num, group_config):
        if group_num < 4 and self._board_status[0] == 0:
            group_num += 4
        if self.is_group_available(group_num) is True:
            if group_config is 'Brushed':
                self._group_status[group_num] = 1
                self._groups[group_num] = [Brushed(self._pwm[group_num//4], group_num, 0),
                                           Brushed(self._pwm[group_num//4], group_num, 1)]
                x = self._gpio[group_num//4].readU8(0x01)
                x = x | (0x01 << (group_num % 4))
                self._gpio[group_num//4].write8(0x01, x)
            elif group_config is 'Servo':
                self._group_status[group_num] = 2
                self._groups[group_num] = [Servo(self._pwm[group_num//4], group_num, 0),
                                           Servo(self._pwm[group_num//4], group_num, 1),
                                           Servo(self._pwm[group_num//4], group_num, 2),
                                           Servo(self._pwm[group_num//4], group_num, 3)]
            elif group_config is 'Stepper':
                self._group_status[group_num] = 3
                self._groups[group_num] = [Stepper(self._pwm[group_num//4], self._gpio[group_num//4], group_num)]
                x = self._gpio[group_num//4].readU8(0x01)
                x = x | (0x01 << (group_num % 4))
                self._gpio[group_num//4].write8(0x01, x)
            else:
                print('Invalid Group Config')
                return None
            return self._groups[group_num]
        else:
            if group_num > 4 and self._board_status[0] == 0:
                group_num -= 4
            print('Group Number ', group_num, ' is already in use\n', end='')
            return None

    def release_group(self, group_num, group_config):
        if group_num < 4 and self._board_status[0] == 0:
            group_num += 4
        if self.is_group_available(group_num) is False:
            if group_config is 'Brushed':
                for x in self._groups[group_num]:
                    for y in range(sys.getrefcount(x)):
                        del y
                x = self._gpio[group_num//4].readU8(0x01)
                x = x & ~(0x1 << (group_num % 4))
                self._gpio[group_num//4].write8(0x01, x)
                self._groups[group_num] = None
                self._group_status[group_num] = 0
            elif group_config is 'Servo':
                for x in self._groups[group_num]:
                    for y in range(sys.getrefcount(x)):
                        del y
                self._groups[group_num] = None
                self._group_status[group_num] = 0
            elif group_config is 'Stepper':
                for x in self._groups[group_num]:
                    for y in range(sys.getrefcount(x)):
                        del y
                x = self._gpio[group_num//4].readU8(0x01)
                x = x & ~(0x1 << (group_num % 4))
                self._gpio[group_num//4].write8(0x01, x)
                self._groups[group_num] = None
                self._group_status[group_num] = 0
            else:
                print('Invalid Group Config')
                return None
        else:
            if group_num > 4 and self._board_status[0] == 0:
                group_num -= 4
            print('Group Number ', group_num, 'is not in use\n', end='')
            return None

    def is_group_available(self, group_num):
        if group_num < 4 and self._board_status[0] == 0:
            group_num += 4
        if self._group_status[group_num] == 0:
            return True
        else:
            return False

    def num_groups_available(self):
        y = 0
        for x in range(8):
            if self._group_status[x] == 0 and self._board_status[x//4] == 1:
                y = y + 1
        return y

    def all_suspend(self):
        for i in range(2):
            if self._board_status[i]:
                x = self._gpio[i].readU8(0x01)
                x |= 0x10
                self._gpio[i].write8(0x01, x)

    def all_resume(self):
        for i in range(2):
            if self._board_status[i]:
                x = self._gpio[i].readU8(0x01)
                x &= ~0x10
                self._gpio[i].write8(0x01, x)

    def get_existing_group(self, group_num):
        if group_num < 4 and self._board_status[0] == 0:
            group_num += 4
        return self._groups[group_num]

    def num_boards_connected(self):
        return self._board_status[0] + self._board_status[1]

    def _print_all(self):
        for i in range(2):
            if self._board_status[i]:
                _print_gpio_registers(I2C.get_i2c_device(self._gpio_addresses[i], self._bus))
                _print_pwm_registers(I2C.get_i2c_device(self._pwm_addresses[i], self._bus))


class Brushed(object):

    def __init__(self, i2c, group_num, channel_num):
        self._forward_reg = [None, None, None, None]
        self._reverse_reg = [None, None, None, None]
        self._i2c = i2c
        self._group_num = group_num
        self._channel_num = channel_num
        self._speed = None
        self._direction = None
        self._decay = None
        x = 6 + (group_num % 4)*16 + channel_num*8
        for i in range(4):
            self._forward_reg[i] = x + i
            self._reverse_reg[i] = x + i + 4
        self._i2c.write8(self._forward_reg[_pwm_off_l], 0x00)
        self._i2c.write8(self._forward_reg[_pwm_off_h], 0x00)
        self._i2c.write8(self._reverse_reg[_pwm_off_l], 0x00)
        self._i2c.write8(self._reverse_reg[_pwm_off_h], 0x00)

    def suspend(self):
        f = self._i2c.readU8(self._forward_reg[_pwm_off_h])
        r = self._i2c.readU8(self._reverse_reg[_pwm_off_h])
        f = f | 0x10
        r = r | 0x10
        self._i2c.write8(self._forward_reg[_pwm_off_h], f)
        self._i2c.write8(self._reverse_reg[_pwm_off_h], r)

    def resume(self):
        f = self._i2c.readU8(self._forward_reg[_pwm_off_h])
        r = self._i2c.readU8(self._reverse_reg[_pwm_off_h])
        f = f & 0xEF
        r = r & 0xEF
        self._i2c.write8(self._forward_reg[_pwm_off_h], f)
        self._i2c.write8(self._reverse_reg[_pwm_off_h], r)

    def set_decay(self, decay, update=True):
        if decay is not 'fast' and decay is not 'slow':
            print("Invalid decay specified, please use 'fast' or 'slow'")
        else:
            self._decay = decay
        if update:
            self.update()

    def set_direction(self, direction, update=True):
        if direction is not 'forward' and direction is not 'reverse':
            print('Invalid direction specified, please use "forward" or "reverse"')
        else:
            self._direction = direction
        if update:
            self.update()

    def set_speed(self, speed, update=True):
        if speed > 4095 or speed < 0:
            print('Invalid speed, please use values between 0 and 4095, inclusive')
        else:
            self._speed = speed
        if update:
            self.update()

    def set_velocity(self, velocity, update=True):
        if velocity < -4095 or velocity > 4095:
            print('Invalid velocity, please use values between -4095 and 4095, inclusive')
        else:
            if velocity < 0:
                self._direction = 'reverse'
                self._speed = -velocity
            elif velocity > 0:
                self._direction = 'forward'
                self._speed = velocity
            else:
                self._speed = velocity
        if update:
            self.update()

    def set_all(self, speed=None, direction=None, decay=None, update=True):
        if speed is not None:
            self.set_speed(speed, False)
        if direction is not None:
            self.set_direction(direction, False)
        if decay is not None:
            self.set_decay(decay, False)
        if update:
            self.update()

    def update(self):
        if self._speed == 0:
            self._i2c.write8(self._forward_reg[_pwm_on_l], 0x00)
            self._i2c.write8(self._forward_reg[_pwm_on_h], 0x00)
            self._i2c.write8(self._forward_reg[_pwm_off_l], 0x00)
            self._i2c.write8(self._forward_reg[_pwm_off_h], 0x10)
            self._i2c.write8(self._reverse_reg[_pwm_on_l], 0x00)
            self._i2c.write8(self._reverse_reg[_pwm_on_h], 0x00)
            self._i2c.write8(self._reverse_reg[_pwm_off_l], 0x00)
            self._i2c.write8(self._reverse_reg[_pwm_off_h], 0x10)
            return

        if self._direction is 'forward':
            if self._decay is 'fast':
                self._i2c.write8(self._reverse_reg[_pwm_off_l], 0x00)
                self._i2c.write8(self._reverse_reg[_pwm_off_h], 0x10)
                self._i2c.write8(self._forward_reg[_pwm_off_l], (self._speed & 0xFF))
                self._i2c.write8(self._forward_reg[_pwm_off_h], ((self._speed & 0xF00) >> 8))
            else:
                speed_n = 4095 - self._speed
                self._i2c.write8(self._forward_reg[_pwm_on_l], 0x00)
                self._i2c.write8(self._forward_reg[_pwm_on_h], 0x10)
                self._i2c.write8(self._reverse_reg[_pwm_off_l], (speed_n & 0xFF))
                self._i2c.write8(self._reverse_reg[_pwm_off_h], ((speed_n & 0xF00) >> 8))

        else:
            if self._decay is 'fast':
                self._i2c.write8(self._forward_reg[_pwm_off_l], 0x00)
                self._i2c.write8(self._forward_reg[_pwm_off_h], 0x10)
                self._i2c.write8(self._reverse_reg[_pwm_off_l], (self._speed & 0xFF))
                self._i2c.write8(self._reverse_reg[_pwm_off_h], ((self._speed & 0xF00) >> 8))
            else:
                speed_n = 4095 - self._speed
                self._i2c.write8(self._reverse_reg[_pwm_on_l], 0x00)
                self._i2c.write8(self._reverse_reg[_pwm_on_h], 0x10)
                self._i2c.write8(self._forward_reg[_pwm_off_l], (speed_n & 0xFF))
                self._i2c.write8(self._forward_reg[_pwm_off_h], ((speed_n & 0xF00) >> 8))


class Servo(object):

    def __init__(self, i2c, group_num, line_num):
        self._i2c = i2c
        self._group_num = group_num
        self._line_num = line_num
        self._reg = [None, None, None, None]
        self._position = 1230
        self._lower_bound = 410
        self._upper_bound = 2048
        x = 6 + (group_num % 4)*16 + line_num*4
        for i in range(4):
            self._reg[i] = x + i
            self._i2c.write8(x+i, 0x00)

    def set_bounds(self, lower, upper):
        if lower < 410:
            print("Lower bound should not be less then 410.")
            return
        if upper > 2048:
            print("Upper bound should not be more then 2048.")
            return
        self._lower_bound = lower
        self._upper_bound = upper

    def suspend(self):
        x = self._i2c.readU8(self._reg[_pwm_off_h])
        x = x | 0x10
        self._i2c.write8(self._reg[_pwm_off_h], x)

    def resume(self):
        x = self._i2c.readU8(self._reg[_pwm_off_h])
        x = x & 0xEF
        self._i2c.write8(self._reg[_pwm_off_h], x)

    def set_position(self, position):
        if position != 0:
            if position < self._lower_bound or position > self._upper_bound:
                print('Invalid position, please use values between ', self._lower_bound, ' and ', self._upper_bound, ' inclusive, or 0.\n', end='')
        self._position = position
        self._i2c.write8(self._reg[_pwm_off_l], (self._position & 0xFF))
        self._i2c.write8(self._reg[_pwm_off_h], ((self._position & 0xF00) >> 8))


class Stepper(object):

    def __init__(self, i2c, gpio, group_num):
        self._i2c = i2c
        self._gpio = gpio
        self._group_num = group_num
        self._registers = [None, None, None, None]
        self._standard_states = [[0x10, 0x00, 0x00, 0x00], [0x00, 0x00, 0x10, 0x00], [0x00, 0x10, 0x00, 0x00], [0x00, 0x00, 0x00, 0x10]]
        self._torque_states = [[0x10, 0x00, 0x00, 0x10], [0x10, 0x00, 0x10, 0x00], [0x00, 0x10, 0x10, 0x00], [0x00, 0x10, 0x00, 0x10]]
        self._half_states = [[0x10, 0x00, 0x00, 0x00], [0x10, 0x00, 0x10, 0x00], [0x00, 0x00, 0x10, 0x00], [0x00, 0x10, 0x10, 0x00],
                             [0x00, 0x10, 0x00, 0x00], [0x00, 0x10, 0x00, 0x10], [0x00, 0x00, 0x00, 0x10], [0x10, 0x00, 0x00, 0x10]]
        self._selected_states = self._half_states
        self._selected_size = 8
        self._active_state = 0
        x = 7 + (group_num % 4) * 4
        for j in range(4):
            self._registers[j] = x + j*4
        for j in range(16):
            self._i2c.write8(x+j, 0x00)

    def suspend(self):
        x = self._gpio.readU8(0x01)
        x &= ~(1 << (self._group_num % 4))
        self._gpio.write8(0x01, x)
    
    def resume(self):
        x = self._gpio.readU8(0x01)
        x |= (1 << (self._group_num % 4))
        self._gpio.write8(0x01, x)

    def select_step_method(self, method):
        if method == 'standard':
            self._selected_states = self._standard_states
            self._selected_size = 4
        elif method == 'torque':
            self._selected_states = self._torque_states
            self._selected_size = 4
        elif method == 'half':
            self._selected_states = self._half_states
            self._selected_size = 8
        else:
            print('Supported stepping methods are "full" and "half".')

    def move_one_step(self, direction):
        if direction == 'CCW':
            for i in range(4):
                self._i2c.write8(self._registers[i], self._selected_states[self._active_state][i])
            self._active_state = (self._active_state + 1) % self._selected_size
        elif direction == 'CW':
            for i in range(4):
                self._i2c.write8(self._registers[i], self._selected_states[self._selected_size - 1 - self._active_state][i])
            self._active_state = (self._active_state + 1) % self._selected_size
        else:
            print('Invalid direction, please use "CW" or "CCW"')
            return
        sleep(1.0 / 2000.0)

    def move_num_steps(self, num_steps, direction):
        for i in range(num_steps):
            self.move_one_step(direction)

    def stop_spinning(self):
        self.suspend()
        for i in range(16):
            self._i2c.write8(0x06+i, 0x00)
        self.resume()

    def spin_standard(self, direction):
        load = [[0x00, 0x00, 0xFF, 0x03], [0x00, 0x08, 0xFF, 0x0B], [0x00, 0x04, 0xFF, 0x07], [0x00, 0x0C, 0xFF, 0x0F]]
        reg = 6 + 16*(self._group_num % 4)

        if direction == 'CCW':
            self.suspend()
            for i in range(16):
                self._i2c.write8(reg+i, load[i//4][i % 4])
            self.resume()
        elif direction == 'CW':
            self.suspend()
            for i in range(16):
                self._i2c.write8(reg+i, load[3-i//4][i % 4])
            self.resume()
        else:
            print('Direction must be "CW" or "CCW".')

    def spin_torque(self, direction):
        load = [[0x00, 0x00, 0xFF, 0x07], [0x00, 0x08, 0xFF, 0x0F], [0x00, 0x04, 0xFF, 0x0B], [0x00, 0x0C, 0xFF, 0x03]]
        reg = 6 + 16*(self._group_num % 4)

        if direction == 'CCW':
            self.suspend()
            for i in range(16):
                self._i2c.write8(reg+i, load[i//4][i % 4])
            self.resume()
        elif direction == 'CW':
            self.suspend()
            for i in range(16):
                self._i2c.write8(reg+i, load[3-i//4][i % 4])
            self.resume()
        else:
            print('Direction must be "CW" or "CCW".')

    def spin_half(self, direction):
        load = [[0x00, 0x0E, 0xFF, 0x03], [0x00, 0x06, 0xFF, 0x0B], [0x00, 0x02, 0xFF, 0x07], [0x00, 0x0A, 0xFF, 0x0F]]
        reg = 6 + 16*(self._group_num % 4)

        if direction == 'CCW':
            self.suspend()
            for i in range(16):
                self._i2c.write8(reg+i, load[i//4][i % 4])
            self.resume()
        elif direction == 'CW':
            self.suspend()
            for i in range(16):
                self._i2c.write8(reg+i, load[3-i//4][i % 4])
            self.resume()
        else:
            print('Direction must be "CW" or "CCW".')


if __name__ == '__main__':
    depot = MotorDepot()

    group = depot.get_group(0, 'Stepper')
    zero = group[0]

    zero.select_step_method('half')

    try:
        while True:
            zero.move_num_steps(400, 'CW')
            input('>')
    except KeyboardInterrupt:
        depot.release_group(0, 'Stepper')
        depot.all_suspend()

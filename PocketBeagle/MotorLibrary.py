import sys
from time import sleep

import Adafruit_GPIO.I2C as I2C


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

    # Board Frequency can be set to:
    #     200Hz -> Standard Frequency, Stepper speed severely hampered
    #     1000Hz -> Stepper Focused Frequency, Servos inoperable
    #     1526Hz -> Fastest Frequency, Use if only Brushed and/or Bridged are in use

    def __init__(self, board_zero_freq=200, board_one_freq=200, bus=2):
        self._bus = bus
        self._board_status = [0, 0]
        self._board_freq = [board_zero_freq, board_one_freq]
        self._gpio_addresses = [0x20, 0x21]
        self._pwm_addresses = [0x40, 0x41]
        self._gpio = [None, None]
        self._pwm = [None, None]
        self._groups = [None, None, None, None, None, None, None, None]
        self._group_status = [0, 0, 0, 0, 0, 0, 0, 0]
        if board_zero_freq == 200 | board_zero_freq == 1000 | board_zero_freq == 1526:
            print("Unsupported Frequency supplied for board zero, defaulting to 200Hz")
            self._board_freq[0] = 200
        else:
            self._board_freq[0] = board_zero_freq
        if board_one_freq == 200 | board_one_freq == 1000 | board_one_freq == 1526:
            print("Unsupported Frequency supplied for board one, defaulting to 200Hz")
            self._board_freq[1] = 200
        else:
            self._board_freq[0] = board_zero_freq
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
                x = self._pwm[i].readU8(0x00)
                if x & 0x80 == 0x80:
                    self._pwm[i].write8(0x00, 0x80)
                self._pwm[i].write8(0x00, 0x30)
                prescaler = (round(25000000/(4096*self._board_freq[i])) - 1) & 0xFF
                self._pwm[i].write8(0xFE, prescaler)
                self._pwm[i].write8(0x00, 0x20)
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
            elif group_config is 'Bridged':
                self._group_status[group_num] = 2
                self._groups[group_num] = [Bridged(self._pwm[group_num//4], self._gpio[group_num//4], group_num)]
                x = self._gpio[group_num//4].readU8(0x01)
                x = x | (0x01 << (group_num % 4))
                self._gpio[group_num//4].write8(0x01, x)
            elif group_config is 'Servo':
                self._group_status[group_num] = 3
                self._groups[group_num] = [Servo(self._pwm[group_num//4], group_num, 0),
                                           Servo(self._pwm[group_num//4], group_num, 1),
                                           Servo(self._pwm[group_num//4], group_num, 2),
                                           Servo(self._pwm[group_num//4], group_num, 3)]
            elif group_config is 'Stepper':
                self._group_status[group_num] = 4
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
            elif group_config is 'Bridged':
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

    def __init__(self, pwm, group_num, channel_num):
        self._registers = [0x00, 0x00, 0x00, 0x10, 0x00, 0x00, 0x00, 0x10]
        self._pwm = pwm
        self._group_num = group_num
        self._channel_num = channel_num
        self._speed = None
        self._direction = None
        self._decay = None
        self._base_reg = 6 + (group_num % 4)*16 + channel_num*8

    def _set_power(self, index, power):
        for i in range(4):
            self._registers[index + i] = 0x00
        if power == 0:
            self._registers[index + 3] = 0x10
        elif power == 4096:
            self._registers[index + 1] = 0x10
        else:
            self._registers[index + 2] = power & 0x00FF
            self._registers[index + 3] = (power & 0x0F00) >> 8

    def suspend(self):
        self._set_power(0, 0)
        self._set_power(4, 0)
        self._pwm.writeList(self._base_reg, self._registers)

    def resume(self):
        self.update()

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
        if speed > 4096 or speed < 0:
            print('Invalid speed, please use values between 0 and 4096, inclusive')
        else:
            self._speed = speed
        if update:
            self.update()

    def set_velocity(self, velocity, update=True):
        if velocity < -4096 or velocity > 4096:
            print('Invalid velocity, please use values between -4096 and 4096, inclusive')
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
        if self._direction is 'forward':
            if self._decay is 'fast':
                self._set_power(0, self._speed)
                self._set_power(4, 0)
            else:
                speed_n = 4096 - self._speed
                self._set_power(0, 4096)
                self._set_power(4, speed_n)
        else:
            if self._decay is 'fast':
                self._set_power(0, 0)
                self._set_power(4, self._speed)
            else:
                speed_n = 4096 - self._speed
                self._set_power(0, speed_n)
                self._set_power(4, 4096)
        self._pwm.writeList(self._base_reg, self._registers)


class Bridged(object):

    def __init__(self, pwm, gpio, group_num):
        self._registers = [0x00, 0x00, 0x00, 0x10, 0x00, 0x00, 0x00, 0x10,
                           0x00, 0x00, 0x00, 0x10, 0x00, 0x00, 0x00, 0x10]
        self._pwm = pwm
        self._gpio = gpio
        self._group_num = group_num
        self._speed = None
        self._direction = None
        self._decay = None
        self._base_reg = 6 + (group_num % 4)*16

    def _set_power(self, index, power):
        for i in range(4):
            self._registers[index + i] = 0x00
        if power == 0:
            self._registers[index + 3] = 0x10
        elif power == 4096:
            self._registers[index + 1] = 0x10
        else:
            self._registers[index + 2] = power & 0x00FF
            self._registers[index + 3] = (power & 0x0F00) >> 8

    def suspend(self):
        x = self._gpio.readU8(0x01)
        x = x & ~(0x1 << (self._group_num % 4))
        self._gpio.write8(0x01, x)

    def resume(self):
        x = self._gpio.readU8(0x01)
        x = x | (0x1 << (self._group_num % 4))
        self._gpio.write8(0x01, x)

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
        if speed > 4096 or speed < 0:
            print('Invalid speed, please use values between 0 and 4096, inclusive')
        else:
            self._speed = speed
        if update:
            self.update()

    def set_velocity(self, velocity, update=True):
        if velocity < -4096 or velocity > 4096:
            print('Invalid velocity, please use values between -4096 and 4096, inclusive')
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
        if self._direction is 'forward':
            if self._decay is 'fast':
                self._set_power(0, self._speed)
                self._set_power(4, 0)
                self._set_power(8, self._speed)
                self._set_power(12, 0)
            else:
                speed_n = 4096 - self._speed
                self._set_power(0, 4096)
                self._set_power(4, speed_n)
                self._set_power(8, 4096)
                self._set_power(12, speed_n)
        else:
            if self._decay is 'fast':
                self._set_power(0, 0)
                self._set_power(4, self._speed)
                self._set_power(8, 0)
                self._set_power(12, self._speed)
            else:
                speed_n = 4096 - self._speed
                self._set_power(0, speed_n)
                self._set_power(4, 4096)
                self._set_power(8, speed_n)
                self._set_power(12, 4096)
        self._pwm.writeList(self._base_reg, self._registers)


class Servo(object):

    def __init__(self, pwm, group_num, line_num):
        self._pwm = pwm
        self._group_num = group_num
        self._line_num = line_num
        self._registers = [0x00, 0x00, 0x00, 0x10]
        self._position = 1230
        self._lower_bound = 410
        self._upper_bound = 2048
        self._base_reg = 6 + (group_num % 4)*16 + line_num*4

    def _set_power(self, index, power):
        for i in range(4):
            self._registers[index + i] = 0x00
        if power == 0:
            self._registers[index + 3] = 0x10
        elif power == 4096:
            self._registers[index + 1] = 0x10
        else:
            self._registers[index + 2] = power & 0x00FF
            self._registers[index + 3] = (power & 0x0F00) >> 8

    def suspend(self):
        self._set_power(0, 0)
        self._pwm.writeList(self._base_reg, self._registers)

    def resume(self):
        self._set_power(0, self._position)
        self._pwm.writeList(self._base_reg, self._registers)

    def set_bounds(self, lower, upper):
        if lower < 410:
            print("Lower bound should not be less then 410.")
            return
        if upper > 2048:
            print("Upper bound should not be more then 2048.")
            return
        self._lower_bound = lower
        self._upper_bound = upper

    def get_lower_bound(self):
        return self._lower_bound

    def get_upper_bound(self):
        return self._upper_bound

    def get_position(self):
        return self._position

    def set_position(self, position):
        if position is 0:
            self.suspend()
            return
        if position < self._lower_bound or position > self._upper_bound:
            print('Invalid position, please use values between ',
                  self._lower_bound, ' and ', self._upper_bound, ' inclusive, or 0.\n', end='')
        self._position = position
        self._set_power(0, self._position)
        self._pwm.writeList(self._base_reg, self._registers)


class Stepper(object):

    def __init__(self, pwm, gpio, group_num):
        self._pwm = pwm
        self._gpio = gpio
        self._group_num = group_num
        self._registers = [0x00, 0x00, 0x00, 0x10, 0x00, 0x00, 0x00, 0x10,
                           0x00, 0x00, 0x00, 0x10, 0x00, 0x00, 0x00, 0x10]
        self._standard_states = [[4096, 0, 0, 0], [0, 0, 4096, 0],
                                 [0, 4096, 0, 0], [0, 0, 0, 4096]]
        self._torque_states = [[4096, 0, 0, 4096], [4096, 0, 4096, 0],
                               [0, 4096, 4096, 0], [0, 4096, 0, 4096]]
        self._half_states = [[4096, 0, 0, 0], [4096, 0, 4096, 0],
                             [0, 0, 4096, 0], [0, 4096, 4096, 0],
                             [0, 4096, 0, 0], [0, 4096, 0, 4096],
                             [0, 0, 0, 4096], [4096, 0, 0, 4096]]
        self._selected_states = self._standard_states
        self._current_method = 'standard'
        self._selected_size = 4
        self._current_state = 0
        self._current_spin = None
        self._current_direction = None
        self._base_reg = 6 + (group_num % 4)*16

    def _set_power(self, index, power):
        for i in range(4):
            self._registers[index + i] = 0x00
        if power == 0:
            self._registers[index + 3] = 0x10
        elif power == 4096:
            self._registers[index + 1] = 0x10
        else:
            self._registers[index + 2] = power & 0x00FF
            self._registers[index + 3] = (power & 0x0F00) >> 8

    def suspend(self):
        self.stop_spinning()
        x = self._gpio.readU8(0x01)
        x &= ~(1 << (self._group_num % 4))
        self._gpio.write8(0x01, x)
    
    def resume(self):
        x = self._gpio.readU8(0x01)
        x |= (1 << (self._group_num % 4))
        self._gpio.write8(0x01, x)
        if self._current_spin is None:
            for i in range(4):
                self._set_power(i * 4, self._selected_states[self._current_state][i])
            self._pwm.writeList(self._base_reg, self._registers)
        elif self._current_spin == 'standard':
            self.spin_standard(self._current_direction)
        elif self._current_spin == 'torque':
            self.spin_torque(self._current_direction)
        elif self._current_spin == 'half':
            self.spin_half(self._current_direction)

    def select_step_method(self, method):
        if method == 'standard':
            self._current_method = method
            self._selected_states = self._standard_states
            self._selected_size = 4
            self._current_state = self._current_state % 4
        elif method == 'torque':
            self._current_method = method
            self._selected_states = self._torque_states
            self._selected_size = 4
            self._current_state = self._current_state % 4
        elif method == 'half':
            self._current_method = method
            self._selected_states = self._half_states
            self._selected_size = 8
        else:
            print('Supported stepping methods are "full", "torque", and "half".')

    def get_step_method(self):
        return self._current_method

    def move_one_step(self, direction):
        if direction == 'CCW':
            for i in range(4):
                self._set_power(i*4, self._selected_states[self._current_state][i])
            self._current_state = (self._current_state + 1) % self._selected_size
        elif direction == 'CW':
            current_state_n = self._selected_size-1-self._current_state
            for i in range(4):
                self._set_power(i*4, self._selected_states[current_state_n][i])
            self._current_state = (self._current_state + 1) % self._selected_size
        else:
            print('Invalid direction, please use "CW" or "CCW"')
            return
        self._pwm.writeList(self._base_reg, self._registers)
        sleep(1.0 / 2000.0)

    def move_num_steps(self, num_steps, direction):
        for i in range(num_steps):
            self.move_one_step(direction)

    def stop_spinning(self):
        self._current_spin = None
        self._current_direction = None
        for i in range(4):
            self._set_power(i*4, 0)
        self._pwm.writeList(self._base_reg, self._registers)

    def spin_standard(self, direction):
        load = [0x00, 0x00, 0xFF, 0x03, 0x00, 0x08, 0xFF, 0x0B, 0x00, 0x04, 0xFF, 0x07, 0x00, 0x0C, 0xFF, 0x0F]
        load_n = [0x00, 0x0C, 0xFF, 0x0F, 0x00, 0x04, 0xFF, 0x07, 0x00, 0x08, 0xFF, 0x0B, 0x00, 0x00, 0xFF, 0x03]

        if direction == 'CCW':
            self._current_spin = 'standard'
            self._current_direction = direction
            self._pwm.writeList(self._base_reg, load)
        elif direction == 'CW':
            self._current_spin = 'standard'
            self._current_direction = direction
            self._pwm.writeList(self._base_reg, load_n)
        else:
            print('Direction must be "CW" or "CCW".')

    def spin_torque(self, direction):
        load = [0x00, 0x00, 0xFF, 0x07, 0x00, 0x08, 0xFF, 0x0F, 0x00, 0x04, 0xFF, 0x0B, 0x00, 0x0C, 0xFF, 0x03]
        load_n = [0x00, 0x0C, 0xFF, 0x03, 0x00, 0x04, 0xFF, 0x0B, 0x00, 0x08, 0xFF, 0x0F, 0x00, 0x00, 0xFF, 0x07]

        if direction == 'CCW':
            self._current_spin = 'torque'
            self._current_direction = direction
            self._pwm.writeList(self._base_reg, load)
        elif direction == 'CW':
            self._current_spin = 'torque'
            self._current_direction = direction
            self._pwm.writeList(self._base_reg, load_n)
        else:
            print('Direction must be "CW" or "CCW".')

    def spin_half(self, direction):
        load = [0x00, 0x0E, 0xFF, 0x03, 0x00, 0x06, 0xFF, 0x0B, 0x00, 0x02, 0xFF, 0x07, 0x00, 0x0A, 0xFF, 0x0F]
        load_n = [0x00, 0x0A, 0xFF, 0x0F, 0x00, 0x02, 0xFF, 0x07, 0x00, 0x06, 0xFF, 0x0B, 0x00, 0x0E, 0xFF, 0x03]

        if direction == 'CCW':
            self._current_spin = 'half'
            self._current_direction = direction
            self._pwm.writeList(self._base_reg, load)
        elif direction == 'CW':
            self._current_spin = 'half'
            self._current_direction = direction
            self._pwm.writeList(self._base_reg, load_n)
        else:
            print('Direction must be "CW" or "CCW".')

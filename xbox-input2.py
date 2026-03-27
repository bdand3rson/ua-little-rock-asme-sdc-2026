import pygame
import serial
import time

# ---- SETTINGS ----
SERIAL_PORT = '//dev/serial/by-id/usb-Arduino__www.arduino.cc__0042_4343935353635150E260-if00'
BAUD_RATE = 9600
DEADZONE = 0.15

# ---- CONNECT ARDUINO ----
arduino = serial.Serial(SERIAL_PORT, BAUD_RATE, timeout=1)
time.sleep(2)

# ---- INIT CONTROLLER ----
pygame.init()
pygame.joystick.init()

joystick = pygame.joystick.Joystick(0)
joystick.init()

print("Controller connected:", joystick.get_name())

try:
    while True:
        pygame.event.pump()

        # --------------------
        # TANK DRIVE
        # left stick Y = left motor
        # right stick Y = right motor
        # --------------------
        left_input = -joystick.get_axis(1)   # left stick Y
        right_input = -joystick.get_axis(3)  # right stick Y

        # deadzone
        if abs(left_input) < DEADZONE:
            left_input = 0
        if abs(right_input) < DEADZONE:
            right_input = 0

        # scale to PWM
        left_speed = int(left_input * 255)
        right_speed = int(right_input * 255)

        # send to Arduino
        arduino.write(f"DRIVE,{left_speed},{right_speed}\n".encode())

        # --------------------
        # ARM MOTOR
        # --------------------
        if joystick.get_button(4):  # LB
            arduino.write(b"ARM,255\n")
        elif joystick.get_button(5):  # RB
            arduino.write(b"ARM,-255\n")
        else:
            arduino.write(b"ARM,0\n")

        # --------------------
        # SERVOS
        # --------------------

        # Servo 1 (A/B)
        if joystick.get_button(0):
            arduino.write(b"SERVO1,-1\n")
        elif joystick.get_button(1):
            arduino.write(b"SERVO1,1\n")
        else:
            arduino.write(b"SERVO1,0\n")

        # Servo 2 (X/Y)
        if joystick.get_button(3):
            arduino.write(b"SERVO2,-1\n")
        elif joystick.get_button(4):
            arduino.write(b"SERVO2,1\n")
        else:
            arduino.write(b"SERVO2,0\n")

        # Claw (D-pad left/right)
        hat = joystick.get_hat(0)

        if hat == (-1, 0):
            arduino.write(b"CLAW,-1\n")
        elif hat == (1, 0):
            arduino.write(b"CLAW,1\n")
        else:
            arduino.write(b"CLAW,0\n")

        time.sleep(0.05)

except KeyboardInterrupt:
    print("Stopping...")
    arduino.write(b"STOP_ALL\n")
    arduino.close()
    pygame.quit()
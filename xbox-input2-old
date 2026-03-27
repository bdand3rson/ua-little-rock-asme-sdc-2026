import pygame
import serial
import time
import sys

SERIAL_PORT = '/dev/ttyACM0'
BAUD_RATE = 9600
DEADZONE = 0.15

# ---------------------------
# Xbox mapping
# Verify with your controller test script if needed
# ---------------------------
A_BUTTON = 0
B_BUTTON = 1
X_BUTTON = 2
Y_BUTTON = 3
LB_BUTTON = 4
RB_BUTTON = 5

def connect_arduino():
    try:
        arduino = serial.Serial(SERIAL_PORT, BAUD_RATE, timeout=1)
        time.sleep(2)
        print(f"Connected to Arduino on {SERIAL_PORT}")
        return arduino
    except serial.SerialException as e:
        print("ERROR: Could not connect to Arduino.")
        print(e)
        return None

def connect_controller():
    pygame.init()
    pygame.joystick.init()

    if pygame.joystick.get_count() == 0:
        print("ERROR: No controller detected.")
        return None

    joystick = pygame.joystick.Joystick(0)
    joystick.init()
    print(f"Connected to controller: {joystick.get_name()}")
    return joystick

def apply_deadzone(value, deadzone=DEADZONE):
    if abs(value) < deadzone:
        return 0.0
    return value

def clamp(value, minimum, maximum):
    return max(minimum, min(maximum, value))

def safe_write(arduino, message):
    try:
        arduino.write(message.encode())
    except serial.SerialException as e:
        print("ERROR: Serial write failed.")
        print(e)
        raise

arduino = connect_arduino()
if arduino is None:
    sys.exit(1)

joystick = connect_controller()
if joystick is None:
    arduino.close()
    sys.exit(1)

try:
    while True:
        pygame.event.pump()

        if pygame.joystick.get_count() == 0:
            print("ERROR: Controller disconnected.")
            safe_write(arduino, "STOP_ALL\n")
            break

        # ---------------------------
        # Drivetrain: mixed arcade drive
        # left stick Y = throttle
        # right stick X = steering
        # ---------------------------
        throttle = -joystick.get_axis(1)   # up = positive
        steer = joystick.get_axis(3)

        throttle = apply_deadzone(throttle)
        steer = apply_deadzone(steer)

        left = throttle + steer
        right = throttle - steer

        left = clamp(left, -1.0, 1.0)
        right = clamp(right, -1.0, 1.0)

        left_speed = int(left * 255)
        right_speed = int(right * 255)

        safe_write(arduino, f"DRIVE,{left_speed},{right_speed}\n")

        # ---------------------------
        # Arm motor (DC motor)
        # LB = arm up
        # RB = arm down
        # ---------------------------
        if joystick.get_button(LB_BUTTON):
            safe_write(arduino, "ARM,255\n")
        elif joystick.get_button(RB_BUTTON):
            safe_write(arduino, "ARM,-255\n")
        else:
            safe_write(arduino, "ARM,0\n")

        # ---------------------------
        # Servo controls
        # A/B = wrist servo 1
        # X/Y = wrist servo 2
        # D-pad left/right = claw open/close
        # ---------------------------
        if joystick.get_button(A_BUTTON):
            safe_write(arduino, "SERVO1,-1\n")
        elif joystick.get_button(B_BUTTON):
            safe_write(arduino, "SERVO1,1\n")
        else:
            safe_write(arduino, "SERVO1,0\n")

        if joystick.get_button(X_BUTTON):
            safe_write(arduino, "SERVO2,-1\n")
        elif joystick.get_button(Y_BUTTON):
            safe_write(arduino, "SERVO2,1\n")
        else:
            safe_write(arduino, "SERVO2,0\n")

        hat = joystick.get_hat(0)
        if hat == (-1, 0):
            safe_write(arduino, "CLAW,-1\n")
        elif hat == (1, 0):
            safe_write(arduino, "CLAW,1\n")
        else:
            safe_write(arduino, "CLAW,0\n")

        time.sleep(0.05)

except KeyboardInterrupt:
    print("\nProgram stopped by user.")

except serial.SerialException:
    print("Serial communication error.")

except pygame.error as e:
    print("Pygame/controller error:")
    print(e)

except Exception as e:
    print("Unexpected error:")
    print(e)

finally:
    try:
        if arduino and arduino.is_open:
            arduino.write(b"STOP_ALL\n")
            arduino.close()
    except:
        pass

    pygame.quit()
    print("Clean shutdown complete.")
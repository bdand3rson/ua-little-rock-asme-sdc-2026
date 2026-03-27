import pygame
import time

pygame.init()
pygame.joystick.init()

joystick = pygame.joystick.Joystick(0)
joystick.init()

print("Controller:", joystick.get_name())
print("Axes:", joystick.get_numaxes())

while True:
    pygame.event.pump()

    values = []
    for i in range(joystick.get_numaxes()):
        values.append(round(joystick.get_axis(i), 3))

    print(values, end="\r")
    time.sleep(0.05)
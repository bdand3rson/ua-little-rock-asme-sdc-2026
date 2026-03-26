import pygame
import sys

# Initialize pygame
pygame.init()
pygame.joystick.init()

# Check for controller
if pygame.joystick.get_count() == 0:
    print(" No controller detected.")
    pygame.quit()
    sys.exit()

# Setup window
WIDTH, HEIGHT = 800, 400
screen = pygame.display.set_mode((WIDTH, HEIGHT))
pygame.display.set_caption("Xbox Controller Test")

font = pygame.font.Font(None, 36)
clock = pygame.time.Clock()

# Init joystick
controller = pygame.joystick.Joystick(0)
controller.init()

axis_labels = ["LX", "LY", "LT", "RX", "RY", "RT"]
button_labels = ["A", "B", "X", "Y", "LB", "RB", "View", "Menu", "LStick", "RStick", "Xbox"]

while True:
    for event in pygame.event.get():
        if event.type == pygame.QUIT:
            pygame.quit()
            sys.exit()

    pygame.event.pump()

    # Get inputs
    axes = [round(controller.get_axis(i), 2) for i in range(controller.get_numaxes())]
    buttons = [controller.get_button(i) for i in range(controller.get_numbuttons())]
    hat = controller.get_hat(0) if controller.get_numhats() > 0 else (0, 0)

    # Clear screen
    screen.fill((20, 20, 20))

    # Draw axes
    y = 40
    for i, val in enumerate(axes):
        label = axis_labels[i] if i < len(axis_labels) else f"A{i}"
        text = font.render(f"{label}: {val:+.2f}", True, (200, 200, 200))
        screen.blit(text, (40, y))
        y += 30

    # Draw buttons
    y += 20
    for i, val in enumerate(buttons):
        label = button_labels[i] if i < len(button_labels) else f"B{i}"
        color = (0, 255, 100) if val else (180, 180, 180)
        text = font.render(f"{label}: {val}", True, color)
        screen.blit(text, (40, y))
        y += 25

    # Draw hat (D-pad)
    y += 20
    text = font.render(f"Hat: {hat}", True, (150, 200, 255))
    screen.blit(text, (40, y))

    # Refresh screen
    pygame.display.flip()
    clock.tick(20)

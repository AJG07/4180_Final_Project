# coding:utf-8
import pygame
import socket

# White background with black text
BLACK = (0, 0, 0)
WHITE = (255, 255, 255)

# This is a simple class that will help us print to the screen
# It has nothing to do with the joysticks, just outputting the information.
class TextPrint:
    def __init__(self):
        self.reset()
        self.font = pygame.font.Font(None, 30)

    def defprint(self, screen, textString):
        textBitmap = self.font.render(textString, True, BLACK)
        screen.blit(textBitmap, [self.x, self.y])
        self.y += self.line_height

    def reset(self):
        self.x = 10
        self.y = 10
        self.line_height = 15

    def indent(self):
        self.x += 10

    def unindent(self):
        self.x -= 10


pygame.init()

# Set the width and height of the screen [width,height]
size = [400, 200]
screen = pygame.display.set_mode(size)

pygame.display.set_caption("joy test")

# Loop until the user clicks the close button.
done = False

# Used to manage how fast the screen updates
clock = pygame.time.Clock()

# Initialize the joysticks
pygame.joystick.init()

# Get ready to print
textPrint = TextPrint()

axis1_value = 0.0
axis3_value = 0.0
button6_value = 0
button7_value = 0

sampling_interval = 30  # in milliseconds
last_sample_time = pygame.time.get_ticks()

s = socket.socket(socket.AF_INET, socket.SOCK_STREAM)
server_ip = '172.20.10.7'  # '127.0.0.1'#
server_port = 12345
s.connect((server_ip, server_port))
print("connect successfully!")
# -------- Main Program Loop -----------
while not done:
    # EVENT PROCESSING STEP
    for event in pygame.event.get():  # User did something
        if event.type == pygame.QUIT:  # If user clicked close
            done = True  # Flag that we are done so we exit this loop

        # Possible joystick actions: JOYAXISMOTION JOYBALLMOTION JOYBUTTONDOWN JOYBUTTONUP JOYHATMOTION
        if event.type == pygame.JOYBUTTONDOWN:
            print("Joystick button pressed.")
        if event.type == pygame.JOYBUTTONUP:
            print("Joystick button released.")
    
    # DRAWING STEP
    screen.fill(WHITE)
    textPrint.reset()

    # Get count of joysticks
    joystick_count = pygame.joystick.get_count()
    current_time = pygame.time.get_ticks()
    for i in range(joystick_count):
        joystick = pygame.joystick.Joystick(i)
        axis3_value = joystick.get_axis(1)
        axis1_value = joystick.get_axis(3)
        button6_value = joystick.get_button(6)
        button7_value = joystick.get_button(7)
        current_time = pygame.time.get_ticks()

    if current_time - last_sample_time > sampling_interval:
        # Print or use the values as needed
        if button6_value == 1:
            print(f"Axis 1: {axis1_value}, Axis 3: {axis3_value}, Button 6: {button6_value}, Button 7: {button7_value}")
        data_string = f"{axis1_value},{axis3_value},{button6_value},{button7_value}"
        s.sendall(data_string.encode('utf-8'))
        # Reset the timer
        last_sample_time = current_time
    
    textPrint.defprint(screen, "Connect to Controller")
    textPrint.defprint(screen, "")
    textPrint.defprint(screen, "Left Motor speed: {:.2f}".format(-1 * axis1_value))
    textPrint.defprint(screen, "")
    textPrint.defprint(screen, "Right Motor speed: {:.2f}".format(-1 * axis3_value))
    textPrint.defprint(screen, "")
    if button6_value == 1:
        textPrint.defprint(screen, "Servo move left")
    elif button7_value == 1:
        textPrint.defprint(screen, "Servo move right")
    else:
        textPrint.defprint(screen, "Servo don't move")

    # ALL CODE TO DRAW SHOULD GO ABOVE THIS COMMENT
    pygame.display.flip()

    # Limit to 20 frames per second
    clock.tick(20)

# Close the window and quit.
s.close()
pygame.quit()

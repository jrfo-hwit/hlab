import time
import random
from pimoroni_bus import SPIBus
#from picographics import PicoGraphics, DISPLAY_LCD_240X240, PEN_RGB332
from picographics import PicoGraphics, DISPLAY_PICO_DISPLAY_2, PEN_RGB332
# https://github.com/pimoroni/pimoroni-pico/releases/tag/v1.24.0-beta2 download picow-v1.24.0-beta2-pimoroni-micropython.uf2
import lcd

#rst=2 #reset pin = 20 #com idc extender o reset muda para pino 2 usando i2c1
#cs=9
#dc=3
#lcd.LCD(rst,cs,dc)
lcd.LCD() # com periferico lcd ligado direto


#spibus = SPIBus(cs=17, dc=16, sck=18, mosi=19, bl=4)
spibus = SPIBus(cs=17, dc=4, sck=18, mosi=19, bl=9) #com periferico LCD
#spibus = SPIBus(cs, dc, sck=18, mosi=19) #com idc extender
display = PicoGraphics(display=DISPLAY_PICO_DISPLAY_2, bus=spibus, pen_type=PEN_RGB332,rotate=0)

display.set_backlight(1.0)

WIDTH, HEIGHT = display.get_bounds()

# We're creating 100 balls with their own individual colour and 1 BG colour
# for a total of 101 colours, which will all fit in the custom 256 entry palette!


class Ball:
    def __init__(self, x, y, r, dx, dy, pen):
        self.x = x
        self.y = y
        self.r = r
        self.dx = dx
        self.dy = dy
        self.pen = pen


# initialise shapes
balls = []
for i in range(0, 1):
    r = random.randint(0, 10) + 3
    balls.append(
        Ball(
            random.randint(r, r + (WIDTH - 2 * r)),
            random.randint(r, r + (HEIGHT - 2 * r)),
            r,
            (14 - r) / 2,
            (14 - r) / 2,
            display.create_pen(random.randint(0, 255), random.randint(0, 255), random.randint(0, 255)),
        )
    )

BG = display.create_pen(255, 255, 255)

count=1

def create_new_ball(count=1):
    r = random.randint(0, 10) + 3
    balls.append(
        Ball(
            random.randint(r, r + (WIDTH - 2 * r)),
            random.randint(r, r + (HEIGHT - 2 * r)),
            r,
            (14 - r) / 2,
            (14 - r) / 2,
            display.create_pen(random.randint(0, 255), random.randint(0, 255), random.randint(0, 255)),
        )
    )
    count = count + 1
    return count

while True:
    display.set_pen(BG)
    display.clear()

    for ball in balls:
        ball.x += ball.dx
        ball.y += ball.dy

        xmax = WIDTH - ball.r
        xmin = ball.r
        ymax = HEIGHT - ball.r
        ymin = ball.r

        if ball.x < xmin or ball.x > xmax:
            ball.dx *= -1
            if count < 100:
                 count = create_new_ball(count)
                
        if ball.y < ymin or ball.y > ymax:
            ball.dy *= -1
            if count < 100:
                count = create_new_ball(count)
                
        display.set_pen(ball.pen)
        display.circle(int(ball.x), int(ball.y), int(ball.r))

    display.update()
    #time.sleep(0.01)
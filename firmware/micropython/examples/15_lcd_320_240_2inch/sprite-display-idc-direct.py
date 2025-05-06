import time 
from pimoroni_bus import SPIBus
#from picographics import PicoGraphics, DISPLAY_LCD_240X240, PEN_RGB332
from picographics import PicoGraphics, DISPLAY_PICO_DISPLAY_2, PEN_RGB332

import lcd

# rst=2 #com idc extender o reset muda para pino 2 usando i2c1
# cs=9
# dc=3
# lcd.LCD(rst,cs,dc) 
lcd.LCD() # com periferico lcd ligado direto

#spibus = SPIBus(cs=17, dc=16, sck=18, mosi=19, bl=4)
#display = PicoGraphics(display=DISPLAY_LCD_240X240, bus=spibus, pen_type=PEN_RGB332,rotate=0)

spibus = SPIBus(cs=17, dc=4, sck=18, mosi=19, bl=9) #com periferico LCD
# spibus = SPIBus(cs, dc, sck=18, mosi=19) #com idc extender
display = PicoGraphics(display=DISPLAY_PICO_DISPLAY_2, bus=spibus, pen_type=PEN_RGB332,rotate=0)

# set the display backlight 

display.set_backlight(1.0) 

# set up constants for drawing 

WIDTH, HEIGHT = display.get_bounds() 

BLACK = display.create_pen(0, 0, 0) 
WHITE = display.create_pen(255, 255, 255) 
display.set_font("bitmap8") 
display.load_spritesheet("mario-test.rgb332") 
sprite_x = 0 
sprite_y = 0
scale=4
rx=-(8*scale*2)
ry=0
SPRITE_XMAX = 15 
SPRITE_YMAX = 15 

# fills the screen with black 
BG = display.create_pen(0, 0, 0)

while True: 

    display.set_pen(BG)
    display.clear()
    
    # display.sprite(spritesheet_x (0-15), spritesheet_y (0-15), x, y, scale, RGB332transparent_color ) 
    # eg. there are 16 8x8 sprites per 128 lines across, 0 to 15, and 16 down 0 to 15.
    
    # in this application mario occupies 16x16 (4 tiles of 8x8)
    # thus we need to print 0,0 / 0,1 / 1,0 / 1,1
    display.sprite(sprite_x,    sprite_y,   0+rx, 		0+ry,  			scale, WHITE)
    display.sprite(sprite_x+1,  sprite_y,   8*scale+rx, 0+ry,  			scale, WHITE) 
    display.sprite(sprite_x,    sprite_y+1, 0+rx,  		8*scale+ry,  	scale, WHITE) 
    display.sprite(sprite_x+1,  sprite_y+1, 8*scale+rx, 8*scale+ry,  	scale, WHITE) 
    display.update()
    
    #cycle through the sprites (0, 2, 4, 6 @ x), moving 2 sprites 8x8 to get the next one 16x16
    sprite_x +=2 
    if sprite_x > 7: 
        sprite_x = 0 
    #    sprite_y +=2 
    #if sprite_y > 7: 
    #    sprite_y = 0 
    
    # sprite moving velocity in x axis
    rx +=8
    if rx>240:
        rx=-(8*scale*2)
        ry+=(8*scale*2)
    if ry>(240-(8*scale*2)):
        rx=-(8*scale*2)
        ry=0
    time.sleep(0.016)

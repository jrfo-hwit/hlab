from keypad_lib import Keypad
import time
import machine

NUM_ROWS = 4
NUM_COLS = 4

# Constants for GPIO pins
ROW_PINS = [4, 8, 9, 16]  # The Raspberry Pi Pico pin (GP1) connected row pins
COLUMN_PINS = [17, 18, 19, 20]  # The Raspberry Pi Pico pin (GP1) connected column pins

# Keymap corresponds to the layout of the keypad 4x4
# KEYMAP = ['1', '2', '3', 'A',
#           '4', '5', '6', 'B',
#           '7', '8', '9', 'C',
#           '*', '0', '#', 'D']
KEYMAP = ['D', 'C', 'B', 'A',
          '#', '9', '6', '3',
          '0', '8', '5', '2',
          '*', '7', '4', '1']
# Initialize the keypad
keypad = Keypad(KEYMAP, ROW_PINS, COLUMN_PINS, NUM_ROWS, NUM_COLS)
keypad.set_debounce_time(400) # 400ms, addjust it if it detects twice for single press

print("Keypad 4x4 example")

verde=machine.Pin(11, machine.Pin.OUT)
azul=machine.Pin(12, machine.Pin.OUT)
vermelho=machine.Pin(13, machine.Pin.OUT)

verde.value(0)
azul.value(0)
vermelho.value(0)

# Main loop to check for key presses
while True:
    print("Digite a senha de 4 digitos!!!")
    count=0
    key = None
    senha=['1', '5', '9', 'D']
    check=[0, 0, 0, 0]
    while key == None:
        key = keypad.get_key()
        if key:
            check[count]=key
            key=None
            print('!')
            count = count + 1
        if count == 4:
            break
        
    if senha == check:
        print("Parabéns, você acertou a senha")
        verde.value(1)
        time.sleep(2)
        verde.value(0)
    else:
        print("errou a senha!!!")
        vermelho.value(1)
        time.sleep(2)
        vermelho.value(0)
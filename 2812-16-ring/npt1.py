import board
import neopixel
import time
import sys
pixels = neopixel.NeoPixel(board.D18, 16)
pixels[int(sys.argv[1])] = (55, 0, 0)

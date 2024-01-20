"""
Vishnu Lagudu
vlagudu@purdue.edu

Basic implementation of tetris game
"""
from random import randrange
import pygame, sys

# cell coinfiguration
cell_size = 18
cols      = 10
rows      = 22
maxfps    = 30

# Single Shapes
tetris_shapes = [
  [[1, 1, 1],
   [0, 1, 0]],

  [[0, 2, 2],
   [2, 2, 0]],

  [[3, 3, 0],
   [0, 3, 3]],
  
  [[4, 0, 0],
   [4, 4, 4]],

  [[0, 0, 5],
   [5, 5, 5]],
  
  [[6, 6, 6, 6]],

  [[7, 7],
   [7, 7]]
]

def new_board():
  board = [[0 for x in range(cols)]
           for y in range(rows)]
  board += [[1 for x in range(cols)]]
  return board

class Tetris (object):
   def __init__(self) -> None:
      pygame.init()
      pygame.key.set_repeat (250, 25)
      self.width  = cell_size * (cols + 6)
      self.height = cell_size * rows
      self. 

if __name__ == '__main__':
    def __init__(self):
       pygame.init()
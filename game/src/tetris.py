"""
Vishnu Lagudu
vlagudu@purdue.edu

Basid implementaion of the tetris with pygame
"""

import pygame, random

"""
10 x 20 square grid
shapes: S, Z, I, O, J, L, T
represented in order by 0 - 6
"""

pygame.font.init()

# globals
s_width     = 800
s_height    = 700
play_width  = 300  # meaning 300 // 10 = 30 width per block
play_height = 600  # meaning 600 // 20 = 20 height per block
block_size  = 30
rows        = 20
cols        = 10

top_left_x = (s_width - play_width) // 2
top_left_y = s_height - play_height


# shape formats

S = [['.....',
      '......',
      '..00..',
      '.00...',
      '.....'],
     ['.....',
      '..0..',
      '..00.',
      '...0.',
      '.....']]

Z = [['.....',
      '.....',
      '.00..',
      '..00.',
      '.....'],
     ['.....',
      '..0..',
      '.00..',
      '.0...',
      '.....']]

I = [['..0..',
      '..0..',
      '..0..',
      '..0..',
      '.....'],
     ['.....',
      '0000.',
      '.....',
      '.....',
      '.....']]

O = [['.....',
      '.....',
      '.00..',
      '.00..',
      '.....']]

J = [['.....',
      '.0...',
      '.000.',
      '.....',
      '.....'],
     ['.....',
      '..00.',
      '..0..',
      '..0..',
      '.....'],
     ['.....',
      '.....',
      '.000.',
      '...0.',
      '.....'],
     ['.....',
      '..0..',
      '..0..',
      '.00..',
      '.....']]

L = [['.....',
      '...0.',
      '.000.',
      '.....',
      '.....'],
     ['.....',
      '..0..',
      '..0..',
      '..00.',
      '.....'],
     ['.....',
      '.....',
      '.000.',
      '.0...',
      '.....'],
     ['.....',
      '.00..',
      '..0..',
      '..0..',
      '.....']]

T = [['.....',
      '..0..',
      '.000.',
      '.....',
      '.....'],
     ['.....',
      '..0..',
      '..00.',
      '..0..',
      '.....'],
     ['.....',
      '.....',
      '.000.',
      '..0..',
      '.....'],
     ['.....',
      '..0..',
      '.00..',
      '..0..',
      '.....']]

# index 0 - 6 represent shape
shapes       = [S, Z, I, O, J, L, T]
shape_colors = [(0, 255, 0), (255, 0, 0), (0, 255, 255), (255, 255, 0), (255, 165, 0), (0, 0, 255), (128, 0, 128)]


class Piece(object):
  def __init__(self, col, row, shape) -> None:
    self.x        = col
    self.y        = row
    self.shape    = shape
    self.color    = shape_colors[shapes.index(shape)]
    self.rotation = 0


def create_grid(locked_positions={}):
  grid = [[(0, 0, 0) for _ in range(cols)] for _ in range(rows)]
  
  for x in range(rows):
    for y in range(cols):
      if (x, y) in locked_positions:
        grid[x][y] = locked_positions[(x,y)]
  
  return grid

def convert_shape_format(shape):
  pass

def valid_space(shape, grid):
  pass

def check_lost(positions):
  pass

def get_shape():
  # cols = 5 (x-cord is screen mid)
  # rows = 0 (start falling from top of screen)
  return Piece(5, 0, random.choice(shapes))

def draw_text_middle(text, size, color, surface):  
  pass
   
def draw_grid(surface, grid):

  for i in range (len(grid)):
    for j in range (len(grid[i])):
      pygame.draw.rect(surface, grid[i][j], (top_left_x+(j*block_size), top_left_y+(i*block_size), block_size, block_size), 0)
  
  pygame.draw.rect(surface, (255,0,0), (top_left_x, top_left_y, play_width, play_height), 4)
  

def clear_rows(grid, locked):
  pass

def draw_next_shape(shape, surface):
  pass

def draw_window(surface, grid):
  surface.fill((0, 0, 0))

  # game title
  pygame.font.init()
  font  = pygame.font.SysFont('comicsans', 60)
  label = font.render('TETRIS', 1, (255, 255, 255))

  surface.blit (label, (top_left_x + play_width / 2 - (label.get_width() / 2), 30))

  draw_grid(surface, grid)
  pygame.display.update()


def main(win):
  locked_positions = {}
  grid = create_grid (locked_positions)

  # Main func vars
  change_piece  = False
  run           = True
  current_piece = get_shape ()
  next_piece    = get_shape ()
  clock         = pygame.time.Clock ()
  fall_time     = 0

  # Run the game loop
  while run:
    for event in pygame.event.get() :
      if event.type == pygame.QUIT:
        run = False
        pygame.display.quit()
        quit()
      if event.type == pygame.KEYDOWN:
        if event.key == pygame.K_LEFT:
          current_piece.x -= 1
          if not valid_space (current_piece, grid):
            current_piece.x += 1
        if event.key == pygame.K_RIGHT:
          current_piece.x += 1
          if not valid_space (current_piece, grid):
            current_piece.x -= 1
        if event.key == pygame.K_DOWN:
          current_piece.y += 1
          if not valid_space (current_piece, grid):
            current_piece.y -= 1
        if event.key == pygame.K_UP:
          current_piece.rotation += 1
          # still have to write this logic

    draw_window (win, grid)


def main_menu(win):
  main (win)

win = pygame.display.set_mode ((s_width, s_height))
pygame.display.set_caption ('Tetris')
main_menu(win)  # start game
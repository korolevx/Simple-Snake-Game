# Terminal Snake Game in C

## Description

This is a classic Snake game implemented in C using the ncurses library for terminal-based graphics. The game features simple but addictive gameplay where you control a snake to collect randomly placed food items (`@`) while avoiding collisions with yourself.

## Features

- **Terminal-based interface**: Uses ncurses for smooth terminal rendering
- **Simple controls**: Arrow keys to change direction (UP, DOWN, LEFT, RIGHT)
- **Growing mechanic**: Snake grows longer when eating food
- **Wrapping boundaries**: Snake can wrap around the edges of the playing field
- **Score tracking**: Score increases as snake grows (implicitly tracked by length)
- **Collision detection**: Game ends if snake hits itself
- **Visual distinction**: Snake head (`#`) is different from body (`*`)

## Technical Details

- **Language**: C
- **Dependencies**: ncurses library
- **Field size**: 10x10 grid (customizable via `#define SIZE`)
- **Game speed**: Adjustable via `#define DELAY` (microseconds between frames)

## How to Play

1. Compile the program with ncurses support:  
   `gcc snake.c -lncurses -o snake`
2. Run the executable:  
   `./snake`
3. Use arrow keys to control the snake's direction
4. Try to eat the `@` symbols to grow longer
5. Avoid running into yourself - it's game over!

## Implementation Notes

- Uses a simple grid system with numerical representations of game elements
- Implements snake movement through coordinate array manipulation
- Features boundary wrapping for continuous gameplay
- Includes basic sound feedback (beep) when collecting food

## Customization

You can easily modify:
- Game speed by changing `DELAY` value
- Field size by changing `SIZE` value
- Visual representations by modifying the render function
- Initial snake length in the `init_snake` function

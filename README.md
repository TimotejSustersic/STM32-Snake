# Snake game
# STM32H750B-DK

A project where I made a video game of snakes, using C on a STM32H7 platform

The game needs 4 buttons in pins A0, A1, A3, A5. I used a bread board. The buttons requre a pulldown rezistor.

The game has three states. Start, Game and Game Over. To start the game press the blue button on the STM32H7.
After that you use the buttons to navigate the snake trough grid, whith a goal to eat as many apples as possible.
Each time an apple is eaten, the score will increase, green LED will blink and the snake will grow.
If you hit the border or the snakes body, you DIE.
Game Over displayes the score and after 5 seconds, navigates back to the start.

# GL HF
Timotej Sustersic

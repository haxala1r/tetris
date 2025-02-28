# Tetris

[Showcase](https://github.com/user-attachments/assets/6b6c8427-b582-4cc2-914c-420506e7a0ae)

This repository contains a tetris clone with a bot built in.

When you first start it up you might notice the SCORE at the top of the screen.
Don't be fooled by that, the score you see
is simply the amount of lines that have been cleared, and *NOT* the traditional
score number you'd see in any actual tetris game.

You might also notice a square to the left of the main board in the middle.
This square contains the held piece.

## AI
The built-in AI can be toggled by a button on the main screen. I'm sure you'll
find it, it's pretty hard to miss. There's also a SPEED button, which
will grant the AI the power of super-speed!

The first time I tested this specific
model, it had gotten to a score (read as: cleared lines) of ~4.5 million.
This number will most likely change on how lucky you are when you run it, and
the number I just gave likely isn't reliable because I only ran *one* game
to the very end. I think it's fair to say it can probably do around ~3-4 million
fairly reliably, as the games I ran almost always got that far.

The model itself isn't particularly complex - it uses a very simple heuristic function
to determine the score of a given position, and then makes its move. There is
almost no actual machine learning going on in the final game.
I did however use some very simple algorithm to test different parameters for
the heuristic function and optimize the results.

A human player can do much better than this, as the current bot does not at
all optimize for getting e.g. 4 lines cleared at the same time.

Still, this was a very fun project. I want to do something like this again
in the future.

## Game and controls
As mentioned, you can toggle the AI (and its super-speed feature) by clicking
the buttons on screen.

Here's the rest of the controls:

* UP -> rotates the current tetromino once
* LEFT and RIGHT -> moves the current tetromino left or right by one block, respectively
* DOWN -> Speeds up the fall of the current tetromino by no particular amount
* ENTER -> hard-drops the current tetromino
* SPACE -> Holds the current tetromino

## Building
Builds are only tested on linux (it's definitely possible to build on windows, but I'm not 
making any scripts for that anytime soon)
and you need to have the SDL2 and SDL2 ttf libraries installed. GCC is also required to run the build script. 
After you have those installed, just run the build script:

`./build.sh`

## Credits
Thanks to KineticPlasma Fonts for making the [AppleStorm Font](https://www.fontrepo.com/font/1698/applestorm) used in the game.

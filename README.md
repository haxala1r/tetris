# Tetris

You know it, you love it, and you want it. This repository contains a tetris
clone with my own personal AI (or bot, whichever you want to call it) built in!

When you first start it up you might notice the SCORE at the top of the screen.
Don't be fooled by that evil lie, it's not an actual score! The score you see
is simply the amount of lines that have been cleared, and *NOT* the traditional
score number you'd see in any actual tetris game.

You might also notice a square to the left of the main board in the middle.
This square contains the held piece.

## AI
The built-in AI can be toggled by a button on the main screen. I'm sure you'll
find it, it's pretty hard to miss. There's also a SPEED button, which
will grant the AI the power of super-speed!

The AI is fairly good (if I say so myself). The first time I tested this specific
model, it had gotten to a score (read as: cleared lines) of ~4.5 million.
This number will most likely change on how lucky you are when you run it, and
the number I just gave likely isn't reliable because I only ran *one* game
to the very end. I think it's fair to say it can probably do around ~3-4 million
fairly reliably, as the games I ran almost always got that far.

However, I'm pretty confident it's still fairly good, and of the ~10 tests
I've run, it has cleared at least ~2 million lines (and possibly more, I didn't actually
wait for it to die.)

If you'd like a good comparison, the human world record is currently held by
Harry Hong, with 4.988 lines cleared.

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
Unfortunately, you can only build on linux (it's definitely possible to build on windows, but I am not 
making any scripts for that anytime soon as I use Visual studio for the windows builds myself) 
and you need to have the SDL2 and SDL2 ttf libraries installed. GCC is also required to run the build script. 
After you have those installed, just run:

`./build.sh`

## Credits
Thanks to KineticPlasma Fonts for making the [AppleStorm Font](https://www.fontrepo.com/font/1698/applestorm) used in the game.

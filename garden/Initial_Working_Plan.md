# What's the first step?

I've decided to make this application graphical - by that, I mean
it runs a separate window on a window manager, and not, how *everything*
else I've programmed so far, in the terminal.

## Multimedia library choice

This poses a new challenge for me, and I had to look after graphics
libraries that are suited for my goals. I also want to add 
post-processing effects and things like vertical grid layers with
lower opacity, so it's gonna be a tiny bit complex (All general goals
can be found in [Goals.md](Goals.md)). I know it's a
standard choice for graphical software written in C++, but I want
to give **SFML** a try. I think it's a good choice, because:

- It's used a *lot*, so there will be lots of resources, perfect for a beginner like me.
- It can release for MS Windows, so my friends can play the game
- It integrates with shaders
- It seems to be easy to use
- It seems to be relatively performant

## Generic display backend (frama)

I want to write my own framework for handling graphics on top of SFML, 
which can also be used standalone for other projects. So we're speaking of
writing something to easily display text grids in SFML and applying FX to them.
This framework will be called **frama**, because there can be multiple
grids instanced on the screen at the same time which can be controlled
independently, called "frames".

The first step will be to write the frama framework to a certain extent,
until it's usable enough to start developing on it.

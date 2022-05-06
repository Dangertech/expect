EXPECT drives to be a
*free and open source, minimalistic, procedurally generated,
open-world, sci-fi cyberpunk traditional roguelike written in C++*.

The *minimalistic* part will probably explode violently after a few
months of development, but the most important aspects are really just
the procedural generation, the roguelike genre, and that the language
is C++.

## MY ambitions

The deadline is March 2025, so I'll have some time. I hope my
expectations are realistic, but this is what I want to achieve, very roughly:

- A modern, good-looking traditional roguelike that can be shown on a big screen
  without people wanting to gouge their eyes out
	- Multi-z-layered grid display with the layers which some layers (and
	  individual characters) with lower opacity.
	- "Smooth" panning
	- A rain effect, independent from the grid
	- Bloom
	- A vignette
	- Multiple "windows" (viewports) that can be zoomed independently
	  (just think of cogmind, but the top bar, the sidebar and the actual
	  game view can be zoomed, independently)
- An infinite, procedurally-generated cyberpunk megacity
	- Every tile is fully destructible
	- The city makes heavy use of vertical exploration (skyscrapers, giant buildings, etc.)
	- The procedural generation looks convincing and not too same-y
	- Different "biomes" (like housing areas, industry regions, office buildings,
	  wealthy districts, malls, etc.)
- A AFAIK unique audio system
	- A standard sound system (sounds for opening doors, taking damage, etc.)
	- Music "heat levels": When exploring, the music is ambient, the more
	  critical the situation, the more extreme the music
	- Managing music through specific files, .action files that instruct
	  the game on how to integrate the music pieces
	- Users can import their own music and write their own .action files
	  for them.

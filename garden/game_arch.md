# Fundamental game architecture

Shit, I sit in front of an empty main() function and have no clue about
anything. I have frama, a really solid base, but how do I go on from here?

Now I have to design the most fundamental system in almost any video game.
How do I structure and handle game objects, like enemies and (in expect's case)
every single object encountered in the game? 

An obvious choice would be an Entity Component System. There's a useful
link for that:
- https://www.gamedev.net/articles/programming/general-and-gameplay-programming/the-entity-component-system-c-game-design-pattern-part-1-r4803/

But before being concerned with any of this, I have an even more fundamental
question: How the fuck do video games store all these objects? Do they
just throw them in a giant vector and loop through it every tick to process
everything, or is there something smarter? 

## Update (2022-07-17)

Ok, so now I have written up a basic ECS. I ended up using mostly 
[this](https://www.david-colson.com/2020/02/09/making-a-simple-ecs.html)
tutorial which I adapted a bit to fit it into a nice interface. I have
only a rough idea of what I did but I'm a bit proud I didn't just give
up and used the `entt` framework or something. But how will I integrate
these entities into the game world?

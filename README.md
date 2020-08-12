# Minesweaper v0.11

console game for linux

## How to use

```bash
./minesweaper
```

### Options

You can change basic options in start of code-file
Have fun :)

If you change options - you need to compile program. I make it like this:
```bash
g++ minesweaper.cpp -o minesweaper
```

### Versions
0.1
- There is no win mechanism
- You can open cells and see how algorythm work
- If you find the bomb - the game was over and you can see place of the bombs
- You can mark PREDICT to opened cell  
0.11
- Added the prediction logic: you can add and remove the PREDICT flag
- Added Win Mechanism (now you win if you opened all cells without bombs


=======

### License
GNU GPL

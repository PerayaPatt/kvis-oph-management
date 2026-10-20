# KVIS Open House Management System

A C console program that plans a visit through the booths at an Open House, picking the booths you most want to see within the time you have.


## Features

- Stores booths, their positions and opening times (admin side saves them in .dat file)
- Finds the shortest path between booths (`path.h`)
- Chooses the set of booths with the highest total preference score whose times don't overlap, using weighted interval scheduling with dynamic programming (`interval.h`)
- Suggests the top 3 plans

## Files

| File | Purpose |
| --- | --- |
| `main.c` | Program entry point |
| `front.c`, `front.h` | Screens and menus |
| `data.c`, `data.h` | Data structures |
| `path.h` | Shortest path |
| `interval.h` | Non-overlapping time slots |
| `admin.h` | Save and load booth data |

## How to run

Requires `gcc` and `make`.

```
make
./oh
```

## My part
This was a group project. I was responsible for the route selection system and design

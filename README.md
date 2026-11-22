# RISK-CLI

A command-line version of the board game **Risk**, written in C++. It loads a game from a file, runs full turns (placing units, attacking with dice, fortifying, trading cards) and includes two extra features: a **strategy helper** that finds the cheapest territory to conquer using Dijkstra's algorithm, and **compressed save files** built with Huffman coding.

This was a university course project. The code, comments and in-game messages are in Spanish.

## Features

- **Full game flow** for 3 to 6 players on the real 42-territory Risk map: unit reinforcement (territories + continent bonuses + card trades), dice battles, conquest, fortification and turn order.
- **Cards**: drawing cards and trading sets of three for bonus units (4, 6, 8, 10, 12, 15, then +5).
- **Strategy helper (graph + Dijkstra)**: the board is modeled as a graph where each enemy territory costs the units defending it. A multi-source Dijkstra search from all of the player's territories answers:
  - `costo_conquista`: the cheapest path to conquer a given territory, and where to attack from.
  - `conquista_mas_barata`: which enemy territory is the cheapest to take right now.
- **Huffman compression**: `guardar_comprimido` saves the game state as a binary file encoded with a Huffman tree, and `inicializar` can load it back.
- **Script mode**: `ejecutar_comandos` runs a list of commands from a text file, which makes it easy to test or replay a game.
- Input validation for files and commands (wrong format, unknown territories, invalid moves, wrong game phase).

## Project structure

```
src/
  main.cpp          Command-line interface and command parsing
  Juego.h/.cpp      Game state, map, turns, battles, saving and loading
  Jugador.h/.cpp    Player and card types
  Territorio.h/.cpp Territory type and neighbors
  Continente.h/.cpp Continent type and bonuses
  Estrategia.h/.cpp Board graph and Dijkstra-based conquest planning
  Huffman.h/.cpp    Huffman tree, compression and decompression
examples/
  partida_inicial.txt  Sample starting game (3 players)
  comandos_demo.txt    Sample turn to run with ejecutar_comandos
```

## Build and run

Requires a C++17 compiler (g++ or clang++).

```bash
make          # or: g++ -std=c++17 -O2 -o risk src/*.cpp
./risk
```

## Quick demo

```text
RISK-CLI $ inicializar examples/partida_inicial.txt
RISK-CLI $ conquista_mas_barata Ana
RISK-CLI $ ejecutar_comandos examples/comandos_demo.txt
RISK-CLI $ guardar_comprimido partida.bin
RISK-CLI $ estado_juego
RISK-CLI $ salir
```

Type `menu` to see every command and `ayuda <command>` for details.

## Main commands

| Command | What it does |
| --- | --- |
| `inicializar <file>` | Load a game from a text file or a compressed `.bin` file |
| `obtener_unidades <player>` | Calculate the units a player receives this turn |
| `ubicar_unidad <player> <territory> <amount>` | Place units on one of the player's territories |
| `atacar <player> [<from> <to> <dice...>]` | Attack, with random dice or with given dice values |
| `fortificar <player> [<from> <to> <amount>]` | Move units between connected territories |
| `robar_carta` / `canjear_cartas` | Draw a card / trade three cards for units |
| `turno_siguiente` | End the turn |
| `costo_conquista <player> <territory>` | Cheapest path to conquer a territory (Dijkstra) |
| `conquista_mas_barata <player>` | Cheapest enemy territory to conquer (Dijkstra) |
| `guardar <file.txt>` / `guardar_comprimido <file.bin>` | Save the game as text or Huffman-compressed binary |
| `estado_juego` | Show players, cards and all 42 territories |

## Game file format

Territories are identified by their official code `continent.index` (for example `1.1` = Alaska, `5.6` = Kamchatka).

```
<number of players>
<name> <color>                (one line per player)
<territory code> <color> <units>   (42 lines)
```

An extended format with cards is also supported (see `ayuda inicializar` in the program).

## Team

Developed by **Nicholas Ruiz** and **Andres Felipe Alquerque Cadena** as a university course project.

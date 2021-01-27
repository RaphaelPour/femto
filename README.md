# Femto [![Build Status](https://travis-ci.com/RaphaelPour/femto.svg?branch=master)](https://travis-ci.com/RaphaelPour/femto)

CLI Text-Editor in Vanila C.

## Contents

- [Setup](#setup)
- [Usage](#usage)
- [Key bindings](#key-bindings)
- [License](#license)


## Setup

Gcc and [Tonobo's Task](https://github.com/tonobo/task) are required. `task bin` will build it to `ci-build`.

## Usage

Show help:

```bash
$ femto -h
```
Show version: 
```bash
$ femto -v
```

Open existing file:
```bash
$ femto README.md
```

New file:
```bash
$ femto
```

## Key bindings

| Key          | Description                    |
|--------------|--------------------------------| 
| `Arrow keys` | Navigation                     |
| `ENTER`      | New line                       |
| `BACKSPACE`  | Remove char/line before cursor |
| `DELETE`     | Remove char/line after cursor  |
| `ESC`        | Quit                           |
| `Ctrl+S`     | Save                           |

## License

[GPLv3](https://www.gnu.org/licenses/gpl-3.0.html)

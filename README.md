# Femto [![Build Status](https://travis-ci.com/3vilcookie/femto.svg?branch=master)](https://travis-ci.com/3vilcookie/femto)

CLI Text-Editor in Vanila C.

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
| `Ctrl+C`     | Save                           |

## License

[GPLv3](https://www.gnu.org/licenses/gpl-3.0.html)

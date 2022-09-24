# Assignment-3

## Commands Implemented

- **cd** - `.`, `..`, `-` and `~` flags implemented
- **echo** - single line strings (quotes not implemented)
- **pwd** - prints absolute path of current working directory
- **ls** - lists out all files and directories (`-a` to include hidden files/directories, `-l` to print file/directory information)
- **discover** - To search for an item (enclosed in `""`) or print out all items within a directory (`-d` to only print directories, `-f` to only print files)
- **history** - prints last 10 commands (stored as a tempfile in "/home/[username]" directory)
- **pinfo** - prints process information
- **clear** - clears all previous i/o and sets cursor to top of window
- **jobs** - outputs a list of background processes (`-r` for running only, `-s` for stopped only)
- **sig** - to send the specified signal to the given job number
- **fg** - to bring a background process with the given job number to foreground and continue it if it's stopped
- **bg** - to continue a stopped background process
- Input can be redirected from a file using `<`
- Output can be redirected to a file using `>` (write) or `>>` (append)
- Pipeline `|` can be used to redirect output of one command to input of the next command (only for foreground processes) (does not work properly with Ctrl+C and Ctrl+Z)
- Multiple commands can be passed in one line separated by `;` or `&`
- Commands succeeded by `&` will run in background
- Press Tab to autocomplete with file/directory names

## Compile Using

> make

## Execute Using

> ./shell

## Errors

### cd

- Directory does not exist
- More than one arguments

### pwd

- Any arguments present

### ls

- Invalid file or directory name
- More than 200 files/directories given as arguments
- Invalid flags

### discover

- Can have only one source directory
- Can have only one target source
- Invalid source directory
- Invalid flags

### history

- Any arguments present

### pinfo
- More than one argument
- Invalid PID

### clear
- Any arguments present

### jobs
- Any non flag arguments
- Invalid flags

# sig
- Invalid or absent job number
- Invalid or absent signal ID
- More than 2 arguments

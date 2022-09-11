# Assignment-2 (Mid Submission) 

## Commands Implemented

- **cd** - `.`, `..`, `-` and `~` flags implemented
- **echo** - single line strings (quotes not implemented)
- **pwd** - prints absolute path of current working directory
- **ls** - lists out all files and directories (`-a` to include hidden files/directories, `-l` to print file/directory information)
- **discover** - To search for an item (enclosed in `""`) or print out all items within a directory ('-d' to only print directories, '-f' to only print files)
- **history** - prints last 10 commands (stored as a tempfile in "/home/[username]" directory)
- **pinfo** - prints process information
- **clear** - clear all previous i/o and sets cursot to top of window
- Multiple commands can be passed in one line separated by `;` or `&`
- Commands succeeded by `&` will run in background

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

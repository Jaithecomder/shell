# Assignment-2 (Mid Submission) 

## Commands Implemented

- **cd** - `.`, `..`, `-` and `~` flags implemented
- **echo** - single line strings (quotes not implemented)
- **pwd** - prints absolute path of current working directory
- **ls** - lists out all files and directories (`-a` to include hidden files/directories, `-l` to print file/directory information)
- **history** - prints last 10 commands
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

### history

- Any arguments present

# Shell
A simple shell written in C.

## Build
    $ git clone https://github.com/GaggiX/shell.git
    $ make

## Install
    # make install    
You can test the shell using:

	$ gsh
If you are brave enough you can change your shell with this:

    # echo "/bin/gsh" >> /etc/shells
    $ chsh -s "/bin/gsh"
## Features

 - Basic builtin Commands: `cd`, `exit` and `help`
 - Effective uid
 - Basic parameter expansion

BS-less Display Manager

Completely CLI based display manager powered by ncurses and pam.
Does not use any X or Wayland for both UI or authentication.

*Does not currently work, needs further testing*
*Wait on further commits and updates*
*DO NOT USE UNLESS YOU WANT A BROKEN SYSTEM*

Menu:
    - Selected window manager (managed with very simple config files
    - Username field
    - Masked password field
More:
    - Proram/authentication status
    - Art
    - Power off & reboot
'Dreaded' config:
    Syntax:
        name = "Some window manager"
        desc = "Insert lightweight buzzwords here"
        exec = "Commands or scripts to run to get window manager launched"
    Example:
        name = "DWM"
        desc = "Dynamic Window Manager"
        exec = "dwm"
    - Each individual file has extension '.wm', ex: dwm.wm
    - Each file is also placed under /etc/bdm/
        - For current development purposes, it is actually next to the executable itself just as 'wm'

After successful authentication, it will run what is set to exec to start the window manager.
Pros of using this CLI based display manager is that there are much fewer dependencies to worry about
    - PAM
    - NCURSES
    - GNU C
    - Standard Linux/Unix C libraries

Source code overview:
    - frontend.h/frontend.c: . . . . show display manager, manage input (ncurses)
    - auth.h/auth.c: . . . . . . . . authentication and logon (pam)
    - userstring.h/userstring.c: . . input strings managed with one char at a time
    - panic.h/panic.c: . . . . . . . debug and error handling
    - wmlist.h/wmlist.c: . . . . . . enumerate .wm files and parse them to see and run window managers
    - config.h . . . . . . . . . . . configs, currently just position of the different boxes on screen

TODO's:
    - Clean front end more
    - Test in a clean virtual machine, systemd first
    - When time is right, change wm to /etc/bdm
    - Come up with some cool ASCII art
    - When time is right, cut panic.h
    - Implement logger system that will replace most of panic



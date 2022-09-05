# DEA Editor
A simulator for deterministic finite automata (DFA; shortened to DEA in German).

![DEA Editor Demo gif](https://user-images.githubusercontent.com/9048813/188460795-a3e58a02-e6c9-47c2-b0e4-06246bf08617.gif)

Further screenshots can be found here: https://github.com/t-wissmann/DEA_Editor/wiki/Screenshots

## 1. Author
DEA_Editor was written by Thorsten Wissmann
(http://thorsten-wissmann.de)

## 2. License
It is licensed under the GPL v3
See http://www.gnu.org/licenses/gpl.txt for more details.


## 3. About DEA Editor
DEA Editor is a GUI Education Software.
It helps to understands the theorie of finite state machines (FSM, or DEA in german).
You can create a FSM by creating its state diagram.
Then you can let your new FSM recognize an inputstring.
This process of recognizing is visualized with colors an animations (the current state of the FSM and the last transition is highlighted)
DEA Editor supports drag&drop and an intuitive mouse control.


## 4. Requirements
Requirements to run DEA Editor:
- Qt4 dlls/libraries
Requirements to compile:
- Qt4 development tools (qmake, compiler)

## 5. Howto build/make/(re)compile

    qmake-qt4
    make

The executable can then be found under `bin/dea_editor`

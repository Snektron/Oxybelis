# Oxybelis - OpenGL planet renderer

## Compilation

To compile this project, simply run `make` in the project root. The executable can be found in
`build/target/`, or can be ran directly using `make run`.

## Dependencies

On linux, the project has no dependencies except for X11 and GL, as a GLFW static library is included.
On windows, GLFW must be provided by the system.

## AMD

On AMD cards (at least on mine) the program occasionaly shows a black screen or artefacts on linux. In that
case please restart the program until it is fixed. On windows however, it doesnt work at all. Other cards should be
fine.

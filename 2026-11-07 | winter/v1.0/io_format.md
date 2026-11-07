# Font Problem - IO Format

## Input

First line, a natural number $n$, number of character strings.

Second line, a string, name of input file.

Third line, a string, name of output file.

Fourth line, a string, name of font file.

Next n lines: $S,siz,x,y,dx,dy,lc,ly,rc,ry,clr,\alpha,fade_{in},fade_{out}$:

- The character string, size, position of the top-left corner $(x,y)$ of the first character,
- distance of adjacent characters $(dx,dy)$, shown time interval frame $[(lc,ly),(rc,ry)+fade_{out}]$, color $clr$,
- intransparency $\alpha$, count of fade-in/out frames.

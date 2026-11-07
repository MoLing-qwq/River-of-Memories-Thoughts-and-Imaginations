# Font Problem - IO Format

## Input

First line, a natural number $n$, number of character strings.

Next n lines: $S,siz,x,y,dx,dy,lc,ly,rc,ry,clr,alpha,fade_in_c,fade_out_c$:

- The character string, size, position of the top-left corner $(x,y)$ of the first character,
- distance of adjacent characters $(dx,dy)$, shown time interval frame $[(lc,ly),(rc,ry)+fade_out_c]$, color $clr$,
- intransparency $alpha$, count of fade-in/out frames.

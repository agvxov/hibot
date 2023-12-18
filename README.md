# HIBOT
> IRC bot written in C, responsible for formatting source code.

Hibot can join (currently) a single IRC channel, specifying the target from the cli:

    $ hibot <server>:<port> <channel>

Then, if/when it receives a direct message,
it will echo it back to the destination channel,
applying simple syntax highlighting rules.

### Supported languages:
+ Ada
+ C/C++
+ Fasm

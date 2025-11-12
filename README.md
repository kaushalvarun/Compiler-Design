# Compiler Lex, C & YACC Programs
## Installed Dependencies
- flex
- byacc
- bison

## Compiling and Running  C programs
gcc c_program.c -o c_program.exe
./c_program.exe

## Compiling and Running LEX Programs:
lex hello.l
gcc lex.yy.c -o hello
./hello

## Compiling and Running YACC Programs:
lex hello.l
bison -d hello.y
gcc lex.yy.c y.tab.c
./a.out

## Lex programs division
...token declarations...
%{
...C defiinitions...

%}
%%
...Patterns & Rules...
%%

...Subroutines...
int main() {
    yylex();
    ...MORE SUBROUTINE CODE...
    return 0;
}

int yywrap() {
    return 1;
}

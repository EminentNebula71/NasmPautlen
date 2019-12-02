all:
	gcc -o generador main_2_generacion.c generacion.c
	./generador salida.asm
	nasm -g -o main_2_generacion.o -f elf32 salida.asm
	gcc -m32 -o main_2_generacion main_2_generacion.o alfalib.o

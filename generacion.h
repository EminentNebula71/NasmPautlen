
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define ENTERO 1
#define BOOLEANO 0

void escribir_cabecera_compatibilidad(FILE* fpasm);
void escribir_subseccion_data(FILE* fpasm);
void escribir_cabecera_bss(FILE* fpasm);
void declarar_variable(FILE* fpasm, char * nombre,  int tipo,  int tamano);
void escribir_segmento_codigo(FILE* fpasm);
void escribir_inicio_main(FILE* fpasm);
void escribir_fin(FILE* fpasm);
void escribir_operando(FILE* fpasm, char *nombre, int es_variable);
void asignar (FILE* fpasm, char *nombre, int es_variable);
void sumar(FILE* fpasm, int es_variable_1, int es_variable_2);
void restar(FILE* fpasm, int es_variable_1, int es_variable_2);

void multiplicar(FILE* fpasm, int es_variable_1, int es_variable_2);
void dividir(FILE* fpasm, int es_variable_1, int es_variable_2);
void o(FILE* fpasm, int es_variable_1, int es_variable_2);

void y(FILE* fpasm, int es_variable_1, int es_variable_2);
void cambiar_signo(FILE* fpasm, int es_variable);
void no(FILE* fpasm, int es_variable, int cuantos_no);
void igual(FILE* fpasm, int es_variable1, int es_variable2, int etiqueta);
void distinto(FILE* fpasm, int es_variable1, int es_variable2, int etiqueta);
void menor_igual(FILE* fpasm, int es_variable1, int es_variable2, int etiqueta);

void mayor_igual(FILE* fpasm, int es_variable1, int es_variable2, int etiqueta);
void menor(FILE* fpasm, int es_variable1, int es_variable2, int etiqueta);

void mayor(FILE* fpasm, int es_variable1, int es_variable2, int etiqueta);
void leer(FILE* fpasm, char* nombre, int tipo);

void escribir(FILE* fpasm, int es_variable, int tipo);

void declararFuncion(FILE * fd_asm, char * nombre_funcion, int num_var_loc);
void retornarFuncion(FILE * fd_asm, int es_variable);
void escribirParametro(FILE* fpasm, int pos_parametro, int num_total_parametros);
void escribirVariableLocal(FILE* fpasm, int posicion_variable_local);
void asignarDestinoEnPila(FILE* fpasm, int es_variable);
void operandoEnPilaAArgumento(FILE * fd_asm, int es_variable);
void llamarFuncion(FILE * fd_asm, char * nombre_funcion, int num_argumentos);
void limpiarPila(FILE * fd_asm, int num_argumentos);

void ifthenelse_inicio(FILE * fpasm, int exp_es_variable, int etiqueta);
void ifthen_inicio(FILE * fpasm, int exp_es_variable, int etiqueta);
void ifthen_fin(FILE * fpasm, int etiqueta);
void ifthenelse_fin_then( FILE * fpasm, int etiqueta);
void ifthenelse_fin( FILE * fpasm, int etiqueta);
void while_inicio(FILE * fpasm, int etiqueta);
void while_exp_pila (FILE * fpasm, int exp_es_variable, int etiqueta);
void while_fin( FILE * fpasm, int etiqueta);
void escribir_elemento_vector(FILE * fpasm,char * nombre_vector,int tam_max, int exp_es_direccion);

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "generacion.h"



/************ G14 **********************************************************************/

void escribir_cabecera_compatibilidad(FILE* fpasm)
{
/* FUNCIÓN PARA PODER HACER EL CÓDIGO MULTIPLATAFORMA U OTROS PARÁMETROS GENERALES TAL VEZ SE PUEDA QUEDAR VACÍA */

/*
segment compatibilidad
resd xx 1 34
*/
fprintf(fpasm, "\nsegment compatibilidad\n");
fprintf(fpasm, "resd xx 1 34\n");

}

/************** G13 ********************************************************************/

void escribir_subseccion_data(FILE* fpasm) {
/*Declaracion del segmento de datos*/
fprintf(fpasm, "\nsegment .data \n");
/*Declaracion del error de dividir entre 0*/
fprintf(fpasm, "msg_error_division db \"Error division\" , 0\n" );
fprintf(fpasm, "msg_error_indice_vector db \"Error indice vector\" , 0\n" );
}

/*********** G12 ***********************************************************************/

void escribir_cabecera_bss(FILE* fpasm)
{
/* FUNCIÓN PARA ESCRIBIR EL INICIO DE LA SECCIÓN .bss:
    AL MENOS HAY QUE DECLARAR LA VARIABLE AUXILIAR PARAerr_div0 GUARDAR EL PUNTERO DE PILA __esp
*/

/*
segment .bss
__esp resd 1
*/

fprintf(fpasm, "segment .bss\n");
fprintf(fpasm, "__esp resd 1\n");

}

/*********** G11  **********************************************************************/
/* tipo no hace falta porque para nosotros todo es entero en esta versión, se deja por compatibilidad con futuras versiones*/

/* GENERA EL CÓDIGO ASOCIADO EN LA SECCIÓN .bss PARA DECLARAR UNA VARIABLE CON
    SU NOMBRE (HAY QUE PRECEDER DE _)
    EL TAMANO (1 PARA VARIABLES QUE NO SEAN VECTORES O SU TAMANO EN OTRO CASO
    TIPO NOSOTROS USAREMOS ESTE AÑO ENTERO O BOOLEANO

*/
void declarar_variable(FILE* fpasm, char * nombre,  int tipo,  int tamano){
    /*Control de errores*/
if((fpasm == NULL)||((strcmp(nombre, "") == 0)||(nombre == NULL))||((tipo != ENTERO)&&(tipo != BOOLEANO))||(tamano <= 0)){
         fprintf(stderr, "ERROR EN FUN: declarar_variable\n");
            return;
 }

 fprintf(fpasm, "_%s resd %d\n", nombre, tamano);

return;
}
/*LA FUNCIÓN ESTÁ BIEN: G11*/

/************** G10 **********************************************************************/

void escribir_segmento_codigo(FILE* fpasm)
{
/* ESCRIBE EL INICIO DE LA SECCIÓN DE CÓDIGO
    DECLARACIONES DE FUNCIONES QUE SE TOMARAN DE OTROS MODULOS
    DECLARACION DE main COMO ETIQUETA VISIBLE DESDE EL EXTERIOR
*/

/*
segment .text
    global main
    extern scan_int, print_int, scan_float, print_float, scan_boolean, print_boolean
    extern print_endofline, print_blank, print_string
    extern alfa_malloc, alfa_free, ld_float

*/

fprintf (fpasm, "\nsegment .text \nglobal main \nextern scan_int, print_int, scan_float, print_boolean, \nextern print_endofline, print_blank, print_string\nextern alfa_malloc, alfa_free, ld_float, scan_boolean\n");

}

/*********  G9 *************************************************************************/

void escribir_inicio_main(FILE* fpasm)
{
/* ESCRIBE EL PRINCIPIO DEL CÓDIGO PROPIAMENTE DICHO
    ETIQUETA DE INICIO
    SALVAGUARDA DEL PUNTERO DE PILA (esp) EN LA VARIABLE A TAL EFECTO (__esp)

*/

fprintf(fpasm, "\nmain:\nmov dword [__esp] , esp\n");

}




/***************** G8 *****************************************************************/

void escribir_fin(FILE* fpasm)
{
/* ESCRITURA DEL FINAL DEL PROGRAMA
    GESTIÓN DE ERROR EN TIEMPO DE EJECUCIÓN (DIVISION POR 0)
    RESTAURACION DEL PUNTERO DE PILA A PARTIR DE LA VARIABLE __esp
    SENTENCIA DE RETORNO DEL PROGRAMA
*/

/*
jmp fin

 division_cero:

  push dword err_div0
  call print_string
  add esp, 4
  call print_endofline

  jmp fin

fin:
  mov dword esp, [__esp]
  ret

*/

fprintf(fpasm, "jmp near fin\n");
fprintf(fpasm, "fin_error_division:\n");
fprintf(fpasm, "push dword msg_error_division\n");
fprintf(fpasm, "call print_string\n");
fprintf(fpasm, "add esp, 4\n");
fprintf(fpasm, "call print_endofline\n");
fprintf(fpasm, "jmp near fin\n");
fprintf(fpasm, "fin_indice_fuera_rango:\n");
fprintf(fpasm, "push dword msg_error_indice_vector\n");
fprintf(fpasm, "call print_string\n");
fprintf(fpasm, "add esp, 4\n");
fprintf(fpasm, "call print_endofline\n");
fprintf(fpasm, "jmp near fin\n");
fprintf(fpasm, "fin:\n");
fprintf(fpasm, "mov esp, [__esp]\n");
fprintf(fpasm, "ret\n");
}


/*************G7 *********************************************************************/

void escribir_operando(FILE* fpasm, char *nombre, int es_variable)
{
/* SE INTRODUCE EL OPERANDO nombre EN LA PILA
    SI ES UNA VARIABLE (es_variable == 1) HAY QUE PRECEDER EL NOMBRE DE _
    EN OTRO CASO, SE ESCRIBE TAL CUAL

*/

if(!fpasm){
    return;
}


    if (es_variable == 1)
        fprintf(fpasm, "push dword _%s\n", nombre);
    else
        fprintf(fpasm, "push dword %s\n", nombre);

    return;
}


/************ G6 **********************************************************************/

void asignar (FILE* fpasm, char *nombre, int es_variable)
{
/* ESCRIBE EL CÓDIGO PARA REALIZAR UNA ASIGNACIÓN DE LO QUE ESTÉ EN LA CIMA DE LA PILA A LA VARIABLE nombre
    SE RECUPERA DE LA PILA LO QUE HAYA POR EJEMPLO EN EL REGISTRO eax
    SI es_variable == 0 (ES UN VALOR) DIRECTAMENTE SE ASIGNA A LA VARIABLE _nombre
    EN OTRO CASO es_variable == 1 (ES UNA DIRECCIÓN, UN NOMBRE DE VARIABLE) HAY QUE OBTENER SU VALOR DESREFERENCIANDO
EL VALOR ES [eax]
*/
if(es_variable==0){
    fprintf(fpasm, "pop dword [_%s]\n", nombre);
}
else {
    fprintf(fpasm, "pop dword eax\n");
    fprintf(fpasm, "mov dword eax, [eax]\n");
    fprintf(fpasm, "mov dword [_%s], eax\n", nombre);
}

}


/*************** G5 *******************************************************************/

void sumar(FILE* fpasm, int es_variable_1, int es_variable_2){
    fprintf(fpasm, "pop dword eax\n");
    fprintf(fpasm, "pop dword ebx\n");

    if(es_variable_2 == 1) {   /* VARIABLE ESTA DEFINIDA COMO 1*/
        fprintf(fpasm, "mov dword eax, [eax]\n");
    }

    if(es_variable_1 == 1) {
        fprintf(fpasm, "mov dword ebx, [ebx]\n");
    }

    fprintf(fpasm, "add eax, ebx\n");
    fprintf(fpasm, "push dword eax\n");
}

void restar(FILE* fpasm, int es_variable_1, int es_variable_2){
    fprintf(fpasm, "pop dword eax\n");
    fprintf(fpasm, "pop dword ebx\n");

    if(es_variable_2 == 1) {
        fprintf(fpasm, "mov dword eax, [eax]\n");
    }

    if(es_variable_1 == 1) {
        fprintf(fpasm, "mov dword ebx, [ebx]\n");
    }

    fprintf(fpasm, "sub eax, ebx\n");
    fprintf(fpasm, "push dword eax\n");
}

void multiplicar(FILE* fpasm, int es_variable_1, int es_variable_2){
    fprintf(fpasm, "pop dword eax\n");
    fprintf(fpasm, "pop dword ebx\n");

    if(es_variable_2 == 1) {
        fprintf(fpasm, "mov dword eax, [eax]\n");
    }

    if(es_variable_1 == 1) {
        fprintf(fpasm, "mov dword ebx, [ebx]\n");
    }

    fprintf(fpasm, "imul ebx\n");
    fprintf(fpasm, "push dword eax\n");
}

void dividir(FILE* fpasm, int es_variable_1, int es_variable_2){
    fprintf(fpasm, "pop dword eax\n");
    fprintf(fpasm, "pop dword ebx\n");

    if(es_variable_2 == 1) {
        fprintf(fpasm, "mov dword eax, [eax]\n");
    }

    if(es_variable_1 == 1) {
        fprintf(fpasm, "mov dword ebx, [ebx]\n");
    }

    fprintf(fpasm, "mov dword edx, 0\n");
    fprintf(fpasm, "cmp ebx, edx\n");
    fprintf(fpasm, "je msg_error_division\n");

    fprintf(fpasm, "idiv ebx\n");
    fprintf(fpasm, "push dword eax\n");
}

void o(FILE* fpasm, int es_variable_1, int es_variable_2)
{
    fprintf(fpasm, "pop dword eax\n");
    fprintf(fpasm, "pop dword ebx\n");

    if(es_variable_2 == 1) {
        fprintf(fpasm, "mov dword eax, [eax]\n");
    }

    if(es_variable_1 == 1) {
        fprintf(fpasm, "mov dword ebx, [ebx]\n");
    }

    fprintf(fpasm, "or eax, ebx\n");
    fprintf(fpasm, "push dword eax\n");
}


void y(FILE* fpasm, int es_variable_1, int es_variable_2)
{
    fprintf(fpasm, "pop dword eax\n");
    fprintf(fpasm, "pop dword ebx\n");

    if(es_variable_2 == 1) {
        fprintf(fpasm, "mov dword eax, [eax]\n");
    }

    if(es_variable_1 == 1) {
        fprintf(fpasm, "mov dword ebx, [ebx]\n");
    }

    fprintf(fpasm, "and eax, ebx\n");
    fprintf(fpasm, "push dword eax\n");
}


/************** G4 ********************************************************************/

void cambiar_signo(FILE* fpasm, int es_variable)
{
/* GENERA EL CÓDIGO PARA CAMBIAR DE SIGNO LO QUE HAYA EN LA CIMA DE LA PILA
TENIENDO EN CUENTA QUE PUEDE SER UN VALOR INMEDIATO O UNA DIRECCION (VER ASIGNAR)

*/
if (es_variable == 1){
   fprintf(fpasm, "pop dword eax\n");
   fprintf(fpasm, "mov dword eax, [eax]\n");
   fprintf(fpasm, "neg eax\n");
   fprintf(fpasm, "push eax\n");
}else{
   fprintf(fpasm, "pop dword eax\n");
   fprintf(fpasm, "neg eax\n");
   fprintf(fpasm, "push eax\n");
}
/*imul -> neg*/
}


/**********   G3 ************************************************************************/
void no(FILE* fpasm, int es_variable, int cuantos_no)
{

    fprintf(fpasm, "pop eax\n");

    if(es_variable == 1) {
        fprintf(fpasm, "mov dword eax, [eax]\n");
    }

    /*

    pop eax
    mov eax, [eax]

    cmp eax, 0
    je _uno

    mov eax, 0
    jmp _fin_not

    _uno:
    mov eax, 1

    _fin_not:
    push eax

    */
    
    fprintf(fpasm, "cmp eax, 0\n");
    fprintf(fpasm, "je _uno_%d\n", cuantos_no);
    fprintf(fpasm, "mov eax, 0\n");
    fprintf(fpasm, "jmp _fin_not_%d\n", cuantos_no);
    fprintf(fpasm, "_uno_%d:\n", cuantos_no);
    fprintf(fpasm, "mov eax, 1\n");
    fprintf(fpasm, "_fin_not_%d:\n", cuantos_no);
    fprintf(fpasm, "push eax\n");

}


/**************** G2 ******************************************************************/

void igual(FILE* fpasm, int es_variable1, int es_variable2, int etiqueta)
{
/* GENERA EL CÓDIGO PARA COMPARAR SI LO QUE HAY EN LAS DOS PRIMERAS (DESDE LA CIMA)
POSICIONES DE LA PILA ES IGUAL, TENIENDO EN CUENTA QUE HAY QUE INDICAR SI SON DIRECCIONES O NO
*/

fprintf(fpasm, "pop dword eax\n");
fprintf(fpasm, "pop dword ebx\n");

if(es_variable1 == 1){ /* Si es variable es = 1 */
        fprintf(fpasm, "mov dword eax, [eax]\n");
}

if(es_variable2 == 1){ /* Si es variable es = 1 */
    fprintf(fpasm, "mov dword ebx, [ebx]\n");
}

fprintf(fpasm,"cmp eax, ebx\n");
fprintf(fpasm, "je igual\n");
fprintf(fpasm, "push dword 0\n");
fprintf(fpasm, "jmp near fin_igual_%d\n", etiqueta);
fprintf(fpasm, "igual:\n");
fprintf(fpasm, "push dword 1\n");
fprintf(fpasm, "fin_igual_%d:\n", etiqueta);

}
void distinto(FILE* fpasm, int es_variable1, int es_variable2, int etiqueta)
/* GENERA EL CÓDIGO PARA COMPARAR SI LO QUE HAY EN LAS DOS PRIMERAS (DESDE LA CIMA)
POSICIONES DE LA PILA ES DISTINTO, TENIENDO EN CUENTA QUE HAY QUE INDICAR SI SON
DIRECCIONES O NO
*/
{
fprintf(fpasm, "pop dword eax\n");
fprintf(fpasm, "pop dword ebx\n");

if(es_variable1 == 1){ /* Si es variable es = 1 */
    fprintf(fpasm, "mov dword eax, [eax]\n");
}

if(es_variable2 == 1){ /* Si es variable es = 1 */
    fprintf(fpasm, "mov dword ebx, [ebx]\n");
}

fprintf(fpasm,"cmp eax, ebx\n");
fprintf(fpasm, "jne distinto\n");
fprintf(fpasm, "push dword 0\n");
fprintf(fpasm, "jmp near fin_distinto_%d\n", etiqueta);
fprintf(fpasm, "distinto:\n");
fprintf(fpasm, "push dword 1\n");
fprintf(fpasm, "fin_distinto_%d:\n", etiqueta);

}

void menor_igual(FILE* fpasm, int es_variable1, int es_variable2, int etiqueta)
/* GENERA EL CÓDIGO PARA COMPARAR SI LO QUE HAY EN LA PRIMERA POSICION DE LA PILA
 ES MENOR O IGUAL A LA SEGUNDA POSICION, TENIENDO EN CUENTA QUE HAY QUE INDICAR
 SI SON DIRECCIONES O NO
*/
{
fprintf(fpasm, "pop dword eax\n");
fprintf(fpasm, "pop dword ebx\n");

if(es_variable1 == 1){ /* Si es variable es = 1 */
    fprintf(fpasm, "mov dword eax, [eax]\n");
}

if(es_variable2 == 1){ /* Si es variable es = 1 */
    fprintf(fpasm, "mov dword ebx, [ebx]\n");
}

fprintf(fpasm,"cmp eax, ebx\n");
fprintf(fpasm, "jle menor_igual_%d\n", etiqueta);
fprintf(fpasm, "push dword 0\n");
fprintf(fpasm, "jmp near fin_menor_igual_%d\n", etiqueta);
fprintf(fpasm, "menor_igual_%d:\n", etiqueta);
fprintf(fpasm, "push dword 1\n");
fprintf(fpasm, "fin_menor_igual_%d:\n", etiqueta);

}

void mayor_igual(FILE* fpasm, int es_variable1, int es_variable2, int etiqueta)
/* GENERA EL CÓDIGO PARA COMPARAR SI LO QUE HAY EN LA PRIMERA POSICION DE LA PILA
 ES MAYOR O IGUAL A LA SEGUNDA POSICION, TENIENDO EN CUENTA QUE HAY QUE INDICAR
 SI SON DIRECCIONES O NO
*/
{
fprintf(fpasm, "pop dword eax\n");
fprintf(fpasm, "pop dword ebx\n");

if(es_variable1 == 1){ /* Si es variable es = 1 */
    fprintf(fpasm, "mov dword eax, [eax]\n");
}

if(es_variable2 == 1){ /* Si es variable es = 1 */
    fprintf(fpasm, "mov dword ebx, [ebx]\n");
}

fprintf(fpasm,"cmp eax, ebx\n");
fprintf(fpasm, "jge mayor_igual_%d\n", etiqueta);
fprintf(fpasm, "push dword 0\n");
fprintf(fpasm, "jmp near fin_mayor_igual_%d\n", etiqueta);
fprintf(fpasm, "mayor_igual_%d:\n", etiqueta);
fprintf(fpasm, "push dword 1\n");
fprintf(fpasm, "fin_mayor_igual_%d:\n", etiqueta);

}

void menor(FILE* fpasm, int es_variable1, int es_variable2, int etiqueta)
/* GENERA EL CÓDIGO PARA COMPARAR SI LO QUE HAY EN LA PRIMERA POSICION DE LA PILA
 ES MENOR A LA SEGUNDA POSICION, TENIENDO EN CUENTA QUE HAY QUE INDICAR
 SI SON DIRECCIONES O NO
*/
{
fprintf(fpasm, "pop dword eax\n");
fprintf(fpasm, "pop dword ebx\n");

if(es_variable1 == 1){ /* Si es variable es = 1 */
    fprintf(fpasm, "mov dword eax, [eax]\n");
}

if(es_variable2 == 1){ /* Si es variable es = 1 */
    fprintf(fpasm, "mov dword ebx, [ebx]\n");
}

fprintf(fpasm,"cmp eax, ebx\n");
fprintf(fpasm, "jl menor_%d\n", etiqueta);
fprintf(fpasm, "push dword 0\n");
fprintf(fpasm, "jmp near fin_menor_%d\n", etiqueta);
fprintf(fpasm, "menor_%d:\n", etiqueta);
fprintf(fpasm, "push dword 1\n");
fprintf(fpasm, "fin_menor_%d:\n", etiqueta);

}

void mayor(FILE* fpasm, int es_variable1, int es_variable2, int etiqueta)
/* GENERA EL CÓDIGO PARA COMPARAR SI LO QUE HAY EN LA PRIMERA POSICION DE LA PILA
 ES MAYOR A LA SEGUNDA POSICION, TENIENDO EN CUENTA QUE HAY QUE INDICAR
 SI SON DIRECCIONES O NO
*/
{
fprintf(fpasm, "pop dword eax\n");
fprintf(fpasm, "pop dword ebx\n");

if(es_variable1 == 1){ /* Si es variable es = 1 */
    fprintf(fpasm, "mov dword eax, [eax]\n");
}

if(es_variable2 == 1){ /* Si es variable es = 1 */
    fprintf(fpasm, "mov dword ebx, [ebx]\n");
}

fprintf(fpasm,"cmp eax, ebx\n");
fprintf(fpasm, "jg near mayor_%d\n", etiqueta);
fprintf(fpasm, "push dword 0\n");
fprintf(fpasm, "jmp near fin_mayor_%d\n", etiqueta);
fprintf(fpasm, "mayor_%d:\n", etiqueta);
fprintf(fpasm, "push dword 1\n");
fprintf(fpasm, "fin_mayor_%d:\n", etiqueta);

}


/************ G1 **********************************************************************/

void leer(FILE* fpasm, char* nombre, int tipo)
{
/* GENERA EL CÓDIGO PARA LEER UNA VARIABLE DE NOMBRE nombre Y TIPO tipo (ESTE
AÑO SÓLO USAREMOS ENTERO Y BOOLEANO) DE CONSOLA LLAMANDO A LAS CORRESPONDIENTES
FUNCIONES DE ALFALIB (scan_int Y scan_boolean)

*/
    if (!fpasm || !nombre) return;

    fprintf(fpasm, "push dword _%s\n", nombre);
    if(tipo == ENTERO){
        fprintf(fpasm, "call scan_int\n");
    }else if(tipo == BOOLEANO){
        fprintf(fpasm, "call scan_boolean\n");
    }
    fprintf(fpasm, "add esp, 4\n");
}


void escribir(FILE* fpasm, int es_variable, int tipo)
{
/* GENERA EL CÓDIGO PARA ESCRIBIR POR PANTALLA LO QUE HAYA EN LA CIMA DE LA PILA
TENIENDO EN CUENTA QUE PUEDE SER UN VALOR INMEDIATO (es_referncia == 0) O UNA
DIRECCION (es_variable == 1) Y QUE HAY QUE LLAMAR A LA CORRESPONDIENTE
FUNCIÓN DE ALFALIB (print_int O print_boolean) DEPENDIENTO DEL TIPO (tipo == BOOLEANO
O ENTERO )
*/
    if (!fpasm) return;

    if(es_variable == 1){
        fprintf(fpasm, "pop dword eax\n");
        fprintf(fpasm, "mov eax, [eax]\n");
        fprintf(fpasm, "push dword eax\n");
    }
    if(tipo == ENTERO){
        fprintf(fpasm, "call print_int\n");
    }else if(tipo == BOOLEANO){
        fprintf(fpasm, "call print_boolean\n");
    }

    fprintf(fpasm, "add esp, 4\n");
    fprintf(fpasm, "call print_endofline\n");

}

void declararFuncion(FILE * fd_asm, char * nombre_funcion, int num_var_loc){
/*
- Generación de código para iniciar la declaración de una función.
- Es necesario proporcionar
    - Su nombre
    - Su número de variables locales
*/
    int resta = num_var_loc * 4;

    fprintf(fd_asm, "_%s:\n", nombre_funcion);
    fprintf(fd_asm, "push ebp\n");
    fprintf(fd_asm, "mov ebp, esp\n");
    fprintf(fd_asm,"sub esp, %d\n", resta);

}

void retornarFuncion(FILE * fd_asm, int es_variable){
/*
Generación de código para el retorno de una función.
○ La expresión que se retorna está en la cima de la pila.
○ Puede ser una variable (o algo equivalente) en cuyo caso exp_es_direccion vale 1
○ Puede ser un valor concreto (en ese caso exp_es_direccion vale 0)
*/
    if(!fd_asm)
        return;

    fprintf(fd_asm, "pop eax\n");
    if(es_variable == 1){
        fprintf(fd_asm, "mov dword eax, [eax]\n");
    }
    fprintf(fd_asm,"mov esp, ebp\n");
    fprintf(fd_asm,"pop ebp\n");
    fprintf(fd_asm,"ret\n");
}

void escribirParametro(FILE* fpasm, int pos_parametro, int num_total_parametros){
/*
Función para dejar en la cima de la pila la dirección efectiva del parámetro que ocupa la
posición pos_parametro (recuerda que los parámetros se ordenan con origen 0) de un total
de num_total_parametros
*/
    if(!fpasm)
        return;

    int aux = 4 * (1 + num_total_parametros - pos_parametro);

    fprintf(fpasm, "lea eax, [ebp + %d]\n", aux);
    fprintf(fpasm, "push dword eax\n");

}

void escribirVariableLocal(FILE* fpasm, int posicion_variable_local){
/*
Función para dejar en la cima de la pila la dirección efectiva de la variable local que ocupa
la posición posicion_variable_local (recuerda que ordenadas con origen 1)
*/

    if(!fpasm)
        return;

    int resta = posicion_variable_local * 4;

    fprintf(fpasm, "lea eax, [ebp - %d]\n", resta);
    fprintf(fpasm, "push dword eax\n");
}

void asignarDestinoEnPila(FILE* fpasm, int es_variable){
/*
●Función para poder asignar a un destino que no es una variable “global” (tipo _x) por
ejemplo parámetros o variables locales (ya que en ese caso su nombre real de alto nivel, no
se tiene en cuenta pues es realmente un desplazamiento a partir de ebp: ebp+4 o ebp-8 por
ejemplo).
● Se debe asumir que en la pila estará
    ○ Primero (en la cima) lo que hay que asignar
    ○ Debajo (se ha introducido en la pila antes) la dirección donde hay que asignar
● es_variable
    ○ Es 1 si la expresión que se va a asignar es algo asimilable a una variable
    (identificador, o elemento de vector)
    ○ Es 0 en caso contrario (constante u otro tipo de expresión)
*/
    if(!fpasm)
        return;

    fprintf(fpasm, "pop dword ebx\n");
    fprintf(fpasm, "pop dword eax\n");
    if (es_variable == 1){
        fprintf(fpasm, "mov dword eax, [eax]\n");
    }
    fprintf(fpasm, "mov dword [ebx], eax\n");
    fprintf(fpasm, "push dword ebx\n");
}

void operandoEnPilaAArgumento(FILE * fd_asm, int es_variable){
/*
● Como habrás visto en el material, nuestro convenio de llamadas a las funciones asume que
los argumentos se pasan por valor, esto significa que siempre se dejan en la pila “valores” y
no “variables”
● Esta función realiza la tarea de dado un operando escrito en la pila y sabiendo si es variable
o no (es_variable) se deja en la pila el valor correspondiente
*/

    if(!fd_asm)
        return;

    if(es_variable == 1){
        fprintf(fd_asm, "pop eax\n");
        fprintf(fd_asm,"mov eax, [eax]\n");
        fprintf(fd_asm,"push eax\n");
    }

}

void llamarFuncion(FILE * fd_asm, char * nombre_funcion, int num_argumentos){
/*
● Esta función genera código para llamar a la función nombre_funcion asumiendo que los
argumentos están en la pila en el orden fijado en el material de la asignatura.
● Debe dejar en la cima de la pila el retorno de la función tras haberla limpiado de sus
argumentos
● Para limpiar la pila puede utilizar la función de nombre limpiarPila
*/
    if(!fd_asm)
        return;

    fprintf(fd_asm, "call _%s\n",nombre_funcion);
    fprintf(fd_asm,"add esp, %d\n", num_argumentos*4);
    fprintf(fd_asm,"push dword eax\n");
}

void limpiarPila(FILE * fd_asm, int num_argumentos){
/*
● Genera código para limpiar la pila tras invocar una función
● Esta función es necesaria para completar la llamada a métodos, su gestión dificulta el
conocimiento por parte de la función de llamada del número de argumentos que hay en la
pila
*/
    if(!fd_asm)
        return;

    fprintf(fd_asm, "add esp, %d * 4\n", num_argumentos);

}

void ifthen_inicio(FILE * fpasm, int exp_es_variable, int etiqueta){
    if(!fpasm)
        return;

    fprintf(fpasm, "pop eax\n");
    if(exp_es_variable == 1)
        fprintf(fpasm, "mov eax, [eax]\n");

    fprintf(fpasm, "cmp eax, 0\n");
    fprintf(fpasm, "je near finthen_fin_%d\n", etiqueta);

}

void ifthen_fin(FILE * fpasm, int etiqueta){
    if(!fpasm)
        return;

    fprintf(fpasm, "finthen_fin_%d:\n", etiqueta);


}

void ifthenelse_inicio(FILE * fpasm, int exp_es_variable, int etiqueta){
    if(!fpasm)
        return;

    fprintf(fpasm, "pop eax\n");
    if(exp_es_variable == 1)
            fprintf(fpasm, "mov eax, [eax]\n");

    fprintf(fpasm, "cmp eax, 0\n");
    fprintf(fpasm, "je near else_%d\n", etiqueta);
}

void ifthenelse_fin_then( FILE * fpasm, int etiqueta){
    if(!fpasm)
        return;

    fprintf(fpasm, "jmp fin_then_else_%d\n", etiqueta);
    fprintf(fpasm, "else_%d:\n", etiqueta);
}

void ifthenelse_fin( FILE * fpasm, int etiqueta){
    if(!fpasm)
        return;

    fprintf(fpasm, "fin_then_else_%d:\n", etiqueta);
}

void while_inicio(FILE * fpasm, int etiqueta){
    if(!fpasm)
        return;

    fprintf(fpasm, "inicio_while_%d:\n", etiqueta);

}

void while_exp_pila (FILE * fpasm, int exp_es_variable, int etiqueta){
    if(!fpasm)
        return;

    fprintf(fpasm, "pop eax\n");
    if (exp_es_variable == 1){
        fprintf(fpasm, "mov eax, [eax]\n");
    }
    fprintf(fpasm, "cmp eax, 0\n");
    fprintf(fpasm, "je near fin_while_%d\n", etiqueta);
}

void while_fin( FILE * fpasm, int etiqueta){
    if(!fpasm)
        return;

    fprintf(fpasm, "jmp near inicio_while_%d\n", etiqueta);
    fprintf(fpasm, "fin_while_%d:\n", etiqueta);
}

void escribir_elemento_vector(FILE * fpasm,char * nombre_vector,int tam_max, int exp_es_direccion){
    if(!fpasm)
        return;

    fprintf(fpasm, "pop dword eax\n");
    if (exp_es_direccion == 1){
        fprintf(fpasm, "mov eax, [eax]\n");
    }
    fprintf(fpasm, "cmp eax, 0\n");
    fprintf(fpasm, "jl near fin_indice_fuera_rango\n");
    fprintf(fpasm, "cmp eax, %d - 1\n", tam_max);
    fprintf(fpasm, "jg near fin_indice_fuera_rango\n");
    fprintf(fpasm, "mov dword edx, _%s\n", nombre_vector);
    fprintf(fpasm, "lea eax, [edx + eax*4]\n");
    fprintf(fpasm, "push dword eax\n");
}

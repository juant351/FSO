// TORIBIO GONZALEZ HECTOR - 12437502E 
// TORRES VILORIA JUAN - 44919220J



#include <stdio.h>
#include <pthread.h>
#include <stdlib.h>
#include <stdbool.h>
#include <semaphore.h>
// DEFINIR TIPOS DE DATOS.

struct dataBuffer{
	char m_dec;
	char m_uni;
	char m_char;
	bool m_bFin;
 };

// VARIABLES GLOBABLES.

struct dataBuffer* buffer1;
int tamBuffer;               //Variable para el tamaño del buffer
char decodificado[100];      //Vector de caracteres decodificados
int indexC = 0;
sem_t hayDato, hayEspacio, mutexConsumidores;
// Metodo para contar el nº caracteres de una cadena.

//HILOS.
int stringSize(char v[] )
{
   int size = 0;
   for (int i=0; v[i]!= 0; i++)
   {
      size++;
   }
   return size;
}
void *productor (void *arg){
	printf("Arrancando el productor\n");
	fflush(stdout);
	FILE *fich;
	fich = (FILE *) arg;
	char cadena[10];
	struct dataBuffer dato;
	int index = 0;	
	//Mientras haya caracteres.
	while(fscanf(fich, "%s", cadena) != EOF){
		// Validar si tiene 3 caracteres.
   		
		if(stringSize(cadena) == 3){
			printf("%s\n", cadena);
			fflush(stdout);
			dato.m_dec = cadena[0]; //Decimal es la primera posicion de la cadena.
			dato.m_uni = cadena[1]; //Unidad es la segunda posicion de la cadena.
     			dato.m_char = cadena[2]; //Caracter es la tercer posicion de la cadena.
			dato.m_bFin = false;	//Ver si es fin de buffer.

			//Escritura en buffer.

			sem_wait(&hayEspacio);	//Esperar a que haya espacio en el buffer.

			buffer1[index] = dato; //Escribir dato en la posicion index del buffer.
			index = (index+1)% tamBuffer;

			sem_post(&hayDato); // Señala que hay dato en el buffer.
	
		}
		}
		dato.m_bFin = true;
		sem_wait(&hayEspacio);

		buffer1[index] = dato;
		index = (index + 1)% tamBuffer;

		sem_post(&hayDato);
		pthread_exit(NULL);
}

void *consumidor (void *arg){
	printf("Arrancando el consumidor\n");
	fflush(stdout);
	struct dataBuffer dato;
	int index = 0;
	while(1){
		sem_wait(&hayDato);		//Esperar a que haya dato
    sem_wait(&mutexConsumidores); // Esperar si ya hay un consumidor vaciando el buffer. (obteniendo datos)
		dato = buffer1[index];
   
		  if(dato.m_bFin != true){
		    indexC = (indexC + 1)% tamBuffer;
	    }
     else{
       sem_post(&mutexConsumidores);
       sem_post(&hayDato);
       break;
     }
     sem_wait(&mutexConsumidores);
	   sem_post(&hayEspacio);		//Señala que hay un hueco en el buffer (lo vacia el consumidor el dato).
				
		if((dato.m_dec <= 'm' && dato.m_dec >= 'd') && (dato.m_uni >='F' && dato.m_uni <='O') && (dato.m_char >= '!' && dato.m_char <= '{')){
			printf("De  locasos");
			int decenas = 	(dato.m_dec - 100)*10;
			int unidades = (dato.m_uni - 70);
			int posicion = decenas + unidades;
			decodificado[posicion] = dato.m_char - 1;
		}
	}
	pthread_exit(NULL);


}
// MAIN.

int main(int argc, char *argv[]){
	FILE *fichInput;

	//Valida si el numero de argumentos pasados al programa es 5
	if(argc != 5){
		printf("Numero de argumentos incorrecto");
		return -1;
	}
	
	// Validacion del primer argumento (el fichero).
	fichInput = fopen(argv[1], "r");

	if(fichInput == NULL){
		return -1;
	}

	// Validacion del tercer argumento.
	if(sscanf(argv[3], "%d", &tamBuffer) != 1){
		return -1;
	}
	// Hacemos las validaciones al principio, para no inicializar hilos y buffers innecesarios.
	buffer1 = (struct dataBuffer*)malloc(tamBuffer*sizeof(struct dataBuffer));
	
	if(buffer1 == NULL){
		printf("Error al asignar memoria para el buffer circular");
		return -1;
	}
	// Inicializacion de semaforos
	sem_init(&hayDato,0,0); // Hay dato = 0.
	sem_init(&hayEspacio, 0, tamBuffer); // Hay espacio = tamBuffer.
  sem_init(&mutexConsumidores,0,1);
	
	//Creacion de hilos.i
	pthread_t tProductor, tConsumidor;
	pthread_create(&tProductor, NULL, productor, (void*)fichInput);
	pthread_create(&tConsumidor, NULL, consumidor, NULL);

	// Esperar a que acaben los hilos.
	pthread_join(tProductor, NULL);
	pthread_join(tConsumidor, NULL);
	
	// Liberar memoria.
	sem_destroy(&hayDato);
	sem_destroy(&hayEspacio);
  sem_destroy(&mutexConsumidores);
	for(int i = 0; i<100; i++){
		printf("%c\n", decodificado[i]);
	}

	free(buffer1);
	return 1;
}

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

struct datVector{
	int m_procesados;
	int m_incorrectos;
	int m_maxIndex;
};

// VARIABLES GLOBABLES.

int numCons;
struct datVector* vector;
int consumidorTerminado;
struct dataBuffer* buffer1;
int tamBuffer;               //Variable para el tamaño del buffer
char decodificado[100];      //Vector de caracteres decodificados
int indexC = 0;

sem_t hayDato, hayEspacio, mutexConsumidores, hayDatoConsumidor, hayEspacioConsumidor;
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
	FILE *fich;
	fich = (FILE *) arg;
	char cadena[10];
	struct dataBuffer dato;
	int index = 0;	
	//Mientras haya caracteres.
	while(fscanf(fich, "%s", cadena) != EOF){
		// Validar si tiene 3 caracteres.
   		
		if(stringSize(cadena) == 3){
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
	int *id = (int*) arg;
	struct dataBuffer dato;
	int proceso = 0;
	int incorrectos = 0;
	int mxIndex = 0;
	struct datVector datoCons;
	while(1){
		sem_wait(&hayDato);		//Esperar a que haya dato
		sem_wait(&mutexConsumidores); // Esperar si ya hay un consumidor vaciando el buffer. (obteniendo datos)
		  dato = buffer1[indexC];
   		if(dato.m_bFin != true){
		      indexC = (indexC + 1)% tamBuffer;
	    	}else{
       			sem_post(&mutexConsumidores);
       			sem_post(&hayDato);
       			break;
     		}
     		sem_post(&mutexConsumidores);
	   	  sem_post(&hayEspacio);		//Señala que hay un hueco en el buffer (lo vacia el consumidor el dato).

		if((dato.m_dec <= 'm' && dato.m_dec >= 'd') && (dato.m_uni >='F' && dato.m_uni <='O') && (dato.m_char >= '!' && dato.m_char <= '{')){
       proceso++;
			int decenas = 	(dato.m_dec - 100)*10;
			int unidades = (dato.m_uni - 70);
			int posicion = decenas + unidades;
			decodificado[posicion] = dato.m_char - 1;
			if (posicion > mxIndex){
				mxIndex = posicion;
			}
		}else{
   proceso++;
			incorrectos++;
		}
	}
  datoCons.m_procesados = proceso;
  datoCons.m_incorrectos = incorrectos;
  datoCons.m_maxIndex = mxIndex;
  
	vector[*id] = datoCons;
	sem_wait(&hayEspacioConsumidor);
	consumidorTerminado = *id;
	sem_post(&hayDatoConsumidor);
	pthread_exit(NULL);


}

void *lector (void *arg){    
  FILE *fichSalida = (FILE*)arg;
	struct datVector dato;
	int totalProceso = 0;
  int totalIncorrectos = 0;
  int maxIndex = 0;
	int index;
  int correctos;
	for(int i = 0; i< numCons; i++){
		sem_wait(&hayDatoConsumidor);
		index = consumidorTerminado;
		dato = vector[index];
    correctos = dato.m_procesados - dato.m_incorrectos;
    fprintf(fichSalida, "Hilo %d\n",index);
		fprintf(fichSalida,"\tTokens procesados: %d\n",dato.m_procesados);
    fprintf(fichSalida,"\tTokens correctos: %d\n",correctos);
    fprintf(fichSalida,"\tTokens incorrectos: %d\n",dato.m_incorrectos);
    fprintf(fichSalida,"\tMaxIndex: %d\n",dato.m_maxIndex);
		totalProceso += dato.m_procesados;
    totalIncorrectos += dato.m_incorrectos;
    if (dato.m_maxIndex > maxIndex){
      maxIndex = dato.m_maxIndex;
    }
		sem_post(&hayEspacioConsumidor);
	}
  fprintf(fichSalida, "Resultado final (los que procesa el consumidor final). \n");
  fprintf(fichSalida,"\tTokens procesados: %d\n",totalProceso);
  fprintf(fichSalida,"\tTokens correctos: %d\n",totalProceso-totalIncorrectos);
  fprintf(fichSalida,"\tTokens incorrectos: %d\n",totalIncorrectos);
  fprintf(fichSalida,"\tMaxIndex: %d\n",maxIndex);
  if(maxIndex == ((totalProceso-totalIncorrectos)-1)){//Comprueba si el mensaje es corercto
        fprintf(fichSalida, "Mensaje: Correcto\n\n");
    }else{
        fprintf(fichSalida, "Mensaje: Incorrecto\n\n");
    }
  fprintf(fichSalida, "Mensaje traducido: ");
 	for(int i = 0; i<100; i++){
		fprintf(fichSalida, "%c", decodificado[i]);
	} 
   pthread_exit(NULL);
}

// MAIN.

int main(int argc, char *argv[]){
	FILE *fichInput;
	FILE *fichOutput;
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
  fichOutput = fopen(argv[2], "w");
  
  if(fichOutput == NULL){
		return -1;
	}
   
	// Validacion del tercer argumento.
	if(sscanf(argv[3], "%d", &tamBuffer) == 0){
		return -1;
	}
 
 	if(sscanf(argv[4], "%d", &numCons) < 1){
		return -1;
	}

	// Hacemos las validaciones al principio, para no inicializar hilos y buffers innecesarios.
	buffer1 = (struct dataBuffer*)malloc(tamBuffer*sizeof(struct dataBuffer));
	
	if(buffer1 == NULL){
		printf("Error al asignar memoria para el buffer circular");
		return -1;
	}
 
 vector = (struct datVector*)malloc(numCons*sizeof(struct datVector));
 
 if(vector == NULL){
		printf("Error al asignar memoria para el vector");
		return -1;
	}
 
	// Inicializacion de semaforos
	sem_init(&hayDato,0,0); // Hay dato = 0.
	sem_init(&hayEspacio, 0, tamBuffer); // Hay espacio = tamBuffer.
	sem_init(&mutexConsumidores, 0, 1);
	sem_init(&hayDatoConsumidor, 0, 0);
	sem_init(&hayEspacioConsumidor, 0, 1);
	
	//Creacion de hilos.i
	pthread_t tProductor, tLector;
  pthread_t tconsumidor[numCons];
  int I[numCons];
	pthread_create(&tProductor, NULL, productor, (void*)fichInput);
  for (int i=0; i<numCons; i++){
    I[i] = i;
	  pthread_create(&tconsumidor[i], NULL, consumidor, (void *)&I[i]);
  }
	pthread_create(&tLector, NULL, lector, (void*)fichOutput);

	// Esperar a que acaben los hilos.
	pthread_join(tProductor, NULL);
  for (int i=0; i<numCons; i++){
	  pthread_join(tconsumidor[i], NULL);
  }
  
	pthread_join(tLector, NULL);
	// Liberar memoria.
	sem_destroy(&hayDato);
	sem_destroy(&hayEspacio);
	sem_destroy(&mutexConsumidores);
	sem_destroy(&hayDatoConsumidor);
	sem_destroy(&hayEspacioConsumidor);
  fclose(fichInput);
  fclose(fichOutput);

  free(vector);
	free(buffer1);
	return 1;
}

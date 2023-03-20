// TORIBIO GONZALEZ HECTOR - 12437502E 
// TORRES VILORIA JUAN - 44919220J


#include <studio.h>
#include <pthread.h>

// DEFINIR TIPOS DE DATOS.

struct dataBuffer{
	char m_dec;
	char m_uni;
	char m_char;
	bool m_bFin;
}

// VARIABLES GLOBABLES.

struct dataBuffer* buffer1;
int tamBuffer;
char decodificado[100];
sem_t hayDato, hayEspacio;

// Metodo para contar el nº caracteres de una cadena.
int cuentaStr(char cadena[]){
	int cont = 0;
	for(int i = 0; i < cadena.length; i++){
		if(cadena[i]!= NULL && cadena[i] != ' '){
			cont++;
		}
	}
	return cont;
}

//HILOS.
void *productor (void *arg){
	FILE *fich;
	fich = (FILE *) arg;
	char cadena[10];
	struct dataBuffer dato;
	
	//Mientras haya caracteres.
	while(fscanf(fich, "%s", cadena) != EOF){
		// Validar si tiene 3 caracteres.
		if(cuentaStr(cadena) == 3){
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
		pthread_exit(NULL);
	}
}

void *consumidor (void *arg){
	struct dataBuffer dato;
	int index = 0;
	while(1){
		dato = buffer1[index];
		
		if(dato.m_bFin == true){
			break;
		}
		sem_wait(&hayDato);	//Esperar a que haya dato.
		index = (index + 1)& tamBuffer;
		sem_post(&hayEspacio); // Señala que hay hueco en el buffer (lo vacia al consumir el dato).
		
		if(dato.m_dec > 'm'){
			int posicion = 	(dato.m_dec - 100)*10;
			decodificado[posicion] = dato.m_char -1;
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
	if(scanf(argv[3], "%d", &tamBuffer) != 1){
		return -1;
	}

	// Hacemos las validaciones al principio, para no inicializar hilos y buffers innecesarios.
	
	buffer1 = (struct dataBuffer*)malloc(tamBuffer*sizeof(struct dataBuffer));
	
	if(buffer1 = NULL){
		printf("Error al asignar memoria para el buffer circular");
		return -1;
	}
	
	// Inicializacion de semaforos
	sem_init(&hayDato,0,0); // Hay dato = 0.
	sem_init(&hayEspacio, 0, tamBuffer); // Hay espacio = tamBuffer.
	
	//Creacion de hilos.
	pthread_t tProductor, tConsumidor;
	pthread_create(&tProductor, NULL, productor, NULL);
	pthread_create(&tConsumidor, NULL, consumidor, NULL);

	// Esperar a que acaben los hilos.
	pthread_join(&tProductor);
	pthread_join(&tConsumidor);
	
	// Liberar memoria.
	sem_destroy(&hayDato);
	sem_destroy(&hayEspacio);

	free(buffer1);

}

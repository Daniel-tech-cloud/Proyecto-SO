#include <stdio.h>
#include <pthread.h>
#include <stdlib.h>
#include <unistd.h>


char *seccrit1;
char *seccrit2;
int contador;
//int aux


typedef struct producto{
	int seccion;
	int index;
	int inicio;
	int fila;
	int limite;
	int numero;
	char *compania;
	char *mensaje;

} producto;

pthread_mutex_t mutexconsumidor[2], mutexproductor[3], mutexContador;

static void *producir(void* p){
	producto prod = *(producto *) p;
	
	printf("Hilo %d valor inicio %d, valor limite %d\n",prod.index, prod.inicio, prod.limite);
	for (int i = prod.inicio; i < prod.limite; ++i){		
		while(1){
			/*
				Intentamos desbloquear la sección crítica 1
				Retorna: 
					0 si se pudo bloquear
					1 si no se pudo bloquear
			*/
		 	
			 /*Sección crítica uno */
			
			
			if(!pthread_mutex_trylock(&mutexproductor[0])){
				prod.fila = 1;
				printf("Número: ");
				scanf("%d", &prod.numero);
				printf("Compania: ");
				prod.compania = malloc(sizeof(char) * 20);
				//printf("Holaaa");
				fgets(prod.compania, 10, stdin);

				seccrit1 = (char*)&prod.numero;
				seccrit1 = prod.compania;
				printf("Numero: %d compania: %s", prod.numero, prod.compania);
				pthread_mutex_unlock(&mutexconsumidor[0]);
				break;
			}
			else if(!pthread_mutex_trylock(&mutexproductor[1])) {	
				prod.mensaje = malloc(sizeof(char) * 20);
				fgets(prod.mensaje, 20, stdin);
				prod.fila = 2;			
				seccrit2 = prod.mensaje;
				printf("Mensaje: %s", prod.mensaje);
				pthread_mutex_unlock(&mutexconsumidor[1]);
				break;
			}
		}
	}
	pthread_exit(0);
}

static void *consumir(void* p){
	producto prod = *(producto *) p;
	while(1){
		while(1){
			if(!pthread_mutex_trylock(&mutexconsumidor[0])){
				//prod.seccion=1;
				printf("Consumidor: %d numero leido: %d compañía: %s Leido de la fila %d en la sección critica 1", prod.index, prod.numero, prod.compania, prod.fila);
				pthread_mutex_lock(&mutexContador);
				contador++;
				pthread_mutex_unlock(&mutexContador);
				pthread_mutex_unlock(&mutexproductor[0]);
				
				break;
			}else if (!pthread_mutex_trylock(&mutexconsumidor[1])){
				//prod.seccion = 2;
				printf("Consumidor: %d numero leido: %d compañía: %s Leido de la fila %d en la sección critica 2", prod.index, prod.numero, prod.compania, prod.fila);
				pthread_mutex_lock(&mutexContador);
				contador++;
				pthread_mutex_unlock(&mutexContador);
				pthread_mutex_unlock(&mutexproductor[1]);
				
				break;
			}
			
		}
		pthread_mutex_lock(&mutexContador);
		if(contador >= prod.limite){
			/*pthread_mutex_unlock(&mutexproductor[0]);
			pthread_mutex_unlock(&mutexproductor[1]);
			pthread_mutex_unlock(&mutexContador);

			pthread_mutex_unlock(&mutexconsumidor[0]);

			pthread_mutex_unlock(&mutexconsumidor[1]);*/
			break;
		}
		pthread_mutex_unlock(&mutexContador);

	}
	pthread_exit(0);
}

int main(){
	//Variables
	seccrit1 = malloc(sizeof(char) * 2);
	seccrit2 = malloc(sizeof(char) * 2);
	pthread_t hiloproductor[3], hiloconsumidor[2];
	producto p[3];
	producto m[2];
	int inicio = 0;
	contador = 0;

	//Inicializamos los mutex
	pthread_mutex_init(&mutexconsumidor[0], NULL);
	pthread_mutex_init(&mutexproductor[0], NULL);
	pthread_mutex_init(&mutexconsumidor[1], NULL);
	pthread_mutex_init(&mutexproductor[1], NULL);
	pthread_mutex_init(&mutexContador, NULL);

	//Cerramos los mutex
	pthread_mutex_lock(&mutexconsumidor[0]);
	pthread_mutex_lock(&mutexconsumidor[1]);

	int cantidadPorProductor = 3;
	int cantidadPorConsumidor = 0;
	printf("Cantidad: ");
	scanf("%d", &cantidadPorProductor);

	cantidadPorConsumidor = cantidadPorProductor * 3;
	
	//Creamos ambos hilos	
	for (int i = 0; i < 3; i++){
		p[i].index = i+1;
		p[i].inicio = 0;
		p[i].limite = cantidadPorProductor;
		//p[i].valor = 11*(i+1);

		pthread_create(&hiloproductor[i], NULL, producir, (void *) &p[i]);

	}
	for (int i = 0; i < 2; i++){
		m[i].index = i+1;
		m[i].inicio = 0;
		m[i].limite = cantidadPorConsumidor-1;

		pthread_create(&hiloconsumidor[i], NULL, consumir, (void *) &m[i]);
	}

	//El padre espera a los procesos.
	for (int i = 0; i < 3; i++)
		pthread_join(hiloproductor[i], NULL);

	for (int i = 0; i < 2; i++)
		pthread_join(hiloconsumidor[i], NULL);

	//Se destruyen los mutex
	pthread_mutex_destroy(&mutexconsumidor[0]);
	pthread_mutex_destroy(&mutexproductor[0]);
	pthread_mutex_destroy(&mutexconsumidor[1]);
	pthread_mutex_destroy(&mutexproductor[1]);
}


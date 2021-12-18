#include <stdio.h>
#include <pthread.h>
#include <stdlib.h>
#include <unistd.h>

int seccrit1, seccrit2;
int contador;

typedef struct producto{
	int index;
	int inicio;
	int limite;
	int valor;

} producto;

pthread_mutex_t mutexconsumidor[2], mutexproductor[2], mutexContador;

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
			if(!pthread_mutex_trylock(&mutexproductor[0])){
				seccrit1 = prod.valor;
				printf("El hilo %d produjo  el valor %d\n", prod.index, seccrit1);
				pthread_mutex_unlock(&mutexconsumidor[0]);
				break;
			}
			else if(!pthread_mutex_trylock(&mutexproductor[1])) {				
				seccrit2 = prod.valor;
				printf("El hilo %d produjo  el valor %d\n", prod.index, seccrit2);
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
				printf("el hilo %d consumio el valor %d\n", prod.index,seccrit1);
				pthread_mutex_lock(&mutexContador);
				contador++;
				pthread_mutex_unlock(&mutexContador);
				pthread_mutex_unlock(&mutexproductor[0]);
				
				break;
			}else if (!pthread_mutex_trylock(&mutexconsumidor[1])){
				printf("el hilo %d consumio el valor %d\n", prod.index,seccrit2);
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

	int cantidadPorProductor = 0;
	int cantidadPorConsumidor = 0;

	scanf("%d", &cantidadPorProductor);

	cantidadPorConsumidor = cantidadPorProductor * 3;
	
	//Creamos ambos hilos	
	for (int i = 0; i < 3; i++){
		p[i].index = i+1;
		p[i].inicio = 0;
		p[i].limite = cantidadPorProductor;
		p[i].valor = 11*(i+1);

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
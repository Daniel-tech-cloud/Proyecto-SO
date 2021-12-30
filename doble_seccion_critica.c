#include <stdio.h>
#include <pthread.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>

int contador;

typedef struct producto{
	int seccion;
	int index;
	int inicio;
	int fila;
	int limite;
	char numero[10];
	char compania[20];
	char mensaje[250];

} producto;

producto produc;
producto seccrit1[2];
producto seccrit2[2];

pthread_mutex_t mutexConsumidor, mutexProductor, mutexContador;

static void *producir (void * p) {
    producto prod = * (producto *) p;
    // Para saber el hilo que produce
    printf("Hilo %d valor inicio %d, valor limite %d\n",prod.index, prod.inicio, prod.limite);
	printf("\n");

    for (int i = prod.inicio; i < prod.limite; ++i){		
		while(1){
			if(!pthread_mutex_trylock(&mutexProductor)){
				// Compañia y teléfono
				strcpy(produc.compania, "Telcel");
				strcpy(produc.numero, "5524905676");
				
				printf("Mensaje: %s", produc.mensaje);
				printf("Número: %s", produc.numero);
				printf("\n");
				printf("Compañía: %s", produc.compania);
				printf("\n");
				// seccrit1[0] = prod.valor;
				// printf("El hilo %d produjo  el valor %d\n", prod.index, seccrit1);
				
				pthread_mutex_unlock(&mutexConsumidor);
				break;
			}else{
				pthread_mutex_trylock(&mutexProductor);
				// Mensaje
				strcpy(produc.mensaje, "Este es un mensaje");
				printf("Mensaje: %s", produc.mensaje);
				printf("\n");
				// seccrit2[0] = prod.valor;
				// printf("El hilo %d produjo  el valor %d\n", prod.index, seccrit2);
				pthread_mutex_unlock(&mutexConsumidor);
				break;
			}
		}
	}
	pthread_exit(0);
}

static void consumir(void p){
	producto prod = *(producto *) p;
	while(1){
		while(1){
			if(!pthread_mutex_trylock(&mutexConsumidor)){
                // Leeremos la sección crítica 1
				// printf("el hilo %d consumio el valor %d\n", prod.index,seccrit1);
				pthread_mutex_lock(&mutexContador);
				contador++;
				pthread_mutex_unlock(&mutexContador);
				pthread_mutex_unlock(&mutexProductor);
				
				break;
			}else{
				pthread_mutex_lock(&mutexConsumidor);
                // Leeremos la sección crítica 2
				// printf("el hilo %d consumio el valor %d\n", prod.index,seccrit2);
				pthread_mutex_lock(&mutexContador);
				contador++;
				pthread_mutex_unlock(&mutexContador);
				pthread_mutex_unlock(&mutexProductor);
				
				break;
			}
			
		}
		pthread_mutex_lock(&mutexContador);
		if(contador >= prod.limite){

			pthread_mutex_unlock(&mutexProductor);
			pthread_mutex_unlock(&mutexContador);		
			pthread_mutex_unlock(&mutexConsumidor);
			break;
		}
		pthread_mutex_unlock(&mutexContador);

	}
	pthread_exit(0);
}

// main
int main(){
	
	//Variables
	pthread_t hiloproductor[3], hiloconsumidor[2];
	producto p[3];
	producto m[2];
	int inicio = 0;
	contador = 0;

	//Inicializamos los mutex
	pthread_mutex_init(&mutexConsumidor, NULL);
	pthread_mutex_init(&mutexProductor, NULL);
	pthread_mutex_init(&mutexContador, NULL);

	//Cerramos los mutex
	pthread_mutex_lock(&mutexConsumidor);
	pthread_mutex_lock(&mutexProductor);

	int cantidadPorProductor = 0;
	int cantidadPorConsumidor = 0;

	scanf("%d", &cantidadPorProductor);

	cantidadPorConsumidor = cantidadPorProductor * 3;
	
	//Creamos ambos hilos	
	for (int i = 0; i < 3; i++){
		p[i].index = i+1;
		p[i].inicio = 0;
		p[i].limite = cantidadPorProductor;
		// p[i].compania[0] = "Prueba";
		// p[i].numero[0]= "5524905676";

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
	pthread_mutex_destroy(&mutexConsumidor);
	pthread_mutex_destroy(&mutexProductor);
}
#include <iostream>
#include <pthread.h>

using namespace std;

struct dados {
    int* vetor;
    int identificador_thread;
};

int tamanho_vetor = 100000;
int numero_threads = 4;

void* soma(void* p){
    dados* ptr = (dados*)p;
    int n = ptr->identificador_thread;
    
    int* soma = new int;

    *soma = 0;

    for(int i = n * (tamanho_vetor / numero_threads); i < (n + 1) * (tamanho_vetor / numero_threads); i++) {
        *soma = (*soma) + ptr->vetor[i];
    }
     
    pthread_exit(soma);
}

int main(void){
    int* vetor_inteiro = new int[tamanho_vetor];
    
    srand(time(NULL));

    for(int i = 0; i < tamanho_vetor; i++) {
        vetor_inteiro[i] = rand() % 10 + 1;
    }
    
    dados dados_thread[numero_threads];

    for(int i = 0; i < numero_threads; i++) {
        dados_thread[i].identificador_thread = i;
        dados_thread[i].vetor = vetor_inteiro;
    }

    pthread_t tid[numero_threads];
    
    for(int i = 0; i < numero_threads; i++) {
        pthread_create(&tid[i], NULL, soma, &dados_thread[i]);
    }

    int* somas[numero_threads];
    
    for(int i = 0; i < numero_threads; i++) {
        pthread_join(tid[i], (void**)&somas[i]);
    }

    int somaParalelo = 0;

    for(int i = 0; i < numero_threads; i++) {
        somaParalelo += *(somas[i]);

        delete somas[i];
    }
    
    cout << "Soma Paralela = " << somaParalelo << endl;

    int somaComum = 0;

    for(int i = 0; i < tamanho_vetor; i++) {
        somaComum += vetor_inteiro[i];
    }

    cout << "Soma Comum = " << somaComum << endl;

    delete[] vetor_inteiro;
    
    return 0;
}
/*******************************************************************************
 * Escreva um código que cria um array contendo n valores inteiros e utilize 
 * threads para paralelizar a soma de todos os valores.
 * Você pode definir o array como global para que as threads possam acessá-lo 
 * diretamente.
 * Extra: modifique o código definindo o array como local e use uma estratégia 
 * para passá-lo como parâmetro para as threads.
 */

#include <pthread.h>
#include <iostream>
#include <sys/sysinfo.h>
#include <chrono>
using namespace std;

const int tam=250000000;            //reduzir tam caso tenha menos de 1GB livre
const int nthreads=get_nprocs();    //mudar para 1 para comparar tempo gasto

//! IMPLEMENTAÇÃO UTILIZANDO ARRAY GLOBAL
// int *valores= new int[tam];

// void *somaGlobal(void *p) {
//     int id=((long)p);
//     int ini=id*tam/nthreads;
//     int fim=(id+1)*tam/nthreads;
//     long soma=0;
//     for (int i=ini; i<fim; ++i)
//         soma+=valores[i];
//     pthread_exit((void*)soma);
// }

struct params{
    int tid;
    int *dados;
};

void *somaStruct(void *p) {    
    params par=*(params*)p;
    int id=par.tid;
    int ini=id*tam/nthreads;
    int fim=(id+1)*tam/nthreads;
    long soma=0;
    for (int i=ini; i<fim; ++i)
        soma+= par.dados[i];
    delete (params*)p;
    pthread_exit((void*)soma);
}

int main() {
    int *valores=new int[tam];
    for (int i=0; i<tam; ++i)
        valores[i]=i;
    
    chrono::time_point<chrono::high_resolution_clock> tp1,tp2;
    tp1=chrono::high_resolution_clock::now();

    pthread_t threads[nthreads];
    for (long i=0; i<nthreads; ++i) {
        //! CRIAÇÃO DA THREAD CONSIDERANDO ARRAY GLOBAL
        // pthread_create(&threads[i],NULL,somaGlobal,(void*)i);
        //! CRIAÇÃO DA THREAD CONSIDERANDO STRUCT
        params *p = new params;
        p->tid = i;
        p->dados = valores;
        pthread_create(&threads[i],NULL,somaStruct,(void*)p);
    }

    long soma=0;
    // Fazer join em cada thread t contida em threads
    for (pthread_t &t:threads) {
        int *r;
        pthread_join(t,(void**)&r);
        soma+=(long)r;
    }

    tp2=chrono::high_resolution_clock::now();
    double tempo=chrono::duration<double,ratio<1,1000>>(tp2-tp1).count();

    cout << "Soma dos valores: " << soma <<endl;
    cout << "Tempo gasto com "<<nthreads<<" threads: "<<tempo<<"ms" <<endl;
    delete[] valores;
    return 0;
}
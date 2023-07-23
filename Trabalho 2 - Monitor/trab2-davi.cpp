#include <iostream>
#include <thread>
#include <mutex>
#include <condition_variable>
#include <barrier>
#include <vector>

using namespace std;

int n1 = 1000; 
int n2 = 100;
int N = 100;
int C = 10;

class MonitorCarro {
private:
    int capacidade;
    int qtdPassageiros;
    bool esperando_encher;
    bool esperando_esvaziar;
    mutex mux;
    condition_variable_any cheio, vazio, esperando_entrar, esperando_sair;

public:
    MonitorCarro(int c) {
        capacidade = c;
        qtdPassageiros = 0;
        esperando_encher = false;
        esperando_esvaziar = false;
    }

    void espera_encher() {
        mux.lock();

        esperando_encher = true;

        esperando_entrar.notify_all();

        while(qtdPassageiros < capacidade) cheio.wait(mux);

        esperando_encher = false;

        mux.unlock();
    }

    void espera_esvaziar() {
        mux.lock();

        esperando_esvaziar = true;

        esperando_sair.notify_all();

        while(qtdPassageiros > 0) vazio.wait(mux);

        esperando_esvaziar = false;

        mux.unlock();
    }

    void entra_no_carro() {
        mux.lock();

        while(!esperando_encher || (capacidade == qtdPassageiros)) esperando_entrar.wait(mux);

        qtdPassageiros++;

        cheio.notify_one();

        mux.unlock();
    }

    void sai_do_carro() {
        mux.lock();

        while(!esperando_esvaziar) esperando_sair.wait(mux);

        qtdPassageiros--;

        vazio.notify_one();

        mux.unlock();
    }
};

MonitorCarro montanharussa(C);

void carroT() {
    for(int i = 0; i < n1; i++) {
        montanharussa.espera_encher();
        montanharussa.espera_esvaziar();
    }
}

barrier barreira(N);

void clienteT() {
    for(int i = 0; i < n2; i++) {
        montanharussa.entra_no_carro();
        montanharussa.sai_do_carro();
        
        barreira.arrive_and_wait();
    }
}

int main() {
    thread carro(carroT);
    vector<thread> clientes;
    
    for(int i = 0; i < N; i++) clientes.push_back(thread(clienteT));

    carro.join();

    for(int i = 0; i < N; i++) clientes[i].join();

    return 0;
}
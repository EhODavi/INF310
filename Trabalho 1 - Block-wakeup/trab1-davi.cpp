#include <iostream>
#include <thread>
#include <mutex>
#include <map>
#include <set>

using namespace std;

int bSize, pin, pout, cont;
int *buffer;
mutex muxBW, muxPC;
map<thread::id, int> A;
set<thread::id> produtoresEsperando, consumidoresEsperando;

void block() {
    bool sair = false;
    thread::id eu = this_thread::get_id();

    do {
        muxBW.lock();

        if(A[eu] > 0) {
            A[eu]--;
            sair = true;
        }

        muxBW.unlock();
    } while(!sair);
}

void wakeup(thread::id t) {
    muxBW.lock();
    A[t]++;
    muxBW.unlock();
}

void produzir(int dado) {
    muxPC.lock();

    while(cont == bSize) {
        produtoresEsperando.insert(this_thread::get_id());
        muxPC.unlock();
        block();
        muxPC.lock();
        produtoresEsperando.erase(this_thread::get_id());
    }

    buffer[pin] = dado;
    pin = (pin + 1) % bSize;
    ++cont;
    
    if(!consumidoresEsperando.empty()) {
        thread::id t = *consumidoresEsperando.begin();
        wakeup(t);
    }

    muxPC.unlock();
}

void consumir() {
    muxPC.lock();

    while(cont == 0) {
        consumidoresEsperando.insert(this_thread::get_id());
        muxPC.unlock();
        block();
        muxPC.lock();
        consumidoresEsperando.erase(this_thread::get_id());
    }

    pout = (pout + 1) % bSize;
    --cont;
    
    if(!produtoresEsperando.empty()) {
        thread::id t = *produtoresEsperando.begin();
        wakeup(t);
    }

    muxPC.unlock();
}

void produzirT() {
    for(int i = 0; i < 1000000; i++) {
        produzir(i);
    }
}

void consumirT() {
    for(int i = 0; i < 1000000; i++) {
        consumir();
    }
}

int main() {
    bSize = 1000;
    buffer = new int[bSize];
    pin = pout = cont = 0;

    thread t1(produzirT);
    thread t2(consumirT);

    t1.join();
    t2.join();

    delete [] buffer;

    return 0;
}
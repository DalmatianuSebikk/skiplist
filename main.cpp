#include <iostream>
#include <cstdlib>
#include <cmath>
#include <cstring>
#include <ctime>

const int MAX_LEVEL = 17;
//SKIP LIST
// 1. Ce este SkipList?
// SkipList este o structura de date complexa introdusa pentru a eficientiza cautarea elementelor intr-o lista inlantuita.
// Intr-o lista inlantuita sortata daca dorim sa cautam un element vom avea complexitatea O(n), pentru ca noi nu putem sari
// la elementul respectiv direct, trebuie sa trecem prin restul elementelor pana la cel cautat (lucru datorat faptului ca
// la Linked list elementele sunt pozitionate random in memorie, spre deosebire de array, structura ce "pune elementele unele langa
// altele" in memorie). Daca vorbim de complexitatea cautarii unui element intr-un arbore binar de cautare, parca suna mai bine,
// avem un O(log2N) din cauza injumatatirilor. La fel si la un array sortat, vom avea O(log2N), pentru ca putem folosi cautarea
// binara.

// 2. Ce putem face la o lista inlantuita ca sa fie SkipList.
// Consideram o lista simplu inlantuita: 0 -> 1 -> 2 -> 3 -> ..... -> n (sortata). Noi vrem sa cautam un element anume din aceasta
// lista, fie el n - 5 (presupunem n = 20, 21 .. alte numere mari). Pai nu pot face altceva decat sa "iterez" prin lista pana cand
// gasesc ceea ce vreau eu sa gasesc.
// Putem face o smecherie de felul urmator. Facem "ceva" astfel incat niste noduri sa mai aiba o referinta in plus catre un nod mai
// indepartat. Si uite asa putem face o "lista inlantuita" de felul urmator:

// 0 ------------>3------------> 6 ------------>9 ------------>...
// 0 -> 1 -> 2 -> 3 -> 4 -> 5 -> 6 -> 7 -> 8 -> 9 -> 10 -> ..... -> n

// Adica: 0 pe langa referinta catre 1 va mai avea si o referinta catre 3. De asemenea, 3 va avea si o referinta catre 6, [...].
// Daca vrem sa cautam de exemplu numarul 13, in loc de 13 salturi pana ajungem la ce vrem noi sa cautam, o sa avem urmatoarea "ruta":
// 0 -> 3 -> 6 -> 9 -> 12 -> 13. Adica o sa avem 6 pasi de facut, fata de 13.
// Complexitatea noua va fi O(ne + ns), unde ne = numarul de noduri din expresie, ns = numarul de noduri care mai trebuie parcurse
// in cazul in care elementul cautat nu se afla intre doua noduri din expresie.

// Complexitatea scade cu cat faci mai multe niveluri.

class Nod {
private:
    int valoare;

    // Facem un vector de pointeri pentru a face legaturile pe un nivel diferit;
    Nod **next;
public:

    //-------------GETTERE-----------
    [[nodiscard]] int getValoare() const {
        return valoare;
    }

    [[nodiscard]] Nod **getNext() const {
        return next;
    }

    //-------------SETTERE-----------
    void setValoare(int valoare) {
        Nod::valoare = valoare;
    }

    void setNext(Nod **next) {
        Nod::next = next;
    }

    //-------------CONSTRUCTOR-----------
    Nod(int nivel, int &valoare) {
        // Aloc memorie ca sa inaintez.
        next = new Nod *[nivel + 1];

        // Apoi umplu nivelul de mai sus cu 0;
        memset(next, 0, sizeof(Nod*) * (nivel + 1));

        //pun valoarea
        this -> valoare = valoare;
    }

    //-------------DESTRUCTOR-----------
    virtual ~Nod() {
        delete[] next; // Stergem vectorul de pointeri.
    }
};

class SkipList {
private:
    Nod* head;
    int valoare;
    int nivel;


public:
    //-------------CONSTRUCTOR-----------
    SkipList() {
        head = new Nod(MAX_LEVEL, valoare);
        nivel = 0;
    }

    //-------------DESTRUCTOR-----------
    virtual ~SkipList() {
        delete head;
    }
    //-------------FUNCTII CARE AJUTA LA CONSTRUIT -----------

    // 1. FUNCTIA DE RANDOM (alegem nivelul random la care sa se puna elementul)
    float floatRandom() {
        return (float) rand() / RAND_MAX;
    }
    int randomLevel() {
//        float random = (float) rand() / RAND_MAX;
//        int level = 0; // primul nivel
//        while(random < p && level < maxLevel) {
//            level ++;
//            random = (float)rand() / RAND_MAX;
//        }
//        return level;
        static bool primul = true;
        if(primul){
            srand((unsigned)time(NULL));
            primul = false;
        }
        int level = (int)(log(floatRandom()) / (log(1.-0.5)));
        return (level < MAX_LEVEL) ? level : MAX_LEVEL;
    }

    Nod* creeazaNod(int valoare, int nivel) {
        Nod *n = new Nod(valoare, nivel);
        return n;
    }

    // 2. INSERARE ELEMENT IN SKIPLIST
    void inserareElement(int value)
    {
        Nod *nodCurent = head; // avem nevoie doar de head
        // mai facem un vector de update-uri
        Nod *update[MAX_LEVEL + 1];
        // si il initializam cu zero folosim memset
        memset(update, 0, sizeof(Nod*) * (MAX_LEVEL + 1));

        // pornesti de la cel mai mare nivel din skiplist. Cat timp valoarea este mai mare decat cea a nodului de langa cel
        // curent, muti pointerul inainte. Altfel, inseram nodul curent in vectorul update si ne mutam un nivel mai jos si
        // continuam sa cautam.

        for (int i = nivel;i >= 0;i--)
        {
            while (nodCurent->getNext()[i] != NULL && nodCurent->getNext()[i]->getValoare() < value)
            {
                nodCurent = nodCurent->getNext()[i];
            }
            update[i] = nodCurent;
        }
        nodCurent = nodCurent->getNext()[0];

        // in cazul in care nodulCurent este null, atunci am ajuns la finalul nivelului SAU trebuie sa inseram nodul
        // intre update[0] si nodulCurent

        if (nodCurent == NULL || nodCurent->getValoare() != value)
        {
            // generam un nivel random pe care sa punem;
            int nivelRandomGenerat = randomLevel();

            // daca nivelul random este mai mare decat nivelul SkipListului (va fi nodul cu cel mai mare nivel inserat in lista)
            // initializam valoarea vectorului de update cu pointerul catre head

            if (nivelRandomGenerat > nivel)
            {
                for (int i = nivel + 1;i <= nivelRandomGenerat;i++)
                {
                    update[i] = head;
                }
                nivel = nivelRandomGenerat;
            }
            nodCurent = new Nod(nivelRandomGenerat, value);
            for (int i = 0;i <= nivelRandomGenerat;i++)
            {
                nodCurent->getNext()[i] = update[i]->getNext()[i];
                update[i]->getNext()[i] = nodCurent;
            }
        }
    }

    bool cautare(int valoare){
        Nod* nodCurent = head;
        // merg pe fiecare nivel din SkipList.
        for(int i = nivel; i >= 0; i--){
            // fac asta pana cand nod->next->valoare mai mic decat valoare, dupa ma duc pe urmatorul nivel de mai jos.
            while(nodCurent -> getNext()[i] != nullptr && nodCurent -> getNext()[i]->getValoare() < valoare){
                nodCurent = nodCurent -> getNext()[i];
            }
        }
        nodCurent = nodCurent->getNext()[0];
        return (nodCurent != nullptr && nodCurent->getValoare() == valoare);
    }
//    void inserareElement(int valoare) {
//        Nod *nodCurent = head; // avem nevoie doar de head
//
//        // mai facem un vector de update-uri
//        Nod *update[MAX_LEVEL + 1];
//        memset(update, 0, sizeof(Nod*) * (MAX_LEVEL + 1));
//        // si il initializam cu zero folosim memset
////        memset(update, 0, sizeof(Nod*)*(maxLevel + 1)); nu mai folosim memset
//
//        // pornesti de la cel mai mare nivel din skiplist. Cat timp valoarea este mai mare decat cea a nodului de langa cel
//        // curent, muti pointerul inainte. Altfel, inseram nodul curent in vectorul update si ne mutam un nivel mai jos si
//        // continuam sa cautam.
//
////        int index = nivel;
////        while(index >= 0){
////            while(nodCurent->getNext()[index] != nullptr && nodCurent->getNext()[index] ->getValoare() < valoare){
////                nodCurent = nodCurent->getNext()[index];
////            }
////            update[index] = nodCurent;
////            index--;
////        }
//        for(int i = nivel; i >= 0; i--){
//            while(nodCurent-> getNext()[i] != nullptr && nodCurent->getValoare() < valoare){
//                nodCurent = nodCurent -> getNext()[i];
//            }
//            update[i] = nodCurent;
//        }
//        nodCurent = nodCurent -> getNext()[0];
//
//        // in cazul in care nodulCurent este null, atunci am ajuns la finalul nivelului SAU trebuie sa inseram nodul
//        // intre update[0] si nodulCurent
//        if(nodCurent == nullptr || nodCurent->getValoare() != valoare) {
//            // generam un nivel random pe care sa punem;
//            int nivelRandomGenerat = randomLevel();
//
//            // daca nivelul random este mai mare decat nivelul SkipListului (va fi nodul cu cel mai mare nivel inserat in lista)
//            // initializam valoarea vectorului de update cu pointerul catre head
//            if(nivelRandomGenerat > nivel) {
//                for (int i = nivel + 1; i < nivelRandomGenerat; ++i) {
//                    update[i] = head;
//                }
//                nivel = nivelRandomGenerat;
//
//            }
//            nodCurent = new Nod(nivelRandomGenerat, valoare);
//
//            for( int i = 0; i <= nivelRandomGenerat; i++) {
//                nodCurent -> getNext()[i] = update[i]->getNext()[i];
//                update[i]->getNext()[i] = nodCurent;
//            }
//
//            std::cout << "Nod cu valoarea " << valoare << " inserat cu succes.\n";
//        }
//
//    }

    // STERGEREA UNUI ELEMENT DIN SKIPLIST
    void stergereElement(int valoare){
        Nod *nodCurent = head;
        Nod *update[MAX_LEVEL + 1];

        memset(update, 0, sizeof(Nod*) * (MAX_LEVEL + 1));

        for(int i = nivel; i >= 0; i--){
            // parcurg nivelurile
            while(nodCurent->getNext()[i] != nullptr && nodCurent->getNext()[i]->getValoare() < valoare){
                nodCurent = nodCurent->getNext()[i];
            }
            update[i] = nodCurent;
        }

        nodCurent = nodCurent -> getNext()[0];

        if(nodCurent->getValoare() == valoare){
            // atunci l-am gasit si il stergem;
            for(int i = 0; i < nivel; i++){
                if(update[i]->getNext()[i] != nodCurent){
                    // daca nu avem noduri cu legatura spre nodul pe care vrem sa-l stergem, le lasam asa, mergem mai departe
                    break;
                }
                update[i]->getNext()[i] = nodCurent->getNext()[i]; // altfel, stergem legaturile
            }
            delete nodCurent;
            while(nivel > 0 && head->getNext()[nivel] == nullptr){
                nivel--; // adica daca nu mai avem elemente pe respectivul nivel putem sa-l consideram inexistent.
            }
        }
    }

    void succesorElement(int valoare){
        Nod *nodCurent = head;
        Nod *update[MAX_LEVEL + 1];
        memset(update, 0, sizeof(Nod*) * (MAX_LEVEL + 1));

        for(int i = nivel; i >= 0; i--){
            // parcurg nivelurile
            while(nodCurent->getNext()[i] != nullptr && nodCurent->getNext()[i]->getValoare() < valoare){
                nodCurent = nodCurent->getNext()[i];
            }
            update[i] = nodCurent;
        }

        nodCurent = nodCurent -> getNext()[0];

        if(nodCurent->getValoare() == valoare) {
            std::cout << "Succesorul lui " << nodCurent->getValoare() << " este: " << nodCurent->getNext()[0]->getValoare();
        }
    }

    void predecesorElement(int valoare){
        Nod *nodCurent = head;
        Nod *update[MAX_LEVEL + 1];
        memset(update, 0, sizeof(Nod*) * (MAX_LEVEL + 1));

        for(int i = nivel; i >= 0; i--){
            while(nodCurent->getNext()[i]->getNext()[i] != nullptr && nodCurent->getNext()[i]->getNext()[i]->getValoare() < valoare){
                nodCurent = nodCurent->getNext()[i];
            }
            update[i] = nodCurent;
        }

        nodCurent = nodCurent -> getNext()[0];

        if(nodCurent->getNext()[0]->getValoare() == valoare){
            std::cout << "Predecesorul este: " << nodCurent->getValoare() << '\n';
        }
    }

    void afisareSkipList(){
        for(int i = 0; i < nivel; i++){
            Nod* nod = head -> getNext()[i];
            std::cout << "Nivelul: " << i << ": ";
            while(nod != nullptr){
                // traversez nivelul
                std::cout << nod->getValoare()<< " ";
                nod = nod->getNext()[i];
            }
            std::cout << '\n';
        }
    }
};

// din cauza ca fac cu clase s-ar putea sa crape, ia sa vad daca fac cu struct (spoiler alert: a fost ok, sunt eu prostea)
//struct Nod {
//    int valoare;
//    Nod **next;
//    Nod(int nivel, int &valoare) {
//        // Aloc memorie ca sa inaintez.
//        next = new Nod *[nivel + 1];
//
//        // Apoi umplu nivelul de mai sus cu 0;
//        memset(next, 0, sizeof(Nod*) * (nivel + 1));
//
//        //pun valoarea
//        this -> valoare = valoare;
//    }
//};
//
//struct SkipList {
//    Nod* head;
//    int valoare;
//    int nivel;
//
//    //------------CONSTRUCTOR----------
//    SkipList() {
//        head = new Nod(MAX_LEVEL, valoare);
//        nivel = 0;
//    }
//
//    //-------------DESTRUCTOR-----------
//    virtual ~SkipList() {
//        delete head;
//    }
//
////    1. FUNCTIA DE RANDOM (alegem nivelul random la care sa se puna elementul)
//    float floatRandom() {
//        return (float) rand() / RAND_MAX;
//    }
//    int randomLevel() {
////        float random = (float) rand() / RAND_MAX;
////        int level = 0; // primul nivel
////        while(random < p && level < maxLevel) {
////            level ++;
////            random = (float)rand() / RAND_MAX;
////        }
////        return level;
//        static bool primul = true;
//        if(primul){
//            srand((unsigned)time(NULL));
//            primul = false;
//        }
//        int level = (int)(log(floatRandom()) / (log(1.-0.5)));
//        return (level < MAX_LEVEL) ? level : MAX_LEVEL;
//    }
//
//    // 2. INSERARE ELEMENT IN SKIPLIST
//    void inserareElement(int value)
//    {
//        Nod *nodCurent = head;
//        Nod *update[MAX_LEVEL + 1];
//        memset(update, 0, sizeof(Nod*) * (MAX_LEVEL + 1));
//        for (int i = nivel;i >= 0;i--)
//        {
//            while (nodCurent->next[i] != NULL && nodCurent->next[i]->valoare < value)
//            {
//                nodCurent = nodCurent->next[i];
//            }
//            update[i] = nodCurent;
//        }
//        nodCurent = nodCurent->next[0];
//        if (nodCurent == NULL || nodCurent->valoare != value)
//        {
//            int nivelRandomGenerat = randomLevel();
//            if (nivelRandomGenerat > nivel)
//            {
//                for (int i = nivel + 1;i <= nivelRandomGenerat;i++)
//                {
//                    update[i] = head;
//                }
//                nivel = nivelRandomGenerat;
//            }
//            nodCurent = new Nod(nivelRandomGenerat, value);
//            for (int i = 0;i <= nivelRandomGenerat;i++)
//            {
//                nodCurent->next[i] = update[i]->next[i];
//                update[i]->next[i] = nodCurent;
//            }
//        }
//    }
//
//    void afisareSkipList(){
//        for(int i = 0; i < nivel; i++){
//            Nod* nod = head -> next[i];
//            std::cout << "Nivelul: " << i << ":";
//            while(nod != nullptr){
//                // traversez nivelul
//                std::cout << nod->valoare<< " ";
//                nod = nod->next[i];
//            }
//        }
//    }
//
//};




int main() {
    srand(42);
    SkipList lista;
    lista.inserareElement(1);
    lista.inserareElement(2);
    lista.inserareElement(3);
    lista.inserareElement(4);
    lista.inserareElement(5);
    lista.inserareElement(6);
    lista.inserareElement(9);
    lista.afisareSkipList();
    lista.stergereElement(4);
    lista.stergereElement(3);
    lista.succesorElement(2);
    lista.predecesorElement(3);
//    std::cout << lista.cautare(3) << '\n';
    lista.afisareSkipList();
    return 0;
}

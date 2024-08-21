#include <iostream>
#include <fstream>
#include <vector>
#include <cmath>
#include <iomanip>
#include <queue>
#include <algorithm>

constexpr int MAX_Y = 12;
constexpr int MAX_X = 28;

//Número que representa la x cortando el paso
#define INFI 999 

// Para imprimir color por consola
#define RED "\033[31m"
#define BLUE "\033[34m"
#define RESET "\033[0m"

using namespace std;

// estas coordenas y lineas sirven para identificar las barreras y las malezas centrales
struct Coor { int x, y; };
struct Linea { Coor a, b; };

class planificador {
    //variables
    int matriz[MAX_Y][MAX_X];
    vector<vector<double>> matrizM;
    vector<Linea> lineas;
    vector<Coor> coor;
    vector<Coor> malezas;

    int contadorM = 0, n3 = 0;

    //métodos
private:
    bool procesarArchivo(string &texto);
    void maleza3x3();
    double distanciaM(int x1, int y1, int x2, int y2);
    bool bloqueado(int ax1, int ay1, int ax2, int ay2);
    void bfsCicloHamiltoniano();
    void guardarLineas();
    void showMatriz();
    void showMatrizM();
    void showBarreras();
    void showCoorMalezas();

public:
    planificador() {}
    void procesarMapa(string &txt);

};

//Método para mostrar la matriz inicial
void planificador::showMatriz() {
    for (int i = 0; i < MAX_Y; i++) {
        for (int j = 0; j < MAX_X; j++) {
            if (matriz[i][j] == INFI) {
                 cout << BLUE << "x" << RESET << " "; //solo para que sea vea visual las x
            }
            else if (matriz[i][j] == 1) {
                cout << RED << matriz[i][j] << RESET << " "; // Imprimir '1' en rojo
            }
            else {
                cout << matriz[i][j] << " ";
            }
        }
        cout << endl;
    }
    cout << endl;
}

//Método para mostrar la matriz de distancias
void planificador::showMatrizM() {
    for (int i = 0; i < n3; i++) {
        for (int j = 0; j < n3; j++) {
            cout << "[" << setw(4) << matrizM[i][j] << "]" << " ";
        }
        cout << endl;
    }
    cout << endl;
}

//Método para mostrar todas las líneas formadas por X
void planificador::showBarreras() {
    for (int i = 0; i < lineas.size(); i++) {
        cout << "Linea " << i + 1 << ":  X1= " << lineas[i].a.x << " Y1= " << lineas[i].a.y << " |  X2= " << lineas[i].b.x << " Y2= " << lineas[i].b.y << endl;
    }
}

//Método para guardar todas las líneas formadas por X
void planificador::guardarLineas() {
    Linea l;

    for (int i = 0; i < coor.size(); i++) {
        l.a = coor[i];
        Coor aux = coor[i];
        for (int j = i + 1; j < coor.size(); j++) {
            if (coor[j].y == aux.y && coor[j].x == aux.x + 1) {
                aux = coor[j];
                coor.erase(coor.begin() + j);
                j--;
            }
            else if (coor[j].y == aux.y + 1 && coor[j].x == aux.x) {
                aux = coor[j];
                coor.erase(coor.begin() + j);
                j--;
            }
            else if (coor[j].y == aux.y + 1 && coor[j].x == aux.x + 1) {
                aux = coor[j];
                coor.erase(coor.begin() + j);
                j--;
            }
            else if (coor[j].y == aux.y + 1 && coor[j].x == aux.x - 1) {
                aux = coor[j];
                coor.erase(coor.begin() + j);
                j--;
            }
        }
        l.b = aux;
        lineas.push_back(l);
    }
}

//Método para leer el archivo y guardar la matriz original
bool planificador::procesarArchivo(string &texto) {
    char c;
    Coor cr;
    ifstream archivo(texto);

    if (archivo.is_open()) {
        cout << "- Se pudo abrir el archivo." << endl << endl;

        for (int i = 0; i < MAX_Y; i++) {
            for (int j = 0; j < MAX_X; j++) {
                archivo.get(c);
                if (c == '\n') {
                    archivo.get(c);
                }
                if (c == 'x') {
                    matriz[i][j] = INFI;
                    cr.x = j;
                    cr.y = i;
                    coor.push_back(cr);
                }
                else if (c == '1') {
                    contadorM++;
                    matriz[i][j] = c - 48;
                }
                else { matriz[i][j] = c - 48; }
            }
        }
        return true;
    }
    else {
        cout << "- No se pudo abrir el archivo" << endl;
        return false;
    }
}

//Método para calcular la distancia entre dos malezas
double planificador::distanciaM(int x1, int y1, int x2, int y2) {
    if (!bloqueado(x1, y1, x2, y2)) {
        double distancia = sqrt(pow(abs(x1 - x2), 2) + pow(abs(y1 - y2), 2));
        distancia = round(distancia * 100) / 100;
        return distancia;
    }
    else { return INFI; }
}

//Método para verificar si entre dos puntos hay una X
bool planificador::bloqueado(int ax1, int ay1, int ax2, int ay2) {
    auto onSegment = [](Coor p, Coor q, Coor r) -> bool {
        return q.x <= max(p.x, r.x) && q.x >= min(p.x, r.x) &&
               q.y <= max(p.y, r.y) && q.y >= min(p.y, r.y);
    };

    auto orientation = [](Coor p, Coor q, Coor r) -> int {
        int val = (q.y - p.y) * (r.x - q.x) - (q.x - p.x) * (r.y - q.y);
        if (val == 0) return 0; // colinear
        return (val > 0) ? 1 : 2; // clock or counterclock wise
    };

    auto doIntersect = [&](Coor p1, Coor q1, Coor p2, Coor q2) -> bool {
        int o1 = orientation(p1, q1, p2);
        int o2 = orientation(p1, q1, q2);
        int o3 = orientation(p2, q2, p1);
        int o4 = orientation(p2, q2, q1);

        if (o1 != o2 && o3 != o4)
            return true;

        if (o1 == 0 && onSegment(p1, p2, q1)) return true;
        if (o2 == 0 && onSegment(p1, q2, q1)) return true;
        if (o3 == 0 && onSegment(p2, p1, q2)) return true;
        if (o4 == 0 && onSegment(p2, q1, q2)) return true;

        return false;
    };

    Coor p1 = {ax1, ay1}, q1 = {ax2, ay2};

    for (const auto& l : lineas) {
        Coor p2 = l.a, q2 = l.b;
        if (doIntersect(p1, q1, p2, q2)) {
            return true;
        }
    }
    return false;
}

//Método que busca las malezas 3x3, encuentra la posición central (nodos) y crea una matriz de distancias entre todos los nodos teniendo en cuenta las X.
void planificador::maleza3x3() {
    malezas.push_back({ 0,0 });
    for (int i = 1; i < MAX_Y - 1; i++) {
        for (int j = 1; j < MAX_X - 1; j++) {
            if (matriz[i][j] == 1) {
                bool rodeado = true;
                for (int x = -1; x <= 1; x++) {
                    for (int y = -1; y <= 1; y++) {
                        if (matriz[i + x][j + y] != 1) {
                            rodeado = false;
                            break;
                        }
                    }
                    if (!rodeado) break;
                }
                if (rodeado) {
                    malezas.push_back({ j, i });
                }
            }
        }
    }

    n3 = malezas.size();
    matrizM.resize(n3, vector<double>(n3, 0));

    for (int i = 0; i < n3; i++) {
        for (int j = i + 1; j < n3; j++) {
            double dist = distanciaM(malezas[i].x, malezas[i].y, malezas[j].x, malezas[j].y);
            matrizM[i][j] = matrizM[j][i] = dist; //ida y vuelta tienen la misma distancia
        }
    }

}

//Método para mostrar los centros de malezas 3x3 por donde tiene que pasar el drone
void planificador::showCoorMalezas() {
    int i = 0;
    for (const Coor& c : malezas) {
        cout << "Vertice " << i << ": (" << c.x << ", " << c.y << ")" << endl;
        i++;
    }
}

// Algoritmo de BFS para encontrar el ciclo Hamiltoniano de costo mínimo
void planificador::bfsCicloHamiltoniano() {
    vector<int> path; // Vector para almacenar el camino actual que se está explorando.
    queue<vector<int>> q; // Cola para almacenar los caminos a explorar.
    q.push({ 0 }); // Se empieza desde el primer nodo (nodo 0).

    double minCost = INFI; // Inicializa el costo mínimo encontrado con un valor grande (INFI).
    vector<int> minPath; // Vector para almacenar el camino de costo mínimo.

    while (!q.empty()) { // Bucle que se ejecuta mientras haya caminos por explorar en la cola!
        vector<int> rutaAcceso = q.front(); // Obtiene el primer camino de la cola.
        q.pop(); // Elimina el camino actual de la cola.

        if (rutaAcceso.size() == n3) { // Si se ha visitado todos los nodos (se ha formado un ciclo completo)!
            double cost = 0;
            for (int i = 0; i < n3 - 1; i++) {
                cost += matrizM[rutaAcceso[i]][rutaAcceso[i + 1]]; // Calcula el costo del camino actual.
            }
            cost += matrizM[rutaAcceso.back()][0]; // Agrega el costo de regresar al nodo inicial (cierra el ciclo).

            if (cost < minCost) { // Si el costo del camino actual es menor que el costo mínimo encontrado hasta ahora.
                minCost = cost; // Actualiza el costo mínimo.
                minPath = rutaAcceso; // Actualiza el camino de costo mínimo.
            }
            continue; // Continua con la siguiente iteración del bucle.
        }

        for (int i = 0; i < n3; i++) { // Si aún no se han visitado todos los nodos, se recorren todos los nodos para generar nuevos caminos!
            if (find(rutaAcceso.begin(), rutaAcceso.end(), i) == rutaAcceso.end()) { // Si el nodo i no está en el camino actual.
                rutaAcceso.push_back(i); // Agrega el nodo i al camino actual.
                q.push(rutaAcceso); // Agrega el nuevo camino a la cola para ser explorado.
                rutaAcceso.pop_back(); // Elimina el nodo i del camino actual para seguir explorando otros caminos.
            }
        }
    }

    if (minCost == INFI) { // Si no se encontró un ciclo Hamiltoniano.
        cout << "No se encontró un ciclo Hamiltoniano posible." << endl;
    }
    else { // Si se encontró un ciclo Hamiltoniano de costo mínimo.
        cout << "El ciclo Hamiltoniano de costo mínimo es: " << endl;
        for (int i : minPath) { // Imprime el camino de costo mínimo.
            cout << i << " ";
        }
        cout << "0" << endl;
        cout << "Con un costo de: " << minCost << endl; // Imprime el costo mínimo del ciclo Hamiltoniano.
        
    }
}

//Método principal para procesar el mapa
void planificador::procesarMapa(string &txt) {
    if (procesarArchivo(txt)) {
        
        guardarLineas(); //guardar barreras
        maleza3x3(); //guardar vertices y caminos (tomando en cuenta las barreras)
        
        cout << "- Se procesó el archivo." << endl << endl;
        cout << "Esta es la matriz original: " << endl << endl;
        showMatriz();
        cout << "Hay " << contadorM << " malezas en total" << endl << endl;
        cout << "Hay " << n3 - 1  << " malezas 3x3" << endl << endl;
        cout << "--------------------------------------" << endl << endl;
        cout << "Barreras (lineas con x):" << endl << endl;
        showBarreras();
        cout << endl << "--------------------------------------" << endl << endl;
        cout << "Esta es la matriz de distancias: " << endl << endl;
        showMatrizM();
        cout << "--------------------------------------" << endl << endl;
        cout << "El drone tiene que pasar por:" << endl << endl;
        showCoorMalezas();
        cout << endl << "--------------------------------------" << endl << endl;
        bfsCicloHamiltoniano(); // Calcula el ciclo Hamiltoniano
    }
}

int main() {
    string nombreArchivo = "mapa.txt";
    planificador mapa;
    mapa.procesarMapa(nombreArchivo);
}
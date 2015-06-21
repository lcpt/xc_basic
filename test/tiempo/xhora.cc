//xHora.cpp
//Prueba del objeto Hora.

using namespace std;
#include "../src/hora.h"
#include <iostream>

int main(void)
  {
    unsigned short int horas;
    unsigned short int min;
    double ss;
    cout << "Horas: ";
    cin >> horas;
    cout << "Minutos: ";
    cin >> min;
    cout << "Segundos: ";
    cin >> ss;
    Hora h(horas,min,ss);
    cout << h << endl;
    cout << h+7 << endl;
    cout << h-7 << endl;
    cout << h+100 << endl;
    cout << Hora(10,20,30)+Hora(24,20,30) << endl;
    cout << Hora(10,20,30)-Hora(10,20,30) << endl;
    h.Time();
    cout << "Hora GMT: " << h << endl;
    h.LocalTime();
    cout << "Hora local: " << h << endl;
    h.TimeOfDay();
    cout << "Hora GMT: " << h << endl;
    h.LocalTimeOfDay();
    cout << "Hora local: " << h << endl;
    return 0;
  }

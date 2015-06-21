//xMuestra.cc

#include "matemat/funciones/src/estadisticas/Muestra.h"
#include "matemat/funciones/src/estadisticas/Gumbel.h"
#include <iostream>


int main(void)
  {
    Muestra m;
    m.push_back(69.0);
    m.push_back(104.0);
    m.push_back(128.0);
    m.push_back(11.0);
    m.push_back(117.0);
    m.push_back(113.0);
    m.push_back(70.0);
    m.push_back(122.0);
    m.push_back(93.0);
    m.push_back(64.0);
    m.push_back(69.0);
    m.push_back(52.0);
    m.push_back(105.0);
    m.push_back(85.0);

//     m.push_back(8.7);
//     m.push_back(10.1);
//     m.push_back(9.2);
//     m.push_back(17.1);
//     m.push_back(15.8);
//     m.push_back(9.1);
//     m.push_back(31.0);
//     m.push_back(10.0);
//     m.push_back(10.5);
//     m.push_back(15.8);
//     m.push_back(9.4);
//     m.push_back(8.3);
//     m.push_back(9.2);
//     m.push_back(13.0);
//     m.push_back(23.6);
//     m.push_back(7.7);
//     m.push_back(7.2);
//     m.push_back(13.0);
//     m.push_back(21.7);
//     m.push_back(12.2);
//     m.push_back(17.9);
//     m.push_back(17.0);
//     m.push_back(11.2);
//     m.push_back(12.4);
//     m.push_back(19.4);
//     m.push_back(15.2);
//     m.push_back(7.0);
//     m.push_back(6.5);
//     m.push_back(20.4);
//     m.push_back(9.5);

    std::cout << "tamaño de la muestra: " << m.size() << std::endl;
    double x_= m.Media();
    std::cout << "media: " << x_ << std::endl;
    double sigma= m.DesviacionTipica();
    std::cout << "desviación típica: " << sigma << std::endl;

    double y_= 0.51; //
    double sigma_asterisco= 1.0094;

    double alpha= sigma_asterisco/sigma;
    double u= x_ - y_/alpha;
    std::cout << "Parámetros distribución de Gumbel: " << std::endl;
    std::cout << "  alfa: " << alpha << std::endl;
    std::cout << "     u: " << u << std::endl;

    Gumbel g(alpha,u);
    
    double T= 500;
    std::cout << "Periodo de retorno: " << T << std::endl;
    double i_max_T= g.Inv(1.0-1.0/T);
    std::cout << "Intensidad media máxima para T= " << T << " años: " << i_max_T << std::endl;

    T= 1000;
    std::cout << "Periodo de retorno: " << T << std::endl;
    i_max_T= g.Inv(1.0-1.0/T);
    std::cout << "Intensidad media máxima para T= " << T << " años: " << i_max_T << std::endl;
    
    return 0;
  }

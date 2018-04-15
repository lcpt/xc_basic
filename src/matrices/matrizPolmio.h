//----------------------------------------------------------------------------
//  biblioteca xc_basic; funciones y clases de uso general
//
//  Copyright (C)  Luis Claudio Pérez Tato
//
//  Este software es libre: usted puede redistribuirlo y/o modificarlo 
//  bajo los términos de la Licencia Pública General GNU publicada 
//  por la Fundación para el Software Libre, ya sea la versión 3 
//  de la Licencia, o (a su elección) cualquier versión posterior.
//
//  Este software se distribuye con la esperanza de que sea útil, pero 
//  SIN GARANTÍA ALGUNA; ni siquiera la garantía implícita 
//  MERCANTIL o de APTITUD PARA UN PROPÓSITO DETERMINADO. 
//  Consulte los detalles de la Licencia Pública General GNU para obtener 
//  una información más detallada. 
//
// Debería haber recibido una copia de la Licencia Pública General GNU 
// junto a este programa. 
// En caso contrario, consulte <http://www.gnu.org/licenses/>.
//----------------------------------------------------------------------------
//matrizPolmio.h
//Matriz de polinomios

#ifndef MATRIZPOLMIO_H
#define MATRIZPOLMIO_H

#include "matrizZ.h"
#include "xc_basic/src/funciones/polinomios/Polinomio.h"
#include "m_double.h"

typedef matrizZ<Polinomio> polmio_matriz;

class matrizPolmio: public polmio_matriz
  {
    matrizPolmio(const m_double &m);

    void eval(short unsigned int j,const double &val);
    void eval(short unsigned int j,const Polinomio &val);
    void eval(const vZ_double &v);
    void eval(const mZ_double &v);
  public:
    matrizPolmio(size_type n_rows=1,size_type cols=1,const Polinomio &val=Polinomio())
      : polmio_matriz(n_rows,cols,val) {}
    matrizPolmio(const matrizPolmio &otra) : polmio_matriz(otra) {}
    matrizPolmio &operator=(const matrizPolmio &m)
      {
	polmio_matriz::operator=(m);
        return *this;
      }
    matrizPolmio &Trn(void);
    matrizPolmio GetTrn(void) const;

    matrizPolmio NeutroSuma(void) const
      { return matrizPolmio(getNumberOfRows(),getNumberOfColumns(),Polinomio::neutro_suma()); }
    matrizPolmio NeutroProducto(void) const
      { return matrizPolmio(getNumberOfRows(),getNumberOfColumns(),Polinomio::neutro_producto()); }

    matrizPolmio Eval(short unsigned int j,const double &val) const;
    matrizPolmio Eval(short unsigned int j,const Polinomio &val) const;
    matrizPolmio Eval(const vZ_double &v) const;
    matrizPolmio Eval(const mZ_double &v) const;

    matrizPolmio &operator*=(const double &d);

    matrizPolmio &operator+=(const matrizPolmio &mp)
      {
	polmio_matriz::operator+=(mp);
        return *this;
      }
    matrizPolmio &operator-=(const matrizPolmio &mp)
      {
	polmio_matriz::operator-=(mp);
        return *this;
      }
    matrizPolmio &operator+=(const m_double &m);
    matrizPolmio &operator-=(const m_double &m);

    matrizPolmio &Primitiva(short unsigned int k);
    matrizPolmio &Diferencial(short unsigned int k);

    friend matrizPolmio operator*(const matrizPolmio &m1,const matrizPolmio &m2);
    friend matrizPolmio operator*(const m_double &m,const matrizPolmio &mE)
      {
        matrizPolmio retval= mE*matrizPolmio(m);
        return retval;
      }
    friend matrizPolmio operator*(const matrizPolmio &mE, const m_double &m)
      { return m*mE; }
    friend matrizPolmio operator-(const m_double &m1,const matrizPolmio &m2);
  };

inline matrizPolmio neutro_suma(const matrizPolmio &mp)
  { return mp.NeutroSuma(); }
inline matrizPolmio neutro_producto(const matrizPolmio &mp)
  { return mp.NeutroProducto(); }


inline matrizPolmio Eval(const matrizPolmio &m,short unsigned int k,const double &d)
//Evalua los polinomios de la matriz con el polinomio p.
  { return m.Eval(k,d); }

inline matrizPolmio Eval(const matrizPolmio &m,short unsigned int k,const Polinomio &p)
//Evalua los polinomios de la matriz con el polinomio p.
  { return m.Eval(k,p); }

Polinomio Eval(const Polinomio &p,const matrizPolmio &vp);
m_double Eval(const matrizPolmio &p,const m_double &x);
matrizPolmio Eval(const matrizPolmio &m1,const matrizPolmio &m2);

inline void CambioVar(matrizPolmio &m,short unsigned int j,const Polinomio &cambio)
  { m= Eval(m,j,cambio); }
inline void CambioVar(matrizPolmio &m,const matrizPolmio &cambio)
  { m= Eval(m,cambio); }

matrizPolmio operator+(const matrizPolmio &m1,const matrizPolmio &m2);
matrizPolmio operator-(const matrizPolmio &m1,const matrizPolmio &m2);
matrizPolmio operator+(const matrizPolmio &m1,const m_double &m2);
inline matrizPolmio operator +(const m_double &m1,const matrizPolmio &m2)
  { return m2+m1; }
matrizPolmio operator-(const matrizPolmio &m1,const m_double &m2);

Polinomio dot(const matrizPolmio &v1,const m_double &v2);
Polinomio dot(const m_double &v1,const matrizPolmio &v2);

matrizPolmio operator *(const Polinomio &p,const m_double &m);
inline matrizPolmio operator*(const m_double &m,const Polinomio &p)
  { return p*m; }
matrizPolmio operator*(const matrizPolmio &m,const double &d);
inline matrizPolmio operator *(const double &d,const matrizPolmio &m)
  { return m*d; }
inline matrizPolmio operator /(const matrizPolmio &m,const double &d)
  { return m*(1/d); }

matrizPolmio Primitiva(const matrizPolmio &m,short unsigned int k);
matrizPolmio Diferencial(const matrizPolmio &m, short unsigned int k);

matrizPolmio Integral(const matrizPolmio &m,short unsigned int j,const double &x0,const double &x1);
matrizPolmio Integral(const matrizPolmio &m,short unsigned int j,const Polinomio &x0,const Polinomio &x1);
matrizPolmio Integral(const matrizPolmio &m,short unsigned int j,const Polinomio &x0,const double &x1);
matrizPolmio Integral(const matrizPolmio &m,short unsigned int j,const double &x0,const Polinomio &x1);
matrizPolmio Derivada(const matrizPolmio &m,short unsigned int j,const double &x);
matrizPolmio Derivada(const matrizPolmio &m,short unsigned int j,const Polinomio &x);
 
#endif

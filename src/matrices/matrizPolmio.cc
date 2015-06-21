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
//matrizPolmio.cc

#include "matrizPolmio.h"
#include "m_double.h"

matrizPolmio::matrizPolmio(const m_double &m)
  : polmio_matriz(m.getNumFilas(),m.getNumCols())
  {
    for(register size_type i=1;i<=fls;i++)
      for(register size_type j=1;j<=cls;j++)
        (*this)(i,j)= Polinomio(m(i,j));
  }
void matrizPolmio::eval(short unsigned int k,const double &val)
  {
    for(register size_type i=1;i<=fls;i++)
      for(register size_type j=1;j<=cls;j++)
        (*this)(i,j)= (*this)(i,j).Eval(k,val);
  }
void matrizPolmio::eval(short unsigned int k,const Polinomio &val)
  {
    for(register size_type i=1;i<=fls;i++)
      for(register size_type j=1;j<=cls;j++)
        (*this)(i,j)= (*this)(i,j).Eval(k,val);
  }
void matrizPolmio::eval(const vZ_double &v)
  {
    for(register size_type i=1;i<=fls;i++)
      for(register size_type j=1;j<=cls;j++)
        (*this)(i,j)= (*this)(i,j).Eval(v);
  }
void matrizPolmio::eval(const mZ_double &v)
  {
    for(register size_type i=1;i<=fls;i++)
      for(register size_type j=1;j<=cls;j++)
        (*this)(i,j)= (*this)(i,j).Eval(v);
  }
matrizPolmio matrizPolmio::Eval(short unsigned int j,const double &val) const
  {
    matrizPolmio retval(*this);
    retval.eval(j,val);
    return retval;
  }
matrizPolmio matrizPolmio::Eval(short unsigned int j,const Polinomio &val) const
  {
    matrizPolmio retval(*this);
    retval.eval(j,val);
    return retval;
  }
matrizPolmio matrizPolmio::Eval(const vZ_double &v) const
  {
    matrizPolmio retval(*this);
    retval.eval(v);
    return retval;
  }
matrizPolmio matrizPolmio::Eval(const mZ_double &v) const
  {
    matrizPolmio retval(*this);
    retval.eval(v);
    return retval;
  }
matrizPolmio &matrizPolmio::Trn(void)
  {
    polmio_matriz::Trn();
    return *this;
  }
matrizPolmio matrizPolmio::GetTrn(void) const
  {
    matrizPolmio m= *this;
    m.Trn();
    return m;
  }
matrizPolmio &matrizPolmio::operator*=(const double &d)
  {
    const size_type sz= size();      
    for(register size_type i= 0;i<sz;i++)
      (*this)[i]*=d;
    return *this;
  }
matrizPolmio &matrizPolmio::operator+=(const m_double &m)
  {
    if (!CompDim(*this,m))
      {
        std::cerr << "Matrices de dimensiones distintas en operador +=" << std::endl;
        abort();
      }
    matrizPolmio::size_type fls= getNumFilas(), cls= getNumCols();
    register matrizPolmio::size_type i,j;
    for (i=1;i<=fls;i++)
      for (j=1;j<=cls;j++)
        (*this)(i,j)+= m(i,j);
    return *this;
  }
matrizPolmio &matrizPolmio::operator-=(const m_double &m)
  {
    if (!CompDim(*this,m))
      {
        std::cerr << "Matrices de dimensiones distintas en operador +=" << std::endl;
        abort();
      }
    matrizPolmio::size_type fls= getNumFilas(), cls= getNumCols();
    register matrizPolmio::size_type i,j;
    for (i=1;i<=fls;i++)
      for (j=1;j<=cls;j++)
        (*this)(i,j)-= m(i,j);
    return *this;
  }

matrizPolmio &matrizPolmio::Primitiva(short unsigned int k)
  {
    const matrizPolmio::size_type fls= getNumFilas();
    const matrizPolmio::size_type cls= getNumCols();
    matrizPolmio::size_type i,j;
    for (i=1;i<=fls;i++)
      for (j=1;j<=cls;j++)
        (*this)(i,j)= (*this)(i,j).Primitiva(k);
    return *this;
  }

matrizPolmio &matrizPolmio::Diferencial(short unsigned int k)
  {
    const matrizPolmio::size_type fls= getNumFilas();
    const matrizPolmio::size_type cls= getNumCols();
    matrizPolmio::size_type i,j;
    for (i=1;i<=fls;i++)
      for (j=1;j<=cls;j++)
        (*this)(i,j)= (*this)(i,j).Parcial(k);
    return *this;
  }

matrizPolmio operator*(const matrizPolmio &m1,const matrizPolmio &m2)
  {
    matrizPolmio producto(m1.fls,m2.cls);
    if (m1.cls != m2.fls)
      {
        std::cerr << "Matrices de dimensiones incompatibles en producto." << std::endl;
        std::cerr << "  m1= " << m1 << std::endl;
        std::cerr << "  m2= " << m2 << std::endl;
        return producto;
      }
    matrizPolmio::size_type i=1,j=1;
    for (i= 1;i<=m1.fls;i++)
      for (j= 1;j<=m2.cls;j++)
        producto(i,j) = dot(m1.GetFila(i),m2.GetCol(j));
    return producto;
  }
matrizPolmio operator+(const matrizPolmio &m1,const matrizPolmio &m2)
  {
    matrizPolmio retval(m1);
    retval+=(m2);
    return retval;
  }
matrizPolmio operator-(const matrizPolmio &m1,const matrizPolmio &m2)
  {
    matrizPolmio retval(m1);
    retval+=(m2);
    return retval;
  }
matrizPolmio operator+(const matrizPolmio &m1,const m_double &m2)
  {
    matrizPolmio retval(m1);
    retval+=(m2);
    return retval;
  }
matrizPolmio operator-(const matrizPolmio &m1,const m_double &m2)
  {
    matrizPolmio retval(m1);
    retval-=(m2);
    return retval;
  }
matrizPolmio operator-(const m_double &m1,const matrizPolmio &m2)
  { return matrizPolmio(m1)-m2; }

matrizPolmio operator-(const matrizPolmio &m)
  {
    matrizPolmio retval(m);
    retval.Neg();
    return retval;
  }

//Devuelve el valor del polinomio en el punto vp.
Polinomio Eval(const Polinomio &p,const matrizPolmio &vp)
  {
    Polinomio result= p;
    size_t i,sz= vp.getNumFilas();
    for (i=1;i<=sz;i++) result= result.Eval(i,vp(i,1));
    return result;
  }

m_double Eval(const matrizPolmio &p,const m_double &x)
  {
    matrizPolmio::size_type fls= p.getNumFilas();
    matrizPolmio::size_type cls= p.getNumCols();
    m_double f(fls,cls);
    matrizPolmio::size_type i,j;
    for (i=1;i<=fls;i++)
      for (j=1;j<=cls;j++)
        f(i,j)= p(i,j).Eval(x);
    return f;
  }

matrizPolmio Eval(const matrizPolmio &m1,const matrizPolmio &m2)
  {
    matrizPolmio::size_type fls= m1.getNumFilas();
    matrizPolmio::size_type cls= m1.getNumCols();
    matrizPolmio f(fls,cls);
    matrizPolmio::size_type i,j;
    for (i=1;i<=fls;i++)
      for (j=1;j<=cls;j++)
        f(i,j)= Eval(m1(i,j),m2);
    return f;
  }
Polinomio dot(const matrizPolmio &v1,const m_double &v2)
//Producto escalar de dos vectores.
//v1: Vector fila.
//v2: Vector columna.
  {
    const matrizPolmio::size_type cls= v1.getNumCols();
    if (cls != v2.getNumFilas())
      {
        std::cerr << "Matrices de dimensiones incompatibles en producto escalar." << std::endl;
        abort();      
      }
    Polinomio retval= Polinomio::neutro_suma();
    for(register matrizPolmio::size_type i=1;i<=cls;i++)
      retval+= v1(1,i) * v2(i,1);
    return retval;
  }
Polinomio dot(const m_double &v1,const matrizPolmio &v2)
//Producto escalar de dos vectores.
//v1: Vector fila.
//v2: Vector columna.
  {
    const m_double::size_type cls= v1.getNumCols();
    if (cls != v2.getNumFilas())
      {
        std::cerr << "Matrices de dimensiones incompatibles en producto escalar." << std::endl;
        std::cerr << "  v1= " << v1 << std::endl;
        std::cerr << "  v2= " << v2 << std::endl;
        abort();
      }
    Polinomio retval= Polinomio::neutro_suma();
    for(register matrizPolmio::size_type i=1;i<=cls;i++)
      retval+= v1(1,i) * v2(i,1);
    return retval;  
  }
matrizPolmio operator*(const Polinomio &p,const m_double &m)
  {
    const m_double::size_type fls= m.getNumFilas();
    const m_double::size_type cls= m.getNumCols();
    matrizPolmio prod(fls,cls);
    for(register matrizPolmio::size_type i= 1;i<=fls;i++)
      for(register matrizPolmio::size_type j= 1;j<=cls;j++)
        prod(i,j)= m(i,j) * p;
    return prod;
  }
matrizPolmio operator*(const matrizPolmio &m,const double &d)
  {
    matrizPolmio retval(m);
    retval*=(d);
    return retval;
  }

matrizPolmio Primitiva(const matrizPolmio &m,short unsigned int k)
  {
    matrizPolmio retval(m);
    retval.Primitiva(k);
    return k;
  }

matrizPolmio Diferencial(const matrizPolmio &m, short unsigned int k)
  {
    matrizPolmio retval(m);
    retval.Diferencial(k);
    return k;
  }

matrizPolmio Integral(const matrizPolmio &m,short unsigned int j,const double &x0,const double &x1)
  {
    matrizPolmio p= Primitiva(m,j);
    return Eval(p,j,x1) - Eval(p,j,x0);
  }
matrizPolmio Integral(const matrizPolmio &m,short unsigned int j,const Polinomio &x0,const Polinomio &x1)
  {
    matrizPolmio p= Primitiva(m,j);
    return Eval(p,j,x1) - Eval(p,j,x0);
  }
matrizPolmio Integral(const matrizPolmio &m,short unsigned int j,const Polinomio &x0,const double &x1)
  {
    matrizPolmio p= Primitiva(m,j);
    return Eval(p,j,x1) - Eval(p,j,x0);
  }
matrizPolmio Integral(const matrizPolmio &m,short unsigned int j,const double &x0,const Polinomio &x1)
  {
    matrizPolmio p= Integral(m,j,x1,x0);
    return -p;
  }
matrizPolmio Derivada(const matrizPolmio &m,short unsigned int j,const double &x)
  { return Eval(Diferencial(m,j),j,x); }
matrizPolmio Derivada(const matrizPolmio &m,short unsigned int j,const Polinomio &x)
  { return Eval(Diferencial(m,j),j,x); }

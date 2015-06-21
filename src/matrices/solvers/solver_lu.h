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
//solver_lu.h

#ifndef SOLVER_LU_H
#define SOLVER_LU_H

#include "xc_basic/src/matrices/solvers/solver.h"
#include <cmath>
#include "xc_basic/src/matrices/matrizZ.h"
#include "xc_basic/src/util/matem.h"
#include "xc_basic/src/matrices/matsimZ.h"
#include "xc_basic/src/matrices/matdispZ.h"

template<class M,class V>
class SolverLU: public SolverM<M>
  {
  protected:
    typedef SolverM<M> solver_m;
    typedef typename solver_m::tipo_val tipo_val;

    long v;

    virtual tipo_val busca_pivote(const size_t &k,size_t &maxi) const
      {
        //Para cada fila de la columna k a partir de la fila k
        tipo_val c;
        tipo_val c1;
        register size_t i;
        for(i= k, maxi= k, c=solver_m::CERO;i<=this->n;i++)
          {
            c1= (*this->A)(this->P(i),k);
            c1*= sgn(c1);
            if (c1 > c) { c = c1; maxi = i; }
          }
        return c;
      }
    void resto_fila(const size_t &i,const size_t &k,const tipo_val &d)
      {
        const size_t pk= this->P(k);
        for(register size_t j=k+1;j<=this->n;j++) //Columnas siguientes de la fila.
          (*this->A)(i,j)-= d * (*this->A)(pk,j);
      }
    void aplica_pivote(const size_t &k)
      {
        register tipo_val d= (*this->A)(this->P(k),k);
        for(register size_t i= k+1;i<=this->n;i++)
          {
            //Divide el elemento de la columna 
	    //del pivote por el pivote.
            register const size_t pi= this->P(i);
            register const tipo_val &dd= ((*this->A)(pi,k)/= d);
            if(dd!=solver_m::CERO) resto_fila(pi,k,dd);
          }
      }

    virtual bool decomp(void)
      //Devuelve verdadero si puede descomponer la matriz.
      {
        size_t maxi; //Fila del pivote.
        tipo_val c; //Valor del pivote.
        size_t p= 1;
        this->inic_p();

        for(register size_t k= 1;k<= this->n; k++) //Para cada columna.
          {
            c= busca_pivote(k,maxi);
            if (k != maxi)
              {
                p++;
		std::swap(this->P(k),this->P(maxi));
	      }
            if ((*this->A)(this->P(k),k) == solver_m::CERO) //Si el pivote de esa columna es nulo.
              {
                v = -1;
                if(this->verborrea) std::cerr << "Matriz singular" << std::endl;
                return false;
              }
            aplica_pivote(k);
          }
        v=p;
        this->desc= 1;
        return true;
      }
    void triang_inf(V &B) const
    //Recorre el triangulo inferior
      {
        for(register size_t k=1; k<=this->n; k++) 
          {
            register const size_t pk= this->P(k);
            const tipo_val &Bpk= B(pk);
            for(register size_t i=k+1; i<=this->n; i++)
              {
                register const size_t pi= this->P(i);
	        B(pi)-= ( (*this->A)(pi,k) * Bpk );
              }
          }
      }
    V atras(const V &B) const
      //Sustitución hacia atrás.
      {
        register size_t j;
        tipo_val sum;
        V X(B);
        X(this->n)= B(this->P(this->n)) / (*this->A)(this->P(this->n),this->n);
        for(register size_t k=this->n; k>1;)
          {
            k--;
            sum=solver_m::CERO;
            register const size_t pk= this->P(k);
            for(j=k+1; j<=this->n; j++) sum+= (*this->A)(pk,j) * X(j);
            X(k)= (B(pk) - sum) /(*this->A)(pk,k);
          }
        return X;
      }
    virtual V backsub(V &B) const
      {
        if(!check_backsub(B)) exit(1);
        triang_inf(B);
        return atras(B);
      }
    inline void check_decomp(void) const
      { if(!this->desc) std::cerr << "¡Ojo! Se ha llamado a Backsub sin llamar antes a Decomp." << std::endl; }
  public:
    SolverLU(const size_t &verborrea= 0): solver_m(verborrea) {}
    V BackSub(V &B) const
      {
        check_decomp();
        return backsub(B);
      }
    V BackSub(const V &B) const
      {
        check_decomp();
        V C(B);
        return backsub(C);
      }
  };

template<class M,class V>
class SolverConstLU: public SolverLU<M,V>
  {
  protected:
    M a;
  public:
    SolverConstLU(const size_t &verborrea): SolverLU<M,V>(verborrea), a(0,0){}
    void PutMatriz(const M &m)
      {
        a= m;
        SolverLU<M,V>::PutMatriz(a);
      }
  };


template <class treal,class V>
class SolverSimLU: public SolverConstLU<matrizZ<treal>,V>
  {
  public:
    SolverSimLU(const size_t &verborrea)
      : SolverConstLU<matrizZ<treal>,V>(verborrea)
      {}
    void PutMatriz(const matsimZ<treal> &m)
      { SolverConstLU<matrizZ<treal>,V>::PutMatriz(m.GetCompleta()); }
  };

template <class treal,class V>
class SolverDispLU: public SolverLU<matdispZ<treal>,V>
  {
  public:
    typedef SolverLU<matdispZ<treal>,V> solver_lu;
    typedef matdispZ<treal> msp_treal;
    typedef typename msp_treal::value_type tipo_val;
    typedef typename msp_treal::const_c_iterator const_c_iterator;
    typedef typename msp_treal::const_f_iterator const_f_iterator;
    typedef typename msp_treal::c_iterator c_iterator;
    typedef typename msp_treal::f_iterator f_iterator;
    typedef typename msp_treal::sp_vector sp_vector;

  private:
    tipo_val busca_pivote( const c_iterator &ic,
                           const_f_iterator &maxi) const
      {
        //Para cada fila de la columna ic a partir de la fila ic->first
        tipo_val c1,c= solver_lu::CERO;
        const_f_iterator fi= this->A->FlsFind(ic,this->P(ic->first));
        for(maxi= fi,c= solver_lu::CERO;fi!=this->A->FlsEnd(ic);fi++)
          {
            c1= fabs(fi->second);
            if(c1 > c) { c = c1; maxi = fi; }
          }
        return c;
      }
    void aplica_pivote(const c_iterator &ic)
      {
        const size_t k= ic->first;
        tipo_val d= (*this->A)(this->P(k),k);
        std::cout << "***** aplica pivote col: " << k << ' ' << d << std::endl;
	sp_vector &col= this->A->GetCol(ic);
        for(register size_t i= k+1;i<=this->n;i++)
          {
            //Divide el elemento de la columna 
	    //del pivote por el pivote.
            const double &dd= (col[this->P(i)]/=d);
            if(dd!=solver_lu::CERO) resto_fila(this->P(i),k,dd);
          }
      }
    bool decomp(void)
      //Devuelve verdadero si puede descomponer la matriz.
      {
        std::cout << "parece que falla" << std::endl;
        const_f_iterator maxi; //Fila del pivote.
        tipo_val c; //Valor del pivote.
        size_t p= 1;
        this->inic_p();

	c_iterator ic;
        for(ic= this->A->ClsBegin();ic!= this->A->ClsEnd(); ic++) //Para cada columna.
          {
            size_t k= ic->first;
            c= busca_pivote(ic,maxi);
            if (k != maxi->first)
              {
                p++;
		std::swap(this->P(k),this->P(maxi->first));
	      }
            if ((*this->A)(this->P(k),k) == solver_lu::CERO) //Si el pivote de esa columna es nulo.
              {
                this->v = -1;
                if(this->verborrea) std::cerr << "Matriz singular" << std::endl;
                return false;
              }
            //aplica_pivote(ic->first);
            aplica_pivote(ic);
          }
        this->v=p;
        this->desc= 1;
        return true;
      }
  };

#endif

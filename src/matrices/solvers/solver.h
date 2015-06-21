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
//solver.h

#ifndef SOLVER_H
#define SOLVER_H

#include "xc_basic/src/matrices/m_sizet.h"

class Solver
  {
  private:
    m_sizet PP;
  protected:
    size_t verborrea; //0: No mostrar ningún mensaje de error o aviso.
                      //1: Mostrar sólo los importantes.
                      //2: Mostrar todos los mensajes.
    size_t n;
    int desc;

    inline const size_t &P(size_t i) const
      { return PP(i); }
    inline size_t &P(size_t i)
      { return PP(i); }

    void inic_p(void);
    int check_pp(const size_t num_filas) const;
  public:
    Solver(const size_t &vrbrr= 1): PP(0,0), verborrea(vrbrr), n(0), desc(0) {}
    void SetVerborrea(const size_t &v)
      { verborrea= v; }
  };

template<class M>
class SolverM: public Solver
  {
  protected:
    M *A;

    typedef typename M::value_type tipo_val;
    static const tipo_val CERO;

    virtual bool decomp(void)= 0;

    inline int check_matriz(void) const
      {
        if(A->getNumCols() != A->getNumFilas())
          {
	    if(verborrea)
              std::cerr << "La matriz no es cuadrada" << std::endl;
	    return 0;
          }
        return 1;
      }

    template <class V>
    inline int check_backsub(const V &B) const
      {
        if(!check_matriz()) return 0;
        if(B.getNumFilas()!=A->getNumFilas() || B.getNumCols()!=1)
          {
	    if(verborrea) std::cerr << "La matriz " << B << " no tiene las mismas filas que " << A << std::endl;
	    return 0;
          }
        return check_pp(A->getNumFilas());
      }
  public:
    SolverM(const size_t &vrbrr= 1): Solver(vrbrr), A(NULL) {}
    void PutMatriz(M &m)
      {
        A= &m;
        n= A->getNumCols();
        desc= 0;
      }
    bool Decomp(void)
      {
        if(!A)
          {
	    if(verborrea) std::cerr << "No hay matriz que descomponer." << std::endl;
            return false;
          }
        if(!desc)
          return decomp();
        else
          return true;
      }
    virtual ~SolverM(void) {}
  };


template<class M>
const typename SolverM<M>::tipo_val SolverM<M>::CERO= neutro_suma(typename SolverM<M>::tipo_val());


#endif

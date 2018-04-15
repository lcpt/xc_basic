//----------------------------------------------------------------------------
//  biblioteca bibXCLcmd; bilioteca de comandos para el intérprete del lenguaje
//  de entrada de datos.
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
//TritrizIndices.cc

#include "TritrizIndices.h"

TritrizIndices::TritrizIndices(const size_t &capas,const size_t &n_rows,const size_t &n_columns)
  : std::vector<MatrizIndices>(capas,MatrizIndices(n_rows,n_columns)) {}

//! @brief Return el número de elementos de la tritriz.
size_t TritrizIndices::NumIndices(void) const
  {
    const size_t sz= this->size();
    if(sz<1)
      return 0;
    else
      {
        const MatrizIndices &capa= (*this)(1); 
        return sz*capa.getNumberOfRows()*capa.getNumberOfColumns();
      }
  }

//! @brief Return el number of rows de la tritriz.
size_t TritrizIndices::getNumberOfRows(void) const
  {
    size_t retval= 0;
    if(this->size())
      retval= (*this)[0].getNumberOfRows();
    return retval;
  }

//! @brief Return el number of columns de la tritriz.
size_t TritrizIndices::getNumberOfColumns(void) const
  {
    size_t retval= 0;
    if(this->size())
      retval= (*this)[0].getNumberOfColumns();
    return retval;
  }

//! @brief Inserta el objeto en el stream de salida.
inline std::ostream &operator<<(std::ostream &os,const TritrizIndices &t)
  {
    const size_t ncapas= t.GetCapas();
    for(size_t i=1;i<=ncapas;i++)
      os << t(i);
    return os;
  }

//! @brief Agrega a todos los elementos el valor
//! que is being passed as parameter.
void TritrizIndices::Offset(const VIndices &vi)
  {
    const size_t nCapas= GetCapas();
    for(size_t i= 1;i<= nCapas;i++)
      get_capa(i).Offset(vi);
  }

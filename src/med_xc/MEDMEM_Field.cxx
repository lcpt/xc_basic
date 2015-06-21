// Copyright (C) 2007-2012  CEA/DEN, EDF R&D, OPEN CASCADE
//
// Copyright (C) 2003-2007  OPEN CASCADE, EADS/CCR, LIP6, CEA/DEN,
// CEDRAT, EDF R&D, LEG, PRINCIPIA R&D, BUREAU VERITAS
//
// This library is free software; you can redistribute it and/or
// modify it under the terms of the GNU Lesser General Public
// License as published by the Free Software Foundation; either
// version 2.1 of the License.
//
// This library is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
// Lesser General Public License for more details.
//
// You should have received a copy of the GNU Lesser General Public
// License along with this library; if not, write to the Free Software
// Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307 USA
//
// See http://www.salome-platform.org/ or email : webmaster.salome@opencascade.com
//

#include "MEDMEM_Field.hxx"
#include "MEDMEM_Mesh.hxx"

using namespace std;
using namespace MEDMEM;
using namespace MED_EN;

// ---------------------------------
// FIELD_ : Constructors
// ---------------------------------
FIELD_::FIELD_(): 
  _isRead(false),
  _isMinMax(false),
  _name(""), _description(""), _support((SUPPORT *)NULL),
  _numberOfComponents(0), _numberOfValues(0),
  _iterationNumber(-1),_time(0.0),_orderNumber(-1),
  _valueType(MED_EN::MED_UNDEFINED_TYPE),
  _interlacingType(MED_EN::MED_UNDEFINED_INTERLACE)
{
  MESSAGE_MED("Constructeur FIELD_ sans parametre");
}

FIELD_::FIELD_(const SUPPORT * Support, const int NumberOfComponents):
  _isRead(false),
  _isMinMax(false),
  _name(""), _description(""), _support(Support),
  _numberOfComponents(NumberOfComponents),
  _iterationNumber(-1),_time(0.0),_orderNumber(-1),
  _valueType(MED_EN::MED_UNDEFINED_TYPE),
  _interlacingType(MED_EN::MED_UNDEFINED_INTERLACE)
{
  MESSAGE_MED("FIELD_(const SUPPORT * Support, const int NumberOfComponents)");

  _numberOfValues = Support->getNumberOfElements(MED_ALL_ELEMENTS);
  _componentsTypes.resize(NumberOfComponents);
  _componentsNames.resize(NumberOfComponents);
  _componentsDescriptions.resize(NumberOfComponents);
  _componentsUnits.resize(NumberOfComponents);
  _MEDComponentsUnits.resize(NumberOfComponents);
  for(int i=0;i<NumberOfComponents;i++) {
    _componentsTypes[i] = 0 ;
  }
  if(_support)
    _support->addReference();
}

FIELD_& FIELD_::operator=(const FIELD_ &m) {

  if ( this == &m) return *this;

  _isRead             = m._isRead ;
  _isMinMax           = m._isMinMax ;
  _name               = m._name;
  _description        = m._description;
  if(_support!=m._support)
    {
      if(_support)
        _support->removeReference();
      _support=m._support;   //Cf Opérateur de recopie du Support?
      if(_support)
        _support->addReference();
    }
  _numberOfComponents = m._numberOfComponents;
  _numberOfValues     = m._numberOfValues;

  _componentsTypes.resize(_numberOfComponents);
  for (int i=0; i<m._numberOfComponents; i++)
    {_componentsTypes[i]=m._componentsTypes[i];}

  _componentsNames.resize(_numberOfComponents);
  _componentsDescriptions.resize(_numberOfComponents);
  _componentsUnits.resize(_numberOfComponents);
  _MEDComponentsUnits.resize(_numberOfComponents);

  for (int i=0; i<m._numberOfComponents; i++)
    {_componentsNames[i]=m._componentsNames[i];}
  for (int i=0; i<m._numberOfComponents; i++)
    {_componentsDescriptions[i]=m._componentsDescriptions[i];}
  for (int i=0; i<m._numberOfComponents; i++)
    {_componentsUnits[i] = m._componentsUnits[i];}
  // L'operateur '=' est defini dans la classe UNIT
  for (int i=0; i<m._numberOfComponents; i++)
    {_MEDComponentsUnits[i] = m._MEDComponentsUnits[i];}

  _iterationNumber = m._iterationNumber;
  _time            = m._time;
  _orderNumber     = m._orderNumber;

  // _valueType et _interlacingType doivent uniquement être recopiés 
  // par l'opérateur de recopie de FIELD<T,...>

  //_drivers = m._drivers ; // PG : Well, same driver, what about m destructor !

  return *this;
}

FIELD_::FIELD_(const FIELD_ &m)
{
  _isRead = m._isRead ;
  _isMinMax = m._isMinMax ;
  _name = m._name;
  _description = m._description;
  _support = m._support;
  if(_support)
    _support->addReference();
  _numberOfComponents = m._numberOfComponents;
  _numberOfValues = m._numberOfValues;
  copyGlobalInfo(m);
  //_valueType = m._valueType;
  // _valueType et _interlacingType doivent uniquement être recopiés 
  // par l'opérateur de recopie de FIELD<T,...>
  //_drivers = m._drivers ; // PG : Well, same driver, what about m destructor !
}

FIELD_::~FIELD_()
{
  MESSAGE_MED("~FIELD_()");

  MESSAGE_MED("In this object FIELD_ there is(are) " << _drivers.size() << " driver(s)");

  for (unsigned int index=0; index < _drivers.size(); index++ )
    {
      SCRUTE_MED(_drivers[index]);
      if ( _drivers[index] != NULL) delete _drivers[index];
    }
  _drivers.clear();
  if(_support)
    _support->removeReference();
  _support=0;
}

/*!
  \if developper
  PROVISOIRE : retourne des volumes, surfaces ou longueurs suivant les cas
  \endif
*/
FIELD<double>* FIELD_::_getFieldSize(const SUPPORT *subSupport) const
{
  FIELD<double> *p_field_size= NULL;

  const SUPPORT* support = subSupport;
  if ( !support )
    {
      if ( getSupport()->getEntity() == MED_EN::MED_NODE )
        support = getSupport()->getMesh()->getSupportOnAll( MED_EN::MED_CELL );
      else
        support = getSupport();
      support->addReference();
    }
  const GMESH* mesh = getSupport()->getMesh();
  switch (getSupport()->getEntity())
    {
    case MED_EN::MED_CELL :
      switch (mesh->getMeshDimension() ) 
        {
        case 1:
          p_field_size=mesh->getLength( support );
          break;
        case 2:
          p_field_size=mesh->getArea( support );
          break;
        case 3:
          p_field_size=mesh->getVolume( support );
          break;
        }
      break;

    case MED_EN::MED_FACE :
      p_field_size=mesh->getArea( support );
      break;

    case MED_EN::MED_EDGE :
      p_field_size=mesh->getLength( support );
      break;
    case MED_EN::MED_NODE : // issue 0020120: [CEA 206] normL2 on NODE field
      {
        switch (mesh->getMeshDimension() ) 
          {
          case 1:
            p_field_size=mesh->getLength( support );
            break;
          case 2:
            p_field_size=mesh->getArea( support );
            break;
          case 3:
            p_field_size=mesh->getVolume( support );
            break;
          }
        break;
      }
    }
  if(!subSupport && support)
    support->removeReference();
  return p_field_size;
}


/*! 
  \if developper
  Check up the compatibility of field before computing sobolev norm 
  \endif
*/
void FIELD_::_checkNormCompatibility(const FIELD<double>* support_volume,
                                     const bool           nodalAllowed) const throw (MEDEXCEPTION)
{
  string diagnosis;

  if( getSupport()->getEntity() == MED_EN::MED_NODE)
    {
      if ( !nodalAllowed )
        {
          diagnosis="Cannot compute sobolev norm on a field "+getName()+
            " : it has support on nodes!";
          throw MEDEXCEPTION(diagnosis.c_str());
        }
      if ( !getSupport()->getMesh() )
        {
          diagnosis="Cannot compute Lnorm of nodal field "+getName()+
            " : it's support has no mesh reference";
          throw MEDEXCEPTION(diagnosis.c_str());
        }
      if ( !getSupport()->getMesh()->getIsAGrid() &&
           !( (const MESH*)getSupport()->getMesh() )->existConnectivity(MED_EN::MED_NODAL,MED_EN::MED_CELL) )
        {
          diagnosis="Cannot compute Lnorm of nodal field"+getName()+
            " : it's supporting mesh has no nodal connectivity data";
          throw MEDEXCEPTION(diagnosis.c_str());
        }
    }

  if (getNumberOfValues()*getNumberOfComponents()<= 0) // Size of array has to be strictly positive
    {
      diagnosis="Cannot compute the norm of "+getName()+
        " : it size is non positive!";
      throw MEDEXCEPTION(diagnosis.c_str());
    }

  if( getSupport()->getNumberOfElements(MED_EN::MED_ALL_ELEMENTS) != getNumberOfValues() ) {
    diagnosis="Cannot compute Lnorm of "+getName()+
      " : the suppors size not corresponded to number of elements!";
    throw MEDEXCEPTION(diagnosis.c_str());
  }

  if (getGaussPresence() ) {
    diagnosis="Cannot compute Lnorm of "+getName()+
      " : Gauss numbers greater than one are not yet implemented!";
    throw MEDEXCEPTION(diagnosis.c_str());
  }

  if(support_volume) // if the user has supplied the volume
    {
      if ( getSupport()->getEntity() == MED_EN::MED_NODE )
        {
          if (support_volume->getNumberOfValues()!=
              getSupport()->getMesh()->getNumberOfElements(MED_EN::MED_CELL,MED_ALL_ELEMENTS))
            {
              diagnosis="Cannot compute Lnorm of nodal field "+getName()+
                " : the volume furnished has wrong number of values";
              throw MEDEXCEPTION(diagnosis.c_str());
            }
          return;
        }
      if(support_volume->getSupport()!=getSupport())
        {
          diagnosis="Cannot compute Lnorm of "+getName()+
            " : the volume furnished has not the same support!";
          throw MEDEXCEPTION(diagnosis.c_str());
        }
      if(support_volume->getNumberOfValues()!=getNumberOfValues())
        {
          diagnosis="Cannot compute Lnorm of "+getName()+
            " : the volume furnished has not the same number of values!";
          throw MEDEXCEPTION(diagnosis.c_str());
        }
      if( getSupport()->getNumberOfElements() != 
          support_volume->getSupport()->getNumberOfElements() ) {
        diagnosis="Cannot compute Lnorm of "+getName()+
          " : the supports have not the same number of elements!";
        throw MEDEXCEPTION(diagnosis.c_str());
      }
    }

}

/*! 
  \if developper
  Check up the compatibility of fields before performing an arithmetic operation
  \endif
*/
void FIELD_::_checkFieldCompatibility(const FIELD_& m, const FIELD_& n, bool checkUnit) throw (MEDEXCEPTION)
{
    string diagnosis;

    // check-up, fill diagnosis if some incompatibility is found.

    // Ne pas vérifier l'entrelacement
    // Le compilo s'en occupe Rmq from EF

    if(m._support != n._support)
      {
        if(!(*m._support==*n._support))
          diagnosis+="They don't have the same support!";
      }
    else if(m._numberOfComponents != n._numberOfComponents)
      diagnosis+="They don't have the same number of components!";
    else if (m._valueType != n._valueType)
      diagnosis+="They don't have the same type!";
    else if(m._numberOfValues != n._numberOfValues)
      diagnosis+="They don't have the same number of values!";
    else
      {
        if(checkUnit)
          {
            for(int i=0; i<m._numberOfComponents; i++)
              {
                // Not yet implemented   
                //          if(m._componentsTypes[i] != n._componentsTypes[i])
                //          {
                //              diagnosis+="Components don't have the same types!";
                //              break;
                //          }
                if(m._MEDComponentsUnits[i] != n._MEDComponentsUnits[i])
                  {
                    diagnosis+="Components don't have the same units!";
                    break;
                  }
              }
          }
      }

    if(diagnosis.size()) // if fields are not compatible : complete diagnosis and throw exception
    {
        diagnosis="Field's operation not allowed!\nThe fields " + m._name + " and " 
                 + n._name + " are not compatible.\n" + diagnosis;
        throw MEDEXCEPTION(diagnosis.c_str());
    }

    if( m.getNumberOfValues()<=0 || m.getNumberOfComponents()<=0) // check up the size is strictly positive
    {
        diagnosis="Field's operation not allowed!\nThe fields " + m._name + " and " 
                 + n._name + " are empty! (size<=0).\n";
        throw MEDEXCEPTION(diagnosis.c_str());
    }

}

void FIELD_::_deepCheckFieldCompatibility(const FIELD_& m, const FIELD_& n , bool checkUnit ) throw (MEDEXCEPTION)
{
  string diagnosis;

  // check-up, fill diagnosis if some incompatibility is found.

  // Ne pas vérifier l'entrelacement
  // Le compilo s'en occupe Rmq from EF

    if(m._support != n._support)
      {
        if(!(m._support->deepCompare(*n._support)))
          diagnosis+="They don't have the same support!";
      }
    else if (m._valueType != n._valueType)
      diagnosis+="They don't have the same type!";
    else if(m._numberOfComponents != n._numberOfComponents)
      diagnosis+="They don't have the same number of components!";
    else if(m._numberOfValues != n._numberOfValues)
      diagnosis+="They don't have the same number of values!";
    else
      {
        if(checkUnit)
          {
            for(int i=0; i<m._numberOfComponents; i++)
              {
                if(m._MEDComponentsUnits[i] != n._MEDComponentsUnits[i])
                  {
                    diagnosis+="Components don't have the same units!";
                    break;
                  }
              }
          }
      }

    if(diagnosis.size()) // if fields are not compatible : complete diagnosis and throw exception
    {
        diagnosis="Field's operation not allowed!\nThe fields " + m._name + " and " 
                 + n._name + " are not compatible.\n" + diagnosis;
        throw MEDEXCEPTION(diagnosis.c_str());
    }

    if( m.getNumberOfValues()<=0 || m.getNumberOfComponents()<=0) // check up the size is strictly positive
    {
        diagnosis="Field's operation not allowed!\nThe fields " + m._name + " and " 
                 + n._name + " are empty! (size<=0).\n";
        throw MEDEXCEPTION(diagnosis.c_str());
    }
} 


void     FIELD_::rmDriver      (int index)
{
  MESSAGE_MED("void FIELD_::rmDriver(int index) : removing the driver " << index);
}

int      FIELD_::addDriver     (driverTypes driverType, 
                                const string & fileName,
                                const string & driverFieldName,
                                MED_EN::med_mode_acces access)
{
  MESSAGE_MED("int FIELD_::addDriver(driverTypes driverType, const string & fileName, const string & driverFieldName) : adding the driver " << driverType << " fileName = " << fileName.c_str() << " driverFieldName = " << driverFieldName.c_str());
  return 0;
}

int      FIELD_::addDriver     (GENDRIVER & driver)
{
  MESSAGE_MED("int FIELD_::addDriver(GENDRIVER & driver) : driver " << driver);
  return 0;
}

void     FIELD_::openAppend    ( void )                               {}
void     FIELD_::write         (const GENDRIVER &,
                                MED_EN::med_mode_acces)               {}
void     FIELD_::write         (driverTypes driverType,
                                const std::string & fileName,
                                MED_EN::med_mode_acces medMode)       {}
void     FIELD_::writeAppend   (const GENDRIVER &)                    {}
void     FIELD_::write         (int ) {}
void     FIELD_::writeAppend   (int , const string & ) {}
void     FIELD_::read          (int )                                 {}
void     FIELD_::read          (const GENDRIVER &)                    {}
void     FIELD_::read          (driverTypes driverType, const std::string & fileName){}
void     FIELD_::copyGlobalInfo(const FIELD_& m)
{  

  _componentsTypes.resize(_numberOfComponents);
  _componentsNames.resize(_numberOfComponents);
  _componentsDescriptions.resize(_numberOfComponents);
  _componentsUnits.resize(_numberOfComponents);
  _MEDComponentsUnits.resize(_numberOfComponents);

  for (int i=0; i<m._numberOfComponents; i++)
    {_componentsTypes[i]=m._componentsTypes[i];}

  for (int i=0; i<m._numberOfComponents; i++)
    _componentsNames[i]=m._componentsNames[i];
  for (int i=0; i<m._numberOfComponents; i++)
    _componentsDescriptions[i]=m._componentsDescriptions[i];

  for (int i=0; i<m._numberOfComponents; i++)
    _componentsUnits[i] = m._componentsUnits[i];
  
  // L'operateur '=' est defini dans la classe UNIT
  for (int i=0; i<m._numberOfComponents; i++)
    {_MEDComponentsUnits[i] = m._MEDComponentsUnits[i];}

  _iterationNumber = m._iterationNumber;
  _time = m._time;
  _orderNumber = m._orderNumber;
}

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
//persist.h
//Lightweight persistence in c++

#ifndef PERSIST_H
#define PERSIST_H

#include <typeinfo> //Se coloca la primera y se compila con -frtti
#include <set>
#include <string>
#include <iostream>

class Cargador
  {
  public:
    Cargador(void) {}
    inline virtual ~Cargador(void) {}
    virtual void *Load(void *ref)= 0;
    virtual void Save(const std::string &cn,const void *ref)= 0;
  };

class ClassRecord
  {
    std::string class_name; //El nombre de la clase del objeto.
    int size; //El tamagno de un objeto de esa clase en bytes.
    void *(*newfunc)(void); //La que devuelve un nuevo objeto del mismo tipo.
    Cargador *crg; //El objeto que lee y escribe desde y a memoria secundaria.
    const std::type_info& (*tirfunc)(void);
  public:
    ClassRecord(void) {}
    ClassRecord( const std::string &c_name,int sz,
                 void *(*newf)(void),
                 Cargador *carg,
                 const std::type_info &(*tirf)(void)
               )
      {
        class_name= c_name;
        size= sz;
        newfunc= newf;
        crg= carg;
        tirfunc= tirf;
      }
    ClassRecord(const std::string &c_name)
      {
        class_name= c_name;
        size= 0;
        newfunc= NULL;
        crg= NULL;
        tirfunc= NULL;
      }
    ClassRecord(const ClassRecord &otro)
      {
        class_name= otro.class_name;
        size= otro.size;
        newfunc= otro.newfunc;
        crg= otro.crg;
        tirfunc= otro.tirfunc;
      }
    int GetObjSize(void) const
      { return size; }
    void *NewPtr(void) const
      { return newfunc(); }
    const std::type_info& TypeId(void) const
      { return tirfunc(); }
    void *Read(void *ref) const
      {
        crg->Load(ref);
        return ref;
      }
    void Write(const void *ref) const
      { crg->Save(class_name,ref); }
    friend std::ostream &operator<<(std::ostream &os,const ClassRecord &cr)
      { return os << cr.class_name << ',' << cr.size << ',' << cr.newfunc; }
    friend int operator<(const ClassRecord &a,const ClassRecord &b)
      { return (a.class_name < b.class_name); }
    friend int operator==(const ClassRecord &a,const ClassRecord &b)
      {
        if((const std::string &)a.class_name == (const std::string &)b.class_name) return 1;
        return 0;
      }
  };

class ClassInfo
  {
    typedef std::set<ClassRecord,std::less<ClassRecord> > set_or;
    static set_or pst_classes; //Clases persistentes (como yo ;-) ).
  public:
    ClassInfo(void) {}
    static void add_class_info( const std::string &class_name,int size,
                                void *(*newfunc)(void),
                                Cargador *crg,
                                const std::type_info& (*tirfunc)(void)
                              )
      { pst_classes.insert(ClassRecord(class_name,size,newfunc,crg,tirfunc)); }
    static void add_class_info(const ClassRecord &cr)
      { pst_classes.insert(cr); }
    static const ClassRecord *GetClassRecord(const std::string &class_name)
      {
        set_or::const_iterator cr_iter= pst_classes.find(ClassRecord(class_name));
        if(cr_iter != pst_classes.end())
          return &(*(cr_iter));
        else
          return NULL;
      }
    static const ClassRecord *GetClassRecord(const std::type_info &tir)
      {
        for(set_or::const_iterator i=pst_classes.begin();i!=pst_classes.end();i++)
          if((*i).TypeId() == tir) return &(*i);
        return NULL;
      }
    friend std::ostream &operator<<(std::ostream &os,const ClassInfo &)
      {
        for(set_or::const_iterator i=pst_classes.begin();i!=pst_classes.end();i++)
          os << *i << std::endl;
        return os;
      }
  };

template<class Type>
class MetaObj : public Type
  {
  public:
    MetaObj(void *self): Type(*(Type *) self) {}
    static void *newobj(void)
      { return new Type(); }
    static const std::type_info &tir(void)
      { return typeid(Type); }
  };

const ClassInfo &class_info(void);

template<class T,class INP,class OUT>
class CargaT: public Cargador
  {
  protected:
    INP *inp;
    OUT *out;
  public:
    CargaT(INP *input,OUT *output)
      {
        inp= input;
        out= output;
      }
    virtual void *Load(void *ref)
      {
        if(ref)
          {
            T *tmp= static_cast<T *>(ref);
            if(inp) (*inp) >> (*tmp);
          }
        return ref;
      }
    virtual void Save(const std::string &cn,const void *ref)
      {
        if(ref)
          {
            T *tmp= static_cast<T *>(ref);
            if(out) (*out) << '"' << cn << '"' << " " << (*tmp);
          }
      }
  };

#define METAOBJNAME(name) name ## _metaobj

#define CLASSRECORD(name,crg) \
  ClassRecord(#name,sizeof(name),MetaObj<name>::newobj,crg,MetaObj<name>::tir)

#define DEFINE_PST_CLASS(name,crg) \
  MetaObj<name> METAOBJNAME(name)(); \
  ClassInfo::add_class_info(CLASSRECORD(name,crg));

#endif

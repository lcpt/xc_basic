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
//Lexico.cc

#include "Lexico.h"

//Operadores binarios.
OpSuma Lexico::suma;
OpResta Lexico::resta;
OpProd Lexico::prodt;
OpDiv Lexico::divn;
OpDivEnt Lexico::dive;
OpMod Lexico::modulo;
OpPot Lexico::pot;
OpMenor Lexico::menor;
OpMenorIgual Lexico::menor_igual;
OpIgual Lexico::igual;
OpMayor Lexico::mayor;
OpMayorIgual Lexico::mayor_igual;
OpDistinto Lexico::distinto;
OpAnd Lexico::And;
OpOr Lexico::Or;
OpNot Lexico::Not;
OpMax Lexico::max;
OpMin Lexico::min;
OpMacaulay0 Lexico::F0;
OpMacaulayN Lexico::F1= OpMacaulayN(1);
OpMacaulayN Lexico::F2= OpMacaulayN(2);
OpMacaulayN Lexico::F3= OpMacaulayN(3);
OpMacaulayN Lexico::F4= OpMacaulayN(4);

//Operadores unarios.
OpNeg Lexico::neg;
OpInv Lexico::inv;
OpSign Lexico::sign;
OpAbs Lexico::abs;
OpSqr Lexico::sqr;
OpSqrt Lexico::raiz2;
OpExp Lexico::expn;
OpLn Lexico::ln;
OpLg Lexico::lg;
OpPi Lexico::pi;
OpSin Lexico::seno;
OpCos Lexico::cose;
OpTan Lexico::tang;
OpAtan Lexico::arctg;
OpSec Lexico::sec;
OpCosec Lexico::cosec;
OpCotg Lexico::cotg;
OpDeg2Rad Lexico::deg2rad;
OpRad2Deg Lexico::rad2deg;
OpRound Lexico::round;
OpTrunc Lexico::trunc;
OpToNum Lexico::to_num;

Lexico::Lexico(void)
  {}

//! @brief Devuelve un puntero al operador unario cuyo símbolo se pasa
//! como parámetro.
Segnal *Lexico::CalcDirOpUnario(const std::string &nmb)
  {
    if(nmb == neg.GetNombre()) return &neg;
    if(nmb == inv.GetNombre()) return &inv;
    if(nmb == abs.GetNombre()) return &abs;
    if(nmb == sqr.GetNombre()) return &sqr;
    if(nmb == raiz2.GetNombre()) return &raiz2;
    if(nmb == expn.GetNombre()) return &expn;
    if(nmb == ln.GetNombre()) return &ln;
    if(nmb == lg.GetNombre()) return &lg;
    if(nmb == pi.GetNombre()) return &pi;
    if(nmb == seno.GetNombre()) return &seno;
    if(nmb == cose.GetNombre()) return &cose;
    if(nmb == tang.GetNombre()) return &tang;
    if(nmb == sec.GetNombre()) return &sec;
    if(nmb == cosec.GetNombre()) return &cosec;
    if(nmb == cotg.GetNombre()) return &cotg;
    if(nmb == deg2rad.GetNombre()) return &deg2rad;
    if(nmb == rad2deg.GetNombre()) return &rad2deg;
    if(nmb == arctg.GetNombre()) return &arctg;
    if(nmb == round.GetNombre()) return &round;
    if(nmb == trunc.GetNombre()) return &trunc;
    if(nmb == to_num.GetNombre()) return &to_num;
    return NULL;
  }

//! @brief Devuelve un puntero al operador binario cuyo símbolo se pasa
//! como parámetro.
Segnal *Lexico::CalcDirOpBinario(const std::string &nmb)
  {
    if(nmb == suma.GetNombre()) return &suma;
    if(nmb == resta.GetNombre()) return &resta;
    if(nmb == prodt.GetNombre()) return &prodt;
    if(nmb == divn.GetNombre()) return &divn;
    if(nmb == dive.GetNombre()) return &dive;
    if(nmb == modulo.GetNombre()) return &modulo;
    if(nmb == pot.GetNombre()) return &pot;
    if(nmb == menor.GetNombre()) return &menor;
    if(nmb == menor_igual.GetNombre()) return &menor_igual;
    if(nmb == igual.GetNombre()) return &igual;
    if(nmb == mayor.GetNombre()) return &mayor;
    if(nmb == mayor_igual.GetNombre()) return &mayor_igual;
    if(nmb == distinto.GetNombre()) return &distinto;
    if(nmb == And.GetNombre()) return &And;
    if(nmb == Or.GetNombre()) return &Or;
    if(nmb == Not.GetNombre()) return &Not;
    if(nmb == max.GetNombre()) return &max;
    if(nmb == min.GetNombre()) return &min;
    if(nmb == F0.GetNombre()) return &F0;
    if(nmb == F1.GetNombre()) return &F1;
    if(nmb == F2.GetNombre()) return &F2;
    if(nmb == F3.GetNombre()) return &F3;
    if(nmb == F4.GetNombre()) return &F4;
    return NULL;
  }

//! @brief Devuelve un puntero a la variable cuyo nombre se pasa
//! como parámetro.
Variable *Lexico::CalcDirVar(const std::string &nmb)
  { return vars.CalcDir(nmb); }


//! @brief Devuelve un puntero al operador cuyo símbolo se pasa
//! como parámetro.
Segnal *Lexico::CalcDir(const std::string &nmb)
  {
    //Parentesis
    if(nmb == parenti.GetNombre()) return &parenti;
    if(nmb == parentd.GetNombre()) return &parentd;

    //Operadores binarios.
    Segnal *retval= CalcDirOpBinario(nmb);
    if(!retval) retval= CalcDirOpUnario(nmb);
    if(!retval) retval= CalcDirVar(nmb);
    return retval;
  }

void Lexico::PrintPrioridades(std::ostream &os) const
  {
    os << "Prioridad de los paréntesis:" << std::endl;
    os << parentd.GetNombre() << ": " << parentd.GetPrioridad() << std::endl;
    os << parenti.GetNombre() << ": " << parenti.GetPrioridad() << std::endl;

    os << "Prioridad de operadores binarios." << std::endl;
    os << suma.GetNombre() << ": " << suma.GetPrioridad() << std::endl;
    os << resta.GetNombre() << ": " << resta.GetPrioridad() << std::endl;
    os << prodt.GetNombre() << ": " << prodt.GetPrioridad() << std::endl;
    os << divn.GetNombre() << ": " << divn.GetPrioridad() << std::endl;
    os << dive.GetNombre() << ": " << dive.GetPrioridad() << std::endl;
    os << modulo.GetNombre() << ": " << modulo.GetPrioridad() << std::endl;
    os << pot.GetNombre() << ": " << pot.GetPrioridad() << std::endl;
    os << menor.GetNombre() << ": " << menor.GetPrioridad() << std::endl;
    os << menor_igual.GetNombre() << ": " << menor_igual.GetPrioridad() << std::endl;
    os << igual.GetNombre() << ": " << igual.GetPrioridad() << std::endl;
    os << mayor.GetNombre() << ": " << mayor.GetPrioridad() << std::endl;
    os << mayor_igual.GetNombre() << ": " << mayor_igual.GetPrioridad() << std::endl;
    os << distinto.GetNombre() << ": " << distinto.GetPrioridad() << std::endl;
    os << And.GetNombre() << ": " << And.GetPrioridad() << std::endl;
    os << Or.GetNombre() << ": " << Or.GetPrioridad() << std::endl;
    os << Not.GetNombre() << ": " << Not.GetPrioridad() << std::endl;
    os << max.GetNombre() << ": " << max.GetPrioridad() << std::endl;
    os << min.GetNombre() << ": " << min.GetPrioridad() << std::endl;
    os << F0.GetNombre() << ": " << F0.GetPrioridad() << std::endl;
    os << F1.GetNombre() << ": " << F1.GetPrioridad() << std::endl;
    os << F2.GetNombre() << ": " << F2.GetPrioridad() << std::endl;
    os << F3.GetNombre() << ": " << F3.GetPrioridad() << std::endl;
    os << F4.GetNombre() << ": " << F4.GetPrioridad() << std::endl;

    os << "Prioridad de operadores unarios." << std::endl;
    os << neg.GetNombre() << ": " << neg.GetPrioridad() << std::endl;
    os << inv.GetNombre() << ": " << inv.GetPrioridad() << std::endl;
    os << sign.GetNombre() << ": " << sign.GetPrioridad() << std::endl;
    os << abs.GetNombre() << ": " << abs.GetPrioridad() << std::endl;
    os << sqr.GetNombre() << ": " << sqr.GetPrioridad() << std::endl;
    os << raiz2.GetNombre() << ": " << raiz2.GetPrioridad() << std::endl;
    os << expn.GetNombre() << ": " << expn.GetPrioridad() << std::endl;
    os << ln.GetNombre() << ": " << ln.GetPrioridad() << std::endl;
    os << lg.GetNombre() << ": " << lg.GetPrioridad() << std::endl;
    os << pi.GetNombre() << ": " << pi.GetPrioridad() << std::endl;
    os << seno.GetNombre() << ": " << seno.GetPrioridad() << std::endl;
    os << cose.GetNombre() << ": " << cose.GetPrioridad() << std::endl;
    os << tang.GetNombre() << ": " << tang.GetPrioridad() << std::endl;
    os << arctg.GetNombre() << ": " << arctg.GetPrioridad() << std::endl;
    os << sec.GetNombre() << ": " << sec.GetPrioridad() << std::endl;
    os << cosec.GetNombre() << ": " << cosec.GetPrioridad() << std::endl;
    os << cotg.GetNombre() << ": " << cotg.GetPrioridad() << std::endl;
    os << deg2rad.GetNombre() << ": " << deg2rad.GetPrioridad() << std::endl;
    os << rad2deg.GetNombre() << ": " << rad2deg.GetPrioridad() << std::endl;
    os << round.GetNombre() << ": " << round.GetPrioridad() << std::endl;
    os << trunc.GetNombre() << ": " << trunc.GetPrioridad() << std::endl;
    os << to_num.GetNombre() << ": " << to_num.GetPrioridad() << std::endl;

  }

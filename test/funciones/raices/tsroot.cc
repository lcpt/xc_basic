#include <cstdio>
#include <cstdlib>
#include <cmath>
#include <iostream>
#include "matemat/funciones/src/raices/root.h"
#include "matemat/matrices/src/m_func.h"

double f1(double x, double* params);
double f1_1stDeriv(double x, double* params);
double f1_2ndDeriv(double x, double* params);
double fX(double, double, double*);
double fY(double, double, double*);

double ff1(double*);
double ff2(double*);
double fff1(const m_double &d);
double fff2(const m_double &d);

void pressAnyKey(void);

main()
{
  const double SMALL = 1.e-10;
  double low, high, root;
  double tolerance = 1.0e-8;
  double params[10];
  double roots [10];
  double paramsX[5];
  double paramsY[5];
  int numRoots;
  int maxIter = 50;
  double rootX, rootY;
  int i;

  polyRoot polyRoots[20];
  double coeff[10];
  int polyOrder;

  /*
  low = 2;
  high = 4;
  params[0] = 3;
  std::cout << "**** Bisection method ****" << std::endl;
  std::cout << "Solving exp(x) - " << params[0] << " * x * x = 0" << std::endl;
  std::cout << "Guess interval is [" << low << ',' << high << ']' << std::endl;;
  if (Bisection(low, high, tolerance, &root, params, f1))
    std::cout << "Root = " << root << std::endl << std::endl;
  else
    std::cout << "Failed to obtain root" << std::endl << std::endl;

  std::cout << "**** Newton's method (Approx. Derivative) ****" << std::endl;
  params[0] = 3;
  root = 3;
  std::cout << "Solving exp(x) - " << params[0] << " * x * x = 0" << std::endl;
  std::cout << "Initial guess is " << root << std::endl;
  if (NewtonApprox(&root, tolerance, maxIter, params, f1))
    std::cout << "Raíz= " << root << std::endl << std::endl;
  else
    std::cout << "Failed to obtain root" << std::endl << std::endl;

  printf("**** Newton's method (Supplied Derivative) ****\n");
  params[0] = 3;
  root = 3;
  printf("Solving exp(x) - %lg * x * x = 0\n", params[0]);
  printf("Initial guess is %lg\n", root);
  if (NewtonExact(&root, tolerance, maxIter, params,
      f1, f1_1stDeriv))
    printf("Root = %lg\n\n", root);
  else
    printf("Failed to obtain root\n\n");

  pressAnyKey();

  printf("**** Richmond's method (Approx. Derivative) ****\n");
  params[0] = 3;
  root = 3;
  printf("Solving exp(x) - %lg * x * x = 0\n", params[0]);
  printf("Initial guess is %lg\n", root);
  if (RichmondApprox(&root, tolerance, maxIter, params, f1))
    printf("Root = %lg\n\n", root);
  else
    printf("Failed to obtain root\n\n");

  printf("**** Richmond's method (Supplied Derivative) ****\n");
  params[0] = 3;
  root = 3;
  printf("Solving exp(x) - %lg * x * x = 0\n", params[0]);
  printf("Initial guess is %lg\n", root);
  if (RichmondExact(&root, tolerance, maxIter, params,
      f1, f1_1stDeriv, f1_2ndDeriv))
    printf("Root = %lg\n\n", root);
  else
    printf("Failed to obtain root\n\n");

  low = 2;
  high = 4;
  params[0] = 3;
  printf("**** Combined method ****\n");
  printf("Solving exp(x) - %lg * x * x = 0\n", params[0]);
  printf("Guess interval is [%lg,%lg]\n", low, high);
  if (Combined(low, high, tolerance, &root, maxIter, params, f1))
    printf("Root = %lg\n\n", root);
  else
    printf("Failed to obtain root\n\n");

  low = 2;
  high = 4;
  params[0] = 3;
  printf("**** Brent's method ****\n");
  printf("Solving exp(x) - %lg * x * x = 0\n", params[0]);
  printf("Guess interval is [%lg,%lg]\n", low, high);
  if (Brent(low, high, tolerance, &root, maxIter, params, f1))
    printf("Root = %lg\n\n", root);
  else
    printf("Failed to obtain root\n\n");

  pressAnyKey();

  printf("**** Newton's method (Multiple Roots) ****\n");
  params[0] = 3;
  roots[0] = 2;
  printf("Solving exp(x) - %lg * x * x = 0\n", params[0]);
  printf("Initial guess is %lg\n", roots[0]);
  if (NewtonMultiRoots(roots, &numRoots, 10, tolerance,
      maxIter, params, f1)) {
    for (i = 0; i < numRoots; i++)
      printf("Root # %d = %lg\n", i + 1, roots[i]);
    putchar('\n');
  }
  else
    printf("Failed to obtain root\n\n");

  pressAnyKey();

  printf("**** Deflation method (Polynomial Roots) ****\n");
  polyOrder = 3;
  coeff[0] = -40;
  coeff[1] = 62;
  coeff[2] = -23;
  coeff[3] = 1;
  for (i = polyOrder; i >= 0; i--) {
    if (i > 1 && fabs(coeff[i]) > SMALL)
      printf("%lg * X^%d + ", coeff[i], i);
    else if (i == 1  && fabs(coeff[i]) > SMALL)
      printf("%lg * X + ", coeff[i]);
    else if (i == 0 && fabs(coeff[i]) > SMALL)
      printf("%lg", coeff[i]);
  }
  printf(" = 0\n");
  if (DeflatePolyRoots(coeff, 1.1, roots, &numRoots,
      polyOrder, maxIter, tolerance)) {
    for (i = 0; i < numRoots; i++)
      printf("Root # %d = %lg\n", i + 1, *(roots + i));
  }
  else
    printf("Failed to obtain root\n\n");
  puts("\n");

  printf("**** Lin-Bairstow method (Polynomial Roots) ****\n");
  polyOrder = 3;
  coeff[0] = -40;
  coeff[1] = 62;
  coeff[2] = -23;
  coeff[3] = 1;
  for (i = polyOrder; i >= 0; i--) {
    if (i > 1 && fabs(coeff[i]) > SMALL)
      printf("%lg * X^%d + ", coeff[i], i);
    else if (i == 1  && fabs(coeff[i]) > SMALL)
      printf("%lg * X + ", coeff[i]);
    else if (i == 0 && fabs(coeff[i]) > SMALL)
      printf("%lg", coeff[i]);
  }
  printf(" = 0\n");

  if (LBPolyRoots(coeff, polyRoots, polyOrder, tolerance))
    for (i = 0; i < polyOrder; i++) {
      if (polyRoots[i].isComplex)
        printf("Root # %d = (%lg) +i (%lg)\n", i + 1,
               polyRoots[i].real, polyRoots[i].imag);
      else
        printf("Root # %d = %lg\n", i + 1, polyRoots[i].real);
    }
  else
    printf("Error using the Lin-Bairstow method\n");
  puts("\n");

  pressAnyKey();

  paramsX[0] = -1;
  paramsY[0] = 0.5;
  rootX = 1;
  rootY = 3;
  printf("**** Newton's method for 2 Equations ****\n");
  printf("Solving x * x + y * y + %lg = 0\n", paramsX[0]);
  printf("Solving x * x - y * y + %lg = 0\n", paramsY[0]);
  printf("Initial guess for X is %lg\n", rootX);
  printf("Initial guess for Y is %lg\n", rootY);
  if (Newton2Functions(&rootX, &rootY, tolerance,
      maxIter, paramsX, paramsY,
      fX, fY)) {
    printf("Root X = %lg\n", rootX);
    printf("Root Y = %lg\n\n", rootY);
  }
  else
    printf("Failed to obtain roots\n\n");

  pressAnyKey();
  */
  
  printf("***** Newton's method for multiple equations*****\n");
  size_t numEqns = 2;
  m_func fff(numEqns,1);
  fff(1)= fff1;
  fff(2)= fff2;

  m_double Xguess(numEqns,1,0.0);
  Xguess(1)= -0.16;
  Xguess(2)= 2.7;

  printf("Solving for:\n");
  printf("f1(X0, X1) = X1 * exp(X0) - 2\n");
  printf("f2(X0, X1) = X0^2 + X1 - 4\n");
  std::cout << "Initial guess for X= " << Xguess << std::endl;
  NewtonSimNLE(Xguess, tolerance, 30, fff);
  std::cout << "Las raíces son: " << Xguess << std::endl;
  std::cout << "Comprobamos: " << fff.Eval(Xguess) << std::endl;

  pressAnyKey();

  return 0;
}

double f1(double x, double* params)
  { return exp(x) - params[0] * x * x; }

double f1_1stDeriv(double x, double* params)
  { return exp(x) - 2 * params[0] * x; }

double f1_2ndDeriv(double x, double* params)
  { return exp(x) - 2 * params[0]; }

double fX(double x, double y, double* p)
  { return x * x + y * y + p[0]; }

double fY(double x, double y, double* p)
  { return x * x - y * y  + p[0]; }

double ff1(double* X)
  { return X[1] * exp(X[0]) - 2; }

double ff2(double* X)
  { return X[0] * X[0] + X[1] - 4; }

double fff1(const m_double &d)
  { return d(2) * exp(d(1)) - 2; }

double fff2(const m_double &d)
  { return d(1) * d(1) + d(2) - 4; }

void pressAnyKey()
{
  printf("\nPress any key to continue...");
  getchar();
  puts("\n\n");
}

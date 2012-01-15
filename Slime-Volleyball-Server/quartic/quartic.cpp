#include <iostream>
#include "quartic.hpp"

using namespace magnet::math;

int
main(int , char *[])
{
  //The quarticSolve routine solves quartics of the form 
  //x^4 + a x^3 + b x^2 + c x + d ==0
  
  //As an example we will calculate a quartic with 4 roots
  double realroots[4] = {1.0, 1.0, 23.0, 1000.0 };

  //We can calculate the coefficients of the quartic from the roots
  double a = -realroots[0] - realroots[1] - realroots[2] - realroots[3],
    b = realroots[0] * realroots[1] + realroots[0] * realroots[2]
    + realroots[0] * realroots[3] + realroots[1] * realroots[2]
    + realroots[1] * realroots[3] + realroots[2] * realroots[3],
    c = -realroots[0] * realroots[1] * realroots[2]
    - realroots[0] * realroots[1] * realroots[3]
    - realroots[0] * realroots[2] * realroots[3]
    - realroots[1] * realroots[2] * realroots[3],
    d = realroots[0] * realroots[1] * realroots[2]
    * realroots[3];

  //Now we test the implementation

  a = -5.93295878967653;
  b = 7.86666666666667;
  c = 2.76871410184905;
  d = 16;

  double roots[4];
  size_t rootcount = quarticSolve(a, b, c, d,
				  roots[0], roots[1], 
				  roots[2], roots[3]);
  
  
  std::cout << "Original roots = ";
  for (size_t i(0); i < 4; ++i)
    std::cout << realroots[i] << ", ";

  std::cout << "\nRoot finder found " << rootcount << " roots, ";

  for (size_t i(0); i < rootcount; ++i)
    std::cout << "\n Root " << i << " = " << roots[i];

  std::cout << "\nFinished" << std::endl;
}

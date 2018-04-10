#include <iostream>
#include "xc_basic/src/text/pcl.h"
#include "xc_basic/src/text/hpgl.h"

int main(void)
  {
    PCLReset(std::cout);
    PCLLetter(std::cout);
    PCLPortrait(std::cout);
    PCLFrameSzXDP(std::cout,3600);
    PCLFrameSzYDP(std::cout,3600);
    PCLCurPosX(std::cout,450);
    PCLCurPosY(std::cout,675);
    PCLModoHPGL1(std::cout);
    HPGLInitialize(std::cout);
    HPGLSelectPen(std::cout,1);
    HPGLSetUserScaling(std::cout,0,100,0,100);
    HPGLPenDown(std::cout,100,0);
    HPGLPenDown(std::cout,100,100);
    HPGLPenDown(std::cout,0,100);
    HPGLPenDown(std::cout,0,0);
    HPGLPenUp(std::cout,50,50);
    HPGLCircle(std::cout,25);
    HPGLPCLMode1(std::cout);
    PCLReset(std::cout);
    return 0;
  }

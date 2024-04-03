#include "Testbench.h"
/// for buffer
#include <iostream>
const int SIZE = 25;
double buffer[SIZE];

#include <cmath>
#include "SobelFilter.h"

SobelFilter::SobelFilter(sc_module_name n) : sc_module(n) {  //定義一個module，名稱為:n  sc_module是定義module的語法
  SC_THREAD(do_filter);
  sensitive << i_clk.pos();   ///Generate even when i_clk at positive trigger
  dont_initialize();
  reset_signal_is(i_rst, false);
}
/*
// sobel mask
const int mask[MASK_N][MASK_X][MASK_Y] = {{{-1, -2, -1}, {0, 0, 0}, {1, 2, 1}},

                                          {{-1, 0, 1}, {-2, 0, 2}, {-1, 0, 1}}};
*/
// Gaussian blur
const double mask[MASK_N][MASK_X][MASK_Y] = {{{1, 4, 7, 4, 1},
                                              {4, 16, 26, 16, 4},
                                              {7, 26, 41, 26, 7},
                                              {4, 16, 26, 16, 4},
                                              {1, 4, 7, 4, 1}}};

void SobelFilter::do_filter() {
unsigned char r = 0, g = 0, b = 0;
int x = 0, y = 0;
//int buf_ind = 0;
double grey = 0;
  while (true) {
    for (unsigned int i = 0; i < MASK_N; ++i) {
      val[i] = 0;
    }
    for (unsigned int v = 0; v < MASK_Y; ++v) {
      for (unsigned int u = 0; u < MASK_X; ++u) {   ///[u,v] mask
        
        if(x == 0 || u == MASK_X-1 ) {
          r = i_r.read();
          g = i_g.read();
          b = i_b.read();
          grey = (r + g + b) / 3;  //initial or 新值放入buffer
        }
        else if(x == 0 && y == 0) { ///initial

          r = 0;
          g = 0;
          b = 0;
          grey = (r + g + b) / 3;
        }
        if (x != 0 && y != 0){   // 滿了
        buffer[u+v*5]=buffer[u+v*5+1];       // (x,y)換後應該先重排
        }
        
        
        if (x == 0 && y == 0) buffer[u+5*v] = grey;  /// 0~24  一開始全放
        else if (u != MASK_X - 1 && x != 0) {
          grey = buffer[u+5*v];   ///從buffer給值
        }
        else if (u == MASK_X - 1){

          buffer[u+5*v] = grey;   // 將新值放入buffer
        }

        /*
        unsigned char grey = (i_r.read() + i_g.read() + i_b.read()) / 3;
        */
       
        for (unsigned int i = 0; i != MASK_N; ++i) {
          val[i] += grey * (mask[i][u][v]) / 273;
        }
      }
    }
    // counter for pixels 256 * 256
    x++;
    if(x == 256) {  // if the pixels is 512 need to change to 512;
      y++;
      x = 0;
      }
    double total = 0;
    for (unsigned int i = 0; i != MASK_N; ++i) {
      total += val[i] * val[i];
    }
    int result = (int)(std::sqrt(total)); ///將值轉為int
    o_result.write(result);
    wait(10); //emulate module delay
  }
}


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
unsigned char r, g, b;

  while (true) {
    for (unsigned int i = 0; i < MASK_N; ++i) {
      val[i] = 0;
    }
    for (unsigned int v = 0; v < MASK_Y; ++v) {
      for (unsigned int u = 0; u < MASK_X; ++u) {
        r = i_r.read();
        g = i_g.read();
        b = i_b.read();

        double grey = (r + g + b) / 3;
        /*
        unsigned char grey = (i_r.read() + i_g.read() + i_b.read()) / 3;
        */
       
        for (unsigned int i = 0; i != MASK_N; ++i) {
          val[i] += grey * (mask[i][u][v]) / 273;
        }
      }
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

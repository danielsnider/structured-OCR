#ifndef __MYIMAGE_H_INCLUDED__
#define __MYIMAGE_H_INCLUDED__

#include "opencv2/core/core.hpp"
#include <vector>

using namespace cv;

class MyImage {
  public:
  // Variables:
  char* img_name;
  Mat img;
  vector<Mat> img_pieces;

  // Functions:
  MyImage(char* img_name_full);
  void Print();
  void ShowPieces();
  
  private:
};

#endif // __MYIMAGE_H_INCLUDED__ 
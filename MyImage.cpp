#include "opencv2/highgui/highgui.hpp"
#include "opencv2/core/core.hpp"
#include "opencv2/imgproc/imgproc.hpp"

#include "MyFuncs.h"
#include "MyImage.h"

#include <iostream>
#include <vector>
#include <string>

using namespace cv;
using namespace std;

MyImage::MyImage(char* img_name_full){
  //set MyImage vars
  img_name = img_name_full;

  //load image
  img = imread(img_name, 1); 

  if (img.empty()){
      cout << "\nCannot open image!" << endl;
      exit(6);
  }
}

void MyImage::Print(){
  cout << "\nMyImage::Print():\n";
  cout << "img_name: " << img_name << endl;
  cout << "img.empty(): " << img.empty() << endl;
  cout << "img.rows: " << img.rows << endl;
  cout << "img.cols: " << img.cols << endl;
  cout << "img.depth(): " << img.depth() << endl;
  cout << "img.channels(): " << img.channels() << endl;
  cout << "img_pieces().size(): " << img_pieces.size() << endl;
  cout << "img.at<uchar>(0,0): " << img.at<uchar>(0,0) << endl;
  cout << "img(Range(0,2),Range(0,2)):\n" << img(Range(0,2),Range(0,2)) << endl;
}

void MyImage::ShowPieces(){
  
  cout << "\n About to display pieces for img: \n ";
  Print();

  for (int x = 0; x < img_pieces.size(); x++)
  {
    imshow("image piece", img_pieces[x]);
    waitKey();
    }
  destroyWindow("image piece");
}


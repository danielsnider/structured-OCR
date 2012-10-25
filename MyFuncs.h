#ifndef __FUNCS_H_INCLUDED__
#define __FUNCS_H_INCLUDED__

#include "opencv2/core/core.hpp"
#include <vector>

using namespace cv;

Mat PreProc(Mat I);
void mouseHandler(int event, int x, int y, int flags, void *param);
Mat UserDrawnSelection(Mat img);
void UserDrawnSelectionHelper(Mat I);
vector<Mat> SeperateTermDef(Mat img);
vector<Mat> FindTextLines(Vector<Mat> img_pieces);
void OCRandSave(Vector<Mat> img_pieces);
std::string TesseractLine(Mat img);
void cleanUp();


#endif // __FUNCS_H_INCLUDED__ 
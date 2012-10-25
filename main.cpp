#include <iostream>

#include "MyImage.h"
#include "MyFuncs.h"

using std::cout;
using std::system;

void QuizEye(char* arg)
{
  // Load image
  MyImage my_im(arg);
  // Allow user to crop image
  my_im.img = UserDrawnSelection(my_im.img);
  // split terms column from definitions column
  my_im.img_pieces = SeperateTermDef(my_im.img);
  // Split lines of text into pieces
  my_im.img_pieces = FindTextLines(my_im.img_pieces);
  // Display pieces
  my_im.ShowPieces();
  // OCR pieces and save systematically       
  OCRandSave(my_im.img_pieces);
  // Wait
  //system("sleep 9999");
  // Clean up
  cleanUp();
}


int main(int argc, char** argv)
{
  cout << "Usage: $0 [image to process]\n\n";

  QuizEye(argv[1]);
}

#include "opencv2/highgui/highgui.hpp"
#include "opencv2/core/core.hpp"
#include "opencv2/imgproc/imgproc.hpp"
#include <vector>
#include <iostream>
#include <string>
#include <fstream>
#include "MyFuncs.h"

using namespace std;
using namespace cv;

//global variables used to pass information between functions for img cropping
Mat IMG;
std::vector<int> RECTANGLE;
int WINX=800;
int WINY=600;
//Change variables 'thres' and 'consthres' in functions below to alter the tolerance of vision algorithms

Mat PreProc(Mat I){
//Binarize images and remove noise to make processing images easier

  // accept only char (ie. 8-bit) typ matrices, and are not continous
  CV_Assert(I.depth() == CV_8U);
 
  //change to black and white
  cvtColor(I, I, CV_RGB2GRAY);
  adaptiveThreshold(I, I,1000,1,0,81,14);

  //dilate to remove stray black pixels
  int size = 2; 
  Mat element = getStructuringElement( MORPH_ELLIPSE, 
      Size( size + 1 , size + 1 ), 
      Point( size, size ) );
  dilate( I, I, element );

  //erode
  erode( I, I, element ); //enlarge black surface area

  imshow("Preprocessed", I);
  waitKey();

  return I;
}

void UserDrawnSelectionHelper(Mat I){
//Create window and set callback for user cropping

  /* create new window and register mouse handler */
  namedWindow("Please crop", CV_WINDOW_NORMAL);
  resizeWindow("Please crop", WINX, WINY);
  setMouseCallback( "Please crop", mouseHandler, NULL);
 
  /* display the image */
  imshow("Please crop", I);
  waitKey();
  destroyWindow("Please crop");
}

void mouseHandler(int event, int x, int y, int flags, void* param) {
// Capture where mouse clicks occure for cropping. 
//Left click is the top left crop coords. Right click is the bottom right crop coords.

  switch(event) {
      /* left button down */
      case CV_EVENT_LBUTTONDOWN:
          cout <<"Left button down (" << x << "," << y << ")\n";
          RECTANGLE[0] = x;
          RECTANGLE[1] = y;
          break;
     
      /* right button down */
      case CV_EVENT_RBUTTONDOWN:
          cout <<"Right button down (" << x << "," << y << ")\n";
          RECTANGLE[2] = x;
          RECTANGLE[3] = y;
          break;
  }
}

Mat UserDrawnSelection(Mat img){
//User assisted image crop

  int x, y, width, height;

  // variable init
  RECTANGLE.push_back(0); //top left x
  RECTANGLE.push_back(0); //top left y
  RECTANGLE.push_back(0); //bottom right x
  RECTANGLE.push_back(0); //bottom right y

  //display img, ask user to perform click-based crop, set RECTANGLE vector with crop coords
  UserDrawnSelectionHelper(img); 

  x = RECTANGLE[0];
  y = RECTANGLE[1];
  width = RECTANGLE[2] - RECTANGLE[0];
  height = RECTANGLE[3] - RECTANGLE[1];

  if (x == 0 && y == 0 && width == 0 && height == 0) {
    cout << "No selection made...\n";
  }
  else {
    Mat I (img, Rect(x, y, width, height));
    img = I;
    // imshow("cropped", img);
    // waitKey();
    // destroyWindow("cropped");
  }
  return img;
}

vector<Mat> SeperateTermDef(Mat I){
//Seperate left column of text (the terms) in an image from the right column of text (the definitions)

  int channels = I.channels();
  int nRows = I.rows * channels; // channels should always be 1 in this function but this is still here for reusability
  int nCols = I.cols;
  uchar* p; // will store row of pixels
  vector<Mat> img_pieces;

  int j; // for loops
  int i = 0;
  int count = 0;
  int cons = 0;
  int thres = nRows / 100; //amount of ink in a column(1%) 
  int consmax = (nCols / 100)*2;
  int middle;

  // Stage 1: find leading whitespace left of text stop when black is found
  do
  {
    count = 0;
    for ( j = 0; j < nRows; ++j) {
      p = I.ptr<uchar>(j);
      if (p[i] == 255){ // if pixel is white
        count++;
      }
    }
    cons = (count != nRows ? ++cons : 0); //if a largely white col is found, count that. else restart count
    i++;
  } while (cons < consmax-5 && i < nCols);
  
  count = 0;
  cons = 0;

  // Stage 2: scanning rightward, continue until white is found
  while (cons < consmax && i < nCols) /// looking for consecutive cols of mostely white
  {
    count = 0;
    for ( j = 0; j < nRows; ++j) {
      p = I.ptr<uchar>(j);
      if (p[i] == 255){
        count++;
      }
    }
    cons = (count == nRows ? ++cons : 0); //if a largely white col is found, count that. else restart count
    i++;
  }

  // Stage 3: scanning rightward, find black
  do
  {
    count = 0;
    for ( j = 0; j < nRows; ++j) {
      p = I.ptr<uchar>(j);
      if (p[i] == 255){
        count++;
      }
    }
    i++;
  } while (count > (nRows/100)*96 && i < nCols); //stop once a column that is only 96% white is found

  if (i > (nCols/100)*96){ // the dividing whitespace doesn't divide, it spans to the end of the img
    imshow("ERROR! img piece doesn't look right", I);
    waitKey();
    destroyWindow("ERROR! img piece doesn't look right");
    exit(99);
  }
  else { // split the image in half
    do
    {
      count = 0;
      for ( j = 0; j < nRows; ++j) {
        p = I.ptr<uchar>(j);
        if (p[i] == 255){
          count++;
        }
      }
      i--;
    } while (count != nRows);

    middle = i - 5;

    Mat term(I, Rect(0, 0, middle, I.rows));
    Mat def(I, Rect(middle, 0, I.cols - middle, I.rows)); 
    img_pieces.push_back(term);
    img_pieces.push_back(def);
  }
  return img_pieces;
}

vector<Mat> FindTextLines(Vector<Mat> img_pieces) {
//Seperate lines of text in an image into multiple images

  vector<Mat> temp_img_pieces;

  //Scan the pixels in each row of pixels,.if enough black pixels are found in enough
  // consecutive rows, create start and end values and create an img_piece subimage. 
  for (int y = 0; y < img_pieces.size(); ++y)
  {
    //pre processing MUST be done before now. Currently done with PreProc() called from SeperateTermDef()
    Mat I = img_pieces[y];
    
    int channels = I.channels();
    int nRows = I.rows * channels;
    int nCols = I.cols;
    uchar* line;
    vector<int> lines;
    vector<Range> lines_of_text;
    Mat temp;

    int i,j; // for loops
    int len;
    int avglen = 0;
    int count = 0;
    int cons = 0;
    int consthres = 15;
    int thres = 2; //amount of ink in a line 
    int line_loc, upper_loc, lower_loc;

    // Step 1: Find vertical position of lines of text
    for( i = 0; i < nRows; ++i) //loop for rows
    {
      count = 0;
      line = I.ptr<uchar>(i);
      for ( j = 0; j < nCols; ++j) //scan all pixels in row
      {
        if (line[j] == 0){
          count++; //count black pixels
        }
      }
      if (count > thres){ // if enough black pixels are found
        cons++;
      }
      else {
        if (cons > consthres){ //if enough rows in a row are found to have signficant black, add the point to a list
          lines.push_back(i);
        }
        cons = 0;
      }
    }

    // Step 2: Compute lines_of_text ranges for each 'line' found by searching above and below lines of text for where the text ends
    for (int x = 0; x < lines.size(); x++){
      upper_loc = -1;
      line_loc = lines[x];

      // Step 2.1: Set upper text area bound
      while (upper_loc == -1){
        count = 0;
        line_loc--;
        if (line_loc < 0){
          imshow("ERROR! upper_loc went out of bounds", I);
          waitKey();
          destroyWindow("ERROR! upper_loc went out of bounds");
          exit(99);
        }
        line = I.ptr<uchar>(line_loc);

        for ( j = 0; j < nCols; ++j) //scan all pixels in row
        {
          if (line[j] == 0){ //if pixel == black
            count++;
          }
        }
        if (count == 0) { //if a row of pixels are all white 
          upper_loc = line_loc - 1; 
        }
      }
      
      // Step 2.2: Set lower text area bound
      line_loc = lines[x];
      lower_loc = -1;

      while (lower_loc == -1){
        count = 0;
        line_loc++;
        if (line_loc >= I.rows) { 
          imshow("ERROR! lower_loc went out of bounds", I);
          waitKey();
          destroyWindow("ERROR! lower_loc went out of bounds");
          exit(99);
        }
        line = I.ptr<uchar>(line_loc);

        for ( j = 0; j < nCols; ++j) //scan all pixels in row
        {
          if (line[j] == 0){ //if pixel == black
            count++;
          }
        }
        if (count == 0) { //if a row of pixels are all white 
          lower_loc = line_loc + 1;
        }
      }
      lines_of_text.push_back(Range(upper_loc, lower_loc));
    }

    // Step 3: Compute average length of each line of text
    for (int x = 0; x < lines_of_text.size(); x++)
    {
      avglen += (lines_of_text[x].end - lines_of_text[x].start);
    }
    avglen = avglen / lines_of_text.size();

    // Step 4: Add each line of text image to a vector if it's length is not abnormally short
    for (int x = 0; x < lines_of_text.size(); x++)
    {
      len = lines_of_text[x].end - lines_of_text[x].start;
      if (len > avglen / 2){ // will fail on unintened imgs possibly bc avg l is squewed? check centetns of ranges of lines_of_text
        temp = I(lines_of_text[x], Range::all());
        temp_img_pieces.push_back(temp);
      }
    }
  }
  return temp_img_pieces;
}

void OCRandSave(Vector<Mat> img_pieces) {
//Perform OCR on all given images

  std::vector<string> lines;
  ofstream out;

  // Step 1: execute tesseract OCR on 'img_pieces', store text in 'lines'
  for (int i = 0; i < img_pieces.size(); ++i)
  {
    lines.push_back(TesseractLine(img_pieces[i]));
  }

  // Step 2: print and save OCR'ed text
  out.open("output.csv");
  for (int i = 0; i < img_pieces.size()/2; ++i)
  {
    cout << lines[i] << " <---> " << lines[i + img_pieces.size()/2] << endl;
    out << lines[i] << "," << lines[i + img_pieces.size()/2] << endl;
  }

  out.close();

  //   Note about definition structure:
  //-In a given image, terms are assumed to all be in a left column, their definitons in a right column.
  //-Terms and definitions are stored in img_pieces in the following order: 
  // 1 5
  // 2 6
  // 3 7
  // 4 8
  //-Structure is determined by matching the first and the half+1 position of the terms and definitions vector called img_pieces
}

std::string TesseractLine(Mat img) {
//Tesseract OCR one image at a time and return string result

  char cmd[] = "tesseract -l eng temp.jpg temp"; // Tesseract system command
  std::string line = "";

  
  imwrite("temp.jpg", img); 
  
  if (system(cmd) == 0) {
    ifstream in("temp.txt");
    getline(in, line);
    in.close();


  } 
  else {
    cout << "Error: tesseract command failed\n";
  }

  return line;
}

void cleanUp() {
  destroyWindow("Preprocessed");
  system("rm temp.*");
}
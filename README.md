Grid Layout Structured Text Reader
===========================

Images containing text with a two column grid structure will have it's text recognized using Tesseract OCR and it's grid structure preserved. Text and structure is saved to a comma delimited file, `output.csv`. 

![image](https://dl.dropbox.com/u/345086/project%20desc.png)

Install
-------
Linux:

    chmod +x ./build.sh
    ./build.sh

Usage
-----

    ./a.out [image file]

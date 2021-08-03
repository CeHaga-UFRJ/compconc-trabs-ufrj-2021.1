#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include "lib/timer.h"

#define STB_IMAGE_IMPLEMENTATION
#include "lib/stb_image.h"

#define STB_IMAGE_WRITE_IMPLEMENTATION
#include "lib/stb_image_write.h"

unsigned char *RGBToGray(unsigned char *img, int width, int height, int numColors){
  unsigned char *out;
  out = (unsigned char *)malloc(sizeof(unsigned char) * width * height);
  for(int i = 0; i < height; i++){
    for(int j = 0; j < width; j++){
      unsigned char red = img[i*numColors*width + j*numColors];
      unsigned char green = img[i*numColors*width + j*numColors + 1];
      unsigned char blue = img[i*numColors*width + j*numColors + 2];
      unsigned char gray = (unsigned char)(red * 30.0/100.0 + green * 59.0/100.0 + blue * 11.0/100.0);
      out[i*width + j] = gray;
    }
  }
  return out;
}

int main(int argc, char *argv[]){
  unsigned char *input;
  unsigned char *output;
  char *inputFileName;
  char *outputFileName;
  int width;
  int height;

  inputFileName = argv[1];
  outputFileName = argv[2];

  input = stbi_load(inputFileName, &width, &height, NULL, 3);
  if(!input){
    printf("Deu merda\n");
    exit(1);
  }

  output = RGBToGray(input, width, height, 3);

  if(stbi_write_jpg(outputFileName, width, height, 1, output, 90) == 0){
    printf("Deu merda outra vez\n");
    exit(2);
  }

  printf("Foi\n");

  return 0;
}
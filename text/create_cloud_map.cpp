#include <iostream>
#include <cstdlib>
#include <cstdio>
#include <queue>
#include <stack>
#include <cstring>
#include <cmath>
#include <cfloat>
#include <map>
#include <fstream>
#define NUMBER_OF_INPUT 1900
#define NUMBER_OF_TESTING 263
#define INPUT_MAX 65536
#define INPUT_SIZE 32

int smooth = 1;

using namespace std;
map<string, double> mapclass[8]; 
map<string, double>::iterator it;
map<string, double>::iterator it2;
int total_words_in_class[8];
int unique_words_in_class[8];
double prior[8];
double confusion_matrix[8][8];
double testing_label_count[8];

struct Pair{
  double value;
  string word;
};
class sort_by_value{
  public:
    bool operator()(Pair& p1, Pair& p2)
    {
      if(p1.value < p2.value)
        return true;
      else
        return false;
    }
};
void get_training(char fileName[]){
  int i,j;
  char *pch;
  char current_input[INPUT_MAX];
  FILE *fp = fopen(fileName, "r");
  int row = 0;
  int current_class;
  char current_word[1024];
  int current_count;
  int waitNumber = 0;
  ofstream myfile[8];
  myfile[0].open ("training0.txt");
  myfile[1].open ("training1.txt");
  myfile[2].open ("training2.txt");
  myfile[3].open ("training3.txt");
  myfile[4].open ("training4.txt");
  myfile[5].open ("training5.txt");
  myfile[6].open ("training6.txt");
  myfile[7].open ("training7.txt");
  
  for(i = 0; i < NUMBER_OF_INPUT; i++){
    fgets(current_input, INPUT_MAX, fp);
    pch = strtok(current_input, " ");
    current_class = atoi(pch);
    pch = strtok(NULL, ":");
    while(pch[0] !=10){
      strcpy(current_word, pch);
      pch = strtok(NULL, " ");
      current_count = atoi(pch);
      for(j = 0; j < current_count; j++){
        myfile[current_class] << current_word  << " ";
      }
      pch = strtok(NULL, ":");
    }
  }

  myfile[0].close();
  myfile[1].close();
  myfile[2].close();
  myfile[3].close();
  myfile[4].close();
  myfile[5].close();
  myfile[6].close();
  myfile[7].close();
}
int main(int argc, char *argv[]){
  int i, j, k, l;
  double sum = 0.0;
  if(argc == 2){
    get_training(argv[1]);
  }
} 


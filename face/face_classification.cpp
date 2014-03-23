///////////////////////////////////////////////////////////////////////////////
////////                                                                           //
//////// Program Name : face_classification.cpp                                    //
//////// Course CRN   : CS440 - Artificial Intelligence                            //
//////// Instructor   : Prof. Svetlana Lazebnik                                    // 
//////// Author       : Bo-Yu, Chiang                                              //
//////// NetID / UIN  : bchiang3 / 677629729                                       //
//////// Email        : jyt0532@gmail.com                                          //
//////// Affiliation  : CS Dept., University of Illinois at Urbana-Champaign       //
//////// Latest update: 12/13/2013                                                 //
////////                                                                           //
//////// Copyright (c) 2013, Bo-Yu, Chiang                                         //
////////                                                                           //
//////// This program is free software: you can redistribute it and/or modify      //
//////// it under the terms of the GNU General Public License as published by      //
//////// the Free Software Foundation, either version 3 of the License, or         //
//////// (at your option) any later version.                                       //
////////                                                                           //
//////// This program is distributed in the hope that it will be useful,           //
//////// but WITHOUT ANY WARRANTY; without even the implied warranty of            //
//////// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the             //
//////// GNU General Public License for more details.                              //
////////                                                                           //
//////// You should have received a copy of the GNU General Public License         //
//////// along with this program.  If not, see <http://www.gnu.org/licenses/>.     //
////////                                                                           //
////////                                                                           //
////////                                                                           //
/////////////////////////////////////////////////////////////////////////////////////

#include <iostream>
#include <cstdlib>
#include <cstdio>
#include <queue>
#include <stack>
#include <cstring>
#include <cmath>
#include <cfloat>

#define NUMBER_OF_INPUT 451
#define NUMBER_OF_TESTING 150
#define MAX_INPUT 1024
#define INPUT_SIZE 80
char training[NUMBER_OF_INPUT][INPUT_SIZE][INPUT_SIZE];
char testing[NUMBER_OF_INPUT][INPUT_SIZE][INPUT_SIZE];
int training_label[NUMBER_OF_INPUT];
int predicted_testing_label[NUMBER_OF_TESTING];
int testing_label[NUMBER_OF_TESTING];
double confusion_matrix[2][2];
int prototypical[2];
int testing_label_count[2];
double likelihood[10][INPUT_SIZE][INPUT_SIZE][2];
double prior[2];
int number_of_class[2];
int smooth = 1;
double odds[INPUT_SIZE][INPUT_SIZE][2][2];

using namespace std;

void evaluate_testing(){
  int i, j, k, l;
  char *pch;
  int attr_num;
  double y[2];
  int row = 0;
  int correct_count = 0;
  double max;
  double prototypical_max[2];
  for(i = 0; i < NUMBER_OF_TESTING; i++){
    max = -DBL_MAX;
    for(l = 0; l < 2; l++){
      y[l] = log(prior[l]);
      //y[l] = 0.0;
      for(j = 0; j < 70; j++){
        for(k = 0; k < 70; k++){
          if(testing[i][j][k] == '#'){
            y[l] += log(likelihood[l][j][k][1]);
          }else{
            y[l] += log(likelihood[l][j][k][0]);
          }
        }
      }
    }
    if(i ==61){
      printf("%lf %lf\n", y[0], y[1]);
    }
    for(l = 0; l < 2; l++){
      if(max < y[l]){
        max = y[l];
        predicted_testing_label[i] = l;
      }
    }
  }
}
void get_training(char fileName[]){
  int i,j;
  FILE *fp = fopen(fileName, "r");
  int row = 0;
  while(fgets(training[row/70][row%70], INPUT_SIZE, fp)!=NULL){
    row++;
  }
}
void get_training_label(char fileName[]){
  int i,j;
  FILE *fp = fopen(fileName, "r");
  int row = 0;
  for(i = 0; i < 451; i++){
    fscanf(fp, "%d", &training_label[i]);
  }
}
void get_testing(char fileName[]){
  int i,j;
  FILE *fp = fopen(fileName, "r");
  int row = 0;
  while(fgets(testing[row/70][row%70], INPUT_SIZE, fp)!=NULL){
    row++;
  }
}
void get_testing_label(char fileName[]){
  int i,j;
  FILE *fp = fopen(fileName, "r");
  int row = 0;
  for(i = 0; i < 150; i++){
    fscanf(fp, "%d", &testing_label[i]);
  }
}
void compute_prior(){
  int i, j;
  double sum = 0.0;
  for(i = 0; i < NUMBER_OF_INPUT; i++){
    number_of_class[training_label[i]]++;
  }
  for(i = 0; i < 10; i++){
    prior[i] = (double)number_of_class[i]/(double)NUMBER_OF_INPUT;
    //printf("prior[%d] = %lf\n", i, prior[i]);
  }
}
void compute_likelihood(){
  int i, j, k, l;
  for(i = 0; i < NUMBER_OF_INPUT; i++){
    for(j = 0; j < 70; j++){
      for(k = 0; k < 70; k++){
        if(training[i][j][k] == '#'){
          likelihood[ training_label[i] ][j][k][1]++;
        }else{ 
          likelihood[ training_label[i] ][j][k][0]++;
        }
      }
    }
  }
  for(i = 0; i < 2; i++){
    for(j = 0; j < 70; j++){
      for(k = 0; k < 70; k++){
        for(l = 0; l < 2; l++){
          likelihood[ i ][j][k][l] += smooth;
          likelihood[ i ][j][k][l] /= (number_of_class[ i ] +2*smooth );
        }
      }
    }
  }
}
void compute_accuracy(){
  int i, j, k;
  int count= 0;
  for(i = 0; i < NUMBER_OF_TESTING; i++){
    confusion_matrix[testing_label[i]][predicted_testing_label[i]]++;
    testing_label_count[testing_label[i]]++;
    if(predicted_testing_label[i] == testing_label[i]){
      count++;
    }else{
      printf("%dth testing sample, Correct %d, Misclassified to %d\n", i, testing_label[i], predicted_testing_label[i]);
      for(j = 0; j < 70; j++){
        for( k = 0; k < 70; k++){
          printf("%c", testing[i][j][k]);
        }
    //    printf("\n");
      }      
    }
  }
  for(i = 0; i < 2; i++){
    for(j = 0; j < 2; j++){
      confusion_matrix[i][j] /= testing_label_count[i];
    }
    printf("Classification rate for %d = %lf\n", i, confusion_matrix[i][i]);
  }
  //printf("%d\n", count);
  printf("Accuracy: %lf\n", (double)count/(double)NUMBER_OF_TESTING);
}
void print_confusion_matrix(){
  int i,j;
  for(i = 0; i < 2; i++){
    for(j = 0 ; j < 2; j++){
      printf("%3.2lf  ", confusion_matrix[i][j]);
    }
    printf("\n");
  }
}
void copy_confusion_matrix(double copied_confusion_matrix[10][10]){
  int i, j;
  for(i = 0 ; i < 10; i++){
    for(j = 0; j < 10; j++){
      copied_confusion_matrix[i][j] = confusion_matrix[i][j];
    }
  }
}
void print_log_likelihood(int num){
  int i, j;
  for(i = 0; i < 28; i++){
      for(j = 0; j < 28; j++){
        if(log(likelihood[num][i][j][1]) > -0.5){
          printf("#");
        }else if(log(likelihood[num][i][j][1]) > -1){
          printf("+");
        }else if(log(likelihood[num][i][j][1]) > -2){
          printf("-");
        }else{ 
          printf(" ");
        }
      }
      printf("\n");
    }
}
void compute_and_print_odds(int class1, int class2){
  int i, j, k, l;
  for(i = 0; i < 70; i++){
    for(j = 0; j < 70; j++){
      odds[i][j][class1][class2] = likelihood[class1][i][j][1]/likelihood[class2][i][j][1];
    }
  }
  for(i = 0; i < 70; i++){
    for(j = 0; j < 70; j++){
      if(log(odds[i][j][class1][class2]) > 1){
        printf("#");
      }else if(log(odds[i][j][class1][class2]) > 0){
        printf("+");
      }else if(log(odds[i][j][class1][class2]) > -1){
        printf("-");
      }else{ 
        printf(" ");
      }
    }
    printf("\n");
  }

}
int main(int argc, char *argv[]){
  int i, j, k, l;
  double sum = 0.0;
  if(argc == 5){
    get_training(argv[1]);
    get_training_label(argv[2]);
    get_testing(argv[3]);
    get_testing_label(argv[4]);
    compute_prior();
    compute_likelihood();
    evaluate_testing();
    compute_accuracy();
    print_confusion_matrix();
    compute_and_print_odds(0, 1);
  }
} 


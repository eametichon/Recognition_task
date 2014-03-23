#include <iostream>
#include <cstdlib>
#include <cstdio>
#include <queue>
#include <stack>
#include <cstring>
#include <cmath>
#include <cfloat>

#define NUMBER_OF_INPUT 5000
#define NUMBER_OF_TESTING 1000
#define MAX_INPUT 1024
#define INPUT_SIZE 32
char training[NUMBER_OF_INPUT][INPUT_SIZE][INPUT_SIZE];
char testing[NUMBER_OF_INPUT][INPUT_SIZE][INPUT_SIZE];
int training_label[NUMBER_OF_INPUT];
int predicted_testing_label[NUMBER_OF_TESTING];
int testing_label[NUMBER_OF_TESTING];
double confusion_matrix[10][10];
int prototypical[10];
int testing_label_count[10];
double likelihood[10][INPUT_SIZE][INPUT_SIZE][2];
double prior[10];
int number_of_class[10];
int smooth = 1;
double odds[INPUT_SIZE][INPUT_SIZE][10][10];

double theta[8][64][2];
double pi[2];
int label0 = 0;
int label1 = 0;

using namespace std;

void evaluate_testing(){
  int i, j, k, l;
  char *pch;
  int attr_num;
  double y[10];
  int row = 0;
  int correct_count = 0;
  double max;
  double prototypical_max[10];
  for(i = 0; i < 10; i++){
    prototypical_max[i] = -DBL_MAX;
    prototypical[i] = -1;
  }
  for(i = 0; i < NUMBER_OF_TESTING; i++){
    max = -DBL_MAX;
    for(l = 0; l < 10; l++){
      //y[l] = log(prior[l]);
      y[l] = 0.0;
      for(j = 0; j < 28; j++){
        for(k = 0; k < 28; k++){
          if(testing[i][j][k] == '#' || testing[i][j][k] == '+'){
            y[l] += log(likelihood[l][j][k][1]);
          }else{
            y[l] += log(likelihood[l][j][k][0]);
          }
        }
      }
    }
    for(l = 0; l < 10; l++){
      if(max < y[l]){
        max = y[l];
        predicted_testing_label[i] = l;
      }
    }
    if(max > prototypical_max[predicted_testing_label[i]]){
      prototypical_max[predicted_testing_label[i]] = max;
      prototypical[predicted_testing_label[i]] = i;
    }
  }
}
void get_training(char fileName[]){
  int i,j;
  FILE *fp = fopen(fileName, "r");
  int row = 0;
  while(fgets(training[row/28][row%28], INPUT_SIZE, fp)!=NULL){
    row++;
  }
}
void get_training_label(char fileName[]){
  int i,j;
  FILE *fp = fopen(fileName, "r");
  int row = 0;
  for(i = 0; i < 5000; i++){
    fscanf(fp, "%d", &training_label[i]);
  }
}
void get_testing(char fileName[]){
  int i,j;
  FILE *fp = fopen(fileName, "r");
  int row = 0;
  while(fgets(testing[row/28][row%28], INPUT_SIZE, fp)!=NULL){
    row++;
  }
}
void get_testing_label(char fileName[]){
  int i,j;
  FILE *fp = fopen(fileName, "r");
  int row = 0;
  for(i = 0; i < 1000; i++){
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
void compute_likelihood(int smooth){
  int i, j, k, l;
  for(i = 0; i < 10; i++){
    for(j = 0; j < 28; j++){
      for(k = 0; k < 28; k++){
        likelihood[i][j][k][0] = 0.0;
        likelihood[i][j][k][1] = 0.0;
      }
    }
  }
  for(i = 0; i < NUMBER_OF_INPUT; i++){
    for(j = 0; j < 28; j++){
      for(k = 0; k < 28; k++){
        if(training[i][j][k] == '#' || training[i][j][k] == '+'){
          likelihood[ training_label[i] ][j][k][1]++;
        }else{ 
          likelihood[ training_label[i] ][j][k][0]++;
        }
      }
    }
  }
  for(i = 0; i < 10; i++){
    for(j = 0; j < 28; j++){
      for(k = 0; k < 28; k++){
        for(l = 0; l < 2; l++){
          likelihood[ i ][j][k][l] += smooth;
          likelihood[ i ][j][k][l] /= (number_of_class[ i ] +2*smooth );
        }
      }
    }
  }
}
double compute_accuracy(){
  int i, j;
  int count= 0;
  for(i = 0; i < NUMBER_OF_TESTING; i++){
    confusion_matrix[testing_label[i]][predicted_testing_label[i]]++;
    testing_label_count[testing_label[i]]++;
    if(predicted_testing_label[i] == testing_label[i]){
      count++;
    }
  }
  for(i = 0; i < 10; i++){
    for(j = 0; j < 10; j++){
      confusion_matrix[i][j] /= testing_label_count[i];
    }
    //printf("Classification rate for %d = %lf\n", i, confusion_matrix[i][i]);
  }
  return (double)count/(double)NUMBER_OF_TESTING;
  //printf("%d\n", count);
  //printf("Accuracy: %lf\n", (double)count/(double)NUMBER_OF_TESTING);
}
void print_confusion_matrix(){
  int i,j;
  for(i = 0; i < 10; i++){
    for(j = 0 ; j < 10; j++){
      printf("%3.2lf  ", confusion_matrix[i][j]);
    }
    printf("\n");
  }
}
void print_prototypical(){
  int i,j, k;
  for(i = 0; i < 10; i++){
    printf("Most prototypical instance of %d:\n", i);
    printf("====%d====\n", prototypical[i]);
    for(j = 0; j < 28; j++){
      for(k = 0 ; k < 28; k++){
        printf("%c", testing[prototypical[i]][j][k]);
      }
      printf("\n");
    }
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
void select_most_confuse_pair(double copied_confusion_matrix[10][10], int &confuse_from, int &confuse_to){
  int i, j;
  double max = 0;
  for(i = 0 ; i < 10; i++){
    for(j = 0; j < 10; j++){
      if(i == j){
        continue;
      }
      if(copied_confusion_matrix[i][j] > max){
        max = copied_confusion_matrix[i][j];
        confuse_from = i;
        confuse_to = j;
      }
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
  for(i = 0; i < 28; i++){
    for(j = 0; j < 28; j++){
      odds[i][j][class1][class2] = likelihood[class1][i][j][1]/likelihood[class2][i][j][1];
    }
  }
  for(i = 0; i < 28; i++){
    for(j = 0; j < 28; j++){
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
void compute_four_pair_of_odds_ratio(){
  int i, j;
  double copied_confusion_matrix[10][10]; 
  copy_confusion_matrix(copied_confusion_matrix);
  int confuse_from; 
  int confuse_to;
  int times;
  for(times = 0; times < 4; times++){
    select_most_confuse_pair(copied_confusion_matrix, confuse_from, confuse_to);
    printf("Pair %d: %d and %d\n", times + 1, confuse_from, confuse_to);
    printf("log likelihood for %d:\n", confuse_from);
    print_log_likelihood(confuse_from);
    printf("log likelihood for %d:\n", confuse_to);
    print_log_likelihood(confuse_to);
    compute_and_print_odds(confuse_from, confuse_to);
    
    printf("%d %d\n", confuse_from, confuse_to);
    copied_confusion_matrix[confuse_from][confuse_to] = 0.0;
  }
  /*for(i = 0; i<10;i++){
    for(j =0; j<10;j++){
      printf("%lf ", copied_confusion_matrix[i][j]);
    }
    printf("\n");
  }*/

}
int main(int argc, char *argv[]){
  int i, j, k, l;
  double max = 0.0;
  double current_accuracy;
  int max_smooth = 0;
  if(argc == 5){
    get_training(argv[1]);
    get_training_label(argv[2]);
    get_testing(argv[3]);
    get_testing_label(argv[4]);
    compute_prior();
    for(i = 0; i < 50; i++){
      compute_likelihood(i);
      evaluate_testing();
      current_accuracy = compute_accuracy();

      printf("k = %d   Accuracy = %lf\n", i, current_accuracy);
      if(current_accuracy > max){
        max_smooth = i;
        max = current_accuracy;
      }
    }
    printf("argMax_k = %d   Accuracy = %lf\n", max_smooth, max);
  //  dfs(maze, Row, Column, start_x, start_y, end_x, end_y);
/*    for(i= 0; i < 10; i++){
      printf("%lf\n", prior[i]);
      sum += prior[i];
    }*/
    //for(i = 0; i<1000; i++)
    //printf("%d\n", testing_label[i]);
    /*
    for(i = 0; i < 10; i++){
      for(j = 0; j < 28; j++){
        for(k = 0; k < 28; k++){
          for(l = 0; l < 2; l++){
            if(likelihood[i][j][k][l] > 1)
            {
              printf("wrong\n");
            }
          }
        }
      }
    }*/
  }
} 


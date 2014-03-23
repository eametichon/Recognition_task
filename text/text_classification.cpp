///////////////////////////////////////////////////////////////////////////////
////////                                                                           //
//////// Program Name : text_classification.cpp                                    //
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
#include <map>

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
void evaluate_testing(char fileName[]){
  int i, j, k, l;
  char *pch;
  int attr_num;
  char current_input[INPUT_MAX];
  FILE *fp = fopen(fileName, "r");
  double y[8];
  int correct_count = 0;
  int correct_class;
  char current_word[1024];
  int current_count;
  double max;
  int predicted_class;
  for(i = 0; i < NUMBER_OF_TESTING; i++){
    for(j = 0; j < 8; j++){
      //y[j] = 0.0;
      y[j] = log(prior[j]);
    }
    fgets(current_input, INPUT_MAX, fp);
    pch = strtok(current_input, " ");
    correct_class = atoi(pch);
    pch = strtok(NULL, ":");
    while(pch[0] !=10){
      strcpy(current_word, pch);
      pch = strtok(NULL, " ");
      current_count = atoi(pch);
      for(j = 0; j < 8; j++){
        it = mapclass[j].find(current_word);
        if(it == mapclass[j].end()){
          y[j] += 
            log(1.0/(double)(total_words_in_class[j] + unique_words_in_class[j]))*current_count;
        }else{
          y[j] += log(it->second)*current_count; 
        }
      }
      pch = strtok(NULL, ":");
    }
    max = y[0];
    predicted_class = 0;
    for(j = 1; j < 8; j++){
      if(max < y[j]){
        max = y[j];
        predicted_class = j;
      }
    }

    confusion_matrix[correct_class][predicted_class]++;
    testing_label_count[correct_class]++;
    if( predicted_class == correct_class){
      correct_count++;    
    }
  }
  for(i = 0; i < 8; i++){
    for(j = 0; j < 8; j++){
      confusion_matrix[i][j] /= testing_label_count[i];
    }
    printf("Classification rate for class %d = %lf\n", i, confusion_matrix[i][i]);
  }
  printf("Accuracy = %lf\n", (double)correct_count/(double)NUMBER_OF_TESTING);
  
  for(i = 0; i < 8; i++){
    for(j = 0; j < 8; j++){
      printf("%3.2lf ", confusion_matrix[i][j]);
    }
    printf("\n");
  }


}
void compute_prior(){
  int i;
  int total_words = 0;
  for(i = 0; i < 8; i++){
    total_words += total_words_in_class[i];
  }
  for(i = 0; i < 8; i++){
    prior[i] = (double)total_words_in_class[i]/(double)total_words;
  }
}
void compute_likelihood(){
  int i,j;
  for(i = 0; i < 8; i++){
    for(it = mapclass[i].begin(); it != mapclass[i].end(); it++){
      it -> second = 
        (double)(it -> second + 1)/(double)(total_words_in_class[i] + unique_words_in_class[i]);
    }
  }
}
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
  for(i = 0; i < NUMBER_OF_INPUT; i++){
    fgets(current_input, INPUT_MAX, fp);
    pch = strtok(current_input, " ");
    current_class = atoi(pch);
    pch = strtok(NULL, ":");
    while(pch[0] !=10){
      strcpy(current_word, pch);
      pch = strtok(NULL, " ");
      current_count = atoi(pch);
      total_words_in_class[current_class] += current_count;
      it = mapclass[current_class].find(current_word);
      if(it == mapclass[current_class].end()){
        mapclass[current_class][current_word] = current_count;
        unique_words_in_class[current_class] += 1;
      }else{
        it -> second += current_count;
      }
      pch = strtok(NULL, ":");
    }
    //it = mapclass[3].find("back");
    //printf("%d\n", it->second);
    //printf("end\n\n");
  }
}
void print_top20_words_with_highest_likelihood(){
  int i, j;
  Pair current_pair;
  for(i = 0; i < 8; i++){
    priority_queue<Pair, vector<Pair>, sort_by_value> pq;
    for(it = mapclass[i].begin() ; it != mapclass[i].end() ; it++){
      Pair tmp = {it->second, it->first};
      pq.push(tmp);
    }
    printf("Class %d\n", i);
    for(j = 0; j < 20; j++){
      current_pair = pq.top();
      pq.pop();
      printf("%dth word: %s    %lf\n",j+1, current_pair.word.c_str(), current_pair.value);
    }
  }
}
void copy_confusion_matrix(double copied_confusion_matrix[8][8]){
  int i, j;
  for(i = 0 ; i < 8; i++){
    for(j = 0; j < 8; j++){
      copied_confusion_matrix[i][j] = confusion_matrix[i][j];
    }
  }
}
void select_most_confuse_pair(double copied_confusion_matrix[8][8], int &confuse_from, int &confuse_to){
  int i, j;
  double max = 0;
  for(i = 0 ; i < 8; i++){
    for(j = 0; j < 8; j++){
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
void compute_and_print_odds_reverse(int confuse_from, int confuse_to){
  int i, j;
  map<double, string> odd_map; 
  map<double, string>::iterator odd_it;
  printf("\n");
  printf("Odd ratio for class %d over class %d:\n", confuse_from, confuse_to);

  for(it = mapclass[confuse_from].begin() ; it != mapclass[confuse_from].end() ; it++){
    it2 = mapclass[confuse_to].find(it -> first);                                         
    if(it2 == mapclass[confuse_to].end()){
      odd_map[it->second*(double)(total_words_in_class[j] + unique_words_in_class[j])] = it->first;
    }else{
      odd_map[it->second/it2->second] = it->first;
    }
  }
  odd_it = odd_map.end();
  odd_it--;
  for(j = 0; j < 20; j++){
    printf("%dth word: %s %lf\n", j+1, odd_it->second.c_str(), odd_it->first);
    odd_it--;
  }

}

void compute_four_pair_of_odds_ratio(){
  int i, j;
  double copied_confusion_matrix[8][8]; 
  copy_confusion_matrix(copied_confusion_matrix);
  int confuse_from; 
  int confuse_to;
  int times;
  for(times = 0; times < 4; times++){
    select_most_confuse_pair(copied_confusion_matrix, confuse_from, confuse_to);
    printf("Pair %d: %d and %d\n", times + 1, confuse_from, confuse_to);
    //reverse_mapclass();
    compute_and_print_odds_reverse(confuse_from, confuse_to);
    compute_and_print_odds_reverse(confuse_to, confuse_from);
    copied_confusion_matrix[confuse_from][confuse_to] = 0.0;
  }
}
int main(int argc, char *argv[]){
  int i, j, k, l;
  double sum = 0.0;
  if(argc == 3){
    get_training(argv[1]);
    compute_prior();
    compute_likelihood();
    evaluate_testing(argv[2]);
    print_top20_words_with_highest_likelihood();
    compute_four_pair_of_odds_ratio();
  }
} 


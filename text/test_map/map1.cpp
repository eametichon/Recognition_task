#include <iostream>
#include <map>
#include <string.h>

using namespace std;

int main()
{
  map<string, int> mapTest;
  map<string, int>::iterator it;
  mapTest["input1"] = 1;
  mapTest["input2"] = 221;
  mapTest["input3"] = 3;

  it=mapTest.find("input2");
  if(it ==mapTest.end()){
    printf("not found\n");
  }else{
    it->second++;
    //printf("%d\n", it->second);
  }
  it=mapTest.find("input2");
  if(it ==mapTest.end()){
    printf("not found\n");
  }else{
  //  it->second++;
    printf("%d\n", it->second);
  }

  /*
  if(mapTest.find("input1") == mapTest.end())
    mapTest["input1"] = 10;

  for(it = mapTest.begin() ; it != mapTest.end() ; it++)
    printf("%s : %s\n", it -> first, it->second);*/
//    cout<<it->first<<" "<<it->second<<endl;

  return 0;
}

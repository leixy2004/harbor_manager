//
// Created by xinya on 2024/3/23.
//
#include <fstream>
#include <iostream>
#include <cstdio>
int perform(double mid) {
  std::fstream ff("dg.txt");
  ff << mid << std::endl;
  ff.close();
  auto p= _popen(
      R"(D:\Downloads\WindowsReleasev1.1\PreliminaryJudge.exe -m D:\Downloads\WindowsReleasev1.1\maps\map1.txt D:\Code\Huawei\harbor_manager\cmake-build-release\harbor_manager.exe 2>&1"
)","r");
    int score;
  char str[111];
  char c;
  while (c=fgetc(p)) {
    fputc(c,stdout);
  }
//  while (fgets(str,111,p)!=nullptr) ;
//  fgets(str,111,p);
//  fscanf(p,"%s",str);
//    fscanf(p,R"({"status":"Successful","score":%d})",&score);
    fclose(p);
  printf("%s\n",str);
    return score=1;
}

int main() {
  double L = 0.1, R = 5;
  while (R - L > 1e-8) {
    double l = L + (R - L) / 3, r = R - (R - L) / 3;
    printf("%lf %lf\n", l, r);
    if (perform(l) < perform(r)) R = r;
    else L = l;
  }
  std::cout<<L<<std::endl;
}
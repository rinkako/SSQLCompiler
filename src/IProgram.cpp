#include "IBridge.h"

int main(int argc, char* argv[]) {
  IBridge* core = IBridge::Invoke();
  core->Init(argc, argv);
  core->StartDash();
  return 0;
}


/********************************************************
Author:      编译原理计应1班12小组
Date:        2014-11-22
Description: 程序入口
*********************************************************/
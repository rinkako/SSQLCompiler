#include "IBridge.h"

int main(int argc, char* argv[]) {
  IBridge* core = IBridge::Invoke();
  core->Init(argc, argv);
  core->StartDash();
  return 0;
}


/********************************************************
Author:      ����ԭ���Ӧ1��12С��
Date:        2014-11-22
Description: �������
*********************************************************/
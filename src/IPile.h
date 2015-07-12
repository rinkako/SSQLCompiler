#ifndef ___PILE_IMAKI
#define ___PILE_IMAKI
#include "ITypeContainer.h"
#include "IDatabase.h"

class IPile {
public:
  //函数作用： 构造函数
  //参数列表： N/A
  //返 回 值： N/A
  IPile();

  //函数作用： 复位函数
  //参数列表： N/A
  //返 回 值： N/A
  void Reset();

  //函数作用： 设置语法分析树
  //参数列表：
  //   _myNode 匹配树根节点
  //返 回 值： N/A
  void SetParseTree(SyntaxTreeNode*);

  //函数作用： 启动语义分析器
  //参数列表： N/A
  //返 回 值： IBridgeProxy 用于语法解释的代理容器
  IBridgeProxy Semanticer();

private:
  //函数作用： 语法树遍历
  //参数列表： 
  //   myproxy 语法解释的代理容器
  //    mynode 递归节点
  //      flag 标识符触发器类型：0-表，1-列，2-主列，3-列的初值
  //返 回 值： N/A
  void Mise(IBridgeProxy&, SyntaxTreeNode*, int);

  // 语法树
  SyntaxTreeNode* parseTree;
  // 当前序号
  int proxyID;
  // 最后操作列
  istr lastPi;
}; /* IPile */

#endif /* ___PILE_IMAKI */


/********************************************************
Author:      编译原理计应1班12小组
Date:        2014-11-30
Description: SSQL语义分析器
*********************************************************/
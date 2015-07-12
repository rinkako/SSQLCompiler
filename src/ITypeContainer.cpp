#include "ITypeContainer.h"

// Token构造器
Token::Token() {
  aLine = 0;
  aTag = 0;
  indexOfCode = 0;
  aColumn = 0;
  length = 0;
  errorCode = 0;
  errorInfo = "";
  detail = "";
}

// Token拷贝构造器
Token::Token(const Token &_other) {
  this->indexOfCode = _other.indexOfCode;
  this->errorCode = _other.errorCode;
  this->errorInfo = _other.errorInfo;
  this->aColumn = _other.aColumn;
  this->length = _other.length;
  this->detail = _other.detail;
  this->aLine = _other.aLine;
  this->aType = _other.aType;
  this->aTag = _other.aTag;
}

// TokenStream构造器
TokenStream::TokenStream() {
  this->Reset();
}

// TokenStream拷贝构造器
TokenStream::TokenStream(const TokenStream &_other) {
  this->_iPointer = _other._iPointer;
  for (int i = 0; i < _other.Length(); i++) {
    this->Add(_other._tokenContainer[i]);
  }
}

// TokenStream追加函数
void TokenStream::Add(Token* _org) {
  this->_tokenContainer.push_back(_org);
}

// TokenStream长度函数
int TokenStream::Length() const {
  return _tokenContainer.size();
}

// TokenStream复位函数
void TokenStream::Reset() {
  _iPointer = 0;
  _tokenContainer.clear();
}

// TokenStream输出到流
ios& operator<<(ios &os, const TokenStream &ts) {
  for (int i = 0; i < ts.Length(); i++) {
    os << "Token Series No. " << i << NEWLINE
      << " Type: " << TokenOutputTable[ts._tokenContainer[i]->aType] << NEWLINE
      << " At Row: " << ts._tokenContainer[i]->aLine
      << "  Col: " << ts._tokenContainer[i]->aColumn << NEWLINE
      << " Detail: " << ts._tokenContainer[i]->detail << NEWLINE
      << " ErrorBit: " << ts._tokenContainer[i]->errorCode
      << " (" << (ts._tokenContainer[i]->errorCode ? "Error" : "OK") << ")" << NEWLINE << NEWLINE;
  }
  return os;
}

// TokenStream输出到文本
istr TokenStream::ToString() {
  istr builder = "";
  for (TokenList::iterator iter = _tokenContainer.begin();
    iter != _tokenContainer.end(); iter++) {
    builder += (*iter)->aType != TokenType::token_startEnd ? (*iter)->detail + " " : "";
  }
  return builder += NEWLINE;
}

// TokenStream下一Token
Token* TokenStream::Next() {
  if (_iPointer > 0 && _iPointer < (int)this->Length()) {
    return _tokenContainer[_iPointer++];
  }
  else {
    return NULL;
  }
}

// TokenStream下一句子
TokenList TokenStream::NextSentence() {
  TokenList sentenceBuilder;
  _sPointer = _iPointer;
  // 循环直至下一句子
  while (true) {
    if (_sPointer >= (int)_tokenContainer.size()) {
      break;
    }
    if (_tokenContainer[_sPointer]->aType == TokenType::token_Semicolon_ || _sPointer == 0) {
      break;
    }
    _sPointer++;
  }
  if (_sPointer != 0) {
    _sPointer = _iPointer;
  }
  // 如果越界就结束
  if (_sPointer >= (int)_tokenContainer.size()) {
    return sentenceBuilder;
  }
  // 否则取句子
  while (_sPointer >= 0 && _sPointer < (int)this->Length()) {
    sentenceBuilder.push_back(_tokenContainer[_sPointer]);
    if (_tokenContainer[_sPointer++]->aType == TokenType::token_Semicolon_) {
      break;
    }
  }
  _iPointer = _sPointer;
  return sentenceBuilder;
}

// CandidateFunction的构造器
CandidateFunction::CandidateFunction() {
  candidateProcesser = NULL;
}

// CandidateFunction的拷贝构造器
CandidateFunction::CandidateFunction(const CandidateFunction& _other) {
  this->candidateProcesser = _other.candidateProcesser;
  this->candidateType = _other.candidateType;
}

// CandidateFunction带产生式的构造器
CandidateFunction::CandidateFunction(iHandle _proc, CFunctionType _pt) {
  this->SetProc(_proc, _pt);
}

// CandidateFunction设置产生式
void CandidateFunction::SetProc(iHandle _proc, CFunctionType _pt) {
  this->candidateProcesser = _proc;
  this->candidateType = _pt;
}

// CandidateFunction获得产生式向量
iHandle CandidateFunction::GetProc() {
  return this->candidateProcesser;
}

// CandidateFunction获得产生式类型
CFunctionType CandidateFunction::GetType() {
  return this->candidateType;
}

// CandidateFunction调用产生式
SyntaxTreeNode* CandidateFunction::Call(SyntaxTreeNode* _root, void* _parser, SyntaxType _syntax, istr _detail) {
  //return NULL;
  return this->candidateProcesser(_root, _parser, this->candidateType, _syntax, _detail);
}

// LL1SyntaxParserMap带尺寸的构造器
LL1SyntaxParserMap::LL1SyntaxParserMap(int row, int col) {
  this->iParserMap = new CandidateFunction**[row];
  for (iPtr i = 0; i < row; i++) {
    iParserMap[i] = new CandidateFunction*[col];
  }
  iLeftNodes.clear();
  iNextLeaves.clear();
  iRowCount = row;
  iColCount = col;
}

// LL1SyntaxParserMap设置行类型
void LL1SyntaxParserMap::SetRow(int row, SyntaxType left) {
  if (0 <= row && row < this->iRowCount) {
    iLeftNodes.insert(SyntaxPair(left, row));
  }
}

// LL1SyntaxParserMap设置列类型
void LL1SyntaxParserMap::SetCol(int col, TokenType leave) {
  if (0 <= col && col < this->iColCount) {
    iNextLeaves.insert(TokenPair(leave, col));
  }
}

// LL1SyntaxParserMap设置节点分析函数
void LL1SyntaxParserMap::SetCellular(int row, int col, CandidateFunction* proc) {
  if (0 <= row && row < this->iRowCount &&
    0 <= col && col < this->iColCount) {
    this->iParserMap[row][col] = proc;
  }
}

// LL1SyntaxParserMap设置给定产生式的函数
void LL1SyntaxParserMap::SetCellular(SyntaxType left, TokenType leave, CandidateFunction* proc) {
  this->SetCellular(this->iLeftNodes[left], this->iNextLeaves[leave], proc);
}

// LL1SyntaxParserMap取得节点的处理函数
CandidateFunction* LL1SyntaxParserMap::GetCFunction(int row, int col) {
  return this->iParserMap[row][col];
}

// LL1SyntaxParserMap取得产生式的处理函数
CandidateFunction* LL1SyntaxParserMap::GetCFunction(SyntaxType left, TokenType leave, iHandle nilserver) {
  if (left == SyntaxType::epsilonLeave) {
    return new CandidateFunction(nilserver, CFunctionType::umi_epsilon);
  }
  CandidateFunction* candidator = this->GetCFunction(this->iLeftNodes[left], this->iNextLeaves[leave]);
  return candidator == NULL ? new CandidateFunction(NULL, CFunctionType::umi_errorEnd) : candidator;
}

// SyntaxTreeNode构造器
SyntaxTreeNode::SyntaxTreeNode() {
  children.clear();
  parent = NULL;
  candidateFunction = NULL;
}

// SyntaxTreeNode拷贝构造器
SyntaxTreeNode::SyntaxTreeNode(const SyntaxTreeNode& _other) {
  this->candidateFunction = _other.candidateFunction;
  this->parent = _other.parent;
  this->errorCode = _other.errorCode;
  this->errorInfo = _other.errorInfo;
  this->mappedBegin = _other.mappedBegin;
  this->mappedLength = _other.mappedLength;
  this->mappedList = _other.mappedList;
  this->children = _other.children;
  this->nodeValue = _other.nodeValue;
  this->nodeName = _other.nodeName;
  this->nodeType = _other.nodeType;
  this->aTag = _other.aTag;
}

// SyntaxTreeNode输出函数
istr SyntaxTreeNode::ToString() {
  istr builder = "";
  int identation = 0;
  GetTree(builder, this, identation);
  return builder;
}

// SyntaxTreeNode递归遍历显示
void SyntaxTreeNode::GetTree(istr &builder, SyntaxTreeNode* myNode, int &identation)
{
  // 如果空就没必要继续了
  if (myNode == NULL) {
    return;
  }
  // 画树
  builder += DrawTree(myNode) + myNode->nodeName;
  if (myNode->nodeSyntaxType >= SyntaxType::Unknown
    && myNode->nodeSyntaxType != SyntaxType::epsilonLeave
    && myNode->nodeSyntaxType != SyntaxType::tail_startEndLeave) {
    builder += " (" + myNode->nodeValue + ")";
  }
  builder += NEWLINE;
  // 缩进并打印结果
  identation++;
  for (int i = 0; i < (int)myNode->children.size(); i++) {
    GetTree(builder, myNode->children[i], identation);
  }
  // 回归缩进
  identation--;
}

// SyntaxTreeNode获取缩进
istr SyntaxTreeNode::DrawTree(SyntaxTreeNode* myNode) {
  // 若空就不需要继续了
  if (myNode == NULL) {
    return "";
  }
  // 取父母节点，若空就不需要画线了
  SyntaxTreeNode* parent = myNode->parent;
  if (parent == NULL) {
    return "";
  }
  // 否则查询祖父母节点来看父母节点的排位
  std::vector<bool> lstline;
  while (parent != NULL)
  {
    SyntaxTreeNode* pp = parent->parent;
    int indexOfParent = 0;
    if (pp != NULL) {
      for (; indexOfParent < (int)pp->children.size(); indexOfParent++) {
        if (parent == pp->children[indexOfParent]) {
          break;
        }
      }
      lstline.push_back(indexOfParent < (int)pp->children.size() - 1);
    }
    parent = pp;
  }
  // 画纵向线
  istr builder = "";
  for (int i = lstline.size() - 1; i >= 0; i--) {
    builder += lstline[i] ? "│  " : "    ";
  }
  // 获得自己在兄弟姐妹中的排行
  parent = myNode->parent;
  int indexOfParent = 0;
  for (; indexOfParent < (int)parent->children.size(); indexOfParent++) {
    if (myNode == parent->children[indexOfParent]) {
      break;
    }
  }
  // 如果是最后一个就不要出头了
  if (indexOfParent < (int)parent->children.size() - 1) {
    builder += "├─";
  }
  else {
    builder += "└─";
  }
  return builder;
}

// ITable构造函数
ITable::ITable(int _id, istr _name, StrVec _pi, bool &_flag) {
  // 拷贝属性
  this->id = _id;
  this->name = _name;
  int cid = 0;
  for (StrVec::iterator iter = _pi.begin();
    iter != _pi.end(); iter++, cid++) {
    // 重名检测
    for (StrVec::iterator inner = this->Pi.begin();
      inner != this->Pi.end(); inner++) {
      if ((*inner) == (*iter)) {
        _flag = true;
        return;
      }
    }
    this->Pi.push_back((*iter));
    PiDict.insert(PilePair(*iter, cid));
  }
  // 调整默认向量大小
  int noc = _pi.size();
  // 初始置零
  for (int i = 0; i < noc; i++) {
    DefaultPi.push_back(0);
  }
  // 调整表的列大小
  this->table.resize(noc);
}

// ITable清空
void ITable::Clear() {
  for (unsigned int i = 0; i < this->table.size(); i++) {
    this->table[i].clear();
  }
}


/********************************************************
Author:      编译原理计应1班12小组
Date:        2014-11-30
Description: 定义各类需要用到的数据结构
*********************************************************/

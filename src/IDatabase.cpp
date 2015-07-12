#include "IDatabase.h"

// IDatabase构造函数
IDatabase::IDatabase() {
  this->Reset();
}

// IDatabase列出所有表
void IDatabase::ShowTable() {
  cout << NEWLINE << "Tables in this database: (All "
    << this->Size() << " table";
  if (this->Size() > 1) {
    cout << "s";
  }
  cout << ")" << NEWLINE;
  for (TableVec::iterator iter = _tableContainer.begin();
    iter != _tableContainer.end(); iter++) {
    cout << (*iter).name << "\t(" << iter->Pi[0];
    for (StrVec::iterator inner = iter->Pi.begin() + 1;
      inner != iter->Pi.end(); inner++) {
      cout << ", " << *inner;
    }
    cout << ")" << NEWLINE;
  }
  cout << endl;
}

// IDatabase清空
void IDatabase::Reset() {
  this->_param.clear();
  this->_tableContainer.clear();
}

// IDatabase表数量
int IDatabase::Size() {
  return this->_tableContainer.size();
}

// IDatabase查询表序号
int IDatabase::TableNo(istr name) {
  int no = 0;
  for (TableVec::iterator iter = _tableContainer.begin();
    iter != _tableContainer.end(); iter++, no++) {
    if ((*iter).name == name) {
      return no;
    }
  }
  return -1;
}

// IDatabase参数字典
int IDatabase::Reference(istr paraname) {
  return this->_param[paraname];
}

// IDatabase表是否存在
bool IDatabase::Exist(istr name) {
  return this->TableNo(name) != -1;
}

// IDatabase执行一条语句
bool IDatabase::Interpreter(IBridgeProxy &proxy) {
  // 取得操作码
  DashType opCode = proxy.opCode;
  // 启动计时器
  double startTime = clock();
  bool res = false;
  // 清空参数字典
  _param.clear();
  switch (opCode)
  {
  case DashType::dash_create:
    res = this->Create(proxy.opTable, proxy.Pi, proxy.PrimaryPi, proxy.DefaPi, proxy.encounter, proxy.errorBit);
    break;
  case DashType::dash_delete:
    res = this->Delete(proxy.opTable, proxy.CondPi, proxy.condPtr, &proxy);
    break;
  case DashType::dash_insert:
    res = this->Insert(proxy.opTable, proxy.Pi, proxy.ProxyPi, proxy.errorBit);
    break;
  case DashType::dash_select:
    res = this->Select(proxy.opTable, proxy.Pi, proxy.isStar, proxy.CondPi, proxy.condPtr, &proxy);
    break;
  default:
    res = this->iException("No_Object_Exception", proxy.id);
    break;
  }
  // 如果出错就输出错误
  if (res == false) {
    cout << "# This command is ignored due to exception.";
  }
  // 否则输出耗时
  else {
    cout << " (" << (clock() - startTime) / 1000.0f << " sec)";
  }
  cout << endl;
  return res;
}

// IDatabase建表
bool IDatabase::Create(istr name, StrVec &pi, StrVec &ppi, TablePileDictionary &def, int &dpflag, bool &errorbit) {
  // 除零错误
  if (errorbit) {
    this->iException("Exception: 0 cannot be divisor!");
    return false;
  }
  // 表的存在性
  if (this->Exist(name)) {
    this->iException("Create_Exception: Table already existed (" + name + ")");
    return false;
  }
  // 建表并查重复
  int cid = this->_tableContainer.size();
  bool duplicateFlag = false;
  ITable nt(cid, name, pi, duplicateFlag);
  int tableID = this->TableNo(name);
  if (duplicateFlag) {
    this->iException("Create_Exception: Cannot have 2 or more columns with same name in one table");
    return false;
  }
  // 多重主键声明是不允许的
  if (dpflag > 1) {
    this->iException("Create_Exception: No 2 or more primary key declarations");
    return false;
  }
  // 列的存在性
  bool conflag = true;
  for (StrVec::iterator iter = ppi.begin();
    iter != ppi.end();) {
    bool existFlag = false;
    for (StrVec::iterator inner = nt.Pi.begin();
      inner != nt.Pi.end(); inner++) {
      if ((*iter) == (*inner)) {
        existFlag = true;
        break;
      }
    }
    if (!existFlag) {
      this->iException("Create_Exception: primary key ( " + (*iter) + " ) not exist");
      ppi.erase(iter);
      iter = ppi.begin();
      conflag = false;
    }
    else {
      iter++;
    }
  }
  // 指定的主键不存在就返回
  if (!conflag) {
    return false;
  }
  nt.PrimaryPi = ppi;
  nt.DefaultPi.resize(nt.Pi.size());
  nt.DefaultPi.assign(nt.Pi.size(), 0);
  for (TablePileDictionary::iterator iter = def.begin();
    iter != def.end(); iter++) {
    int cid = 0;
    for (StrVec::iterator inner = pi.begin();
      inner != pi.end(); inner++) {
      if ((*inner) == (*iter).first) {
        break;
      }
      cid++;
    }
    nt.DefaultPi[cid] = (*iter).second;
  }
  this->_tableContainer.push_back(nt);
  cout << "Query OK, 1 Table affected ";
  return true;
}

// IDatabase删行
bool IDatabase::Delete(istr name, StrVec &condVec, SyntaxTreeNode* cond, IBridgeProxy* iproxy) {
  // 表的存在性
  if (!this->Exist(name)) {
    this->iException("Delete_Exception: Table not exist (" + name + ")");
    return false;
  }
  int tid = this->TableNo(name);
  // 条件列列表的合法性
  for (StrVec::iterator iter = condVec.begin();
    iter != condVec.end(); iter++) {
    bool spotFlag = false;
    for (StrVec::iterator inner = _tableContainer[tid].Pi.begin();
      inner != _tableContainer[tid].Pi.end(); inner++) {
      if (*inner == *iter) {
        spotFlag = true;
        break;
      }
    }
    if (!spotFlag) {
      this->iException("Delete_Exception: Where clause contains column (" + *iter + ") not exist");
      return false;
    }
  }
  // 是否通配
  int affectedCount = 0;
  if (cond == NULL) {
    affectedCount = this->_tableContainer[this->TableNo(name)].table[0].size();
    this->_tableContainer[tid].Clear();
  }
  else {
    // 逐行扫描
    int tid = this->TableNo(name);
    int ccount = _tableContainer[tid].table.size();
    int rcount = ccount ? _tableContainer[tid].table[0].size() : 0;
    std::vector<IntVec::iterator> iterVec;
    for (int i = 0; i < ccount; i++) {
      iterVec.push_back(_tableContainer[tid].table[i].begin());
    }
    for (int i = 0; i < rcount; i++) {
      int j;
      for (j = 0; j < ccount; j++) {
        // 添加字典项目，供AST生成
        _param.insert(PilePair(_tableContainer[tid].Pi[j], _tableContainer[tid].table[j][i]));
      }
      // 删掉满足条件的行
      bool dflag;
      if (dflag = IDatabase::AST(cond, this, iproxy)) {
        if (iproxy->errorBit) {
          this->iException("Exception: 0 cannot be divisor!");
          return false;
        }
        for (unsigned int k = 0; k < iterVec.size(); k++) {
          iterVec[k] = _tableContainer[tid].table[k].erase(iterVec[k]);
        }
        i--;
        rcount--;
        affectedCount++;
      }
      // 递增各列的行游标
      if (!dflag) {
        for (unsigned int k = 0; k < iterVec.size(); k++) {
          iterVec[k]++;
        }
      }
      // 清空参数列表字典
      _param.clear();
    }
    // 除零错误
    if (rcount == 0 && cond != NULL) {
      IDatabase::AST(cond, this, iproxy);
    }
    if (iproxy->errorBit) {
      this->iException("Exception: 0 cannot be divisor!");
      return false;
    }
  }
  if (affectedCount > 1) {
    cout << "Query OK, " << affectedCount << " Rows affected ";
  }
  else {
    cout << "Query OK, " << affectedCount << " Row affected ";
  }
  return true;
}

// IDatabase插入
bool IDatabase::Insert(istr name, StrVec &pilist, IntVec &pivalue, bool &errorbit) {
  // 除零错误
  if (errorbit) {
    this->iException("Exception: 0 cannot be divisor!");
    return false;
  }
  // 检查表存在性
  if (!this->Exist(name)) {
    this->iException("Insert_Exception: Table not exist (" + name + ")");
    return false;
  }
  int tableID = this->TableNo(name);
  // 赋值是否大于列数
  if (pivalue.size() != pilist.size()) {
    this->iException("Insert_Exception: too many or too few values to insert");
    return false;
  }
  // 赋值列的重复性
  for (unsigned int i = 0; i < pilist.size(); i++) {
    for (unsigned int j = i + 1; j < pilist.size(); j++) {
      if (pilist[i] == pilist[j]) {
        this->iException("Insert_Exception: column pending to set ( " + (pilist[i])+" ) duplicated");
        return false;
      }
    }
  }
  // 列的存在性
  bool conflag = true;
  for (StrVec::iterator iter = pilist.begin();
    iter != pilist.end();) {
    bool existFlag = false;
    for (StrVec::iterator inner = _tableContainer[tableID].Pi.begin();
      inner != _tableContainer[tableID].Pi.end(); inner++) {
      if ((*iter) == (*inner)) {
        existFlag = true;
        break;
      }
    }
    if (!existFlag) {
      this->iException("Insert_Exception: column ( " + (*iter) + " ) not exist");
      pilist.erase(iter);
      iter = pilist.begin();
      conflag = false;
    }
    else {
      iter++;
    }
  }
  if (!conflag) {
    return false;
  }
  // 检查主键约束
  IntVec tablePriv;
  // 出现在pilist的主键
  for (StrVec::iterator inner = pilist.begin();
    inner != pilist.end(); inner++) {
    bool noflag = false;
    for (StrVec::iterator iter = this->_tableContainer[tableID].PrimaryPi.begin();
      iter != this->_tableContainer[tableID].PrimaryPi.end(); iter++) {
      if (*iter == *inner) {
        noflag = true;
        break;
      }
    }
    if (noflag) {
      tablePriv.push_back(this->_tableContainer[tableID].PiDict[*inner]);
    }
  }
  // 没有出现在pilist的主键
  for (StrVec::iterator iter = this->_tableContainer[tableID].PrimaryPi.begin();
    iter != this->_tableContainer[tableID].PrimaryPi.end(); iter++) {
    bool noflag = false;
    for (StrVec::iterator inner = pilist.begin();
      inner != pilist.end(); inner++) {
      if (*iter == *inner) {
        noflag = true;
        break;
      }
    }
    if (!noflag) {
      tablePriv.push_back(this->_tableContainer[tableID].PiDict[*iter]);
    }
  }
  // 构造临时表
  IntTable tempTable;
  for (unsigned int i = 0; i < tablePriv.size(); i++) {
    tempTable.push_back(this->_tableContainer[tableID].table[tablePriv[i]]);
  }
  // 构造比较向量
  IntVec compareVec;
  IntVec appendVec;
  for (unsigned int i = 0; i < tablePriv.size(); i++) {
    bool noi = false;
    for (unsigned int j = 0; j < pilist.size(); j++) {
      if (pilist[j] == this->_tableContainer[tableID].Pi[tablePriv[i]]) {
        compareVec.push_back(pivalue[j]);
        noi = true;
        break;
      }
    }
    if (!noi) {
      appendVec.push_back(tablePriv[i]);
    }
  }
  for (unsigned int i = 0; i < appendVec.size(); i++) {
    compareVec.push_back(this->_tableContainer[tableID].DefaultPi[appendVec[i]]);
  }
  // 扫描
  bool validFlag = true;
  unsigned int rcount = tempTable.size() > 0 ? tempTable[0].size() : 0;
  for (unsigned int i = 0; i < rcount; i++) {
    bool noe = false;
    for (unsigned int j = 0; j < tempTable.size(); j++) {
      if (tempTable[j][i] != compareVec[j]) {
        noe = true;
        break;
      }
    }
    if (noe == false && tempTable[0].size()) {
      validFlag = false;
    }
  }
  // 主键冲突
  if (!validFlag) {
    this->iException("Insert_Exception: Primary key constraint violation");
    return false;
  }
  // 用默认值填充
  int lastrow = this->_tableContainer[tableID].table[0].size();
  for (int i = 0; i < (int)this->_tableContainer[tableID].Pi.size(); i++) {
    int defvalue = this->_tableContainer[tableID].DefaultPi[i];
    this->_tableContainer[tableID].table[i].push_back(defvalue);
  }
  // 修正插值
  int c = 0;
  for (StrVec::iterator iter = pilist.begin();
    iter != pilist.end(); iter++, c++) {
    int opCol = this->_tableContainer[tableID].PiDict[*iter];
    this->_tableContainer[tableID].table[opCol][lastrow] = pivalue[c];
  }
  cout << "Query OK, 1 Row affected ";
  return true;
}

// IDatabase查询
bool IDatabase::Select(istr name, StrVec &pi, bool star, StrVec &condVec, SyntaxTreeNode* cond, IBridgeProxy* iproxy) {
  // 表的存在性检验
  if (!this->Exist(name)) {
    this->iException("Query_Exception: Table not exist (" + name + ")");
    return false;
  }
  int tid = this->TableNo(name);
  int affectedCount = 0;
  StrVec pilist = pi;
  int rrcount = _tableContainer[tid].table.size() ? _tableContainer[tid].table[0].size() : 0;
  // 如果有通配符，就是全列查询
  if (star) {
    pilist = _tableContainer[tid].Pi;
  }
  // 条件列列表的合法性
  for (StrVec::iterator iter = condVec.begin();
    iter != condVec.end(); iter++) {    
    bool spotFlag = false;
    for (StrVec::iterator inner = _tableContainer[tid].Pi.begin();
      inner != _tableContainer[tid].Pi.end(); inner++) {
      if (*inner == *iter) {
        spotFlag = true;
        break;
      }
    }
    if (!spotFlag) {
      this->iException("Query_Exception: Where clause contains column ( " + *iter + " ) not exist");
      return false;
    }
  }
  // 列的存在性
  for (StrVec::iterator iter = pilist.begin();
    iter != pilist.end(); iter++) {
    bool existFlag = false;
    for (StrVec::iterator inner = _tableContainer[tid].Pi.begin();
      inner != _tableContainer[tid].Pi.end(); inner++) {
      if ((*iter) == (*inner)) {
        existFlag = true;
        break;
      }
    }
    if (!existFlag) {
      this->iException("Query_Exception: column ( " + (*iter) + " ) is not exist");
      return false;
    }
  }
  int ccount = _tableContainer[tid].table.size();
  int rcount = ccount ? _tableContainer[tid].table[0].size() : 0;
  // 打印表头
  if (cond == NULL) {
    // 头部
    cout << "┌───────────────";
    for (unsigned int i = 1; i < pilist.size(); i++) {
      cout << "┬───────────────";
    }
    cout << "┐\n";
    // 列名
    for (StrVec::iterator iter = pilist.begin();
      iter != pilist.end(); iter++) {
      if ((*iter).length() <= 6) {
        cout << "│" + (*iter) + "\t\t";
      }
      else {
        cout << "│" + (*iter) + "\t";
      }
    }
    cout << "│" << NEWLINE;
  }
  // 扫描列列表
  StrVec pipi;
  for (int i = 0; i < condVec.size(); i++) {
    pipi.push_back(condVec[i]);
  }
  for (int i = 0; i < pilist.size(); i++) {
    bool allocFlag = false;
    for (int j = 0; j < pipi.size(); j++) {
      if (pipi[j] == pilist[i]) {
        allocFlag = true;
        break;
      }
    }
    if (!allocFlag) {
      pipi.push_back(pilist[i]);
    }
  }
  // 逐行扫描
  bool boundflag = true;
  for (int i = 0; i < rcount; i++) {
    istr resbuilder = "";
    if (cond == NULL) {
      cout << "├───────────────";
      for (unsigned int k = 1; k < pilist.size(); k++) {
        cout << "┼───────────────";
      }
      cout << "┤\n";
    }
    for (int j = 0; j < pipi.size(); j++) {
      for (int k = 0; k < ccount; k++) {
        if (_tableContainer[tid].Pi[k] == pipi[j]) {
          if (cond == NULL) {
            char buf[256];
            sprintf(buf, "%d", _tableContainer[tid].table[k][i]);
            istr str = buf;
            if (str.length() <= 6) {
              cout << "│" << str << "\t\t";
            }
            else {
              cout << "│" << str << "\t";
            }
            break;
          }
          else {
            bool visualFlag = false;
            for (int pp = 0; pp < pilist.size(); pp++) {
              if (pilist[pp] == pipi[j]) {
                visualFlag = true;
                break;
              }
            }
            if (visualFlag == true) {
              char buf[256];
              sprintf(buf, "%d", _tableContainer[tid].table[k][i]);
              istr str = buf;
              if (str.length() <= 6) {
                resbuilder += "│" + str + "\t\t";
              }
              else {
                resbuilder += "│" + str + "\t";
              }
            }
            // 添加字典项目，供AST生成
            _param.insert(PilePair(pipi[j], _tableContainer[tid].table[k][i]));
            break;
          }
        }
      }
    }
    // 如果不是全查询，就要满足条件才输出全行
    if (cond != NULL && IDatabase::AST(cond, this, iproxy)) {
      if (iproxy->errorBit) {
        this->iException("Exception: 0 cannot be divisor!");
        return false;
      }
      if (boundflag) {
        // 表头
        cout << "┌───────────────";
        for (unsigned int i = 1; i < pilist.size(); i++) {
          cout << "┬───────────────";
        }
        cout << "┐" << NEWLINE;
        // 列名
        for (StrVec::iterator iter = pilist.begin();
          iter != pilist.end(); iter++) {
          if ((*iter).length() <= 6) {
            cout << "│" + (*iter) + "\t\t";
          }
          else {
            cout << "│" + (*iter) + "\t";
          }
        }
        cout << "│" << NEWLINE;
        boundflag = false;
      }
      cout << "├───────────────";
      for (unsigned int k = 1; k < pilist.size(); k++) {
        cout << "┼───────────────";
      }
      cout << "┤\n";
      cout << resbuilder + "│" << endl;
      affectedCount++;
    }
    else if (cond == NULL) {
      cout << "│" << endl;
    }
    // 清空参数列表字典
    _param.clear();
  }
  // 除零错误
  if (rrcount == 0 && cond != NULL) {
    IDatabase::AST(cond, this, iproxy);
  }
  if (iproxy->errorBit) {
    this->iException("Exception: 0 cannot be divisor!");
    return false;
  }
  // 如果是全查询那么影响数就是行总数
  if (cond == NULL) {
    affectedCount = rcount;
  }
  // 打印表尾
  if (affectedCount == 0 && cond != NULL) {
    // 表头
    cout << "┌───────────────";
    for (unsigned int i = 1; i < pilist.size(); i++) {
      cout << "┬───────────────";
    }
    cout << "┐" << NEWLINE;
    // 列名
    for (StrVec::iterator iter = pilist.begin();
      iter != pilist.end(); iter++) {
      if ((*iter).length() <= 6) {
        cout << "│" + (*iter) + "\t\t";
      }
      else {
        cout << "│" + (*iter) + "\t";
      }
    }
    cout << "│" << NEWLINE;
  }
  // 表尾
  cout << "└───────────────";
  for (unsigned int i = 1; i < pilist.size(); i++) {
	  cout << "┴───────────────";
  }
  cout << "┘" << NEWLINE << "Query OK, ";
  // 影响行数
  if (affectedCount > 1) {
	  cout << affectedCount << " Rows affected ";
  }
  else {
	  cout << affectedCount << " Row affected ";
  }
  return true;
}

// IDatabase异常处理
bool IDatabase::iException(istr _info, int _index) {
  cout << "# Interpreter Exception Spotted." << NEWLINE;
  if (_index != -1) {
    cout << "# When executing SSQL at command: "
      << _index << NEWLINE;
  }
  cout << "# " << _info << endl;
  return false;
}

// IDatabase抽象语法树求值
bool IDatabase::AST(SyntaxTreeNode* mynode, IDatabase* myexec, IBridgeProxy* myproxy) {
  istr optype;
  switch (mynode->nodeSyntaxType)
  {
  case SyntaxType::case_sexpr:
    AST(mynode->children[0], myexec, myproxy); // 因式
    AST(mynode->children[1], myexec, myproxy); // 加项
    mynode->aTag = mynode->children[0]->aTag + mynode->children[1]->aTag;
    break;
  case SyntaxType::case_wexpr:
    if (mynode->candidateFunction->GetType() == CFunctionType::deri___wexpr__wmulti__wexpr_pi_45) {
      AST(mynode->children[0], myexec, myproxy); // 因式
      AST(mynode->children[1], myexec, myproxy); // 加项
      mynode->aTag = mynode->children[0]->aTag + mynode->children[1]->aTag;
    }
    else {
      mynode->aTag = 0;
    }
    break;
  case SyntaxType::case_sexpr_pi:
  case SyntaxType::case_wexpr_pi:
    if (mynode->candidateFunction->GetType() == CFunctionType::deri___sexpr_pi__splus__sexpr_pi_70
      || mynode->candidateFunction->GetType() == CFunctionType::deri___wexpr_pi__wplus__wexpr_pi_72) {
      AST(mynode->children[0], myexec, myproxy); // 加项
      AST(mynode->children[1], myexec, myproxy); // 加项闭包
      mynode->aTag = mynode->children[0]->aTag + mynode->children[1]->aTag;
    }
    break;
  case SyntaxType::case_splus:
  case SyntaxType::case_wplus:
    if (mynode->candidateFunction->GetType() == CFunctionType::deri___splus__plus_smulti_14
      || mynode->candidateFunction->GetType() == CFunctionType::deri___wplus__plus_wmulti_46) {
      AST(mynode->children[1], myexec, myproxy);
      mynode->aTag = mynode->children[1]->aTag; // 加法
    }
    else if (mynode->candidateFunction->GetType() == CFunctionType::deri___splus__minus_smulti_15
      || mynode->candidateFunction->GetType() == CFunctionType::deri___wplus__minus_wmulti_47) {
      AST(mynode->children[1], myexec, myproxy);
      mynode->aTag = (-1) * mynode->children[1]->aTag; // 减法
    }
    else {
      mynode->aTag = 0;
    }
    break;
  case SyntaxType::case_smulti:
  case SyntaxType::case_wmulti:
    AST(mynode->children[0], myexec, myproxy); // 乘项
    AST(mynode->children[1], myexec, myproxy); // 乘项闭包
    mynode->aTag = mynode->children[0]->aTag * mynode->children[1]->aTag;
    break;
  case SyntaxType::case_smultiOpt:
  case SyntaxType::case_wmultiOpt:
    if (mynode->candidateFunction->GetType() == CFunctionType::deri___smultiOpt__multi_sunit__smultiOpt_18
      || mynode->candidateFunction->GetType() == CFunctionType::deri___wmultiOpt__multi_wunit__wmultiOpt_50) {
      AST(mynode->children[1], myexec, myproxy); // 乘项
      AST(mynode->children[2], myexec, myproxy); // 乘项闭包
      mynode->aTag = mynode->children[1]->aTag * mynode->children[2]->aTag; // 乘法
    }
    else if (mynode->candidateFunction->GetType() == CFunctionType::deri___smultiOpt__div_sunit__smultiOpt_19
      || mynode->candidateFunction->GetType() == CFunctionType::deri___wmultiOpt__div_wunit__wmultiOpt_51) {
      AST(mynode->children[1], myexec, myproxy); // 乘项
      AST(mynode->children[2], myexec, myproxy); // 乘项闭包
      if (mynode->children[1]->aTag * mynode->children[2]->aTag == 0) {
        mynode->aTag = 0;
        myproxy->errorBit = true; // 除零错误
      }
      else {
        mynode->aTag = 1.0f / mynode->children[1]->aTag * mynode->children[2]->aTag; // 除法
      }
    }
    else {
      mynode->aTag = 1.0f;
    }
    break;
  case SyntaxType::case_sunit:
  case SyntaxType::case_wunit:
    if (mynode->candidateFunction->GetType() == CFunctionType::deri___sunit__number_21
      || mynode->candidateFunction->GetType() == CFunctionType::deri___wunit__number_53) {
      mynode->aTag = atoi(mynode->children[0]->nodeValue.c_str());
    }
    else if (mynode->candidateFunction->GetType() == CFunctionType::deri___sunit__minus_sunit_22
      || mynode->candidateFunction->GetType() == CFunctionType::deri___wunit__minus_wunit_55) {
      AST(mynode->children[1], myexec, myproxy);
      mynode->aTag = (-1) * mynode->children[1]->aTag;
    }
    else if (mynode->candidateFunction->GetType() == CFunctionType::deri___sunit__plus_sunit_23
      || mynode->candidateFunction->GetType() == CFunctionType::deri___wunit__plus_wunit_56) {
      AST(mynode->children[1], myexec, myproxy);
      mynode->aTag = mynode->children[1]->aTag;
    }
    else if (mynode->candidateFunction->GetType() == CFunctionType::deri___sunit__brucket_sexpr_24
      || mynode->candidateFunction->GetType() == CFunctionType::deri___wunit__brucket_disjunct_57) {
      AST(mynode->children[1], myexec, myproxy);
      mynode->aTag = mynode->children[1]->aTag;
    }
    else if (mynode->candidateFunction->GetType() == CFunctionType::deri___wunit__iden_54) {
      mynode->aTag = myexec->Reference(mynode->children[0]->nodeValue); // 查参数字典
    }
    break;
  case SyntaxType::case_disjunct:
    AST(mynode->children[0], myexec, myproxy); // 合取项
    AST(mynode->children[1], myexec, myproxy); // 析取闭包
    mynode->aTag = mynode->children[0]->aTag + mynode->children[1]->aTag;
    return fabs(mynode->aTag) <= FZERO ? false : true;
    break;
  case SyntaxType::case_disjunct_pi:
    if (mynode->candidateFunction->GetType() == CFunctionType::deri___disjunct_pi__conjunct__disjunct_pi_36) {
      AST(mynode->children[1], myexec, myproxy); // 合取项
      AST(mynode->children[2], myexec, myproxy); // 析取闭包
      mynode->aTag = mynode->children[1]->aTag + mynode->children[2]->aTag;
    }
    else {
      mynode->aTag = false; // 析取false不影响结果
    }
    break;
  case SyntaxType::case_conjunct:
    AST(mynode->children[0], myexec, myproxy); // 布尔项
    AST(mynode->children[1], myexec, myproxy); // 合取闭包
    mynode->aTag = mynode->children[0]->aTag * mynode->children[1]->aTag;
    break;
  case SyntaxType::case_conjunct_pi:
    if (mynode->candidateFunction->GetType() == CFunctionType::deri___conjunct_pi__bool__conjunct_pi_39) {
      AST(mynode->children[1], myexec, myproxy); // 布尔项
      AST(mynode->children[2], myexec, myproxy); // 合取闭包
      mynode->aTag = mynode->children[1]->aTag * mynode->children[2]->aTag;
    }
    else {
      mynode->aTag = true; // 合取true不影响结果
    }
    break;
  case SyntaxType::case_bool:
    if (mynode->candidateFunction->GetType() == CFunctionType::deri___bool__not_bool_42) {
      AST(mynode->children[1], myexec, myproxy); // 非项
      mynode->aTag = mynode->children[1]->aTag != 0 ? false : true;
    }
    else {
      AST(mynode->children[0], myexec, myproxy); // 表达式
      mynode->aTag = mynode->children[0]->aTag;
    }
    break;
  case SyntaxType::case_comp:
    if (mynode->children[1]->candidateFunction->GetType() == CFunctionType::deri___rop__epsilon_80) {
      AST(mynode->children[0], myexec, myproxy); // 左边
      mynode->aTag = mynode->children[0]->aTag;
    }
    else {
      optype = mynode->children[1]->nodeValue; // 运算符
      AST(mynode->children[0], myexec, myproxy); // 左边
      AST(mynode->children[2], myexec, myproxy); // 右边
      mynode->aTag = false;
      if (optype == "<>") {
        mynode->aTag = mynode->children[0]->aTag != mynode->children[2]->aTag;
      }
      else if (optype == "==") {
        mynode->aTag = mynode->children[0]->aTag == mynode->children[2]->aTag;
      }
      else if (optype == ">") {
        mynode->aTag = mynode->children[0]->aTag > mynode->children[2]->aTag;
      }
      else if (optype == "<") {
        mynode->aTag = mynode->children[0]->aTag < mynode->children[2]->aTag;
      }
      else if (optype == ">=") {
        mynode->aTag = mynode->children[0]->aTag >= mynode->children[2]->aTag;
      }
      else if (optype == "<=") {
        mynode->aTag = mynode->children[0]->aTag <= mynode->children[2]->aTag;
      }
    }
    break;
  default:
    break;
  }
  return true;
}


/********************************************************
Author:      编译原理计应1班12小组
Date:        2014-11-30
Description: SSQL执行器
*********************************************************/

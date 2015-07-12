#ifndef ___PILE
#define ___PILE
#define FZERO 1e-15
#define NEWLINE "\n"
#define COLSTARTNUM 1
#define ROWSTARTNUM 1
#define LL1PARSERMAPROW 69
#define LL1PARSERMAPCOL 35
#define SENTENCETERMINATOR ';'
#define FOREVER while (true)

#include <algorithm>
#include <iostream>
#include <fstream>
#include <cstdlib>
#include <cstring>
#include <string>
#include <vector>
#include <stack>
#include <ctime>
#include <map>

class Token;
class ITable;
class TokenStream;
class IBridgeProxy;
class SyntaxTreeNode;
class LL1SyntaxParser;
class CandidateFunction;

using std::cin;
using std::cout;
using std::endl;

typedef enum __RunType {
  RUN_CONSOLE,
  RUN_INFILE,
  RUN_DEBUG
} RunType;
typedef enum __DashType {
  // 空
  dash_nop,
  // 建表
  dash_create,
  // 插入行
  dash_insert,
  // 删除行
  dash_delete,
  // 查询
  dash_select
} DashType;
typedef enum __CharaType {
  // 未知
  cUnknown,
  // 字母
  Letter,
  // 数字
  Number,
  // _
  UnderLine,
  // .
  Dot,
  // ,
  Comma,
  // +
  Plus,
  // -
  Minus,
  // *
  Multiply,
  // /
  Divide,
  // %
  Percent,
  // ^
  Xor,
  // &;
  And,
  // |
  Or,
  // ~
  Reverse,
  // $
  Dollar,
  // <
  LessThan,
  // >
  GreaterThan,
  // (
  LeftParentheses,
  // )
  RightParentheses,
  // [
  LeftBracket,
  // ]
  RightBracket,
  // {
  LeftBrace,
  // }
  RightBrace,
  // !
  Not,
  // #
  Pound,
  // "\\"
  Slash,
  // ?
  Question,
  // '
  Quotation,
  // "
  DoubleQuotation,
  // :
  Colon,
  // ;
  Semicolon,
  // =
  Equality,
  // space Tab \r\n
  Space,
} CharaType;
typedef enum __ITokenType {
  // 未知的单词符号
  unknown,
  // "create"
  token_create,
  // "table"
  token_table,
  // iden
  token_iden,
  // "("
  token_LeftParentheses_,
  // ")"
  token_RightParentheses_,
  // ";"
  token_Semicolon_,
  // ","
  token_Comma_,
  // null
  epsilon,
  // "int"
  token_int,
  // "primary"
  token_primary,
  // "key"
  token_key,
  // "default"
  token_default,
  // "="
  token_Equality_,
  // "+"
  token_Plus_,
  // "-"
  token_Minus_,
  // "*"
  token_Multiply_,
  // "/"
  token_Divide_,
  // number
  number,
  // "insert"
  token_insert,
  // "into"
  token_into,
  // "values"
  token_values,
  // "delete"
  token_delete,
  // "from"
  token_from,
  // "where"
  token_where,
  // "||"
  token_Or_Or_,
  // "&&"
  token_And_And_,
  // "!"
  token_Not_,
  // "<>"
  token_LessThan_GreaterThan_,
  // "=="
  token_Equality_Equality_,
  // ">"
  token_GreaterThan_,
  // "<"
  token_LessThan_,
  // ">="
  token_GreaterThan_Equality_,
  // "<="
  token_LessThan_Equality_,
  // "select"
  token_select,
  // #
  token_startEnd,
  // 标识符
  identifier,
} TokenType;
typedef enum __ISyntaxType
{
  // <ssql_stmt> ::= <create_stmt> | <insert_stmt> | <delete_stmt> | <query_stmt>;
  case_ssql_stmt,
  // <create_stmt> ::= "create" "table" identifier "(" <decl_list> ")" ";";
  case_create_stmt,
  // <decl_list> ::= <decl> <decl_listpi>;
  case_decl_list,
  // <decl_listpi> ::= "," <decl> <decl_listpi> | null;
  case_decl_listpi,
  // <decl> ::= identifier "int" <default_spec> | "primary" "key" "(" <column_list> ")";
  case_decl,
  // <default_spec> ::= "default" "=" <sexpr> | null;
  case_default_spec,
  // <sexpr> ::= <smulti> <sexpr_pi>;
  case_sexpr,
  // <sexpr_pi> ::= <splus> <sexpr_pi> | null;
  case_sexpr_pi,
  // <splus> ::= "+" <smulti> | "-" <smulti>;
  case_splus,
  // <smulti> ::= <sunit> <smultiOpt>;
  case_smulti,
  // <smultiOpt> ::= "*" <sunit> | "/" <sunit> | null;
  case_smultiOpt,
  // <sunit> ::= number | "-" <sunit> | "+" <sunit> | "(" <sexpr> ")";
  case_sunit,
  // <insert_stmt> ::= "insert" "into" identifier "(" <column_list> ")" "values" "(" <value_list> ")" ";";
  case_insert_stmt,
  // <value_list> ::= <wexpr> <value_listpi>;
  case_value_list,
  // <value_listpi> ::= "," <wexpr> <value_listpi> | null;
  case_value_listpi,
  // <delete_stmt> ::= "delete" "from" identifier <where_clause> ";";
  case_delete_stmt,
  // <where_clause> ::= "where" <disjunct> | null;
  case_where_clause,
  // <disjunct> ::= <conjunct> <disjunct_pi>;
  case_disjunct,
  // <disjunct_pi> ::= "||" <conjunct> <disjunct_pi> | null;
  case_disjunct_pi,
  // <conjunct> ::= <bool> <conjunct_pi>;
  case_conjunct,
  // <conjunct_pi> ::= "&&" <bool> <conjunct_pi> | null;
  case_conjunct_pi,
  // <bool> ::= "(" <disjunct> ")" | "!" <bool> | <comp>;
  case_bool,
  // <comp> ::= <wexpr> <rop> <wexpr>;
  case_comp,
  // <rop> ::= "<>" | "==" | ">" | "<" | ">=" | "<=" | null;
  case_rop,
  // <wexpr> ::= <wmulti> <wexpr_pi>;
  case_wexpr,
  // <wexpr> ::= <wplus> <wexpr_pi> | null;
  case_wexpr_pi,
  // <wplus> ::= "+" <wmulti> | "-" <wmulti>;
  case_wplus,
  // <wmulti> ::= <wunit> <wmultiOpt>;
  case_wmulti,
  // <wmultiOpt> ::= "*" <wunit> | "/" <wunit> | null;
  case_wmultiOpt,
  // <wunit> ::= number | identifier | "-" <wunit> | "+" <wunit> | "(" <wexpr> ")";
  case_wunit,
  // <query_stmt> ::= "select" <select_list> "from" identifier <where_clause> ";";
  case_query_stmt,
  // <select_list> ::= <column_list> | "*";
  case_select_list,
  // <column_list> ::= identifier <column_pi>;
  case_column_list,
  // <column_pi> ::= "," identifier <column_pi> | null;
  case_column_pi,
  // 未知的语法结点符号
  Unknown,
  // "create"
  tail_createLeave,
  // "table"
  tail_tableLeave,
  // iden
  tail_idenLeave,
  // "("
  tail_leftParentheses_Leave,
  // ")"
  tail_rightParentheses_Leave,
  // ";"
  tail_semicolon_Leave,
  // ","
  tail_comma_Leave,
  // null
  epsilonLeave,
  // "int"
  tail_intLeave,
  // "primary"
  tail_primaryLeave,
  // "key"
  tail_keyLeave,
  // "default"
  tail_defaultLeave,
  // "="
  tail_equality_Leave,
  // "+"
  tail_plus_Leave,
  // "-"
  tail_minus_Leave,
  // "*"
  tail_multiply_Leave,
  // "/"
  tail_divide_Leave,
  // number
  numberLeave,
  // "insert"
  tail_insertLeave,
  // "into"
  tail_intoLeave,
  // "values"
  tail_valuesLeave,
  // "delete"
  tail_deleteLeave,
  // "from"
  tail_fromLeave,
  // "where"
  tail_whereLeave,
  // "||"
  tail_or_Or_Leave,
  // "&&"
  tail_and_And_Leave,
  // "!"
  tail_not_Leave,
  // "<>"
  tail_lessThan_GreaterThan_Leave,
  // "=="
  tail_equality_Equality_Leave,
  // ">"
  tail_greaterThan_Leave,
  // "<"
  tail_lessThan_Leave,
  // ">="
  tail_greaterThan_Equality_Leave,
  // "<="
  tail_lessThan_Equality_Leave,
  // "select"
  tail_selectLeave,
  // #
  tail_startEndLeave
} SyntaxType;
typedef enum __FunctionType {
  // <ssql_stmt> -> <create_stmt>
  deri___ssql_stmt__create_stmt_1,
  // <ssql_stmt> -> <insert_stmt>
  deri___ssql_stmt__insert_stmt_2,
  // <ssql_stmt> -> <delete_stmt>
  deri___ssql_stmt__delete_stmt_3,
  // <ssql_stmt> -> <query_stmt>
  deri___ssql_stmt__query_stmt_4,
  // <create_stmt> -> "create" "table" id "(" <decl_list> ")" ";"
  deri___create_stmt__decl_list_5,
  // <decl_list> -> <decl> <decl_listpi>
  deri___decl_list__decl__decl_listpi_6,
  // <decl_listpi> -> "," <decl> <decl_listpi>
  deri___decl_listpi__decl__decl_listpi_67,
  // <decl_listpi> -> epsilon
  deri___decl_listpi__epsilon_7,
  // <decl> -> id "int" <default_spec>
  deri___decl__default_spec_8,
  // <decl> -> "primary" "key" "(" <column_list> ")"
  deri___decl__column_list_9,
  // <default_spec> -> "default" "=" sexpr
  deri___default_spec__sexpr_10,
  // <default_spec> -> epsilon
  deri___default_spec__epsilon_11,
  // <sexpr> -> <smulti> <sexpr_pi>
  deri___sexpr__smulti__sexpr_pi_12,
  // <splus> -> "+" <smulti>
  deri___splus__plus_smulti_14,
  // <splus> -> "-" <smulti>
  deri___splus__minus_smulti_15,
  // <sexpr_pi> -> <splus> <sexpr_pi>
  deri___sexpr_pi__splus__sexpr_pi_70,
  // <sexpr_pi> -> epsilon
  deri___sexpr_pi__epsilon_71,
  // <smulti> -> <sunit> <smultiOpt>
  deri___smulti__sunit__smultiOpt_17,
  // <smultiOpt> -> "*" <sunit> <smultiOpt>
  deri___smultiOpt__multi_sunit__smultiOpt_18,
  // <smultiOpt> -> "/" <sunit> <smultiOpt>
  deri___smultiOpt__div_sunit__smultiOpt_19,
  // <smultiOpt> -> epsilon
  deri___smultiOpt__epsilon_20,
  // <sunit> -> number
  deri___sunit__number_21,
  // <sunit> -> "-" <sunit>
  deri___sunit__minus_sunit_22,
  // <sunit> -> "+" <sunit>
  deri___sunit__plus_sunit_23,
  // <sunit> -> "(" <sexpr> ")"
  deri___sunit__brucket_sexpr_24,
  // <insert_stmt> -> "insert" "into" id "(" <column_list> ")" "values" "(" <value_list> ")" ";"
  deri___insert_stmt__column_list__value_list_28,
  // <value_list> -> <wexpr> <value_listpi>
  deri___value_list__sexpr__value_listpi_29,
  // <value_listpi> -> "," <wexpr> <value_listpi>
  deri___value_listpi__sexpr__value_listpi_30,
  // <value_listpi> -> epsilon
  deri___value_listpi__epsilon_31,
  // <delete_stmt> -> "delete" "from" id <where_clause> ";"
  deri___delete_stmt__where_clause_32,
  // <where_clause> -> "where" <disjunct>
  deri___where_clause__disjunct_33,
  // <where_clause> -> epsilon
  deri___where_clause__epsilon_34,
  // <disjunct> -> <conjunct> <disjunct_pi>
  deri___disjunct__conjunct__disjunct_pi_35,
  // <disjunct_pi> -> "||" <conjunct> <disjunct_pi>
  deri___disjunct_pi__conjunct__disjunct_pi_36,
  // <disjunct_pi> -> epsilon
  deri___disjunct_pi__epsilon_37,
  // <conjunct> -> <bool> <conjunct_pi>
  deri___conjunct__bool__conjunct_pi_38,
  // <conjunct_pi> -> "&&" <bool> <conjunct_pi>
  deri___conjunct_pi__bool__conjunct_pi_39,
  // <conjunct_pi> -> epsilon
  deri___conjunct_pi__epsilon_40,
  // <bool> -> "!" <bool>
  deri___bool__not_bool_42,
  // <bool> -> <comp>
  deri___bool__comp_43,
  // <comp> -> <wexpr> <rop> <wexpr>
  deri___comp__wexpr__rop__wexpr_44,
  // <rop> -> "<>"
  deri___rop__lessgreater_58,
  // <rop> -> "=="
  deri___rop__equalequal_59,
  // <rop> -> ">"
  deri___rop__greater_60,
  // <rop> -> "<"
  deri___rop__less_61,
  // <rop> -> ">="
  deri___rop__greaterequal_62,
  // <rop> -> "<="
  deri___rop__lessequal_63,
  // <rop> -> epsilon
  deri___rop__epsilon_80,
  // <wexpr> -> <wmulti> <wexpr_pi>
  deri___wexpr__wmulti__wexpr_pi_45,
  // <wexpr> -> epsilon
  deri___wexpr__epsilon_81,
  // <wplus> -> "+" <wmulti>
  deri___wplus__plus_wmulti_46,
  // <wplus> -> "-" <wmulti>
  deri___wplus__minus_wmulti_47,
  // <wexpr_pi> -> <wplus> <wexpr_pi>
  deri___wexpr_pi__wplus__wexpr_pi_72,
  // <wexpr_pi> -> epsilon
  deri___wexpr_pi__epsilon_73,
  // <wmulti> -> <wunit> <wmultiOpt>
  deri___wmulti__wunit__wmultiOpt_49,
  // <wmultiOpt> -> "*" <wunit> <wmultiOpt>
  deri___wmultiOpt__multi_wunit__wmultiOpt_50,
  // <wmultiOpt> -> "/" <wunit> <wmultiOpt>
  deri___wmultiOpt__div_wunit__wmultiOpt_51,
  // <wmultiOpt> -> epsilon
  deri___wmultiOpt__epsilon_52,
  // <wunit> -> number
  deri___wunit__number_53,
  // <wunit> -> iden
  deri___wunit__iden_54,
  // <wunit> -> "-" <wunit>
  deri___wunit__minus_wunit_55,
  // <wunit> -> "+" <wunit>
  deri___wunit__plus_wunit_56,
  // <wunit> -> "(" <disjunct> ")"
  deri___wunit__brucket_disjunct_57,
  // <query_stmt> -> "select" <select_list> "from" id <where_clause> ";"
  deri___query_stmt__select_list__where_clause_64,
  // <select_list> -> <column_list>
  deri___select_list__column_list_65,
  // <select_list> -> "*"
  deri___select_list__star_66,
  // <column_list> -> id <column_pi>
  deri___column_list__column_pi_25,
  // <column_pi> -> "," id <column_pi>
  deri___column_pi__comma_column_pi_26,
  // <column_pi> -> epsilon
  deri___column_pi__epsilon_27,
  // BOUNDARY
  DERI_UMI_BOUNDARY,
  // LeavePoint "create"
  umi_create,
  // LeavePoint "table"
  umi_table,
  // LeavePoint iden
  umi_iden,
  // LeavePoint "("
  umi_leftParentheses_,
  // LeavePoint ")"
  umi_rightParentheses_,
  // LeavePoint ";"
  umi_semicolon_,
  // LeavePoint ","
  umi_comma_,
  // LeavePoint null
  umi_epsilon,
  // LeavePoint "int"
  umi_int,
  // LeavePoint "primary"
  umi_primary,
  // LeavePoint "key"
  umi_key,
  // LeavePoint "default"
  umi_default,
  // LeavePoint "="
  umi_equality_,
  // LeavePoint "+"
  umi_plus_,
  // LeavePoint "-"
  umi_minus_,
  // LeavePoint "*"
  umi_multiply_,
  // LeavePoint "/"
  umi_divide_,
  // LeavePoint number
  umi_number,
  // LeavePoint "insert"
  umi_insert,
  // LeavePoint "into"
  umi_into,
  // LeavePoint "values"
  umi_values,
  // LeavePoint "delete"
  umi_delete,
  // LeavePoint "from"
  umi_from,
  // LeavePoint "where"
  umi_where,
  // LeavePoint "||"
  umi_or_Or_,
  // LeavePoint "&&"
  umi_and_And_,
  // LeavePoint "!"
  umi_not_,
  // LeavePoint "<>"
  umi_lessThan_GreaterThan_,
  // LeavePoint "=="
  umi_equality_Equality_,
  // LeavePoint ">"
  umi_greaterThan_,
  // LeavePoint "<"
  umi_lessThan_,
  // LeavePoint ">="
  umi_greaterThan_Equality_,
  // LeavePoint "<="
  umi_lessThan_Equality_,
  // LeavePoint "select"
  umi_select,
  // LeavePoint #
  umi_startEnd,
  // ERROR
  umi_errorEnd
} CFunctionType;

typedef int iPtr;
typedef std::string istr;
typedef std::ostream ios;
typedef std::ifstream ifile;
typedef std::vector<int> IntVec;
typedef std::vector<istr> StrVec;
typedef std::vector<ITable> TableVec;
typedef std::vector<IntVec> IntTable;
typedef std::vector<Token*> TokenList;
typedef std::vector<SyntaxType> SyntaxVector;
typedef std::vector<SyntaxVector> SyntaxDict;
typedef std::vector<SyntaxTreeNode*> SyntaxTreeList;
typedef std::stack<SyntaxType> SyntaxStack;
typedef std::pair<istr, int> PilePair;
typedef std::pair<TokenType, int> TokenPair;
typedef std::pair<SyntaxType, int> SyntaxPair;
typedef std::map<istr, int> TablePileDictionary;
typedef std::map<TokenType, int> TokenDictionary;
typedef std::map<SyntaxType, int> SyntaxDictionary;
typedef CandidateFunction*** ParserMap;
typedef SyntaxTreeNode* (*iHandle)(SyntaxTreeNode*, void*, CFunctionType, SyntaxType, istr);

class Token {
public:
  //函数作用： 构造函数
  //参数列表： N/A
  //返 回 值： N/A
  Token();

  //函数作用： 拷贝构造函数
  //参数列表：
  //    _other 拷贝源
  //返 回 值： N/A
  explicit Token(const Token&);

  // 词类型
  TokenType aType;
  // 命中行
  int aLine;
  // 命中列
  int aColumn;
  // 附加值
  int aTag;
  // 位置戳
  int indexOfCode;
  // 配对长
  int length;
  // 错误位
  int errorCode;
  // 错误码
  istr errorInfo;
  // 原信息
  istr detail;
}; /* Token */

class TokenStream {
public:
  //函数作用： 构造函数
  //参数列表： N/A
  //返 回 值： N/A
  TokenStream();

  //函数作用： 拷贝构造函数
  //参数列表：
  //    _other 拷贝源
  //返 回 值： N/A
  explicit TokenStream(const TokenStream&);

  //函数作用： 复位流
  //参数列表： N/A
  //返 回 值： N/A
  void Reset();

  //函数作用： 添加到流
  //参数列表：
  //      _org 待添加的token实例
  //返 回 值： N/A
  void Add(Token*);

  //函数作用： 得到流的长度
  //参数列表： N/A
  //返 回 值： int 单词流的长度
  int Length() const;

  //函数作用： 输出至标准输出流
  //参数列表：
  //        os 标准输出流
  //        ts 待输出单词流
  //返 回 值： ios& 标准输出流
  friend ios& operator<<(ios&, const TokenStream&);

  //函数作用： 取得下一个Token的指针
  //参数列表： N/A
  //返 回 值： Token* 下一Token指针
  Token* Next();

  //函数作用： 取得下一个句子的单词向量
  //参数列表： N/A
  //返 回 值： TokenList 下一句子单词向量
  TokenList NextSentence();

  //函数作用： 单词流的文本化函数
  //参数列表： N/A
  //返 回 值： istr 单词流的字符串表示
  istr ToString();

  // 流容器
  TokenList _tokenContainer;

private:
  // 流指针
  iPtr _iPointer;
  // 句指针
  iPtr _sPointer;
}; /* TokenStream */

class SyntaxTreeNode {
public:
  //函数作用： 构造函数
  //参数列表： N/A
  //返 回 值： N/A
  SyntaxTreeNode();

  //函数作用： 拷贝构造函数
  //参数列表：
  //    _other 拷贝源
  //返 回 值： N/A
  explicit SyntaxTreeNode(const SyntaxTreeNode&);

  //函数作用： 树的文本化函数
  //参数列表： N/A
  //返 回 值： istr 树的字符串表示
  istr ToString();

  // 绑定处理函数
  CandidateFunction* candidateFunction;
  // 子树向量
  SyntaxTreeList children;
  // 父指针
  SyntaxTreeNode* parent;
  // 命中语法结构类型
  SyntaxType nodeSyntaxType;
  // 命中token附加值
  istr nodeValue;
  // 命中产生式类型
  CFunctionType nodeType;
  // 节点名字
  istr nodeName;
  // 附加值
  double aTag;
  // 错误位
  int errorCode;
  // 错误码
  istr errorInfo;
  // 命中Token所在流
  TokenStream* mappedList;
  // 命中Token位置戳
  int mappedBegin;
  // 命中Token长度
  int mappedLength;

private:
  //函数作用： 树的递归遍历文本化
  //参数列表： N/A
  //返 回 值： istr 树的字符串表示
  void GetTree(istr&, SyntaxTreeNode*, int&);

  //函数作用： 树文本化过程的缩进计算
  //参数列表： N/A
  //返 回 值： istr 树的字符串表示
  istr DrawTree(SyntaxTreeNode*);
}; /* SyntaxTreeNode */

class CandidateFunction {
public:
  //函数作用： 构造函数
  //参数列表： N/A
  //返 回 值： N/A
  CandidateFunction();

  //函数作用： 拷贝构造函数
  //参数列表：
  //    _other 拷贝源
  //返 回 值： N/A
  CandidateFunction(const CandidateFunction&);

  //函数作用： 带产生式的构造函数
  //参数列表：
  //     _proc 特殊处理的函数指针
  //       _pt 产生式类型
  //返 回 值： N/A
  CandidateFunction(iHandle, CFunctionType);

  //函数作用： 绑定产生式函数和类型
  //参数列表：
  //     _proc 特殊处理的函数指针
  //       _pt 产生式类型
  //返 回 值： N/A
  void SetProc(iHandle, CFunctionType);

  //函数作用： 获得产生式函数
  //参数列表： N/A
  //返 回 值： iHandle 产生式的处理函数
  iHandle GetProc();

  //函数作用： 获得产生式函数
  //参数列表： N/A
  //返 回 值： CFunctionType 产生式类型
  CFunctionType GetType();

  //函数作用： 调用产生式处理函数
  //参数列表：
  //     _root 匹配树根节点
  //   _parser 匹配器指针
  //   _syntax 语法类型
  //   _detail 节点信息
  //返 回 值： iHandle 产生式的处理函数
  SyntaxTreeNode* Call(SyntaxTreeNode*, void*, SyntaxType, istr);

private:
  // 产生式特殊处理指针
  iHandle candidateProcesser = NULL;
  // 产生式类型
  CFunctionType candidateType;
};

class LL1SyntaxParserMap {
public:
  //函数作用： 带尺寸的构造器
  //参数列表：
  //       row 行长度
  //       col 列长度
  //返 回 值： N/A
  LL1SyntaxParserMap(int, int);

  //函数作用： 设置行类型
  //参数列表：
  //       row 行长度
  //      left 非终结符类型
  //返 回 值： N/A
  void SetRow(int, SyntaxType);

  //函数作用： 设置列类型
  //参数列表：
  //       col 行长度
  //     leave Token类型
  //返 回 值： N/A
  void SetCol(int, TokenType);

  //函数作用： 设置节点分析函数
  //参数列表：
  //       row 行指针
  //       col 列指针
  //      proc 候选式函数
  //返 回 值： N/A
  void SetCellular(int, int, CandidateFunction*);

  //函数作用： 设置节点分析函数
  //参数列表：
  //      left 非终结符类型
  //     leave Token类型
  //      proc 候选式函数
  //返 回 值： N/A
  void SetCellular(SyntaxType, TokenType, CandidateFunction*);

  //函数作用： 取得节点的处理函数
  //参数列表：
  //       row 行指针
  //       col 列指针
  //返 回 值： CandidateFunction* 候选式函数指针
  CandidateFunction* GetCFunction(int, int);

  //函数作用： 取得节点的处理函数
  //参数列表：
  //      left 非终结符类型
  //     leave Token类型
  // nilserver epsilon处理器
  //返 回 值： CandidateFunction* 候选式函数指针
  CandidateFunction* GetCFunction(SyntaxType, TokenType, iHandle);

private:
  // 行浮标
  int iRowCount;
  // 列浮标
  int iColCount;
  // 产生式左字典
  SyntaxDictionary iLeftNodes;
  // 产生式右字典
  TokenDictionary iNextLeaves;
  // LL1预测表
  ParserMap iParserMap;
};

class IBridgeProxy {
public:
  // 句子号
  int id;
  // 列通配符
  bool isStar;
  // 邂逅主键声明词次数
  int encounter;
  // 错误位
  bool errorBit;
  // 操作码
  DashType opCode;
  // 操作表
  istr opTable;
  // 条件子句的指针
  SyntaxTreeNode* condPtr;
  // 操作列向量
  StrVec Pi;
  // 列赋值向量
  IntVec ProxyPi;
  // 初始值向量
  TablePileDictionary DefaPi;
  // 条件列向量
  StrVec CondPi;
  // 主键列向量
  StrVec PrimaryPi;
}; /* IBridgeProxy */

class ITable {
public:
  //函数作用： 构造函数
  //参数列表：
  //       _id 表号
  //     _name 表名
  //       _pi 列向量
  //     _flag 成功标旗
  //返 回 值： N/A
  ITable(int, istr, StrVec, bool&);

  //函数作用： 清空表
  //参数列表： N/A
  //返 回 值： N/A
  void Clear();

  // 表号
  int id;
  // 表名
  istr name;
  // 列向量
  StrVec Pi;
  // 主键列向量
  StrVec PrimaryPi;
  // 缺省值向量
  IntVec DefaultPi;
  // 列名字典
  TablePileDictionary PiDict;
  // 横放的表
  IntTable table;
}; /* ITable */

// 用于输出Debug单词类型的字符串向量
const istr TokenOutputTable[] = {
  "unknown", "token_create", "token_table", "token_iden",
  "token_LeftParentheses", "token_RightParentheses",
  "token_Semicolon", "token_Comma", "epsilon",
  "token_int", "token_primary", "token_key",
  "token_default", "token_Equality", "token_Plus",
  "token_Minus", "token_Multiply", "token_Divide",
  "number", "token_insert", "token_into",
  "token_values", "token_delete", "token_from",
  "token_where", "token_Or_Or", "token_And_And",
  "token_Not", "token_LessThan_GreaterThan",
  "token_Equality_Equality", "token_GreaterThan",
  "token_LessThan", "token_GreaterThan_Equality",
  "token_LessThan_Equality", "token_select",
  "token_StartEnd", "other"
};

// 用于输出Debug语法树的字符串向量
const istr SyntaxOutputTable[] = {
  "case_ssql_stmt", "case_create_stmt", "case_decl_list", "case_decl_listpi",
  "case_decl", "case_default_spec", "case_sexpr", "case_sexpr_pi", "case_splus",
  "case_smulti", "case_smultiOpt", "case_sunit", "case_insert_stmt",
  "case_value_list", "case_value_listpi", "case_delete_stmt", "case_where_clause",
  "case_disjunct", "case_disjunct_pi", "case_conjunct", "case_conjunct_pi",
  "case_bool", "case_comp", "case_rop", "case_wexpr", "case_wexpr_pi",
  "case_wplus", "case_wmulti", "case_wmultiOpt", "case_wunit",
  "case_query_stmt", "case_select_list", "case_column_list", "case_column_pi",
  "Unknown",  "tail_createLeave", "tail_tableLeave", "tail_idenLeave",
  "tail_leftParentheses_Leave", "tail_rightParentheses_Leave", "tail_semicolon_Leave", 
  "tail_comma_Leave", "epsilonLeave", "tail_intLeave", "tail_primaryLeave", "tail_keyLeave",
  "tail_defaultLeave", "tail_equality_Leave", "tail_plus_Leave", "tail_minus_Leave",
  "tail_multiply_Leave", "tail_divide_Leave", "numberLeave", "tail_insertLeave",
  "tail_intoLeave", "tail_valuesLeave", "tail_deleteLeave", "tail_fromLeave",
  "tail_whereLeave", "tail_or_Or_Leave", "tail_and_And_Leave", "tail_not_Leave",
  "tail_lessThan_GreaterThan_Leave", "tail_equality_Equality_Leave", "tail_greaterThan_Leave",
  "tail_lessThan_Leave", "tail_greaterThan_Equality_Leave", "tail_lessThan_Equality_Leave",
  "tail_selectLeave", "tail_startEndLeave"
};

#endif /* ___PILE */


/********************************************************
Author:      编译原理计应1班12小组
Date:        2014-11-30
Description: 声明各类需要用到的数据结构
*********************************************************/

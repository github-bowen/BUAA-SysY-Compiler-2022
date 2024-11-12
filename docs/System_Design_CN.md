# 设计文档

## 一. 参考编译器介绍

我参考的编译器是教材第 17 章和编译实验网站附件中 PL/0简单编译系统的结构。

PL/0为编译——解释执行程序。编译部分生成的目标代码为PCODE指令。由于我前期没有决定好目标代码，选择参考编译器时主要参考的是前端（词法分析、语法分析、语义分析生成中间代码）的架构。PL/0采用一遍扫描，以语法分析为核心，在语法分析过程中调用词法分析程序取单词，同时也进行语义分析，最终生成目标代码。同时，它进行语义检查，发现错误后就转出错处理程序。

词法分析中，`getsym`子程序用于跳过所有空格读取单词，并将单词符号放入`wsym`或`ssym`中；

语法分析为核心的主程序，PL/0在语法分析过程中调用其他子程序，语法分析中有多个递归子程序；

出错处理中，```error(n : integer )```用于输出错误信息；

符号表管理中，使用全局变量：`table : array[0..txmax] of
                record
                  name : alfa;
                  case kind: objecttyp of
                    constant : (val:integer );
                    variable,prosedure: (level,adr: integer )
                end;` 作为符号表。

## 二. 编译器总体设计

编译器文件组织如下（只显示C++源代码文件`.cpp`，`.h`）

```yacas
.
├── _debug.h
├── ErrorHandler.cpp
├── ErrorHandler.h
├── exceptions
│   ├── FileIOError.h
│   └── ParseEndError.h
├── ICTranslator.cpp
├── ICTranslator.h
├── item
│   ├── ErrorType.h
│   ├── GrammarItem.h
│   ├── ic
│   │   ├── ICEntry.cpp
│   │   ├── ICEntry.h
│   │   ├── ICEntryType.h
│   │   ├── ICItemArray.cpp
│   │   ├── ICItemArray.h
│   │   ├── ICItem.cpp
│   │   ├── ICItemFunc.cpp
│   │   ├── ICItemFunc.h
│   │   ├── ICItem.h
│   │   ├── ICItemImm.h
│   │   ├── ICItemLabel.h
│   │   ├── ICItemString.h
│   │   ├── ICItemType.h
│   │   ├── ICItemVar.cpp
│   │   └── ICItemVar.h
│   ├── reg.h
│   ├── ReservedWord.h
│   ├── Symbol.h
│   ├── symbolTable
│   │   ├── ConstValue.h
│   │   ├── ReferencedEntry.h
│   │   ├── SymbolTable.cpp
│   │   ├── SymbolTableEntry.cpp
│   │   ├── SymbolTableEntry.h
│   │   ├── SymbolTableEntryType.h
│   │   └── SymbolTable.h
│   ├── Token.h
│   ├── variable
│   │   ├── Array1Const.h
│   │   ├── Array1.h
│   │   ├── Array2Const.h
│   │   ├── Array2.h
│   │   ├── FuncParam.h
│   │   ├── FunctionOfInt.h
│   │   ├── FunctionOfVoid.h
│   │   ├── VarConst.h
│   │   └── Var.h
│   ├── WhileBlock.cpp
│   └── WhileBlock.h
├── Lexer.cpp
├── Lexer.h
├── main.cpp
├── MipsTranslator.cpp
├── MipsTranslator.h
├── Parser.cpp
├── Parser.h
└── tree
    ├── ErrorNode.h
    ├── Node.cpp
    └── Node.h
```

该编译器主要分为词法分析、语法分析并建立抽象语法树AST、错误处理并中间代码、生成MIPS目标代码这几部分：

```cpp
int main() {
    if (!input.is_open())
        throw FileIOError("ERROR IN OPENING FILE 'testfile.txt'!");
    if (!normalOutput.is_open())
        throw FileIOError("ERROR IN OPENING FILE 'printAll.txt'");

    // lexical analyzer 词法分析
    auto *lexer = new Lexer();
    std::vector<Token *> &tokens = lexer->parse();

    // 语法分析、生成抽象语法树
    auto *parser = new Parser(tokens);
    Node *root = parser->parse();

    delete lexer;
    delete parser;

#ifdef STAGE_GRAMMAR_ANALYSIS
    grammarItemOutput(root);
    normalOutput << std::flush;
#endif

    // error handler 错误处理、生成中间代码
    auto *errorHandler = new ErrorHandler(root);
    errorHandler->check();
    
    ICTranslator *icTranslator = errorHandler->icTranslator;

#ifdef STAGE_ERROR_HANDLING
    auto it = errorLog.begin();
    while (it != errorLog.end()) {
        errorOutput << it->first << " " << it->second << "\n";
        ++it;
    }
    errorOutput << std::flush;
#endif

#ifdef STAGE_INTERMEDIATE_CODE
    icTranslator->output();
#endif

    // 生成 MIPS 汇编代码
    auto *mipsTranslator = new MipsTranslator(icTranslator);
#ifdef STAGE_MIPS
    mipsTranslator->translate();
#endif

    input.close();
    normalOutput.close();
    errorOutput.close();

    delete errorHandler;
    delete root;

    return 0;
}
```

采用面向对象的方式设计：

- 词法分析调用`Lexer`对象的`parse()`方法，返回得到单词串`tokens`
- 语法分析调用`Parser`对象的`parse(tokens)`方法，传入单词串`tokens`，返回抽象语法树的根节点`root`
- 错误处理、生成中间代码调用`ErrorHandler`对象的`check()`方法，执行完后中间代码就存储在其属性`errorHandler->icTranslator`中
- 生成MIPS汇编代码时，调用`MipsTranslator`对象的`translate()`方法，在翻译时一并输出mips代码

## 三. 词法分析设计

### 编码前的设计

`Lexer.cpp/Lexer.h`

- 词法分析器，读入源程序符号串，提取单词，返回单词串
- 属性：
  - `std::ifstream &input`：输入文件
  - `std::ofstream &output`：输出文件
  - `std::string token`：当前读入的符号串
  - `bool inMultLineComment`：是否在多行注释中，若是则跳过所有读到的字符
  - `Symbol symbol`：当前最新读入单词的类别码
- 方法：
  - `parse()`：开始执行词法分析
  - `getSym()`：读入一个单词
  - `print()`：输出当前读到的单词和类别码（按照词法分析作业要求的格式）
  - `parseLine`：对一行执行词法分析，在`parse()`中被调用

`ReservedWord.h`

- 保留字枚举类

`Symbol.h`

- 类别码枚举类，其中一部分元素与保留字枚举类中元素相同，并通过`const std::map`建立映射关系

### 编码完成之后的修改

- 每次读完一个单词后，缓存`token`忘记清空
- `/`、`/*`和`//`需要进一步预读来确定单词
- C++ 语法方面：`enum class` 没有自带的 `to_string` 方法，要输出的话需要使用 C++ 提供的 STL `std::map<Symbol, std::string> symbol2outputString`

## 四. 语法分析设计

### 编码前的设计

`Parser.cpp/Parser.h`

- 词法分析器，读入源程序符号串，提取单词，返回单词串
- 属性：
  - `std::vector<Token *> &tokens`：词法分析后得到的单词串
  - `int tokenPos`：当前解析到的单词的下标
  - `const int tokenLength`：单词数量
  - `Token *curToken`：类似词法分析中的`std::string token`，`curToken`为当前读到的单词
- 方法：
  - `nextItem()`：`tokenPos`自增，并读入下一个单词到 `curToken`中
  - `parse()`：递归下降入口
  - `parse_XXX()`：语法成分 XXX 的递归下降子程序，如`parse_CompUnit()`、`parse_Decl()`
  - `semicn_before_assign()`：判断接下来先出现分号还是等号，用于在解析`Stmt`时判断先出现`[Exp] ';'`还是`LVal '=' Exp ';'  | LVal '=' 'getint''('')'';'`。

`GrammarItem.h`

- 语法成分枚举类，同样用`std::map<GrammarItem, std::string>`将枚举关键字转换为字符串输出

`Token.h`

- 用于存储词法分析后得到的单词串中的单词。该部分在词法分析阶段没有加上，为了语法分析以及后续错误处理的方便，这里`Token`除了保存单词字符串外，还保存了其对应的类别码`Symbol`和在源程序中所在的行号`lineNumber`。

`Node.cpp/Node.h`

- 抽象语法树中的结点类

- 属性

  - `std::vector<Node *> children`：存储多叉树的孩子结点
  - `Node *parent`：存储父亲结点（实际上没用到这个域）
  - `int depth`：当前节点的深度（实际上也没用到）
  - `bool isLeaf`：是否是叶子结点（实际上可以用`token != nullptr`替代）
  - `GrammarItem grammarItem`：当前节点的语法成分
  - `Token *token`：保存对应的单词

  > 注：`grammarItem`是中间结点（非终结符）用的，用来存储非终结符的语法成分；`token`是给单词（终结符）用的，用来存储终结符的值、类别以及行号等信息。

- 方法

  与数据结构中树的基本操作类似

由于需要建立抽象语法树，在递归下降的时候需要把当前语法成分读入。因此，在进入每个递归子程序时，首先通过`Node *xxx= new Node(GrammarItem::XXX, depth);`创建当前节点，依次迭代建立语法树。

### 编码完成之后的修改

- 左递归建立语法树时存在问题。如：对`AddExp → MulExp | AddExp ('+' | '−') MulExp`，一开始直接将其转化为EBNF表示`AddExp → MulExp {('+' | '−') MulExp}`，这与作业中要求的输出格式不一致；同时，左递归在建立语法树时比较绕。同上例子，读入当前语法成分，在发现其右侧仍然有语法成分时，先将左侧（当前成分）向上打包一层，再继续重复判断。这样可以建立与文法一致的语法树。如下所示：

  ```cpp
  /* AddExp → MulExp | AddExp ('+' | '−') MulExp FIXME: 左递归 */
  /* 改写后: AddExp → MulExp {('+' | '−') MulExp} */
  Node *Parser::parse_AddExp(int depth) {
      Node *current = new Node(GrammarItem::AddExp, depth);
      Node *child = this->parse_MulExp(depth + 1);
      current->addChild(child);
      child->setParent(current);
      while (this->curToken->symbol == Symbol::PLUS
             || this->curToken->symbol == Symbol::MINU) {
          // 在发现右侧还有成分时，先将左侧向上打包一层, FIXME: 这里depth有误
          Node *temp = new Node(GrammarItem::AddExp, depth);
          temp->addChild(current);
          current->setParent(temp);
          current = temp;
          // ('+' | '−')
          current->addChild(new Node(this->curToken, current, depth + 1));
          this->nextItem();
          // MulExp
          child = this->parse_MulExp(depth + 1);
          current->addChild(child);
      }
      // TODO: 递归修改 current 的depth
      return current;
  }
  ```

  在这种方式下，节点所存储的深度`depth`就不一定准确了，但由于后面没有用到，就没有继续修改。

- 读入单词函数`nextItem()`存在问题：一开始是如果读到结尾就抛出异常`ParseEndError`，后面发现这种方式十分不便，改成了读到末尾就直接返回。

## 五. 错误处理设计

> 代码架构从这部分开始乱起来了（×）

### 编码前的设计

`Parser.cpp/Parser.h`

一部分错误需要在语法分析时就解决，否则语法树的结构会有问题。如：缺少右括号类的错误，需要在原来右括号的位置补上一个"错误节点"，之后在`ErrorHandler`处理时，读到该错误节点就输出错误信息。例如，缺少`)`的处理过程如下：

```cpp
// ')'
if (this->curToken->symbol == Symbol::RPARENT) {
    funcDef->addChild(new Node(this->curToken, funcDef, depth + 1));
    this->nextItem();
} else {  // FIXME: j => ErrorType::MissingRPARENT )
    int formerLineNum = this->tokens[tokenPos - 1]->lineNumber;
    funcDef->addChild(new ErrorNode(ErrorType::MissingRPARENT, formerLineNum));
}
```

`ErrorHandler.cpp/ErrorHandler.h`

- 错误处理器，处理除了上述错误以外的所有错误

- 属性：

  - `Node *root`：语法树的根节点，即`CompUnit`
  - `    SymbolTable *currentTable;`：当前符号表

- 方法：

  - `check_XXX()`：再像`Parser`一样进行一次递归下降分析，找出每个语法成分的错误

  - `findParamError()`：找到函数形参与实参之间的错误（两种：个数不匹配，类型不匹配）

    ```cpp
    // 检查函数定义 definedEntry 和 函数调用 calledEntry 中的下面两个错：
    // FIXME: d => ErrorType::ParamNumNotMatch
    // FIXME: e => ErrorType::ParamTypeNotMatch
    // 返回值表示是否有错
    bool ErrorHandler::findParamError(SymbolTableEntry *definedEntry,
                                      std::vector<SymbolTableEntry *> *calledEntry,
                                      int lineNum) {
        auto size = calledEntry->size();
        if (definedEntry->funcParamsNum() != size) {
            errorLog.insert({lineNum, errorType2string.find(
                    ErrorType::ParamNumNotMatch)->second});
            return true;
        }
        std::vector<FuncParam *> *definedFuncParams = definedEntry->getFuncParams();
        for (auto i = 0; i < size; ++i) {
            // hasSameType(SymbolTableEntry *realParam, FuncParam *funcParam)
            bool typeSame = SymbolTableEntry::hasSameType((*calledEntry)[i],
                                                          (*definedFuncParams)[i]);
            if (!typeSame) {
                errorLog.insert({lineNum, errorType2string.find(
                        ErrorType::ParamTypeNotMatch)->second});
                return true;
            }
        }
        return false;
    }
    ```

  - `check_FormatString`，检查字符串：

    ```cpp
    bool ErrorHandler::check_FormatString(Node *node, int *formatNum) {
        *formatNum = 0;
        std::string s = node->getToken()->value;
        bool hasIllegalChar = false;
        for (int i = 1; i < s.size() - 1; ++i) {
            // check <FormatChar>
            auto now = s[i];
            auto next = s[i + 1];  // i < length - 1保证了不越界
            if (now == '%' && (i >= s.size() - 2 || next != 'd')) {
                hasIllegalChar = true;
            } else if (now == '%' && next == 'd') {
                *formatNum = *formatNum + 1;
                ++i;  // 从后两个开始判断
            } else if (now == '\\' && next == 'n') {
                ++i;
            } else if (now == '\\' && next != 'n') {
                hasIllegalChar = true;
            } else {
                int ascii = (int) ((unsigned char) now);
                if (!(ascii == 32 || ascii == 33 || (ascii >= 40 && ascii <= 126))) {
                    hasIllegalChar = true;
                }
            }
        }
        return hasIllegalChar;
    }
    ```

`SymbolTable.cpp/SymbolTable.h`

- 符号表
- 属性：
  - `bool isRoot;`：是否是顶层符号表
  - `SymbolTable *parent`：上一级符号表
  - `std::vector<SymbolTable *> children`：下一级符号表
  - `std::map<std::string, SymbolTableEntry *> name2symbolTableEntry`：保存当前符号表中的所有表项
- 方法：
  - `nameExistedInCurrentTable`：名字是否在当前符号表已经定义
  - `nameExistedInAllTables`：名字是否在全局已经定义
  - `getEntryByNameFromAllTables`：根据名字找到定义的变量

`SymbolTableEntry.cpp/SymbolTableEntry.h`

- 符号表

- 属性：

  - `SymbolTableEntryType type`：符号表项的类型，分为变量、常量、一维数组、一维数组常量、二维数组......

  ```cpp
  Node *node;
  const bool isFuncFParam;  // 函数的形参
  unsigned int defLineNum;
  
  Var *var{nullptr};
  VarConst *varConst{nullptr};
  
  Array1 *array1{nullptr};
  Array1Const *array1Const{nullptr};
  Array2 *array2{nullptr};
  Array2Const *array2Const{nullptr};
  
  FunctionOfInt *functionOfInt{nullptr};
  FunctionOfVoid *functionOfVoid{nullptr};
  
  // 引用表项
  ReferencedEntry *tempEntry{nullptr};  // 该类型不应该保存到符号表 !!!
  // 其对应的真正定义的表项
  SymbolTableEntry *definedEntry{nullptr};
  ```

  为了方便，这里将所有数据类型对应的类都加到了符号表项类的属性中（以指针的形式）。其中，这里的`ReferencedEntry *tempEntry`用于表示引用变量，而不是实际被定义的变量，这个时候其`SymbolTableEntry *definedEntry`必须要赋值，保证能通过该表项找到实际定义点。

  另外，这些数据类型都有相应的类定义对应。

### 编码完成之后的修改

由于指针的滥用导致出现了大量的`Segmentation fault`错误，这里一修改后的`Array2`二维数组的构造和析构函数为例说明：

```cpp
class Array2 {
public:
    const int d1;  // -1 表示是函数形参
    const int d2;
    int **values;

    Array2(int d1, int d2) : d1(d1), d2(d2) {
        values = new int *[d1 + 1];
        for (int i = 0; i < d1 + 1; i++) {
            values[i] = new int[d2 + 1]();
        }
    }

    ~Array2() {
        for (int i = 0; i < d1 + 1; i++) {
            delete[] values[i];
        }
        delete[] values;
        values = nullptr;
    }
};
```

在保证不会出现空指针错误的同时也防止内存泄漏。

## 六. 代码生成设计

### 编码前的设计

`ICTranslator.cpp/ICTranslator.h`

- 中间代码翻译器

- 属性：

  - `std::vector<ICEntry *> *mainEntries`：存储`main`函数中的中间代码（这里我把全局量定义和函数定义放在别的地方）
  - `std::map<int, std::string *> *id2allPureString`：根据id找到所有的“纯字符串”（不含有%d），为了在生成MIPS代码时方便
  - `std::map<std::string *, ICItemFunc *> *name2icItemFunc;`：根据函数名找到函数定义

- 方法：

  - `translate_XXX`：翻译某个过程，如`translate_ConstVarDef`、`translate_printf`、`translate_FuncCall`等等

  - 在翻译不同东西时，需要判断当前是在main函数中还是在自定义函数中：

    ```cpp
    if (currentFunc != nullptr) {
       currentFunc->entries->push_back(new ICEntry(xxx));
    } else {
       mainEntries->push_back(new ICEntry(xxx));
    }
    ```

`MipsTranslator.cpp/MipsTranslator.h`

- MIPS代码生成器

- 属性：

  下面是根据id找到不同数据类型的变量或数组在内存中的位置或首地址：

  ```cpp
  /**
   * main函数专用：存局部变量
   */
  std::map<int, int> localVarId2mem;  // id 为负
  
  std::map<int, int> tempVarId2mem;  // id 为正
  
  std::map<int, int> localArrayId2mem;  // id 为正
  
  std::map<int, int> tempArrayId2mem;  // id 为负
  
  /**
   * 自定义函数专用：存局部变量, offset 相对于当前函数栈的 $sp
   */
  std::map<int, int> localVarId2offset;  // id 为负
  
  std::map<int, int> tempVarId2offset;  // id 为正
  
  std::map<int, int> localArrayId2offset;  // id 为正
  
  std::map<int, int> tempArrayId2offset;  // id 为负
  
  //---------------------
  
  std::map<Reg, bool> regUsage;
  
  std::map<Reg, int> reg2id;
  
  std::map<int, int> funcFVarParamId2offset;
  
  std::map<int, int> funcFArrayParamId2offset;
  ```

生成过程：

我这里直接在`ErrorHandler`类中进行错误处理的同时翻译生成中间代码，造成了较高的代码耦合度：即进行错误处理的同时，一边利用之前建立的符号表，同时加入中间代码对应的数据类型`ICxxx`（IC表示`intermediate code`，中间代码），有`ICItem`、`ICItemFunc`、`ICItemArray`等等；

得到中间代码后，在`MipsTranslator`类中翻译生成目标代码，先翻译位于`.data`段的全局量（包括字符串）

```cpp
// 全局变量、常量
while (mainStream->at(i)->entryType != ICEntryType::MainFuncStart) {
    ICEntry *defEntry = mainStream->at(i);
//        assert(defEntry->isGlobalVarOrConstDef());
    translate_GlobalVarOrArrayDef(defEntry);
    i++;
}

// 字符串片段(纯字符串部分)定义
mipsOutput << "\n# string tokens: \n";
for (const auto &item: *id2allPureString) {
    const int id = item.first;
    const std::string *str = item.second;
    mipsOutput << strId2label(id) << ":  .asciiz   \"" << *str << "\"\n";
}
```

紧接着翻译主函数：

```cpp
// 主函数部分
assert(mainStream->at(i)->entryType == ICEntryType::MainFuncStart);
i++;
mipsOutput << "\n\n.text 0x00400000\n\n# main function\n";
while (i < mainEntryNum) {
#ifdef MIPS_DEBUG
    mipsOutput << std::flush;
#endif
    ICEntry *entry = mainStream->at(i);
    ICItem *op1 = entry->operator1, *op2 = entry->operator2, *op3 = entry->operator3;
    const int opNum = entry->opNum;
    switch (entry->entryType) {
        case ICEntryType::VarDefine: {  // 局部变量
            // ......
        }
        // ......
    }
}
```

### 编码完成之后的修改

这里的bug非常多，首先是数组的，数组在传参时的几种不同类型：

- 在main函数中定义的数组在main函数中压栈
- 在main函数中定义的数组在自定义函数中压栈
- 在自定义函数中定义的数组在自定义函数中压栈
- 在全局定义的数组在main函数中压栈
- 在全局定义的数组在自定义函数中压栈

不同情况下，要存的东西不同，所选择的基地址标准不同相对于`$sp`的偏移的含义也不同

## 七. 代码优化设计

由于时间问题，只做了**常量折叠**，如：

`lw`如果是常数就直接替换成`li`

```cpp
// 非 LVal
if (var->isGlobal) {
    la(reg, var->toString());
    lw(reg, 0, reg);
    return;
}
if (var->isConst) {
    li(reg, var->value);
    return;
}
if (isFuncFParam(var)) {
    addr = funcFVarParamId2offset.find(var->varId)->second;
    if (whenPushingParamsRecursively) {
        addr += 30000;
    }
    lw(reg, addr, Reg::$sp);
    return;
}
if (inSelfDefinedFunc) {
    if (var->isTemp) {
        addr = tempVarId2offset.find(var->tempVarId)->second;
    } else {
        addr = localVarId2offset.find(var->varId)->second;
    }
    if (whenPushingParamsRecursively) {
        addr += 30000;
    }
    lw(reg, addr, Reg::$sp);
} else {
    if (var->isTemp) {
        addr = tempVarId2mem.find(var->tempVarId)->second;
    } else {
        addr = localVarId2mem.find(var->varId)->second;
    }
    lw(reg, addr, Reg::$zero);
}
```


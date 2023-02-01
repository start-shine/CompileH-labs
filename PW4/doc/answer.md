# task1
## 问题1-1

### main.cpp

1. 定义`print_help()`函数，用于打印帮助信息。
    ```c++
    void print_help(const std::string& exe_name) {
    std::cout << "Usage: " << exe_name
                << " [ -h | --help ] [ -p | --trace_parsing ] [ -s | --trace_scanning ] [ -emit-ast ]"
                << " <input-file>"
                << std::endl;
    }
    ```         


2. main函数的主体部分，首先声明了三个类的变量，分别用于和`demoDriver.cpp`,`ErrorReporter.cpp`,`SyntaxTreePrinter`联系，可以用这些文件中定义的相关函数实现功能。
    ```c++
    demoDriver driver;
    SyntaxTreePrinter printer;
    ErrorReporter reporter(std::cerr);
    ```         


3. 根据输入的字符串设置`driver`的值。
    ```c++
    if (argv[i] == std::string("-h") || argv[i] == std::string("--help")) {
        print_help(argv[0]); 
        return 0;
    }
    else if (argv[i] == std::string("-p") || argv[i] == std::string("--trace_parsing"))
        driver.trace_parsing = true; 
    else if (argv[i] == std::string("-s") || argv[i] == std::string("--trace_scanning"))
        driver.trace_scanning = true; 
    else if (argv[i] == std::string("-emit-ast"))
        print_ast = true;
    else {
        filename = argv[i];
    }
    ```
    判断输入的字符串是否是如下参数。
    参数`-h`或`--help`：打印帮助信息后退出。
    参数`-p`或`--trace_parsing`：`driver.trace_parsing`值用于标识是否分析语法，真值表示进行语法分析。
    参数`-s`或`--trace_scanning`：`driver.trace_scanning`值用于标识是否分析词法，真值表示进行词法分析。
    参数`-emit-ast`： 通过AST（Abstract Syntax Tree）复原代码，并输出。
    否则，输入的字符串是文件名。    


4. 对已经赋值的`driver`，调用`demoDriver.cpp`中的`parse`进行语法分析。
    ```c++
    auto root = driver.parse(filename);
    if(print_ast)
        root->accept(printer);// 通过AST（Abstract Syntax Tree）复原代码，并输出。
    ``` 

### demoDriver.h
提供`demoDriver`类的定义。
- 分析树的根节点
    `SyntaxTree::Node *root = nullptr;`
    
- 输入流
    `std::ifstream instream;`

- 文件名
    `std::string file;`

- 词法分析器
    `demoFlexLexer lexer;`     

- 词法扫描和分析
    ```c++
    void scan_begin();
    void scan_end();
    bool trace_scanning;
    ```

- 语法分析器
    `SyntaxTree::Node *parse(const std::string &f);`

- 是否进行语法分析
    `bool trace_parsing;`

- 提供报错信息
    ```c++
    void error(const yy::location &l, const std::string &m);
    void error(const std::string &m);
    ```

### demoDriver.cpp

- 词法和语法分析
    ```c++
    SyntaxTree::Node* demoDriver::parse(const std::string &f)
    {
        file = f; 
        scan_begin();//开始词法分析
        yy::demoParser parser(*this);
        parser.set_debug_level(trace_parsing);
        parser.parse();//开始语法分析
        scan_end();//词法分析结束
        return this->root;//在分析过程中生成了分析树，此处返回
    }
    ```


- 扫描文件并进行词法分析
    ```c++
    void demoDriver::scan_begin()
    {//开始扫描
        lexer.set_debug(trace_scanning);

        // Try to open the file:
        instream.open(file);

        if(instream.good()) {
            lexer.switch_streams(&instream, 0);
        }
        else if(file == "-") {
            //当input-file为-时，将输入流切换为标准输入
            lexer.switch_streams(&std::cin, 0);
        }
        else {
            //打开文件出错
            error("Cannot open file '" + file + "'.");
            exit(EXIT_FAILURE);
        }
    }

    void demoDriver::scan_end()
    {//扫描到结尾，关闭文件
        instream.close();
    }
    ```

- 打印错误信息。
    ```c++
    void demoDriver::error(const yy::location& l, const std::string& m)
    {// 报错，l表示出错行，m表示出错串
        std::cerr << l << ": " << m << std::endl;
    }

    void demoDriver::error(const std::string& m)
    {//报错，m表示出错串
        std::cerr << m << std::endl;
    }
    ```

## 问题1-2
观察语法定义文件中函数定义的部分：
```bison
FuncDef: VOID IDENTIFIER LPARENTHESE RPARENTHESE Block{
    $$ = new SyntaxTree::FuncDef();
    $$->ret_type = SyntaxTree::Type::VOID;
    $$->name = $2;
    $$->body = SyntaxTree::Ptr<SyntaxTree::BlockStmt>($5);
    $$->loc = @$;
  }
  ;
```
main函数的`IDENTIFIER`是"main"，且符合`FuncDef`的语法规则。
可以设置一个`int`型变量`num`记录"main"在FuncDef中作为标识符`IDENTIFIER`出现的次数，如果超过一次则报错。可以保证"main"至多出现一次。
同时，在顶层模块中判断`num`是否为1，如果为0说明"main"一次都没有出现，也需要报错。可以保证"main"至少出现一次。
修改代码如下：
1. 修改`FuncDef`
    ```bison
    FuncDef: VOID IDENTIFIER LPARENTHESE RPARENTHESE Block{
        if(num==1&&$2=="main")     
            {std::cout<<"error\n";exit(1);}
        else if(num==0&&$2=="main") num++;
        $$ = new SyntaxTree::FuncDef();
        $$->ret_type = SyntaxTree::Type::VOID;
        $$->name = $2;
        $$->body = SyntaxTree::Ptr<SyntaxTree::BlockStmt>($5);
        $$->loc = @$;
    }
    ;
    ```
2. 修改顶层模块
    ```bison
    Begin: CompUnit END {
        if(num==0) {std::cout<<"error\n";exit(1);}
        $1->loc = @$;
        driver.root = $1;
        return 0;
    }
    ;
    ```
同时又观察到demo定义的语法规则规定只能有一个`FuncDef`，其实只需要判断`FuncDef`中的`IDENTIFIER`是否是"main"即可。

# Fudan_Digital_EDA
## 复旦数字集成电路设计自动化项目文档

### project要求
以上述`IR`作为输入，我们提供基本的`IR`的`parser`
根据上述`IR`，完成调度、寄存器及操作的绑定、控制逻辑综合，函数输入的数组综合为`RAM`存储器，需要根据`load/store`指令来读写存储器数据。最终生成`RTL代码`。基本计算操作可以调用`RTL`计算模块，或直接使用原始操作符
不超过6位同学为1组，后面的两个`Project`仍按此分组。
最终提交代码、技术报告(包括测试结果)，并注明每位同学贡献。

## 项目成员

| 成员名称 | 学号        |
| -------- | ----------- |
| 郑志宇   | 20307130176 |
| 邱峻蓬   |             |
| 任钰浩   |             |
| 沈笑涵   | 20307130063 |
| 周翔     | 20307130188 |



## 项目说明

本项目实现了从中间表示IR到生成RTL语言的过程。生成了数据流图、控制流图，实现了在块内的调度，完成了寄存器以及操作的绑定，实现了控制逻辑的综合。最终生成一个`RTL`的代码。我们将项目文件编译成了两个可执行文件，分别是适用于`windows` 的`hls.exe` 以及适用于`linux` 的`hls`。经过测试，可执行文件功能正常。

### 功能说明

该工具可以读入`IR`文件，然后执行生成数据流图和控制流图的过程，在有限运算资源的条件下执行调度算法，然后进行寄存器以及计算资源的分配。随后完成控制逻辑的综合。根据上面实现的结果输出一个`verilog`的`module`代码。可以使用`testbench`进行功能测试验证。

### 创新点

项目采用了面向对象的编程方式，项目成员实现了明确的分工，使用了工厂模式以让每个成员的工作相对独立，保证了程序运行与维护的便利性。

我们还使用了`vivado`的`HLS`工具进行功能的验证，有很不错的效果。但是因为生成`verilog`的逻辑与运算的周期存在一定的差异，所以结果并非一致。

### 项目使用方法

#### 环境要求

- `visual studio 2019`及以上版本能正常打开项目中的所有文件
- 注意`src`编码格式为`Unicode(UTF-8)`
- 生成一个`hls`的可执行文件能在`linux`下运行，`hls.exe` 能够在`windows` 下运行。

#### 使用方式

根据自己的环境编写替换`Makefile`，目前项目中的`Makefile`仅仅适用于`window`。

`Windows`：在`testfile` 文件夹中写好测试文件`file.ll` 后，在项目文件夹下运行以下命令：

```cmd
.\hls.exe testfile\dotprod.ll
```

`Linux`：在`testfile` 文件夹中写好测试文件`file.ll` 后，在项目文件夹下运行以下命令：

```powershell
./hls testfile\dotprod.ll
```

### `IR`文件格式声明

#### 变量：

仅支持`int`变量，支持数组，定义与`c`语言类似。如`int a, int a[]`
函数定义：

```c++
define int foo(int a, int b);
```

返回值可以是`int`和`void`。

#### 操作定义：

```pascal
load：从数组中加载一个数据，如b=load(a, 10)就是加载a[10]到b
store: 将数据存储到数组：如store(a, 10, c)，将c存储到a[10]
=：赋值
+: c = a+b
-: c = a-b;
\*: c = a*b;
/: c = a/b;
\==: cond = a==b;
<: cond = a < b;
>: cond = a> b;
>=: cond = a>=b;
<=: cond = a<=b;
br:
	br label：无条件跳转
	br cond label1 label2：有条件跳转‘
phi:
	从不同模块中选择不同的变量值：phi(value1, block_label1, value2, block_label2, ...);
	函数入口的label默认为0。
return：
	返回或返回值。
```

#### 语言实例

```asm
define int dotprod(int a[], int b[], int n)
c = 0;
start:
    # i = 0:i_inc 0:calc
    i = phi(0, 0, i_inc, calc);
    # cl = c:cr 0:calc
    cl = phi(c, 0, cr, calc);
    cond = i >= n;
    br cond ret calc;
calc:
    ai = load(a, i);
    bi = load(b, i);
    ci = ai * bi;
    cr = cl + ci;
    i_inc = i + 1;
    br start;
ret:
	return cl;
```

## 项目的结构

```bash
├── picture # README文档的说明图片
├── projectfile # 项目要求文档与参考资料
│   ├── HSPICE简明教程(复旦大学).pdf
│   ├── proj1_v03_tj.pdf	
│   ├── hspice_sa.pdf	
│   ├── HspiceManual完全手册.pdf
##### 测试文件目录	
├── testfile 
│   ├── SRAM.v
│   ├── dotprod.v
│   ├── tb_dotprod.v
│   ├── gcd.v
│   ├── tb_gcd.v
##### 工作文件夹
├── main.cpp
├── parser.h
├── parser.cpp
├── HLS.h
├── HLS.cpp
├── dataflowgraph.h
├── dataflowgraph.cpp
├── controlflowgraph.h
├── controlflowgraph.cpp
├── schedule.h
├── schedule.cpp
├── leftAlgorithm.h
├── computeresource.h
├── Hungarian_algorithm.h
├── control_logic.h
├── cycleTable.h
├── FSMachine.h
├── FSMachine.cpp
##### 
├── README.md # 项目说明文件
```

## 项目技术细节

### 项目顶层文件

此部分由郑志宇同学维护，接口与功能与小组成员共同商议确定。最终实现了项目的并行推进与项目成员对函数的独立维护。

`HLS`类是项目的顶层模块，它的每一个函数都实现了一个特定的功能：

`├── HLS.h`

`├── HLS.cpp`

```C++
	// 实现图的生成
	void generate_CFG();
	//设置测试时间
	void setTestTime();
	// 遍历所有节点的算法
	void travelaround();
	void travelback();
	// 实现调度算法
	void perform_scheduling();
	// 实现寄存器分配和绑定
	void perform_register_allocation_and_binding();
	// 实现运算资源的分配与绑定
	void perform_calculate_allocation_and_binding();
	// 控制逻辑综合方法
	void synthesize_control_logic();
    	// 生成状态机
	void genFSM();
	// 输出verilog文件
	void outputfile();
```

其中，函数`travelaround`和`travelback`是郑志宇同学给出的按照拓扑排序数据流图的方法，用作参考，并不参与项目功能的实现。

### Part 1生成数据流图以及控制流图

此部分由郑志宇同学完成。

`├── HLS.cpp`

`│   ├──  generate_CFG();`

`├── controlflowgraph.h`

`├── controlflowgraph.cpp`

`├── dataflowgraph.h`

`├── dataflowgraph.cpp`

#### 函数及文件说明

```c++
void HLS::generate_CFG() {
	// 通过IR生成数据流图以及控制流图
	CFG = ControlFlowGraph(parsered);
}
```

这个函数在`HLS`中利用解析出的文件生成了数据流图和控制流图。控制流图中是包含数据流图以及跳转关系的节点，而数据流图由操作节点以及其依赖关系构成。在生成数据流图的时候，控制流图也顺便完成了生成。

##### 首先是数据流图实现的接口：

```c++
	// CreateGraph
	// 
	// 用于标记节点是否被访问
	std::vector<int> Mark;
	// 用于标记节点当前的入度，为0表示可以被直接访问
	std::vector<int> InVertex;
	// 用于标记节点当前的出度
	std::vector<int> OutVertex;
	void Initialize();
	// DFG的创建
	DataFlowGraph(basic_block& bb);

	// operation
	// 创建边
	void CreateEdge(int from, int to);
	// 修改流图的最短周期
	int getPeriod() const {
		return period;
	}

	void setPeriod(int T) {
		period = T;
	}

	// get
	// 节点列表
	std::vector<node>& get_opList() {
		return opList;
	}
	// 输出边列表
	std::vector<BranchEdge>& get_Branches() {
		return Branchs;
	}
	// 块的输入变量列表
	std::vector<InputEdge>& get_inputList() {
		return inputList;
	}
	// 块的输出变量列表
	std::vector<OutputEdge>& get_outputList() {
		return outputList;
	}
	// 模块名
	std::string& get_label() {
		return label;
	}
	// 节点的出度节点
	std::vector<int>& ToVertex(int from) {
		return opList[from].next;
	}
	// 输出变量的哈希表
	std::unordered_map<std::string, int>& myOutvartable(){
		return vartable;
	}
```

主要介绍后面的方法的意义：

`void Initialize()`：初始化入度和出度以及节点的访问标记，以便于开始新一轮的遍历操作。

`std::vector<node>& get_opList()`：操作使用`vector`存储，这样的好处是方便快速索引而且访问速度快

`std::vector<BranchEdge>& get_Branches()`：块内的跳转逻辑，这部分能够得到清晰的块之间的跳转关系，`BranchEdge`标识了块的跳转方向和跳转条件。

`std::vector<InputEdge>& get_inputList()`：块的输入变量组成的`vector`，主要信息有变量来自哪个块以及变量名。

`std::vector<OutputEdge>& get_outputList()`：块的输出变量组成的`vector`，主要信息是外部的块需要哪些变量。

`std::string& get_label()`：获得块的标签。

`std::vector<int>& ToVertex(int from)`：获得节点`from`的出度节点的序号组成的`vector`，`vector`中的每一个`index`都是节点`from`出度的节点。可以用于拓扑排序中寻找出度的节点。

`std::unordered_map<std::string, int>& myOutvartable()`：这是一个哈希表，其最大的意义在于可以从一个变量的名称直接索引到输出这个变量的操作节点的`index`。可以实现节点的快速查找。可以用于反向拓扑排序中寻找入度的节点。

##### 然后是控制流图实现的接口：

```c++
	// 控制流图的生成
	ControlFlowGraph(parser& p);

	// 获得CFG图中的DFG节点
	std::vector<graph_node>& getDFGNodes();

	// 获得DFG在节点向量中的下标
	int getIndex(std::string label);

	//获取下一个块所指向的块的下标
	std::vector<int> NextNode(std::string label);

	std::vector<int> NextNode(int index);

	// 返回信息的接口
	std::string getfuncname() {
		return func_name;
	}
	std::vector<var> &getvar() {
		return vars;
	}
	int getRet_type() const {
		return ret_type;
	}
```

主要介绍一些方法的意义：

`std::vector<graph_node>& getDFGNodes()`：这个方法返回了一个`DFG`节点构成的`vector`，可以实现节点的快速索引。

`int getIndex(std::string label)`：可以实现从标签索引到`DFG`的节点的`index`的操作。

#### 技术细节

控制流图与数据流图的意义在于为后面的调度的实现创建了条件，提供很多方便的接口让后面的实现能够访问。

本部分使用了一些访问速度比较快的结构来实现一部分功能，有效提升了运行的效率：

1. 哈希表索引节点
2. 数组实现有向图

本部分在创建数据流图的时候，在首尾个创建了一个虚拟的节点，首节点意义是作为外部变量输入的索引位置，而末节点的意义是作为需要输出到外部的变量的索引位置。利用哈希表在所有的数据流图中匹配需要的信息并进行更新和存储最终生成了这样的一个完整的数据流图。然后是在创建控制流图的时候为了输出三个输入变量生成了一个名为`fiction_head`的虚拟节点。这个节点的主要意义也在与索引到头部的节点，在生成的verilog的状态机中，这个节点是标志着函数未开始运行的节点。在接收到ap_start的信号之后就会进入到运行状态。

### Part 2完成周期的调度

### Part 3完成寄存器的绑定

### Part 4完成计算资源的绑定

此部分由沈笑涵同学完成。
本部分使用了hls.h中各块内的寄存器与变量的绑定结果REG、和各块DFG中存储node计算结点的opList信息。

其基本思想是Latency约束下的最小资源约束，通过逐个遍历opList中的node信息和对应输入输出变量的寄存器绑定结果，按照应该使用的计算资源类别和输出寄存器编号进行划分，并按照优化目标——输入寄存器的硬件复杂程度最低的标准（即计算资源输入端数据选择器数据选择端数目最低）进行计算资源与计算节点的绑定，并完成输入寄存器的绑定。

#### 函数及文件说明
`├── HLS.h `

```c++
	//计算资源（包括加法器、乘法器和除法器）
	std::vector<computeresource> COR;
	//计算资源匹配结果（匹配的是node结点的编号和计算资源COR的序号）
	std::vector<std::vector<std::pair<int, int>>>  CSP;
```

`├── HLS.cpp `

```c++
	void HLS::perform_calculate_allocation_and_binding();
```
`├── computeresource.h `

`├── Hungarian_alogrithm.h `

##### 计算资源类的定义

该部分完成了绑定的计算资源的基本信息的说明，包括计算资源的类别、输入端绑定寄存器、输出端绑定寄存器、以及相关绑定操作的方法定义
```c++
	class computeresource{
		//声明计算资源是加法器or乘法器or除法器
		int flag;
		//左边输入寄存器
		std::vector<int> Ainputregisters;
		//右边输入寄存器
		std::vector<int> Binputregisters;
		//输出寄存器
		int outputregister;
		//构造函数
		computeresource(int flag1, int outputreg);
		//绑定计算资源左输入寄存器
		void setinputAregisters(int reg) {
			Ainputregisters.push_back(reg);
		}
		//绑定计算资源右输入寄存器
		void setinputBregisters(int reg) {
			Binputregisters.push_back(reg);
		}
		//绑定计算资源输出寄存器
		void setoutputregister(int reg) {
			if (outputregister != -1) {
				//std::cout << "output register has already set! error" << std::endl;
				return;
			}
			else {
				outputregister = reg;
			}
		}
		//查找计算资源左输入端绑定寄存器
		bool findareg(int reg);
		//查找计算资源右输入端绑定寄存器
		bool findbreg(int reg);
	}
```
主要介绍一些定义的想法：

`std::vector<int> Ainputregisters`：计算资源输入端绑定的寄存器一般不止一个，因此通过vector向量存储；

`int outputregister`：一般来说，计算资源输出端仅绑定一个输出寄存器。因此在定义计算资源时，输出端寄存器用一个int变量存储（这可以用来作为判定计算资源是否可以绑定某一计算结点的依据）。

##### 查找每一个块中计算结点node与寄存器绑定结果

该部分完成了对每一个DFG中的node结点和寄存器绑定结果的提取，以及该块内的寄存器绑定结果的查找，可以实现通过输入变量名查找绑定的寄存器编号

```c++
	//提取每一个DFG中的node结点
	std::vector<graph_node> DFGS = CFG.getDFGNodes();
	
	//实现块内寄存器绑定结果的查找
	//该函数完成了通过输入变量名和DFG块寄存器绑定结果，实现块内寄存器绑定结果的查找
	int findregister(std::vector<std::pair<std::string, int>> REGi, std::string val);
```
##### 实现将块内node结点与计算资源的绑定

该计算资源的实例化结果和绑定结果分别存储在hls.h中新定义的两个变量中
```c++
	std::vector<computeresource> COR;
	std::vector<std::vector<std::pair<int, int>>>  CSP;
```

##### 函数接口：
```c++
	std::vector<std::pair<int, int>> bindcomputeresource(DataFlowGraph& DFG, std::vector<std::pair<std::string, int>>REGi, std::vector<computeresource>& CORE) 
```
#### 技术细节：

1.	对每个DFG块中的node结点的Optype进行划分，赋值操作不分配计算资源（Optype = 0），Optype = 3(乘法运算)匹配乘法器，Optype = 4（除法运算）匹配除法器，剩下除了跳转指令和返回指令（该指令均由状态机跳转完成）之外的所有Optype值的操作匹配加法器（该加法器认为可以完成加法操作和减法操作，其中减法操作的实现可以将被减数转为补码后再相加得到，该步骤应由写入寄存器操作完成）。

2.	对按计算资源分类后的node结点，优先匹配已经实例化的空闲计算资源。通过遍历COR中计算资源信息，判断计算资源输出寄存器是否符合该node输出变量分配的寄存器，并同时进行时序判断。若实现匹配，则进入计算资源输入端绑定寄存器步骤；若无符合条件计算资源匹配，则增加相应计算资源。 

3.	对绑定的计算资源的输入端进行输入寄存器绑定。其中由于除法运算本身两变量存在差异（除数与被除数），因此对应计算资源左输入端和右输入端并不对称。而对于加法器和乘法器，优先判断其输入寄存器是否已经与该计算资源匹配，若未完全匹配，则按照代价最低原则（硬件电路的复杂度最低）将其输入寄存器与计算资源输入端进行绑定。

4.	完成每个块内的计算资源绑定后，将最终的计算资源实例化结果和计算资源绑定结果依次push_back，并最终赋值给最终结果COR和CSP中。

### Part 5完成控制逻辑综合

这部分由周翔同学完成。

`├── cycleTable.h `

`├── control_logic.h `

#### 函数及文件说明

```c++
// 控制逻辑综合方法
	void synthesize_control_logic();
```

这个函数利用周期调度、寄存器绑定以及计算资源绑定的结果，汇总每个块、每个周期下运行的所有操作语句`std::vector<Statement>`。其中`Statement`是一个结构体，记录下了对应语句的信息（在后面说明）。

##### 首先是寄存器Register类

```c++
public:
	//寄存器的下标，与任钰浩的pair的索引值相对应
	int reg_index;
	// data中会存储当前周期下寄存器存储的值
	bool getData(int cycle, varPeriod& data)；
```

其中，`varPeriod`是任钰浩同学定义的一个结构，内含变量名`var`、变量起始活跃时间`startp`与终止活跃时间`stopp`。

`bool getData(int cycle, varPeriod& data)`：输入周期数cycle，通过遍历寄存器中存储的所有变量对应的活跃周期，选择出该周期下该寄存器存储的变量名（包含在data中）。

##### 然后是选择器Mux类

```c++
public:
	int mux_index;                //选择器下标
	bool chooseReg(int cycle, DataFlowGraph dfg, std::vector<Register> REGs,
	  std::vector<std::pair<std::string, int>> REGi,
	  std::vector<std::pair<int, int>> CSP,
	  std::vector<computeresource> com,
	  Register& reg, std::string& _var)；
```

`bool chooseReg（）`：默认`reg`下标为-1、`_var`为`NULL`作为未找到的结果。挑选出当前周期cycle、当前模块dfg下，选择器输入端所选取的寄存器reg，以及寄存器中存储的变量_var。这里要注意的是在不同块中活跃的寄存器可能不同，如果直接访问将会导致溢出错误，因此需要先结合寄存器绑定结果REGi，挑选出当前块中会使用到的所有寄存器编号，记为`std::vector<int> curDFGinput;`。之后，利用Register类的`getData()`函数判断当前周期寄存器`i`是否存有数据（记为`v`）。如果存有数据，由于一个寄存器可能会与多个计算资源相连，无法保证`v`一定是在选择器连接的计算资源中被使用的，因此还需要结合计算资源绑定结果，找到相关周期的节点语句node进行判断，得到最后的结果。

经过小组讨论，Register类与Mux类仅用于表示连接的结构，可以反映门级连接，但实际上完成PJ的要求是生成RTL代码，并不需要使用到上述两个类。

##### 最后是控制器类

```c++
public:
	void generateCycles(std::vector<std::pair<std::string, int>> _REG, CFG)；
	std::vector<Cycle> getCycle()；
```

其中，`Cycle`是一个结构，定义如下：

```c++
struct Cycle {
	std::vector<Statement> Statements;
};
struct Statement {
	std::vector<std::string> vars;  //输入变量名
	std::vector<int> regs;          //变量对应的寄存器编号
	int optype;                     //操作类型
	int compute_resource_index;     //绑定的计算资源标号
	int outreg;                     //传出的寄存器编号
	std::vector<std::string> label; //为phi操作而设，记录数据来自哪一个块
};
```

`void generateCycles(std::vector<std::pair<std::string, int>> _REG)`：根据寄存器绑定、计算资源绑定结果，生成一个向量`std::vector<Cycle> C;`，`C[i]`对应周期`i`执行的所有`Statement`。首先要确定`C`的大小。经过与小组成员的协商，由于寄存器绑定的结果是针对每个块而言的，每当传入一个新的块时，所有的寄存器都会被覆盖掉，因此需要事先将本块中需要用到的从其他块中传入的变量导入到对应的寄存器中。
因此，这里将`C`的大小设置为块内语句总周期数加`2`，其中第零周期执行将郑志宇同学生成的`CFG`中的`MemMap`中存储的相关变量数据导入到绑定的寄存器中，最后一个周期执行将本块会传出的数据导入到`MemMap`中。
另外，`node`节点包含沈笑涵同学的计算资源绑定结果，以及不使用计算资源的节点。后者需要特别处理，否则会报溢出错误。

`std::vector<Cycle> getCycle()`：用于返回最终生成的周期表`std::vector<Cycle> C`。

##### 其他函数

```c++
void Pair2Register(DataFlowGraph &DFG, std::vector<std::pair<std::string, int>> REG, 
                    std::vector<Register>& Regs)
```

这一函数用于将任钰浩同学的寄存器绑定结果`vector<pair<string, int>>` 结构转化成`vector<Register>`结构。

#### 技术细节

1. 在`chooseRegs()`与`generateCycles()`函数的编写时，比较容易出现堆栈溢出的问题，这就需要判断每个块内是否真正使用了相应的硬件资源。
2. 为了方便生成Verilog代码的工作，创建了一个Cycle结构，里面存放着对应周期执行的所有操作信息，并且保存了STORE、RET等操作的输出情况。
3. 为了在分块实现寄存器绑定的基础上，保证数据的正确性，这里在开始与结束阶段各加了一拍，实现了寄存器数据的导入与传出。

### Part 6生成`Verilog`代码

这一部分由郑志宇同学与任钰浩同学共同完成，郑志宇同学负责了数据流图的生成，所以负责状态机的跳转部分以及`module`块的生成。任钰浩同学负责寄存器的绑定部分，所以负责寄存器的行为的综合。

#### 生成`module`以及状态机的跳转逻辑

这一部分由郑志宇同学完成

`├── HLS.cpp `

`│   ├── genFSM();`

`├── FSMachine.h`

`├── FSMachine.cpp`

##### 函数及文件说明

```c++
void HLS::genFSM() {
	outputFSM = FSMachine(CFG);
}
```

##### 接口说明

```c++
	std::unordered_map<std::string, std::string>& getStateMap() {
		return stateMapping;
	}
	// 生成module
	void IOdefinationAppend(int ret_type,std::vector<var>& vars);
	// 生成FSM
	void FSMgener(ControlFlowGraph &CFG);
```

##### 技术细节

这一部分的技术细节并未有很多体现，主要还是使用了工厂模式实现用类来代理实现生成输出函数。

基本流程是：

- 实现输入输出的实例化，创建`module`
- 实现状态机的编码，创建状态机寄存器
- 实现状态机的`if`语句以及`always`块的生成

##### 输入的实例化：

我们参考了`vivado`的`HLS`工具生成的`verilog`文件的输入来实例化数组的行为。一个数组对应的是内存中的一块区域，为了在内存中读取或者写入数据，我们需要一个读信号以及一个写信号。此外，我们还需要读入`SRAM`中数据的一个输入信号以及向`SRAM`中写入数据的一个写入信号。综合出来如下所示的信号：

```verilog
(
    input [31:0] var_q0, // 读入数据
    output [31:0] var_ad0, // 写入数据
    output [31:0] var_addresss0, // 偏移地址
    output var_ce0, // 读入使能
    output var_we0 //写入使能
);
```

而一个`int`型的变量则会综合出一个输入端：

```verilog
(
    input [31:0] var
);
```

这样我们就完成了函数输入变量在对应的`module`中的实例化。

函数的输出类型则决定了是否会多出一个输出端口：

```verilog
(
    output [31:0] ap_return
);
```

`module`的运行需要一系列的其他参数，在这里逐一说明：

```verilog
(
    input	ap_clk, // 时钟信号
	input	ap_rst_n, // rst_n信号
	input	ap_start, // 程序的开始信号
	output	reg ap_done // 程序结束信号
);
```

##### 代码块的状态编码

对 $N$个代码块的状态编码主要思路就是生成一个 $N$维的 $0$组成的字符串，然后将其中一位修改为$1$即可完成对每个状态的唯一编码。编码完成之后，使用`std::unordered_map<std::string, std::string>& getStateMap()`函数将块的标签与块的状态对应上。

##### 状态机跳转的实现

状态机跳转示意图：

<img src="picture\FSM.png" width="800px;" />

生成一个`always`块的语句，对于每个块都生成一个`if`逻辑语句块，进行跳转条件的设置，状态机对时钟`ap_clk`的`posedge`敏感。在接收到跳转信号`branch_ready`以及跳转条件`cond`之后进行跳转。不跳转`branch_ready`为 0，无条件跳转只需要`cond`为 1即可，为真为假跳转则需要`cond`的指示来实现。

## 项目测试

### 测试文件 1 dotprod.v

#### IR文件由课程提供

```asm
define int dotprod(int a[], int b[], int n)
c = 0;
start:
    # i = 0:i_inc 0:calc
    i = phi(0, 0, i_inc, calc);
    # cl = c:cr 0:calc
    cl = phi(c, 0, cr, calc);
    cond = i >= n;
    br cond ret calc;
calc:
    ai = load(a, i);
    bi = load(b, i);
    ci = ai * bi;
    cr = cl + ci;
    i_inc = i + 1;
    br start;
ret:
	return cl;
```

#### 测试文件由郑志宇同学提供：

`├── testfile` 

`│   ├── dotprd.v` 

`│   ├── tb_dotprd.v` 

#### 测试结果：

##### 实例化SRAM

为了从数组中读数据，我们初始化了两个`SRAM`，`SRAM_a`，`SRAM_b`，这两个`SRAM` 的`module` 为生成的`module` 的访问数组的功能提供了便利。在这里为了方便起见，我们取`n=10`，然后在`SRAM_a`，`SRAM_b` 中存入了数据：

`a={1,2,3,4,5,6,7,8,9,10};`

`b={10,9,8,7,6,5,4,3,2,1};`

`n=10;`

我们对程序的预期结果是`220`；

##### RTL电路图

<img src="picture\dotprod_tbRTL.png" width="800px;" />

##### 部分内部结构示意：

<img src="picture\dorprod_innerRTL.png" width="800px;" />

##### 测试的结果波形：

<img src="picture\dotprod_tb.png" alt="tb" width="800px;" />

测试的结果如上所示，我们在`testbench`中模拟了一个`SRAM`去为`a_q0`，`b_q0`进行赋值，然后我们接收来自我们的状态机的输入的使能信号，地址信号以及写入的数据，最后完成了此程序的仿真。可以看到，在`ap_done`信号出现的时候，`ap_return`的结果已经正确而且稳定地出现。这与我们的预期结果相符。因为进行了一些寄存器分配，所以`ap_return`的值并不是存储在一个固定的寄存器中，只有在最终才会由分配好的寄存器进行传出。

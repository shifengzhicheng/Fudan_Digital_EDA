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
| 沈笑涵   |             |
| 周翔     |             |



## 项目说明

本项目实现了从中间表示IR到生成RTL语言的过程。生成了数据流图、控制流图，实现了在块内的调度，完成了寄存器以及操作的绑定，实现了控制逻辑的综合。最终生成一个`RTL`的代码。

### 功能说明

该工具可以读入`IR`文件，然后执行生成数据流图和控制流图的过程，在有限运算资源的条件下执行调度算法，然后进行寄存器以及计算资源的分配。随后完成控制逻辑的综合。根据上面实现的结果输出一个`verilog`的`module`代码。可以使用`testbench`进行功能测试验证。

### 创新点

项目采用了面向对象的编程方式，项目成员实现了明确的分工，使用了工厂模式以让每个成员的工作相对独立，保证了程序运行与维护的便利性。

我们还使用了`vivado`的`HLS`工具进行功能的验证，有很不错的效果。但是因为生成`verilog`的逻辑与运算的周期存在一定的差异，所以结果并非一致。

### 项目使用方法

#### 环境要求

- `visual studio 2019`及以上版本能正常打开项目文件
- 编码`Unicode(UTF-8)`
- 生成一个`hls`的可执行文件能在`linux`下运行

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

```c++
define int dotprod(int a[], int b[], int n)
c = 0;
start:
    // i = 0:i_inc 0:calc
    i = phi(0, 0, i_inc, calc);
    // cl = c:cr 0:calc
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
├── testfile # 测试文件目录
│   ├── test_ori #项目所给的运行网表
│   ├── hspice_testfile #hspice测试所用网表
│   ├── AmplifierDC.sp      		
│   ├── AmplifierSweep.sp         	
│   ├── bufferDC.sp
│   ├── bufferSweep.sp
│   ├── dbmixerDC.sp 
│   ├── diftestDC.sp      		
│   ├── diftestSweep.sp         	
│   ├── invertbufferDC.sp
│   └── invertbufferSweep.sp
##### 工作文件夹
├── Top_module.m
├── parse_netlist.m
├── Generate_DCnetlist.m
│   ├── Gen_NodeInfo.m
│   ├── Gen_DeviceInfo.m
│   ├── init_value.m
│   ├── Mos_Calculator.m
│   └── Diode_Calculator.m
├── CalculateDC.m
│   ├──Gen_nextRes.m
│   │   ├──Mos_Calculator.m
│   │   ├──Diode_Calculator.m
│   ├──Gen_baseA.m
│   └──Gen_nextA.m
├── Sweep_DC.m
├── ValueCalc.m
├── portMapping.m
##### 
├── README.md # 项目说明文件
```

## 项目技术细节

### 项目顶层文件

此部分由郑志宇同学维护，接口与功能与小组成员共同商议确定。最终实现了项目的并行推进与项目成员对函数的独立维护。

`HLS`类是项目的顶层模块，它的每一个函数都实现了一个特定的功能：

├── HLS.h

├── HLS.cpp

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

本部分在创建数据流图的时候，在首尾个创建了一个虚拟的节点，首节点意义是作为外部变量输入的索引位置，而末节点的意义是作为需要输出到外部的变量的索引位置。利用哈希表在所有的数据流图中匹配需要的信息并进行更新和存储最终生成了这样的一个完整的数据流图。然后是在创建控制流图的时候为了输出三个输入变量生成了一个名为`fiction_head`的虚拟节点。这个节点的主要意义也在与索引到头部的节点。

### Part 2完成周期的调度

### Part 3完成寄存器的绑定

### Part 4完成计算资源的绑定

### Part 5完成控制逻辑综合

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
	FSMachine(ControlFlowGraph &CFG);
	std::vector<std::string>& getModule();
	std::vector<std::string>& getFSM();
	std::string getFilename();
	std::unordered_map<std::string, std::string>& getStateMap() {
		return stateMapping;
	}
	// 生成module
	void IOdefinationAppend(int ret_type,std::vector<var>& vars);
	// 生成always逻辑块
	std::string alwayslogic(bool Istiming);
	// 生成if逻辑块
	std::string iflogic(std::string cond);
	// elseif逻辑块
	std::string elseiflogic(std::string cond);
	// else逻辑
	std::string elselogic();
	// 生成begin
	void begin(int i);
	// 生成end
	void end(int i);
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
    output ap_return
);
```

`module`的运行需要一系列的其他参数，在这里逐一说明：

```verilog
(
    input	ap_clk, // 时钟信号
	input	ap_rst_n, // rst_n信号
	input	ap_start, // 程序的一些信号
	output	reg ap_idle,
	output	reg ap_ready,
	output	reg ap_done // 程序结束信号
);
```

##### 代码块的状态编码

对 $N$个代码块的状态编码主要思路就是生成一个 $N$维的 $0$组成的字符串，然后将其中一位修改为$1$即可完成对每个状态的唯一编码。编码完成之后，使用`std::unordered_map<std::string, std::string>& getStateMap()`函数将块的标签与块的状态对应上。

##### 状态机跳转的实现

对于每个块都生成一个`always`逻辑语句块，然后用`if`语句进行跳转，状态机对时钟`ap_clk`的`posedge`敏感。在接收到跳转信号`CondReg[1]`以及跳转条件`CondReg[0]`之后进行跳转。不跳转`CondReg[1]`为0，无条件跳转只需要`CondReg[1]`为1即可，为真为假跳转则需要`CondReg[0]`的指示来实现。

## 项目测试

### 测试文件

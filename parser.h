#ifndef PARSER_H
#define PARSER_H
#include <fstream> // 包含文件流头文件
#include <string>  // 包含字符串头文件
#include <vector>  // 包含向量头文件

enum OP_TYPE {
	OP_ASSIGN,  // 赋值操作
	OP_ADD,     // 加法操作
	OP_SUB,     // 减法操作
	OP_MUL,     // 乘法操作
	OP_DIV,     // 除法操作
	OP_LOAD,    // 载入操作
	OP_STORE,   // 存储操作
	OP_BR,      // 分支操作
	OP_LT,      // 小于操作
	OP_GT,      // 大于操作
	OP_LE,      // 小于等于操作
	OP_GE,      // 大于等于操作
	OP_EQ,      // 等于操作
	OP_PHI,     // Phi 操作
	OP_RET      // 返回操作
};

enum RET_TYPE { // 返回类型枚举
	RET_VOID,  // void 类型返回值
	RET_INT    // int 类型返回值
};

class statement { // 语句类
	public:
		statement() {};  // 构造函数
		~statement() {};  // 析构函数

		int get_type() {
			return _type;   // 获取语句类型
		}
		void set_type(int type) {
			_type = type;   // 设置语句类型
		}
		int get_num_oprands() {
			return _num_oprands;   // 获取操作数数量
		}
		void set_num_oprands(int t) {
			_num_oprands = t;   // 设置操作数数量
		}
		std::string & get_oprand(int num) {
			return _oprands[num];   // 获取操作数
		}
		void add_oprand(const std::string & oprand) {
			_oprands.push_back(oprand);   // 添加操作数
		}
		void set_var(const std::string & var) {
			_var = var;   // 设置变量名称
		}
		const std::string get_var() {
			return _var;   // 获取变量名称
		}

	private:
		std::string _line;  // 语句字符串
		int _type;  // 语句类型
		int _num_oprands;  // 操作数数量
		std::string _var;  // 变量名称
		std::vector<std::string> _oprands;  // 操作数向量
};

struct var { // 变量结构体
	public:
		std::string _name;  // 变量名称
		bool _array_flag;  // 是否为数组，0 表示非数组，大于等于 1 表示数组
		var() {};
		var(std::string name, bool array_flag): _name(name), _array_flag(array_flag) {};
		bool operator==(const var& rhs) {
			return this->_name == rhs._name && this->_array_flag == rhs._array_flag;
		}
};

class basic_block { // 基本块类
	public:
		basic_block() {}  // 默认构造函数
		basic_block(std::string & label);  // 带标签名称的构造函数
		void set_name(const std::string & n) {
			_label = n;   // 设置基本块名称
		}
		void add_statement(statement & s);  // 添加语句
		void clear_statements();  // 清除语句
		std::vector<statement> & get_statements() {
			return _statements;   // 获取语句向量
		}
		std::string get_label_name() {
			return _label;   // 获取标签名称
		}

	private:
		std::vector<statement> _statements;  // 语句向量
		std::string _label;  // 标签名称
};

/**
 * @brief 解析器类，用于解析输入的源代码文件，生成基本块
 */
class parser {
	public:
		/**
		 * @brief 构造函数，初始化解析器对象
		 *
		 * @param filename 输入的源代码文件名*/
		parser(const std::string & filename);

		/**
		 * @brief 解析源代码文件，生成基本块
		 *
		 * @return int 返回解析结果，0表示解析成功，-1表示解析失败*/
		int parse();

		/**
		 * @brief 获取生成的基本块向量
		 *
		 * @return std::vector<basic_block>& 返回生成的基本块向量*/
		std::vector<basic_block> & get_basic_blocks() {
			return _blocks;
		}

		/**
		 * @brief 获取函数名
		 *
		 * @return std::string& 返回函数名*/
		std::string & get_function_name() {
			return _function_name;
		}

		/**
		 * @brief 获取函数参数向量
		 *
		 * @return std::vector<var>& 返回函数参数向量*/
		std::vector<var> & get_function_params() {
			return _function_params;
		}

		/**
		 * @brief 获取返回值类型
		 *
		 * @return int 返回返回值类型，0表示void，1表示int，2表示float*/
		int get_ret_type() {
			return _ret_type;
		}

		/**
		 * @brief 获取当前正在解析的源代码行
		 *
		 * @return std::string& 返回当前正在解析的源代码行*/
		std::string & get_current_line() {
			return _line;
		}

	private:
		/**
		 * @brief 解析函数定义
		 *
		 * @param line 当前正在解析的源代码行
		 * @return int 返回解析结果，0表示解析成功，-1表示解析失败*/
		int parse_function(std::string & line);

		/**
		 * @brief 解析语句
		 *
		 * @param line 当前正在解析的源代码行
		 * @param bb 当前正在解析的基本块
		 * @return int 返回解析结果，0表示解析成功，-1表示解析失败*/
		int parse_statement(std::string & line, basic_block & bb);

		/**
		 * @brief 将生成的基本块压入基本块向量中
		 *
		 * @param bb 待添加的基本块*/
		void push_back_basic_block(basic_block & bb) {
			_blocks.push_back(bb);
		}

	private:
		std::string _filename;                      // 输入的源代码文件名
		std::vector<basic_block> _blocks;           // 生成的基本块向量
		std::string _function_name;                 // 函数名
		std::vector<var> _function_params;          // 函数参数向量
		int _ret_type;                              // 返回值类型，0表示void，1表示int，2表示float
		std::string _line;                          // 当前正在解析的源代码行
};

#endif

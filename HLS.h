#ifndef HLS_H
#define HLS_H
#include "parser.h"
struct op {
	// 操作的变量
	vector<var> vars;
	// 操作的
	// 调研自己的函数部分，确定op应该拥有的变量
	// 所有函数应该是修改图里的元素

	// 操作的所需周期
	const int T;
	// 操作开始周期
	int T_start;
	// 操作结束周期
	int T_end;

	// 操作绑定的寄存器

	// 操作绑定的运算资源

	// 操作创建
	op();
	
}
struct control_block {
	// 控制器，第五部分可能用得到，
	// 理论上需要插在块的跳转之间
	vector<int> T;
	vector<bool> cmd;
}
class HLS {
	public:
		HLS(const std::vector<basic_block>& bbs,
		    const std::string& function_name,
			const std::vector<var>& vars,
		    int ret_type)
			: _bbs(bbs),
			  _function_name(function_name),
			  _vars(vars),
			  _ret_type(ret_type) {}
			  
		// 执行转换操作
		void convert() {
			
			// 执行图的生成
			generate_DAG();
			
			// 执行调度
			// 资源约束下求最小Latency
			perform_scheduling();

			// 执行寄存器分配和绑定
			perform_register_allocation_and_binding();
			
			// 实现运算资源的分配与绑定
			perform_calculate_allocation_and_binding();
			
			// 构建控制逻辑
			synthesize_control_logic();
			
		}
		
		// 生成RTL操作方法
		void generate_rtl_operations() const;
		
		// 获取生成的RTL代码
		std::string getRTLCode() const{		
			return _generated_rtl_code;
		}


	private:
		// 输入的块
		std::vector<basic_block> _bbs;
		std::string _function_name;
		std::vector<var> _vars;
		int _ret_type;

		// RTL代码
		std::string _generated_rtl_code;

		// 资源约束条件
		int reg_num=;
		int add_num=;
		int mul_num=;


		// 内部方法
		// 实现图的生成
		void generate_DAG();
		// 实现调度算法
		void perform_scheduling();
		// 实现寄存器分配和绑定
		void perform_register_allocation_and_binding();
		// 实现运算资源的分配与绑定
		void perform_calculate_allocation_and_binding();
		// 控制逻辑综合方法
		void synthesize_control_logic();	

		// 细节实现的子函数放在下面

};

#endif
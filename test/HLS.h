#ifndef HLS_H
#define HLS_H
#include "parser.h"
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
		std::vector<basic_block> _bbs;
		std::string _function_name;
		std::vector<var> _vars;
		int _ret_type;
		std::string _generated_rtl_code;
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
};

#endif

# include "parser.h"
# include "HLS.h"
# include <iostream>

int main() {
	//  std::cout<<"Here! Running!"<<std::endl;
	//  if(argc != 2)
	//  {
	//      std::cout << "Usage:" << argv[0] << "filename\n";
	//      return -1;
	//  }
	const std::string filename = "test.ll";
	parser p(filename);  // 创建解析器对象

	if (p.parse() != 0) { // 执行解析，为0时解析成功
		std::cout << "parsing error" << p.get_current_line() << "\n"; // 输出解析错误信息
		return -1;  // 返回错误状态码
	}

	// 获取解析结果
//	std::vector<basic_block> bbs = p.get_basic_blocks(); // 获取基本块
//	std::string fn_name = p.get_function_name();         // 获取函数名
//	std::vector<var> vars = p.get_function_params();     // 获取函数参数
//	int ret_type = p.get_ret_type();                     // 获取返回值类型
	// 这个block用来打印get到的信息
	{
		//		if (ret_type == RET_INT)
		//			std::cout << "ret type: int" << std::endl;
		//		else
		//			std::cout << "ret type: void" << std::endl;
		//
		//		std::cout << "function name" << fn_name << std::endl;
		//
		//		for (size_t i = 0; i < vars.size(); ++i) {
		//			if (vars[i]._array_flag)
		//				std::cout << "array" << std::endl;
		//			else
		//				std::cout << "non-array" << std::endl;
		//			std::cout << vars[i]._name << std::endl;
		//		}
		//
		//		for (size_t i = 0; i < bbs.size(); ++i) {
		//			// 块标签
		//			std::cout << "Basic Block label: " << bbs[i].get_label_name() << std::endl;
		//			// 块内语句
		//			std::vector<statement> ss = bbs[i].get_statements();
		//			for (size_t j = 0; j < ss.size(); ++j) {
		//				// 块内语句
		//				int type = ss[j].get_type();
		//				if (type != OP_STORE || type != OP_RET) {
		//					std::cout << "value " << ss[j].get_var() << " " << std::endl;
		//				}
		//
		//				std::cout << "OP TYPE:" << ss[j].get_type() << std::endl;
		//				for (int k = 0; k < ss[j].get_num_oprands(); ++k)
		//					std::cout << ss[j].get_oprand(k) << " ";
		//
		//				std::cout << std::endl;
		//			}
		//		}
	}
	// 生成HLS综合器类
	HLS hls(p);
	// 生成数据流图和控制流图
	hls.generate_CFG();
	// 获得DFG节点
	hls.setTestTime();
	hls.travelaround();

	// bind
	hls.perform_register_allocation_and_binding();
	hls.perform_calculate_allocation_and_binding();
	
	/*std::vector<std::vector<std::pair<std::string, int>>> REG;
	std::vector<graph_node> DFGS = hls.getCFG().getDFGNodes();
	for (std::vector<graph_node>::iterator iter = DFGS.begin(); iter != DFGS.end(); iter++)
	{
		REG.push_back(binding((*iter).DFG));
	}*/

	//	HLS Topmodule(bbs,fn_name,vars,ret_type);
	//
	//	Topmodule.convert();
	//
	//	Topmodule.generate_rtl_operations();
	//
	//	std::cout<<Topmodule.getRTLCode();
}



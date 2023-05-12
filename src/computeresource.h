#ifndef COMPUTERESOURCE_H
#define COMPUTERESOURCE_H
#include<vector>

//计算资源定义（默认绑定的node的 vector InputVar中都有一个or两个变量，之后再改）
class computeresource {
public:
	//声明计算资源是加法器or乘法器or除法器
	int flag;
	//左边输入寄存器
	std::vector<int> Ainputregisters;
	//右边输入寄存器
	std::vector<int> Binputregisters;
	//输出寄存器
	std::vector<int> outputregisters;
	//定义计算资源，参数为1为加法器，参数为2为乘法器，参数为3为除法器
	computeresource(int flag1) {
		if (flag1 == 1 || flag1 == 2 || flag1 == 3) {
			flag = flag1;
			
		}
		else {
			//std::cout << "wrong computeresource define!" << std::endl;
		}
	}
	computeresource() {};
	void setinputAregisters(int reg) {
		if(reg>0)
			Ainputregisters.push_back(reg);
	}
	void setinputBregisters(int reg) {
		if(reg>0)
			Binputregisters.push_back(reg);
	}
	bool findareg(int reg) {
		if (Ainputregisters.size() == 0) {
			return false;
		}
		for (std::vector<int>::iterator it = Ainputregisters.begin(); it != Ainputregisters.end(); it++) {
			if (reg == *it)
				return true;
		}
		return false;
	}
	bool findbreg(int reg) {
		if (Binputregisters.size() == 0) {
			return false;
		}
		for (std::vector<int>::iterator it = Binputregisters.begin(); it != Binputregisters.end(); it++) {
			if (reg == *it) return true;
		}
		return false;
	}
	bool findoutreg(int reg) {
		if (outputregisters.size() == 0) {
			return false;
		}
		for (std::vector<int>::iterator it = outputregisters.begin(); it != outputregisters.end(); it++) {
			if (reg == *it) return true;
		}
		return false;
	}
	void setoutputregister(int reg) {
		if(reg>0)
			this->outputregisters.push_back(reg);
	}
};
#endif

#include "parser.h"
#include <sstream>
#include <string.h>

basic_block::basic_block(std::string &label) {
	_label = label;
}

void basic_block::add_statement(statement &s) {
	_statements.push_back(s);
}

parser::parser(const std::string &filename)
	: _filename(filename) {
}

int parser::parse() {
	std::ifstream inf(_filename);
	if (!inf)
		return 1;

	std::string line;
	std::string tok;
	std::istringstream iss;
	int function_parse = 0;
	int parse_error = 0;
	basic_block bb;

	bb.set_name("0"); // set to 0 for default

	if (!inf)
		return 1;

	while (getline(inf, line)) {
		_line = line;
		iss.clear();
		iss.str(line);

		if (!(iss >> tok))
			continue;
		else {
			if (tok == "define") {
				function_parse = parse_function(line);
			} else if (function_parse == 0) {
				if (parse_statement(line, bb) != 0) {
					parse_error = 1;
					break;
				}
			} else {
				parse_error = 1;
				break;
			}
		}
	}
	push_back_basic_block(bb);

	return parse_error;
}

int parser::parse_function(std::string &line) {
	char *tok;
	std::string key;
	char sep[] = " (,)";
	int cnt = 0;
	var p;
	size_t pos;
	int parser_error = 0;

	tok = strtok(&line[0], sep);

	while ((tok = strtok(NULL, sep)) != NULL) {
		cnt++;
		key = std::string(tok);
		if (key == "int" && cnt < 2) {
			_ret_type = RET_INT;
		} else if (key == "void") {
			_ret_type = RET_VOID;
		} else if (cnt == 2) {
			_function_name = key;
		} else if (cnt > 2) {
			if (cnt % 2 == 0) {
				key = std::string(tok);
				pos = key.find_first_of('[');
				if (pos == std::string::npos) {
					p._name = key;
					p._array_flag = false;
				} else {
					p._name = key.substr(0, pos);
					p._array_flag = true;
				}
				_function_params.push_back(p);
			}
		} else {
			parser_error = 1;
		}
	}
	return parser_error;
}

void basic_block::clear_statements() {
	_statements.clear();
}

int parser::parse_statement(std::string &line, basic_block &bb) {
	char *tok;
	std::string key;
	char sep[] = " (,);";
//	int cnt = 0;
	var p;
//	int pos;
	int error = 0;
	statement s;
	int label = 0;

	tok = strtok(&line[0], sep);
	// 1. label
	key = std::string(tok);
	if (key.find(':') != std::string::npos) {
		push_back_basic_block(bb);
		bb.clear_statements();
		std::string name = key.substr(0, key.length() - 1);
		bb.set_name(name);
		label = 1;
	} else if (key == "store") { // store
		s.set_type(OP_STORE);
		s.set_num_oprands(3);
		for (int i = 0; i < 3; ++i) {
			tok = strtok(NULL, sep);
			s.add_oprand(tok);
		}
	} else if (key == "br") { // br
		int num_ops = 0;
		s.set_type(OP_BR);
		while ((tok = strtok(NULL, sep)) != NULL) {
			num_ops++;
			s.add_oprand(tok);
		}
		s.set_num_oprands(num_ops);
	} else if (key == "return") { // ret
		int num_ops = 0;
		s.set_type(OP_RET);
		while ((tok = strtok(NULL, sep)) != NULL) {
			num_ops++;
			s.add_oprand(tok);
		}
		s.set_num_oprands(num_ops);
	} else {
//		char sep2[] = " (,);";
//		int num_ops = 0;
		std::vector<std::string> keys;
		keys.push_back(key);
		while ((tok = strtok(NULL, sep)) != NULL) {
			keys.push_back(tok);
		}
		if (keys[2] == "phi") {
			s.set_var(key);
			s.set_type(OP_PHI);
			s.set_num_oprands(keys.size() - 3);
			for (size_t i = 3; i < keys.size(); ++i)
				s.add_oprand(keys[i]);
		} else if (keys[2] == "load") {
			s.set_var(key);
			s.set_type(OP_LOAD);
			s.set_num_oprands(keys.size() - 3);
			for (size_t i = 3; i < keys.size(); ++i)
				s.add_oprand(keys[i]);
		} else if (keys.size() == 3) { // assign
			s.set_var(key);
			s.set_type(OP_ASSIGN);
			s.set_num_oprands(1);
			s.add_oprand(keys[2]);
		} else if (keys[3] == "+") {
			s.set_var(key);
			s.set_type(OP_ADD);
			s.set_num_oprands(2);
			s.add_oprand(keys[2]);
			s.add_oprand(keys[4]);
		} else if (keys[3] == "-") {
			s.set_var(key);
			s.set_type(OP_SUB);
			s.set_num_oprands(2);
			s.add_oprand(keys[2]);
			s.add_oprand(keys[4]);
		} else if (keys[3] == "*") {
			s.set_var(key);
			s.set_type(OP_MUL);
			s.set_num_oprands(2);
			s.add_oprand(keys[2]);
			s.add_oprand(keys[4]);
		} else if (keys[3] == "/") {
			s.set_var(key);
			s.set_type(OP_DIV);
			s.set_num_oprands(2);
			s.add_oprand(keys[2]);
			s.add_oprand(keys[4]);
		} else if (keys[3] == ">") {
			s.set_var(key);
			s.set_type(OP_GT);
			s.set_num_oprands(2);
			s.add_oprand(keys[2]);
			s.add_oprand(keys[4]);
		} else if (keys[3] == "<") {
			s.set_var(key);
			s.set_type(OP_LT);
			s.set_num_oprands(2);
			s.add_oprand(keys[2]);
			s.add_oprand(keys[4]);
		} else if (keys[3] == "==") {
			s.set_var(key);
			s.set_type(OP_EQ);
			s.set_num_oprands(2);
			s.add_oprand(keys[2]);
			s.add_oprand(keys[4]);
		} else if (keys[3] == ">=") {
			s.set_var(key);
			s.set_type(OP_GE);
			s.set_num_oprands(2);
			s.add_oprand(keys[2]);
			s.add_oprand(keys[4]);
		} else if (keys[3] == "<=") {
			s.set_var(key);
			s.set_type(OP_LE);
			s.set_num_oprands(2);
			s.add_oprand(keys[2]);
			s.add_oprand(keys[4]);
		} else
			error = 1;
	}
	if (error == 0 && label == 0)
		bb.add_statement(s);

	return error;
}

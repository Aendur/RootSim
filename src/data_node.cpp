#include "data_node.h"
#include "register.h"
#include <stdexcept>

DataNode & DataNode::operator+=(DataNode & ds) {
	this->downstream.push_back(std::ref(ds));
	ds.upstream.push_back(std::ref(*this));
	return *this;
}

static const char * UPPER_BAR = "├──┴──┴──┴──┴──┴──┴──┴──┤";
static const char * MIDDL_BAR = "├┤     %10s        │";
static const char * LOWER_BAR = "└───────────────────────┘";

void DataNode::Draw(int x, int y) const {
	value.Draw(x, y);
	char buffer[128];
	snprintf(buffer, 128, MIDDL_BAR, name.c_str());	

	this->Drawable::Draw(x+25, y+2, UPPER_BAR);
	this->Drawable::Draw(x+24, y+3, buffer);
	this->Drawable::Draw(x+25, y+4, LOWER_BAR);
}

const Register DataNode::add(DataNode & node) {
	Register result = 0;
	for (const auto & reg : node.upstream) {
		result += reg.get().Read();
	}
	return result;
}

const Register DataNode::mul(DataNode & node) {
	Register result = 1;
	for (const auto & reg : node.upstream) {
		result *= reg.get().Read();
	}
	return result;
}

const Register DataNode::inv(DataNode & node) {
	if (node.upstream.size() != 1) {
		throw std::logic_error("inv only takes 1 operand");
	}
	return 1 / node.upstream[0].get().Read();
}

const Register DataNode::mov(DataNode & node) {
	if (node.upstream.size() != 1) {
		throw std::logic_error("mov only takes 1 operand");
	}
	return node.upstream[0].get().Read();
}


const Register DataNode::cte(DataNode & node) {
	if (node.upstream.size() != 0) {
		throw std::logic_error("cte does not take any operands");
	}
	return node.value;
}


#ifndef DATA_NODE_H
#define DATA_NODE_H

#include <vector>
#include <functional>
#include <string>
#include "drawable.h"
#include "register.h"

class Register;
class DataNode;

typedef std::reference_wrapper<DataNode> DataNodeRef;
typedef std::function<const Register (DataNode &)> DataNodeAct;
class DataNode : public Drawable {
private:
	std::vector<DataNodeRef> upstream;
	std::vector<DataNodeRef> downstream;
	std::string name;
	Register value;
	DataNodeAct action;
public:
	//DataNode(void) : Drawable() {}
	DataNode(const std::string & name, const Register && reg, DataNodeAct act) : Drawable(), name(name), value(reg), action(act) { }
	DataNode & operator+=(DataNode & downstream);
	inline void Tick(void) { value = action(*this); }
	inline const Register & Read(void) const { return value; }

	void Draw(int x, int y) const;

	static const Register add(DataNode & node);
	static const Register mul(DataNode & node);
	static const Register inv(DataNode & node);
	static const Register mov(DataNode & node);
	static const Register cte(DataNode & node);
};



#endif
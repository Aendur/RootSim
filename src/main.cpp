#include "vtc.h"
#include "register.h"
#include "data_node.h"
#include <cstdio>
#include <stdexcept>
#include <thread>
#include <chrono>


int main(int argc, char ** argv) {
	VTC & vtc = VTC::vtc();

	vtc.clear_screen();
	//KeyCode key = VTC_KEY_UNDEFINED;

	const float INIT = 2.0f;
	float op = 2.0f;
	if (argc > 1) {
		try { op = std::stof(argv[1]); }
		catch (std::exception &) {}
	}

	DataNode operand("operand", Register(op), &DataNode::cte);
	DataNode approx1("approx1", Register( INIT), &DataNode::mul);
	DataNode appinv ("appinv" , Register(-1.0f), &DataNode::inv);
	DataNode complem("complem", Register(-1.0f), &DataNode::mul);
	
	DataNode valadd ("valadd" , Register(-1.0f), &DataNode::add);
	DataNode valhlf ("valhlf" , Register( 0.5f), &DataNode::cte);

	approx1 += appinv;
	appinv  += complem;
	operand += complem;
	approx1 += valadd;
	complem += valadd;
	valadd += approx1;
	valhlf += approx1;

	int i = 0;
	int MAXI = 30;
	do {
		approx1.Draw(  5,10);
		operand.Draw(125, 5);
		appinv .Draw(125,10);
		complem.Draw(125,15);
		valadd .Draw( 65,20);
		valhlf .Draw( 65,25);

		operand.Tick();
		appinv .Tick();
		complem.Tick();
		valadd .Tick();
		valhlf .Tick();
		approx1.Tick();

		//key = vtc.read();
		std::this_thread::sleep_for(std::chrono::duration<int, std::milli>(500));
	} while(++i < MAXI);

	printf("main return 0\n");
	return 0;
}


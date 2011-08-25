/*
  BasicExample.pde - An example of how to use the Arduide's IDEdbg library
  Please open the Utility dock, then select the Debugger tab.
  Press start, it will compile and upload this example.
  Select the Watcher tab to see the frames
*/

#include <IDEdbg.h>

int func2()
{
	DbgNewFrame("another frame");
	char c = 'M';
	char* s = "MuPuFen";

	DbgWatchVariable(c);
	DbgWatchVariable(s);
	
	DbgSendState();
	
	DbgCloseFrame();
	
	return 0;
}

int func1()
{
	DbgNewFrame("frame func1");
	
	double d = 0.369;
	float f = 0.123456;
	int i = 42;
	char c = 'M';
	char* s = "MuPuFen";
	
	DbgWatchVariable(d);
	DbgWatchVariable(f);
	DbgWatchVariable(i);
	DbgWatchVariable(c);
	DbgWatchVariable(s);
	
	DbgSendState();
	
	i = 46;
	func2();
	
	DbgSendState();
	
	DbgCloseFrame();
	
	return 0;
}

void setup()
{
	DbgInit(19200);
	DbgSendTrace("Trace example: decimal=%i, hexa=%x, char=%c and string='%s'", 42, 0x42, 'P', "it works!");
	
	func1();
}

int i = 0;
void loop()
{
	DbgSendTrace("loop: iteration number %i", i++);
	delay(1000);
}

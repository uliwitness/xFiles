// -----------------------------------------------------------------------------
//	Headers
// -----------------------------------------------------------------------------

#include <HyperXCmd.h>
#include "XCmdUtils.h"
#include <SetupA4.h>
#include <A4Stuff.h>


// -----------------------------------------------------------------------------
//	Main entry point
// -----------------------------------------------------------------------------

pascal void main(XCmdPtr paramBlock) {
	EnterCodeResource();

	gXCmdBlock = paramBlock;

	xcmdmain();
	
	gXCmdBlock = NULL;
	
	ExitCodeResource();
}
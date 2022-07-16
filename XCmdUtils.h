// -----------------------------------------------------------------------------
//	Headers
// -----------------------------------------------------------------------------

#include <HyperXCmd.h>
#include <Types.h>


// -----------------------------------------------------------------------------
//	We save the param block here so you don't have to pass it to every function.
// -----------------------------------------------------------------------------

extern XCmdPtr gXCmdBlock;


// -----------------------------------------------------------------------------
//	Signature for your main function.
// -----------------------------------------------------------------------------

extern void xcmdmain(void);

// Main function you specify as the entry point:
extern pascal void	main(XCmdPtr paramBlock);


// -----------------------------------------------------------------------------
//	Prototypes
// -----------------------------------------------------------------------------

extern CharsHandle	GetIndXParameter(short index);
extern Boolean		GetIndXParameter255(short index, Str255 outStr);
extern void			SetReturnValue(const Str255 result);
extern void			AppendReturnValue(const Str255 str);
extern void			AppendReturnValueBytes(long len, const void* bytes);
extern void			AppendReturnValueChars(CharsHandle chars);
extern void			RemoveTrailingReturnFromReturnValue(void);

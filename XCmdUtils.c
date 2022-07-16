// -----------------------------------------------------------------------------
//	Headers
// -----------------------------------------------------------------------------

#include "XCmdUtils.h"
#include <string.h>


// -----------------------------------------------------------------------------
//	Globals
// -----------------------------------------------------------------------------

XCmdPtr gXCmdBlock = NULL;


// -----------------------------------------------------------------------------
//	GetIndXParameter
// -----------------------------------------------------------------------------

CharsHandle GetIndXParameter(short index) {
	if (gXCmdBlock->paramCount < index) return NULL;
	return gXCmdBlock->params[index - 1];
}


// -----------------------------------------------------------------------------
//	GetIndXParameter255
// -----------------------------------------------------------------------------

Boolean GetIndXParameter255(short index, Str255 outStr) {
	if (gXCmdBlock->paramCount < index) return false;
	outStr[0] = GetHandleSize(gXCmdBlock->params[index - 1]) -1;
	memcpy(outStr + 1, *gXCmdBlock->params[index - 1], outStr[0]);
	return true;
}


// -----------------------------------------------------------------------------
//	SetReturnValue
// -----------------------------------------------------------------------------

void SetReturnValue(const Str255 result) {
	if (gXCmdBlock->returnValue == NULL) {
		gXCmdBlock->returnValue = NewHandle(result[0] + 1);
	} else {
		SetHandleSize(gXCmdBlock->returnValue, result[0] + 1);
	}
	memcpy(*gXCmdBlock->returnValue, result + 1, result[0]);
	(*gXCmdBlock->returnValue)[result[0]] = '\0';
}


// -----------------------------------------------------------------------------
//	AppendReturnValue
// -----------------------------------------------------------------------------

void AppendReturnValue(const Str255 result) {
	long stringEnd = 0;
	if (gXCmdBlock->returnValue == NULL) {
		gXCmdBlock->returnValue = NewHandle(result[0] + 1);
	} else {
		stringEnd = strlen(*gXCmdBlock->returnValue);
		SetHandleSize(gXCmdBlock->returnValue, stringEnd + result[0] + 1);
	}
	memcpy(*gXCmdBlock->returnValue + stringEnd, result + 1, result[0]);
	(*gXCmdBlock->returnValue)[GetHandleSize(gXCmdBlock->returnValue) - 1] = '\0';
}


// -----------------------------------------------------------------------------
//	AppendReturnValueBytes
//		Appends bytes as text. Replaces any NUL characters in the bytes with
//		spaces to avoid confusing HyperCard's C strings.
// -----------------------------------------------------------------------------

void AppendReturnValueBytes(long len, const void* bytes) {
	long 	stringEnd = 0;
	char*	dest = NULL;
	long	x = 0;
	
	if (gXCmdBlock->returnValue == NULL) {
		gXCmdBlock->returnValue = NewHandle(len + 1);
	} else {
		stringEnd = GetHandleSize(gXCmdBlock->returnValue) - 1;
		SetHandleSize(gXCmdBlock->returnValue, stringEnd + len + 1);
	}
	memcpy(*gXCmdBlock->returnValue + stringEnd, bytes, len);
	(*gXCmdBlock->returnValue)[GetHandleSize(gXCmdBlock->returnValue) - 1] = '\0';
	
	dest = (*gXCmdBlock->returnValue) + stringEnd;
	for(x = 0; x < len; ++x) {
		if (dest[x] == 0) {
			dest[x] = ' ';
		}
	}
}


// -----------------------------------------------------------------------------
//	RemoveTrailingReturnFromReturnValue
// -----------------------------------------------------------------------------

void AppendReturnValueChars(CharsHandle chars) {
	HLock(chars);
	AppendReturnValueBytes(GetHandleSize(chars) - 1, *chars);
	HUnlock(chars);
}


// -----------------------------------------------------------------------------
//	RemoveTrailingReturnFromReturnValue
// -----------------------------------------------------------------------------

void RemoveTrailingReturnFromReturnValue(void) {
	long handleLen = 0;
	if (gXCmdBlock->returnValue == NULL) return;
	
	handleLen = GetHandleSize(gXCmdBlock->returnValue);
	
	if (handleLen < 2) return;
	if ((*gXCmdBlock->returnValue)[handleLen - 2] != '\r') return;
	
	(*gXCmdBlock->returnValue)[handleLen - 2] = '\0';
	SetHandleSize(gXCmdBlock->returnValue, handleLen - 1);
}

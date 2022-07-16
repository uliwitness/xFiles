// -----------------------------------------------------------------------------
//	Headers
// -----------------------------------------------------------------------------

#include <HyperXCmd.h>
#include <Types.h>
#include <Memory.h>
#include <OSUtils.h>
#include <ToolUtils.h>
#include <Files.h>
#include <Aliases.h>
#include <Errors.h>
#include <string.h>
#include <limits.h>
#include "XCmdUtils.h"
#include "MissingFiles.h"


// -----------------------------------------------------------------------------
//	Prototypes
// -----------------------------------------------------------------------------

OSErr GetVRefNumAndDirIDFromPath(CharsHandle filePath, short *vRefNum, short *dirID);
OSErr AppendIndFilenameInFolder(short vRefNum, short dirID, short dirIndex, CharsHandle dirPath);


// -----------------------------------------------------------------------------
//	GetVRefNumAndDirIDFromPath
// -----------------------------------------------------------------------------

OSErr GetVRefNumAndDirIDFromPath(CharsHandle filePath, short *vRefNum, short *dirID) {
	OSErr		err = noErr;
	Str255		errStr = {0};
	AliasHandle	alias = NULL;
	Boolean		wasChanged = false;
	FSSpec		target = {0};
	CInfoPBRec	catInfo = {0};
	Str255		fileNameBuffer = {0};
		
	HLock(filePath);
	err = NewAliasMinimalFromFullPath(strlen(*filePath), *filePath, NULL, NULL, &alias);
	HUnlock(filePath);
	
	if (err != noErr) {
		NumToString(err, errStr);
		SetReturnValue("\pCan't locate file at file path: ");
		AppendReturnValue(errStr);
		return err;
	}
	
	err = ResolveAlias(NULL, alias, &target, &wasChanged);
	
	DisposeHandle((Handle)alias);
	alias = NULL;

	if (err != noErr) {
		NumToString(err, errStr);
		SetReturnValue("\pError getting folder info from path: ");
		AppendReturnValue(errStr);
		return err;
	} else if (target.vRefNum == 0) { // MoreFiles says this is a bug.
		SetReturnValue("\pCouldn't find volume.");
		return nsvErr;
	}
	
	memcpy(fileNameBuffer, target.name, target.name[0] + 1);	
	catInfo.dirInfo.ioVRefNum = target.vRefNum;
	catInfo.dirInfo.ioDrDirID = target.parID;
	catInfo.dirInfo.ioFDirIndex = 0;
	catInfo.dirInfo.ioNamePtr = fileNameBuffer;
	
	err = PBGetCatInfoSync(&catInfo);
	if (err != noErr) {
		NumToString(err, errStr);
		SetReturnValue("\pError getting folder ID from path: ");
		AppendReturnValue(errStr);
		return err;
	}
	
	*vRefNum = catInfo.dirInfo.ioVRefNum;
	*dirID = catInfo.dirInfo.ioDrDirID;
	
	return noErr;
}


// -----------------------------------------------------------------------------
//	AppendIndFilenameInFolder
// -----------------------------------------------------------------------------

OSErr AppendIndFilenameInFolder(short vRefNum, short dirID, short dirIndex, CharsHandle dirPath) {
	CInfoPBRec	catInfo = {0};
	Str255		fileNameBuffer = {0};
	Str255		errStr = {0};
	OSErr		err = noErr;
	Boolean		isFolder = false;
	long		dirPathLength = strlen(*dirPath);
	int			x = 0;
	
	fileNameBuffer[0] = 0;
	catInfo.dirInfo.ioVRefNum = vRefNum;
	catInfo.dirInfo.ioDrDirID = dirID;
	catInfo.dirInfo.ioFDirIndex = dirIndex;
	catInfo.dirInfo.ioNamePtr = fileNameBuffer;
	
	err = PBGetCatInfoSync(&catInfo);
	if (err == fnfErr) { // Past last file.
		RemoveTrailingReturnFromReturnValue();
		return err;
	} else if (err != noErr) {
		NumToString(err, errStr);
		SetReturnValue("\pError getting next file: ");
		AppendReturnValue(errStr);
		return err;
	}

	if ((catInfo.hFileInfo.ioFlFndrInfo.fdFlags & fInvisible) == fInvisible) { // Skip invisible files.
		return noErr;
	}
	
	for (x = 1; x <= fileNameBuffer[0]; ++x) { // Skip files with "invalid" names like "Icon\r" file used to store custom icons.
		if (fileNameBuffer[x] == '\r') {
			return noErr;
		}
	}
	
	isFolder = (catInfo.hFileInfo.ioFlAttrib & kFolderBit) == kFolderBit;
	AppendReturnValueChars(dirPath);
	if (dirPathLength > 0 && (*dirPath)[dirPathLength - 1] != ':') {
		AppendReturnValue("\p:");
	}
	AppendReturnValue(fileNameBuffer);
	if (isFolder) {
		AppendReturnValue("\p:foldMACS\r");
	} else {
		AppendReturnValue("\p:");
		AppendReturnValueBytes(4, &catInfo.hFileInfo.ioFlFndrInfo.fdType);
		AppendReturnValueBytes(4, &catInfo.hFileInfo.ioFlFndrInfo.fdCreator);
		AppendReturnValue("\p\r");
	}
	
	return noErr;
}


// -----------------------------------------------------------------------------
//	xcmdmain
// -----------------------------------------------------------------------------

void xcmdmain(void)
{
	CInfoPBRec catInfo = {0};
	Str255 		errStr = {0};
	OSErr 		err = noErr;
	short 		dirID = 0;
	short 		fRefNum = 0;
	short 		vRefNum = 0;
	short 		dirIndex = 0;
	CharsHandle folderPath = NULL;
	
	if ((folderPath = GetIndXParameter(1)) == NULL) {
		AppendReturnValue("\pExpected folder path to list as first parameter.");
		return;
	}
	
	if (strcmp("?", *folderPath) == 0) {
		AppendReturnValue("\pSyntax: put|get xFiles(<folder path>)");
		return;
	} else if (strcmp("!", *folderPath) == 0) {
		AppendReturnValue("\p(c) Copyright 2021 by Uli Kusterer, all rights reserved.");
		return;
	}
	
	if (GetVRefNumAndDirIDFromPath(folderPath, &vRefNum, &dirID) != noErr) {
		return;
	}

	for (dirIndex = 1; dirIndex <= SHRT_MAX; ++dirIndex)
	{
		err = AppendIndFilenameInFolder(vRefNum, dirID, dirIndex, folderPath);
		if (err != noErr) break;
	}
}
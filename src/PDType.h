//
// PDType.h
//
// Copyright (c) 2013 Karl-Johan Alm (http://github.com/kallewoof)
// 
// This program is free software: you can redistribute it and/or modify
// it under the terms of the GNU General Public License as published by
// the Free Software Foundation, either version 3 of the License, or
// (at your option) any later version.
// 
// This program is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU General Public License for more details.
// 
// You should have received a copy of the GNU General Public License
// along with this program.  If not, see <http://www.gnu.org/licenses/>.
//

/**
 @file PDType.h Pajdeg type header.
 
 @ingroup PDTYPE
 
 @defgroup PDTYPE PDType
 
 @brief The generic Pajdeg object type.
 
 PDType provides the release and retain functionality to Pajdeg objects. 
 
 @warning Not all types in Pajdeg *are* Pajdeg objects. An object whose type is camel case and begins with capital `PD` is always a Pajdeg object, and can be retained and released, whereas objects using underscore separation in all lower case are non-Pajdeg objects (used by the Pajdeg system), such as pd_btree. The exception to this is primitive objects, such as PDInteger, PDBool, etc., and PDType itself.
 
 ## Concept behind Release and Retain
 
 Releasing and retaining is a method for structuring memory management, and is used e.g. in Objective-C and other languages. The concept is fairly straightforward:
 
 - calling a function that contains the word "Create" means you own the resulting object and must at some point release (PDRelease()) or autorelease (PDAutorelease()) it
 - retaining an object via PDRetain() means you have to release it at some point
 - objects are only valid until the end of the function executing unless they are retained
 
 There are also instances where an object will become invalid in the middle of a function body, if it is released and has no pending autorelease calls.
 
 For example, the following code snippet shows how this works:
 
 @dontinclude examples/add-metadata.c
 
 @skip create
 @until argv
 
 The `PDPipe` object is retained because the function name has the word `Create` in it, so it must be released.
 
 @skip parser
 @until GetPars
 
 The `PDParser` is not retained, as we obtained it through a function whose name did not contain `Create` -- consequently, the `parser` variable must not be released as we don't own a reference to it.
 
 @skip brand
 @until Create
 
 The `meta` object must be released by us, as it has the word `Create`.
 
 @ingroup PDINTERNAL
 
 @{
 */
#ifndef ICViewer_PDType_h
#define ICViewer_PDType_h

#include "PDDefines.h"

/**
 Release a Pajdeg object. 
 
 If the caller was the last referrer, the object will be destroyed immediately.
 
 @param pajdegObject A previously retained or created `PD` object.
 */
extern void PDRelease(void *pajdegObject);

/**
 Retain a Pajdeg object.
 
 @param pajdegObject A `PD` object.
 */
extern void *PDRetain(void *pajdegObject);

/**
 Autorelease a Pajdeg object.
 
 Even if the caller was the last referrer, the object will not be destroyed until the PDPipeExecute() iteration for the current object has ended.
 
 Thus, it is possible to return an object with a zero retain count by autoreleasing it.
 
 @param pajdegObject The object that should, at some point, be released once.
 */
extern void *PDAutorelease(void *pajdegObject);

#endif

/** @} */
//
// PDPage.h
//
// Copyright (c) 2014 Karl-Johan Alm (http://github.com/kallewoof)
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
 @file PDPage.h PDF page header file.
 
 @ingroup PDPAGE
 
 @defgroup PDPAGE PDPage
 
 @brief A PDF page.
 
 @ingroup PDUSER

 A representation of a page inside a PDF document. It is associated with an object and has a reference to the PDParser instance for the owning document.
 
 @{
 */

#ifndef INCLUDED_PDPage_h
#define INCLUDED_PDPage_h

#include "PDDefines.h"

/**
 *  Create a new page instance for the given parser and page index. 
 *
 *  @note Page indices start at 1, not 0.
 *
 *  @param parser    Parser reference
 *  @param pageIndex The index of the page to fetch
 *
 *  @return New PDPage object
 */
extern PDPageRef PDPageCreateForPageWithIndex(PDParserRef parser, PDInteger pageIndex);

/**
 *  Create a new page instance for the given parser and object.
 *
 *  @note The object must belong to the parser, and may not be from a foreign instance of Pajdeg.
 *
 *  @note The object does not necessarily have to be a proper page at the time of creation, but it should (ideally) be one eventually.
 *
 *  @param parser The parser owning object
 *  @param object The /Page object
 *
 *  @return A new page instance for the associated object
 */
extern PDPageRef PDPageCreateWithObject(PDParserRef parser, PDObjectRef object);

/**
 *  Copy this page and all associated objects into the PDF document associated with the pipe, inserting it at pageIndex.
 *
 *  With two separate simultaneous Pajdeg instances A and B with pages A1 A2 A3 and B1 B2 B3, the following operation
 *
    @code
      PDPageRef pageA2 = PDPageCreateForPageWithIndex(parserA, 2); 
      PDPageRef pageB2 = PDPageInsertIntoPipe(pageA2, pipeB, 2);
    @endcode
 *
 *  will result in the output of B consisting of pages B1 A2 B2 B3, in that order.
 * 
 *  @param page      The page object that should be copied
 *  @param pipe      The pipe into which the page object should be inserted
 *  @param pageIndex The resulting page number of the inserted page
 *
 *  @return Reference to the inserted page, autoreleased
 */
extern PDPageRef PDPageInsertIntoPipe(PDPageRef page, PDPipeRef pipe, PDInteger pageIndex);

#endif

/** @} */

/** @} */ // unbalanced, but doxygen complains for some reason


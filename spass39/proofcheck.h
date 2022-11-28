/**************************************************************/
/* ********************************************************** */
/* *                                                        * */
/* *             PROOF CHECKING                             * */
/* *                                                        * */
/* *  Copyright (C) 1998, 1999, 2000, 2001                  * */
/* *  MPI fuer Informatik                                   * */
/* *                                                        * */
/* *  This program is free software; you can redistribute   * */
/* *  it and/or modify it under the terms of the FreeBSD    * */
/* *  Licence.                                              * */
/* *                                                        * */
/* *  This program is distributed in the hope that it will  * */
/* *  be useful, but WITHOUT ANY WARRANTY; without even     * */
/* *  the implied warranty of MERCHANTABILITY or FITNESS    * */
/* *  FOR A PARTICULAR PURPOSE.  See the LICENCE file       * */ 
/* *  for more details.                                     * */
/* *                                                        * */
/* $Revision: 1.3 $                                         * */
/* $State: Exp $                                            * */
/* $Date: 2010-02-22 14:09:58 $                             * */
/* $Author: weidenb $                                       * */
/* *                                                        * */
/* *                                                        * */
/* *             Contact:                                   * */
/* *             Christoph Weidenbach                       * */
/* *             MPI fuer Informatik                        * */
/* *             Stuhlsatzenhausweg 85                      * */
/* *             66123 Saarbruecken                         * */
/* *             Email: spass@mpi-inf.mpg.de                * */
/* *             Germany                                    * */
/* *                                                        * */
/* ********************************************************** */

/* $RCSfile: proofcheck.h,v $ */

#ifndef _PROOFCHECK_H_
#define _PROOFCHECK_H_

#include <string.h>
#include <ctype.h>
#include <math.h>
#include "list.h"
#include "vector.h"

#include "misc.h"
#include "dfg.h"
#include "foldfg.h"
#include "flags.h"
#include "clause.h"
#include "tableau.h"
#include "search.h"
#include "dfg.h"


LIST  pcheck_ReduceSPASSProof(LIST);
char* pcheck_GenericFilename(const char*, const char*);
int   pcheck_SeqProofDepth(LIST);
void  pcheck_DeleteProof(LIST);
BOOL  pcheck_BuildTableauFromProof(LIST, TABLEAU*);
LIST  pcheck_ConvertTermListToClauseList(LIST, FLAGSTORE, PRECEDENCE);
void  pcheck_TableauToProofTask(TABLEAU, const char*, const char*);
BOOL  pcheck_TableauProof(TABLEAU*, LIST);
LIST  pcheck_ParentPointersToParentNumbers(LIST);
LIST  pcheck_ConvertParentsInSPASSProof(PROOFSEARCH, LIST);
void  pcheck_MarkRecursive(LIST); 
LIST  pcheck_ClauseNumberMergeSort(LIST);
void  pcheck_ClauseListRemoveFlag(LIST, CLAUSE_FLAGS);

#endif




/**************************************************************/
/* ********************************************************** */
/* *                                                        * */
/* *                  TABLEAU PROOF TREES                   * */
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
/* $Revision: 1.5 $                                         * */
/* $State: Exp $                                            * */
/* $Date: 2011-05-25 10:03:04 $                             * */
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

/* $RCSfile: tableau.c,v $ */

#include "tableau.h" 
/**************************************************************/
/* previously Inlined Functions                               */
/**************************************************************/
 int  tab_PathLength(TABPATH TabPath)
{
  return TabPath->Length;
}

 TABLEAU tab_PathNthNode(TABPATH TabPath, int n)
{
#ifdef CHECK
  if (n > TabPath->MaxLength) {
    misc_StartErrorReport();
    misc_ErrorReport("\n In tab_PathNthNode:");
    misc_ErrorReport(" Path length is %d,", tab_PathLength(TabPath));
    misc_ErrorReport("\nnode %d was requested.\n", n);
    misc_FinishErrorReport();
  }
#endif  

  return TabPath->Path[n];
}

 TABPATH tab_PathPush(TABLEAU Tab, TABPATH TabPath)
{
  TabPath->Length++;
  TabPath->Path[TabPath->Length] = Tab;
  
#ifdef CHECK
  if (TabPath->Length > TabPath->MaxLength) {
    misc_StartErrorReport();
    misc_ErrorReport("\n In tab_PathPush: Maximum path length %d exceeded\n",
		     TabPath->MaxLength);
    misc_FinishErrorReport();
  }
#endif
  
  return TabPath;
}

 TABLEAU tab_EmptyTableau(void)
{
  return (TABLEAU)NULL;
}

 TABPATH tab_PathPop(TABPATH TabPath)
{
#ifdef CHECK
  if (TabPath->Length <= 0) {
    misc_StartErrorReport();
    misc_ErrorReport("\n In tab_PathPop: Popping from empty path.\n");
    misc_FinishErrorReport();
  }
#endif

  TabPath->Path[TabPath->Length--] = tab_EmptyTableau();
  
  return TabPath;
}

 BOOL tab_PathEmpty(TABPATH P)
{
  return (tab_PathLength(P) == 0);
}

 TABLEAU tab_CreateNode(void)
{
  TABLEAU Node;

  Node = (TABLEAU)memory_Malloc(sizeof(TABLEAU_NODE));
  Node->RightBranch = (TABLEAU)NULL;
  Node->LeftBranch  = (TABLEAU)NULL;
  Node->SplitClause = (CLAUSE)NULL;
  Node->LeftSplitClause   = (CLAUSE)NULL;
  Node->RightSplitClauses = list_Nil();

  Node->Clauses     = list_Nil();
 
#ifdef USE_LABEL
  Node->Label       = 0;
#endif
  
  return Node;
}

 TABPATH tab_PathPrefix(int Level, TABPATH TabPath)
{
  TabPath->Length = Level;
  return TabPath;
}

 TABLEAU tab_PathTop(TABPATH Path)
{
  return tab_PathNthNode(Path, tab_PathLength(Path));
}


 BOOL tab_IsEmpty(TABLEAU Tab)
{
 return (Tab == tab_EmptyTableau());
}

 TABLEAU tab_RightBranch(TABLEAU Tab)
{
 return Tab->RightBranch;
}

 TABLEAU tab_LeftBranch(TABLEAU Tab)
{
 return Tab->LeftBranch;
}

 void tab_SetRightBranch(TABLEAU Tab, TABLEAU SubTab)
{
  Tab->RightBranch = SubTab;
} 

 void tab_SetLeftBranch(TABLEAU Tab, TABLEAU SubTab)
{
  Tab->LeftBranch = SubTab;
} 

 BOOL tab_RightBranchIsEmpty(TABLEAU Tab)
{
  return (Tab->RightBranch == tab_EmptyTableau());
}

 BOOL tab_LeftBranchIsEmpty(TABLEAU Tab)
{
  return (Tab->LeftBranch == tab_EmptyTableau());
}

 CLAUSE tab_SplitClause(TABLEAU Tab)
{
  return Tab->SplitClause;
}

 void tab_SetSplitClause(TABLEAU Tab, CLAUSE C)
{
  Tab->SplitClause = C;
}

 BOOL tab_HasSplit(TABLEAU T)
{
  return (tab_SplitClause(T) != clause_Null());
}

 void tab_AddClause(CLAUSE C,TABLEAU T)
{
  T->Clauses = list_Cons(C,T->Clauses);
}

 LIST tab_Clauses(TABLEAU T)
{
  return T->Clauses;
}

 void tab_SetClauses(TABLEAU T, LIST Clauses)
{
  T->Clauses = Clauses;
}

 CLAUSE tab_LeftSplitClause(TABLEAU T)
{
  return T->LeftSplitClause;
}

 void tab_SetLeftSplitClause(TABLEAU T, CLAUSE C)
{
  T->LeftSplitClause = C;
}


 LIST tab_RightSplitClauses(TABLEAU T)
{
  return T->RightSplitClauses;
}


 void tab_SetRightSplitClauses(TABLEAU T, LIST L)
{
  T->RightSplitClauses = L;
}

 void tab_AddRightSplitClause(TABLEAU T, CLAUSE C)
{
  T->RightSplitClauses = list_Cons(C, T->RightSplitClauses);
}

 BOOL tab_IsLeaf(TABLEAU T)
{
  return (tab_RightBranchIsEmpty(T) && tab_LeftBranchIsEmpty(T));
}

/**************************************************************/
/* Inline Functions ends here                                 */
/**************************************************************/
/* for graph output */
extern BOOL        pcheck_ClauseCg;
extern GRAPHFORMAT pcheck_GraphFormat;

TABPATH tab_PathCreate(int MaxLevel, TABLEAU Tab)
/**************************************************************
  INPUT:   A tableau, its maximum expected depth
  RETURNS: A path consisting of the root  node of the tableau
           which can have a max length of <MaxLevel>
***************************************************************/
{
  TABPATH TabPath;

  TabPath            = (TABPATH)memory_Malloc(sizeof(TABPATH_NODE));
  TabPath->Path      = (TABLEAU*)memory_Malloc(sizeof(TABLEAU)*(MaxLevel+1));
  TabPath->Path[0]   = Tab;
  TabPath->MaxLength = MaxLevel;
  TabPath->Length    = 0;
  
  return TabPath;
}

void tab_PathDelete(TABPATH TabPath)
/**************************************************************
  INPUT:   A tableau path.
  RETURNS: Nothing.
  EFFECTS: The path is deleted.
***************************************************************/
{
  memory_Free(TabPath->Path, (TabPath->MaxLength+1)*sizeof(TABLEAU));
  memory_Free(TabPath, sizeof(TABPATH_NODE));
}


BOOL tab_PathContainsClause(TABPATH Path, CLAUSE Clause)
/**************************************************************
  INPUT:   A tableau path, a clause
  RETURNS: TRUE iff the clause exists on its level (wrt. the
           path) in the tableau
***************************************************************/
{
  LIST Scan;

  if (clause_SplitLevel(Clause) > tab_PathLength(Path))
    return FALSE;

  for (Scan = tab_Clauses(tab_PathNthNode(Path, clause_SplitLevel(Clause))); 
       !list_Empty(Scan); Scan = list_Cdr(Scan)) {
    if (list_Car(Scan) == Clause)
      return TRUE;
  }
  return FALSE;
}

static BOOL tab_PathContainsClauseSoft(TABPATH Path, CLAUSE Clause)
/**************************************************************
  INPUT:   A tableau path, a clause
  RETURNS: TRUE iff the clause is on one of the levels
           in the tableau traversed by the path. Different
	   from tab_PathContainsClauseSoft, since it does
           not expect the clause on its split level.
***************************************************************/
{
  LIST Scan;
  int Level;

  if (clause_SplitLevel(Clause) > tab_PathLength(Path))
    return FALSE;
  
  for (Level = 0; Level <= tab_PathLength(Path); Level++) {
    for (Scan = tab_Clauses(tab_PathNthNode(Path, Level));  
	 !list_Empty(Scan); Scan = list_Cdr(Scan)) {
      if (list_Car(Scan) == Clause)
	return TRUE;
    }
  }
  return FALSE;
}

/* unused */
/*static*/ BOOL tab_PathContainsClauseRobust(TABPATH P, CLAUSE C)
/**************************************************************
  INPUT:   A tableau path, a clause
  RETURNS: TRUE if clause can be find on the path (not
           necessarily on its level)
  EFFECTS: Prints a note if clause cannot be found on
           its level. Intended for debugging.
***************************************************************/
{
  if (tab_PathContainsClause(P,C)) 
    return TRUE;

  if (tab_PathContainsClauseSoft(P,C)) {
    fputs("NOTE: Clause is found on path, but not indexed by level.\n", stderr);
    clause_PParentsFPrint(stderr,C);
    fflush(stderr);
    return TRUE;
  }
  return FALSE;
}


void tab_AddSplitAtCursor(TABPATH Path, BOOL LeftSide)
/**************************************************************
  INPUT:   A tableau path, a flag
  RETURNS: Nothing.
  EFFECTS: Extends the tableau containing the path <Path> to
           the left if <LeftSide> is TRUE, to the right 
	   otherwise
***************************************************************/
{
  TABLEAU Tab, NewBranch;

  Tab = tab_PathTop(Path);
  NewBranch = tab_CreateNode();
  if (LeftSide) {

#ifdef CHECK
    if (!tab_LeftBranchIsEmpty(Tab)) {
      misc_StartErrorReport();
      misc_ErrorReport("\n In tab_AddSplitAtCursor: Recreating existing");
      misc_ErrorReport(" left branch in tableau.\n");
      misc_FinishErrorReport();
    }
#endif

    tab_SetLeftBranch(Tab,NewBranch);
  } else {

#ifdef CHECK    
    if (!tab_RightBranchIsEmpty(Tab)) {
      misc_StartErrorReport();
      misc_ErrorReport("\n In tab_AddSplitAtCursor: Recreating existing");
      misc_ErrorReport(" right branch in tableau.\n");
      misc_FinishErrorReport();
    }
#endif
    tab_SetRightBranch(Tab, NewBranch);
  }
  tab_PathPush(NewBranch, Path);
}

void tab_AddClauseOnItsLevel(CLAUSE C, TABPATH Path)
/**************************************************************
  INPUT:   A clause, a tableau path 
  RETURNS: Nothing
  EFFECTS: Adds the clause on its split level which
           must belong to <Path>
***************************************************************/
{
  int Level; 

  Level = clause_SplitLevel(C);
  if (Level > tab_PathLength(Path)) {
    misc_StartUserErrorReport();
    misc_UserErrorReport("\nError: Split level of some clause ");
    misc_UserErrorReport("\nis higher than existing level."); 
    misc_UserErrorReport("\nThis may be a bug in the proof file."); 
    misc_FinishUserErrorReport();  
  }
  
  tab_AddClause(C, tab_PathNthNode(Path, clause_SplitLevel(C)));
}


int tab_Depth(TABLEAU T)
/**************************************************************
  INPUT:   A tableau
  RETURNS: The depth of the tableau
***************************************************************/
{
  if (tab_IsEmpty(T))
    return 0;
  if (tab_IsLeaf(T))
    return 0;
  else 
    return (misc_Max(tab_Depth(tab_RightBranch(T))+1, tab_Depth(tab_LeftBranch(T)))+1);
}

static BOOL tab_HasEmptyClause(TABLEAU T)
/**************************************************************
  INPUT:   A tableau                                                     
  RETURNS: TRUE iff an empty clause is among the clauses
           on this level
***************************************************************/
{
  LIST Scan;
  
  for (Scan = tab_Clauses(T); !list_Empty(Scan); Scan = list_Cdr(Scan)) 
    if (clause_IsEmptyClause(list_Car(Scan)))
      return TRUE;

  return FALSE;
}


BOOL tab_IsClosed(TABLEAU T)
/**************************************************************
  INPUT:   A Tableau   
  RETURNS: TRUE iff the tableau is closed. (NOTE: FALSE
           if the tableau is empty)
***************************************************************/
{
  if (tab_IsEmpty(T))
    return FALSE;

  if (tab_HasEmptyClause(T))
      return TRUE;
  /* 
   *  now tableau can only be closed
   *  if there has been a split, and
   *  both subtableaus are closed 
   */
  
  if (tab_RightBranchIsEmpty(T) || tab_LeftBranchIsEmpty(T)) {
    printf("\nopen node label: %d", T->Label);
    fflush(stdout);

    return FALSE;
  }
  return tab_IsClosed(tab_RightBranch(T)) && tab_IsClosed(tab_LeftBranch(T));
}

static LIST tab_DeleteFlat(TABLEAU T)
/**************************************************************
  INPUT:   A tableau
  RETURNS: The list of its clauses on the first level of
          the tableau
  EFFECTS: Frees the root tableau node.
***************************************************************/
{
  LIST Clauses;

  Clauses = tab_Clauses(T);
  memory_Free(T, sizeof(TABLEAU_NODE));
  
  return Clauses;
}


static void tab_DeleteGen(TABLEAU T, LIST* Clauses, BOOL DeleteClauses)
/**************************************************************
  INPUT:   A tableau, a list of clauses by reference, a flag 
  RETURNS: Nothing
  EFFECTS: Depending on <DeleteClauses>, all clauses in the
           tableau are added to <Clauses> or just deleted.
 	   The memory for the tableau and its clause lists is
	   freed.
***************************************************************/
{
  if (tab_IsEmpty(T))
    return;
  
  tab_DeleteGen(tab_RightBranch(T), Clauses, DeleteClauses);
  tab_DeleteGen(tab_LeftBranch(T),  Clauses, DeleteClauses);

  list_Delete(tab_RightSplitClauses(T));
  if (DeleteClauses) 
    list_Delete(tab_Clauses(T)); 
  else
    *Clauses = list_Nconc(tab_Clauses(T), *Clauses);
  
  tab_DeleteFlat(T);
  
}

static void tab_DeleteCollectClauses(TABLEAU T, LIST* Clauses)
/**************************************************************
  INPUT:   A tableau, a list of clauses by reference
  RETURNS: Nothing
  EFFECTS: Frees the memory of the tableau, but collects
           its clauses
***************************************************************/
{
  tab_DeleteGen(T, Clauses, FALSE);
}

void tab_Delete(TABLEAU T)
/**************************************************************
  INPUT:   A tableau
  RETURNS: Nothing
  EFFECTS: Frees the memory of the tableau
***************************************************************/
{
  LIST Redundant;
  
  Redundant = list_Nil();
  tab_DeleteGen(T, &Redundant, TRUE);
}

static void tab_SetSplitLevelsRec(TABLEAU T, int Level)
/**************************************************************
  INPUT:   A tableau
  RETURNS: Nothing                                                     
  EFFECTS: The split levels of the clauses in the
           tableau are set to the level of the
	   tableau level they are contained in.
***************************************************************/
{
  LIST Scan;

  if (tab_IsEmpty(T))
    return;
  
  for (Scan = tab_Clauses(T); !list_Empty(Scan); Scan = list_Cdr(Scan)) {
    clause_SetSplitLevel(list_Car(Scan), Level); 
    if (Level >0) {
      clause_ClearSplitField(list_Car(Scan));
      clause_SetSplitFieldBit(list_Car(Scan), Level);
    }
    else 
      clause_SetSplitField(list_Car(Scan), (SPLITFIELD)NULL,0);
  }
  
  tab_SetSplitLevelsRec(tab_RightBranch(T), Level+1);
  tab_SetSplitLevelsRec(tab_LeftBranch(T), Level+1);
}

void tab_SetSplitLevels(TABLEAU T)
/**************************************************************
  INPUT:   A tableau                                                     
  RETURNS: Nothing                                                     
  EFFECTS: The split levels of the clauses in the
           tableau are set to the level of the
	   tableau they belong to.
***************************************************************/
{
  tab_SetSplitLevelsRec(T,0);
}


TABLEAU tab_PruneClosedBranches(TABLEAU T, LIST* Clauses) 
/**************************************************************
  INPUT:   A tableau, a list of clauses by reference.
  RETURNS: The (destructively) reduced tableau: Descendants of
           nodes that have an empty clause are deleted.
  EFFECTS: The tableau is modified.
***************************************************************/
{
  if (tab_IsEmpty(T))
    return T;

  /* if there is an empty clause on this level, delete subtrees */

  if (tab_HasEmptyClause(T)) {

    tab_DeleteCollectClauses(tab_RightBranch(T), Clauses); 
    tab_DeleteCollectClauses(tab_LeftBranch(T), Clauses); 
    tab_SetRightBranch(T, tab_EmptyTableau());
    tab_SetLeftBranch(T, tab_EmptyTableau()); 
    list_Delete(tab_RightSplitClauses(T));
    tab_SetRightSplitClauses(T, list_Nil());
    tab_SetSplitClause(T,clause_Null());
    tab_SetLeftSplitClause(T, clause_Null());
  }
  /* else recursively prune subtrees */
  else {
    tab_SetRightBranch(T, tab_PruneClosedBranches(tab_RightBranch(T), Clauses));
    tab_SetLeftBranch(T, tab_PruneClosedBranches(tab_LeftBranch(T), Clauses));
  }
  
  return T;
}

/**************************************************************
  INPUT:   A clause, a list of integers.
  RETURNS: TRUE iff all the splitlevels of <Clause> appear 
           in the list <LevelsAbove>.
  REMARK: When this becomes a bottleneck, it can be made more efficient
          as the list is ordered.
***************************************************************/
static BOOL tab_AllSplitFieldsInList(CLAUSE Clause, LIST LevelsAbove) {
  int i,j;

  /* level zero not checked */
  for (i = Clause->splitfield_length-1; i >= 0; i--)
    if (Clause->splitfield[i] != 0) {
      for (j = 0; j<=sizeof(SPLITFIELDENTRY)*CHAR_BIT-1; j++)
        if (Clause->splitfield[i] & ((SPLITFIELDENTRY)1 << j))
          if (!list_PointerMember(LevelsAbove,(POINTER)(i*sizeof(SPLITFIELDENTRY)*CHAR_BIT+j)))
            return FALSE;
    }
  
  return TRUE;
}

/**************************************************************
  INPUT:   A Tableau, a list of clauses by reference, a boolean flag
           and a list.
           <T> is the tableau to be updated.
           Whenever a node is deleted its clauses added
           to <ToDelete>.
           <SaveParent> means that the top node of <T> is a right child
           of a node that lost its left, because it contained an unfinished split;
           the only chance for tha parent to survive is to borrow the childs 
           split clause, which can only be legal if the subtableau starting
           in <T> doesn't depend of <T>'s clauses.
           <LevelsAbove> is a list of splitlevels of clauses appearing
           in the ancestors of <T>. The list is being built directly on stack.
           
  RETURNS: The reduced tableau: If a node depends on
           on nodes that have been removed in tab_RemoveIncompleteSplits
           such a node has to removed and a viable
           substitute found.
***************************************************************/
static TABLEAU tab_CheckUnfinishedAndRepair(TABLEAU T, LIST* ToDelete, BOOL SaveParent, LIST LevelsAbove) {
  LIST_NODE node;  
  
#ifdef CHECK
  if (list_Empty(tab_Clauses(T))) {
    misc_StartErrorReport();
    misc_ErrorReport("\n In tab_CheckUnfinishedAndRepair: Tableau node without clauses.");
    misc_FinishErrorReport();
  }
#endif
    
  /* if we are supposed to save our parent with our split,
    our children will have to survive without our clauses.
    This also works for leaves (they always fail)
    as their empty clauses always depend on their own level */
  if (!SaveParent) {
    /* creating a list as we go, on the stack */
    node.car = (POINTER)clause_SplitLevel(list_Car(tab_Clauses(T))); /*any of them will do, so let's take the first one*/
    node.cdr = LevelsAbove;
    LevelsAbove = &node;
  }
  
  if (tab_IsLeaf(T)) {    
    LIST Clauses;
    BOOL Found;
    BOOL NiceFound;
    
    /*there has to be an empty clause and we are fine if at least one of them is fine*/
    Found = FALSE;
    NiceFound = FALSE;
    Clauses = tab_Clauses(T);

    for (; !list_Empty(Clauses); Clauses = list_Cdr(Clauses)) {
      CLAUSE Clause;      
      Clause = list_Car(Clauses);
      
      if (clause_IsEmptyClause(Clause)) {               
        Found = TRUE;
        if (tab_AllSplitFieldsInList(Clause,LevelsAbove)) {
          NiceFound = TRUE;
          break;
        }
      }
    }
  
#ifdef CHECK
    if (!Found) {
      misc_StartErrorReport();
      misc_ErrorReport("\n In tab_CheckUnfinishedAndRepair: Tableau leaf without empty clause.");
      misc_FinishErrorReport();
    }
#endif
    
    if (!NiceFound) { /*clear this node and return empty to report failure*/      
#ifdef CHECK
      if (!list_Empty(tab_RightSplitClauses(T))) {
        misc_StartErrorReport();
        misc_ErrorReport("\n In tab_CheckUnfinishedAndRepair: Tableau leaf with nonempty RightSplitClauses.");
        misc_FinishErrorReport();
      }
#endif
      
      (*ToDelete) = list_Nconc(tab_DeleteFlat(T), *ToDelete);    
      return tab_EmptyTableau();
    } else {
      return T;
    }    
  } else { /* a non-leaf node */
    TABLEAU LeftChild;
    TABLEAU RightChild;
  
    /* non-leaf node => has both children */
#ifdef CHECK
    if (tab_RightBranchIsEmpty(T) || tab_LeftBranchIsEmpty(T)) {
      misc_StartErrorReport();
      misc_ErrorReport("\n In tab_CheckUnfinishedAndRepair: Tableau non-leaf without two children.");
      misc_FinishErrorReport();
    }
#endif
  
    /* the order of the two following statements matters! */
    LeftChild = tab_CheckUnfinishedAndRepair(tab_LeftBranch(T),ToDelete,FALSE,LevelsAbove);
    RightChild = tab_CheckUnfinishedAndRepair(tab_RightBranch(T),ToDelete,tab_IsEmpty(LeftChild),LevelsAbove);

    if (tab_IsEmpty(RightChild)) { /* failure */
      
      /*reconnect so that we can call the recursive delete */
      tab_SetLeftBranch(T, LeftChild);
      tab_SetRightBranch(T,RightChild);
      
      tab_DeleteCollectClauses(T,ToDelete);
    
      return tab_EmptyTableau();
    } else if (tab_IsEmpty(LeftChild) /* and non-empty right */) { /* repairable */
    
      tab_SetLeftBranch(T, tab_LeftBranch(RightChild));
      tab_SetRightBranch(T, tab_RightBranch(RightChild));
      
      tab_SetSplitClause(T, tab_SplitClause(RightChild));
      tab_SetLeftSplitClause(T, tab_LeftSplitClause(RightChild));      
      list_Delete(tab_RightSplitClauses(T));
      tab_SetRightSplitClauses(T, tab_RightSplitClauses(RightChild));

      (*ToDelete) = list_Nconc(tab_DeleteFlat(RightChild), *ToDelete);

      return T;
    } else { /* no problem */      
      tab_SetLeftBranch(T, LeftChild);
      tab_SetRightBranch(T, RightChild);
      return T;
    }
  }
}

TABLEAU tab_RemoveUnfinishedSplits(TABLEAU T, LIST* Clauses)
/**************************************************************
  INPUT:   A Tableau, a list of clauses by reference
  RETURNS: The reduced tableau: If a node depends on
           on nodes that have been removed in tab_RemoveIncompleteSplits
           such a node has to removed and a viable
           substitute found.
  EFFECTS: Whenever a node is deleted its clauses added
           to <Clauses>
***************************************************************/
{
  return tab_CheckUnfinishedAndRepair(T,Clauses,FALSE,list_Nil());
}

TABLEAU tab_RemoveIncompleteSplits(TABLEAU T, LIST* Clauses)
/**************************************************************
  INPUT:   A Tableau, a list of clauses by reference
  RETURNS: The reduced tableau: If a node has exactly one
           successor (that is, the corresponding split was
           not completed), delete the successor and move
	   its subtrees to <T>.
  EFFECTS: The successor node is deleted, and its clauses added
           to <Clauses>
***************************************************************/
{
  LIST    NewClauses;
  TABLEAU Child;

  if (tab_IsEmpty(T))
    return T;

  if (tab_IsLeaf(T))
    return T;

  if (!tab_IsEmpty(tab_RightBranch(T)) &&
      !tab_IsEmpty(tab_LeftBranch(T))) {	   
    tab_SetRightBranch(T, tab_RemoveIncompleteSplits(tab_RightBranch(T), Clauses));
    tab_SetLeftBranch(T, tab_RemoveIncompleteSplits(tab_LeftBranch(T), Clauses));
    return T;
  }
  if (tab_IsEmpty(tab_RightBranch(T))) 
    Child = tab_LeftBranch(T);
  else 
    Child = tab_RightBranch(T);

  Child = tab_RemoveIncompleteSplits(Child, Clauses);

  tab_SetLeftBranch(T, tab_LeftBranch(Child));   
  tab_SetRightBranch(T, tab_RightBranch(Child));

  /* copy split data */

  tab_SetSplitClause(T, tab_SplitClause(Child));
  tab_SetLeftSplitClause(T, tab_LeftSplitClause(Child));
  tab_SetRightSplitClauses(T, tab_RightSplitClauses(Child));

  /* delete ancestors of deleted clauses and remember */

  NewClauses = tab_DeleteFlat(Child);
  (*Clauses) = list_Nconc(NewClauses, *Clauses);

  return T;
}


void tab_CheckEmpties(TABLEAU T)
/**************************************************************
  INPUT:   A tableau 
  RETURNS: Nothing.
  EFFECTS: Prints warnings if non-leaf nodes contain
           empty clauses (which should not be the case
           after pruning any more), of if leaf nodes
	   contain more than one empty clause
***************************************************************/
{
  LIST Scan, Empties;
  BOOL Printem;

  if (tab_IsEmpty(T))
    return;

  /* get all empty clauses in this node */ 
  Empties = list_Nil();
  for (Scan = tab_Clauses(T); !list_Empty(Scan); Scan = list_Cdr(Scan)) {
    if (clause_IsEmptyClause(list_Car(Scan)))
      Empties = list_Cons(list_Car(Scan), Empties);
  }
  Printem = FALSE;
  if (!list_Empty(Empties) && !tab_IsLeaf(T)) {
    puts("\nNOTE: non-leaf node contains empty clauses.");
    Printem = TRUE;
  }
  
  if (tab_IsLeaf(T) && list_Length(Empties) > 1) {
    puts("\nNOTE: Leaf contains more than one empty clauses.");
    Printem = TRUE;
  }
  if (Printem) {
    puts("Clauses:");
    clause_PParentsListPrint(tab_Clauses(T));
  }
  list_Delete(Empties);
  tab_CheckEmpties(tab_LeftBranch(T));
  tab_CheckEmpties(tab_RightBranch(T));
}


void tab_GetAllEmptyClauses(TABLEAU T, LIST* L)
/**************************************************************
  INPUT:   A tableau, a list by reference
  RETURNS: All empty clauses in the tableau prepended to <L>.
***************************************************************/
{
  if (tab_IsEmpty(T))
    return;
  
  tab_GetAllEmptyClauses(tab_LeftBranch(T), L);
  tab_GetAllEmptyClauses(tab_RightBranch(T), L);
}


void tab_GetEarliestEmptyClauses(TABLEAU T, LIST* L)
/**************************************************************
  INPUT  : A tableau, a list of clauses by reference
  RETURNS: Nothing. 
  EFFECTS: For each leaf node, adds empty clauses in
           leaf nodes to <L>. If the leaf node contains only one
	   empty clause, it is added to <L> anyway.
           If the leaf node contains more than one empty clause,
	   the earliest derived empty clause is added to <L>.
***************************************************************/
{
  CLAUSE FirstEmpty;
  LIST   Scan;

  if (tab_IsEmpty(T))
    return;

  if (tab_IsLeaf(T)) {  
    FirstEmpty = clause_Null();
  
    for (Scan = tab_Clauses(T); !list_Empty(Scan); Scan = list_Cdr(Scan)) {
      if (clause_IsEmptyClause(list_Car(Scan))) {
	if (FirstEmpty == clause_Null()) 
	  FirstEmpty = list_Car(Scan);
	else if (clause_Number(FirstEmpty) > clause_Number(list_Car(Scan)))
	  FirstEmpty = list_Car(Scan);
      }
    }
    
    if (FirstEmpty != clause_Null())
      (*L) = list_Cons(FirstEmpty, *L);
  }
  tab_GetEarliestEmptyClauses(tab_LeftBranch(T), L);
  tab_GetEarliestEmptyClauses(tab_RightBranch(T), L);

}

void tab_ToClauseList(TABLEAU T, LIST* Proof)
/**************************************************************
  INPUT:   A tableau <T>, a list of clauses 
  RETURNS: Nothing.
  EFFECTS: All clauses in T are added to <Proof>
***************************************************************/
{
  if (tab_IsEmpty(T))
    return;
  
  (*Proof) = list_Nconc(list_Copy(tab_Clauses(T)), *Proof);

  tab_ToClauseList(tab_LeftBranch(T),Proof);
  tab_ToClauseList(tab_RightBranch(T),Proof);
}


 void tab_ToSeqProofOrdered(TABLEAU T, LIST* Proof)
/**************************************************************
  INPUT:   A tableau <T>, a list of clauses <Proof> representing a 
           proof by reference
  RETURNS: The sequential proof corresponding to the tableau.
***************************************************************/
{
  LIST Scan;
  BOOL RightSplitRead, LeftSplitRead;
  
  if (tab_IsEmpty(T))
    return;

  Scan = tab_Clauses(T); 
  RightSplitRead = LeftSplitRead = FALSE;

  while (!list_Empty(Scan)) {
    /* insert left and right splits and descendants controlled by clause number */

    if (!RightSplitRead && !tab_RightBranchIsEmpty(T) &&
	clause_Number(list_Car(Scan)) <
	clause_Number(list_Car(tab_RightSplitClauses(T)))) {
      tab_ToSeqProofOrdered(tab_RightBranch(T), Proof);
      RightSplitRead = TRUE;
    }
    if (!LeftSplitRead && !tab_LeftBranchIsEmpty(T) &&
	clause_Number(list_Car(Scan)) < 
	clause_Number(tab_LeftSplitClause(T))) {
      tab_ToSeqProofOrdered(tab_LeftBranch(T), Proof);
      LeftSplitRead  = TRUE;
    }
    (*Proof) = list_Cons(list_Car(Scan), *Proof); 
    Scan  = list_Cdr(Scan);
  }
  /* if a split clause with descendants has not been inserted yet, 
     it been generated after all other clauses */

  if (!RightSplitRead)
    tab_ToSeqProofOrdered(tab_RightBranch(T), Proof);
  if (!LeftSplitRead)
    tab_ToSeqProofOrdered(tab_LeftBranch(T), Proof);
}

void tab_PrintTableau(TABLEAU T)
/**************************************************************
  INPUT:   A Tableau
  RETURNS: Nothing.
  EFFECTS: Prints the tableau to stdout
***************************************************************/
{
  LIST Scan;
  printf("\n Node: %zu", (uintptr_t)T);
  printf("\n Split Clause: "); 
  if (T->SplitClause) 
    clause_PrintSpecial(T->SplitClause);
  else
    printf("None");
  printf("\n Empty Clauses: ");
  for (Scan = T->Clauses;!list_Empty(Scan);Scan=list_Cdr(Scan))
    if (clause_IsEmptyClause(list_Car(Scan))) {
      clause_PrintSpecial(list_Car(Scan));
	}
  printf("\n Clauses: ");
  for (Scan = T->Clauses;!list_Empty(Scan);Scan=list_Cdr(Scan))
    if (!clause_IsEmptyClause(list_Car(Scan))) {
      printf("\n");
      clause_PrintSpecial(list_Car(Scan));
	}
  printf("\n Left %zu Right %zu", (uintptr_t)T->LeftBranch, (uintptr_t)T->RightBranch);
  printf("\n");
  if (T->LeftBranch)
    tab_PrintTableau(T->LeftBranch);
  if (T->RightBranch)
    tab_PrintTableau(T->RightBranch);
}
  


/****************************************************************/
/*  SPECIALS FOR GRAPHS                                         */
/****************************************************************/

static void tab_LabelNodesRec(TABLEAU T, int* Num)
/**************************************************************
  INPUT:   A Tableau, a number by reference
  RETURNS: Nothing.
  EFFECTS: Labels the tableau nodes dflr, starting with <Num>
***************************************************************/
{
  if (tab_IsEmpty(T))
    return;
  T->Label = *Num;
  (*Num)++;
  tab_LabelNodesRec(tab_LeftBranch(T), Num);
  tab_LabelNodesRec(tab_RightBranch(T), Num);
}


void tab_LabelNodes(TABLEAU T)
/**************************************************************
  INPUT:   A Tableau, a number by reference
  RETURNS: Nothing.
  EFFECTS: Labels the tableau nodes dflr, starting with 0
***************************************************************/
{
  int Num;

  Num = 0;
  tab_LabelNodesRec(T, &Num);
}


static void tab_FPrintNodeLabel(FILE *File, TABLEAU T)
/**************************************************************
  INPUT:   A file handle, a tableau 
  RETURNS: Nothing.
  EFFECTS: Prints the root node information to <File>:
           clauses, split information
***************************************************************/
{
  LIST Scan;

  /* start printing of node label string */
  
  fprintf(File, "\"label: %d\\n", T->Label);
  
  /* print left and right parts of split */
  fputs("SplitClause : ", File); 
  clause_PParentsFPrint(File, tab_SplitClause(T));
  fputs("\\nLeft Clause : ", File);
  clause_PParentsFPrint(File, tab_LeftSplitClause(T));
  fputs("\\nRightClauses: ", File);
  if (list_Empty(tab_RightSplitClauses(T)))
    fputs("[]\\n", File);
  else {
    clause_PParentsFPrint(File, list_Car(tab_RightSplitClauses(T)));
    fputs("\\n", File);
    for (Scan = list_Cdr(tab_RightSplitClauses(T)); !list_Empty(Scan); Scan = list_Cdr(Scan)) {
      fputs("              ", File);
      clause_PParentsFPrint(File, list_Car(Scan));
      fputs("\\n", File);
    }
  }
  /* print clause at this level */
  if (pcheck_ClauseCg) {
    if (list_Empty(tab_Clauses(T)))
      fputs("[]", File);
    else {
      for (Scan = tab_Clauses(T); !list_Empty(Scan); Scan = list_Cdr(Scan)){
	clause_PParentsFPrint(File, list_Car(Scan)); 
	fputs("\\n", File);
      }
    }
  }
  putc('"', File); /* close string */
}


static void tab_FPrintEdgeCgFormat(FILE* File, int Source, int Target, BOOL Left)
/**************************************************************
  INPUT:   A file handle, two node labels, a flag
  RETURNS: Nothing.
  EFFECTS: Prints the edge denoted by <Source> and <Target>
           to <File>, with an edge label (0/1) according to <Left>.
	   The edge label is added since xvcg cannot handle
           ordered edges.
***************************************************************/
{
  fputs("\nedge: {", File);
  fprintf(File, "\nsourcename: \"%d\"", Source);
  fprintf(File, "\ntargetname: \"%d\"\n", Target);
  fputs("\nlabel: \"", File);
  if (Left) 
    putc('0', File);
  else
    putc('1', File);
  fputs("\"  }\n", File);
} 


static void tab_FPrintEdgesCgFormat(FILE* File, TABLEAU T)
/**************************************************************
  INPUT:   A file handle, a tableau
  RETURNS: Nothing.
  EFFECTS: Prints edge information of <T> in xvcg graph format
           to <File>.
***************************************************************/
{
  if (tab_IsEmpty(T))
    return;

  if (!tab_LeftBranchIsEmpty(T))
    tab_FPrintEdgeCgFormat(File, T->Label, tab_LeftBranch(T)->Label, TRUE);
  if (!tab_RightBranchIsEmpty(T))
    tab_FPrintEdgeCgFormat(File, T->Label, tab_RightBranch(T)->Label, FALSE);
  
  tab_FPrintEdgesCgFormat(File, tab_LeftBranch(T));
  tab_FPrintEdgesCgFormat(File, tab_RightBranch(T));
}


static void tab_FPrintNodesCgFormat(FILE* File, TABLEAU T)
/**************************************************************
  INPUT:   A file handle, a tableau
  RETURNS: Nothing
  EFFECTS: Prints egde information of <T> in xvcg graph format
           to <File>.                                            
***************************************************************/
{
  if (tab_IsEmpty(T))
    return;

  fputs("\nnode: {\n\nlabel: ", File);
  tab_FPrintNodeLabel(File, T);
  putc('\n', File); /* end label section */
  
  fprintf(File, "title: \"%d\"\n", T->Label);
  fputs("  }\n", File);
  
  /* recursion */
  tab_FPrintNodesCgFormat(File, tab_LeftBranch(T));
  tab_FPrintNodesCgFormat(File, tab_RightBranch(T));
  
} 

static void tab_FPrintCgFormat(FILE* File, TABLEAU T)
/**************************************************************
  INPUT:   A file handle, a tableau
  RETURNS: Nothing.
  EFFECTS: Prints tableau as a graph in xvcg format to <File>
***************************************************************/
{
  fputs("graph: \n{\ndisplay_edge_labels: yes\n", File);
  
  tab_FPrintNodesCgFormat(File, T);
  tab_FPrintEdgesCgFormat(File, T); 
  fputs("}\n", File);
}

/* unused */
/*static*/ void tab_PrintCgFormat(TABLEAU T)
/**************************************************************
  INPUT:   A tableau. 
  RETURNS: Nothing.                                                     
  EFFECTS: Print tableau in xvcg format to stdout
***************************************************************/
{
  tab_FPrintCgFormat(stdout, T);
}


/**************************************************************/
/* procedures for printing graph in da Vinci format           */
/**************************************************************/

static void tab_FPrintDaVinciEdge(FILE* File, int L1, int L2)
/**************************************************************
  INPUT:   A file handle, two numbers 
  RETURNS: Nothing 
  EFFECTS: Print an edge in daVinci format
***************************************************************/
{
  fprintf(File, "l(\"%d->%d\","   ,L1,L2);
  fputs("e(\"\",[],\n", File);
  /* print child node as reference */
  fprintf(File, "r(\"%d\")))\n", L2);
}


static void tab_FPrintDaVinciFormatRec(FILE* File, TABLEAU T)
/**************************************************************
  INPUT:   A file handle, a tableau
  RETURNS: Nothing
  EFFECTS: Prints tableau to <File> in daVinci format
***************************************************************/
{
  /* print node label */
  fprintf(File, "l(\"%d\",", T->Label);
  /* print node attributes */
  fputs("n(\"\", [a(\"OBJECT\",", File); 
  tab_FPrintNodeLabel(File, T);
  fputs(")],\n", File);
  
  /* print egde list  */
  putc('[', File);
  if (!tab_LeftBranchIsEmpty(T)) 
    tab_FPrintDaVinciEdge(File, T->Label, tab_LeftBranch(T)->Label); 

  if (!tab_RightBranchIsEmpty(T)) {
    if (!tab_LeftBranchIsEmpty(T))
      putc(',', File);
    tab_FPrintDaVinciEdge(File, T->Label, tab_RightBranch(T)->Label);
  }
  fputs("]))", File); /* this ends the node description */

  if (!tab_LeftBranchIsEmpty(T)) {
    putc(',', File);
    tab_FPrintDaVinciFormatRec(File, tab_LeftBranch(T));
  }
  if (!tab_RightBranchIsEmpty(T)) {
    putc(',', File);
    tab_FPrintDaVinciFormatRec(File, tab_RightBranch(T));
  }
}


static void tab_FPrintDaVinciFormat(FILE* File, TABLEAU T)
/**************************************************************
  INPUT:   A file handle <File>, a tableau
  RETURNS: Nothing
  EFFECTS: Print tableau in daVinci format to <File>
***************************************************************/
{
  fputs("[\n", File);
  tab_FPrintDaVinciFormatRec(File,T);
  fputs("]\n", File);
}


void tab_WriteTableau(TABLEAU T, const char* Filename, GRAPHFORMAT Format)
/**************************************************************
  INPUT:   A tableau, a filename 
  RETURNS: Nothing.
***************************************************************/
{
  FILE*  File;
  
  if (Format != DAVINCI && Format != XVCG) {
    misc_StartUserErrorReport();
    misc_UserErrorReport("\nError: unknown output format for tableau.\n");
    misc_FinishUserErrorReport();
  }

  File = misc_OpenFile(Filename, "w");

  if (Format == DAVINCI)
    tab_FPrintDaVinciFormat(File, T);
  else 
    if (Format == XVCG)
      tab_FPrintCgFormat(File, T);

  misc_CloseFile(File, Filename);
}

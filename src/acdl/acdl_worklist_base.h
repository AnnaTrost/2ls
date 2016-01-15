/*******************************************************************\

Module: ACDL Worklist Management Interface

Author: Rajdeep Mukherjee, Peter Schrammel

\*******************************************************************/

#ifndef CPROVER_ACDL_WORKLIST_BASE_H
#define CPROVER_ACDL_WORKLIST_BASE_H

#include <goto-programs/property_checker.h>

#include "acdl_domain.h"
#include "../ssa/local_ssa.h"

class acdl_worklist_baset : public messaget
{
public:
  typedef std::list<acdl_domaint::statementt> worklistt;

  virtual void initialize(const local_SSAt &)
    { assert(false); }

  virtual void select_vars(const exprt &statement, acdl_domaint::varst &vars);
  virtual void update(const local_SSAt &SSA,
		       const acdl_domaint::varst &vars,
		       const acdl_domaint::statementt &statement=nil_exprt())
    { assert(false); }

  const acdl_domaint::statementt pop ();

  inline bool empty() const { return worklist.empty(); }
  
protected:
  worklistt worklist;
  
  explicit acdl_worklist_baset()
  {
  }  

  virtual ~acdl_worklist_baset() 
  {
  }

  bool check_statement (const exprt &expr, const acdl_domaint::varst &vars);
  void push (const acdl_domaint::statementt &statement);

};

#endif

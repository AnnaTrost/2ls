#include <iostream>

#include <util/simplify_expr.h>
#include "strategy_solver_equality.h"

bool strategy_solver_equalityt::iterate(invariantt &_inv) 
{
  equality_domaint::equ_valuet &inv = 
    static_cast<equality_domaint::equ_valuet &>(_inv);

  worklistt::iterator e_it = todo_equs.begin();
  if(e_it!=todo_equs.end()) //check equalities
  {
    solver.new_context();

    exprt pre_expr = equality_domain.get_pre_equ_constraint(*e_it);

    solver << pre_expr;
    
    exprt post_expr = equality_domain.get_post_not_equ_constraint(*e_it);
    literalt cond_literal = solver.convert(post_expr);

    solver << literal_exprt(cond_literal);

#if 0
    debug() << "Checking equality " << eom;
    debug() << "Pre: " << from_expr(ns, "", pre_expr) << eom;
    debug() << "Post: " << from_expr(ns, "", post_expr) << eom;
#endif

    if(solver() == decision_proceduret::D_SATISFIABLE) 
    { 
#if 0
      debug() << "SAT" << eom;
#endif
      todo_disequs.insert(*e_it);

      solver.pop_context();
    }
    else  //equality holds
    {
#if 0
      debug() << "UNSAT" << eom;
#endif
      
      equality_domain.set_equal(*e_it,inv);

      solver.pop_context();

      solver << pre_expr; //make permanent

      //due to transitivity, we would like to recheck equalities that did not hold
      todo_equs.insert(todo_disequs.begin(),todo_disequs.end());
      todo_disequs.clear();
    }


    todo_equs.erase(e_it);

    //check status of remaining equalities
    /*   worklistt rm_equs;
    for(e_it = todo_equs.begin(); e_it!=todo_equs.end(); e_it++)
    {
      equality_domaint::var_pairt vv = equality_domain.get_var_pair(*e_it);
      if(solver.get(vv.first)!=solver.get(vv.second))
        rm_equs.insert(*e_it);
    }
    for(e_it = rm_equs.begin(); e_it!=rm_equs.end(); e_it++)
    {
      todo_disequs.insert(*e_it);
      todo_equs.erase(*e_it);
      } */
  }
  else //check disequalities
  {
    e_it = todo_disequs.begin();
    if(e_it==todo_disequs.end()) return false; //done

    solver.new_context();

    exprt pre_expr = equality_domain.get_pre_disequ_constraint(*e_it);

    solver << pre_expr;
    
    exprt post_expr = equality_domain.get_post_not_disequ_constraint(*e_it);
    literalt cond_literal = solver.convert(post_expr);

    solver << literal_exprt(cond_literal);

#if 0
    debug() << "Checking disequality " << eom;
    debug() << "Pre: " << from_expr(ns, "", pre_expr) << eom;
    debug() << "Post: " << from_expr(ns, "", post_expr) << eom;
#endif

    if(solver() == decision_proceduret::D_SATISFIABLE) 
    { 
#if 0
      debug() << "SAT" << eom;
#endif      
    }
    else  //equality holds
    {
#if 0
      debug() << "UNSAT" << eom;
#endif      
      equality_domain.set_disequal(*e_it,inv);
      solver << pre_expr; //make permanent
    }

    solver.pop_context();

    todo_disequs.erase(e_it);
  }

  return true;
}

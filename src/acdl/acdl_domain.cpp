/*******************************************************************\

Module: ACDL Domain

Author: Rajdeep Mukherjee, Peter Schrammel

\*******************************************************************/


#define DEBUG


#ifdef DEBUG
#include <iostream>
#endif

#include <util/simplify_expr.h>
#include <util/find_symbols.h>
#include <memory>

#include "../domains/ssa_analyzer.h"
#include "../domains/tpolyhedra_domain.h"

#include "acdl_domain.h"
#include "template_generator_acdl.h"

/*******************************************************************\

Function: acdl_domaint::operator()

  Inputs:

 Outputs:

 Purpose: operator()

\*******************************************************************/

void acdl_domaint::operator()(const statementt &statement,
		  const varst &vars,
		  const valuet &_old_value,
		  deductionst &deductions)
{
#ifdef DEBUG
  std::cout << "[ACDL-DOMAIN] old value: ";
  output(std::cout, _old_value) << std::endl;
#endif

  deductions.reserve(vars.size());
  for(varst::const_iterator it = vars.begin();
      it != vars.end(); ++it)
  {
    ssa_analyzert ssa_analyzer;
    std::unique_ptr<incremental_solvert> solver(
      incremental_solvert::allocate(SSA.ns,true));

    // project _old_value on everything in statement but *it
    valuet old_value;
    remove_var(_old_value,*it,old_value);

#ifdef DEBUG
    std::cout << "[ACDL-DOMAIN] projected(" << it->get_identifier() << "): ";
    output(std::cout, old_value) << std::endl;
#endif

    meet_irreduciblet deduced;

    // inference for booleans
    if(it->type().id()==ID_bool)
    {
      valuet var_value;
      literalt l = solver->solver->convert(*it);
      if(l.is_constant())
	continue;
      solver->solver->set_frozen(l);

      //get handles on meet irreducibles to check them later
      bvt value_literals;
      value_literals.resize(old_value.size());
      *solver << statement;
      for(unsigned i=0; i<old_value.size(); i++)
      {
	value_literals[i] = solver->convert(old_value[i]);
	solver->solver->set_frozen(value_literals[i]);
      }
      solver->set_assumptions(value_literals);
	
      if((*solver)() == decision_proceduret::D_SATISFIABLE)
      {
	exprt m = solver->get(*it);
	if(m.is_true())
	  deduced = *it;
	else
	  deduced = not_exprt(*it);

	//test the complement
	solver->new_context();
	solver->set_assumptions(value_literals);
	*solver << not_exprt(*it);
	if((*solver)() == decision_proceduret::D_SATISFIABLE)
	{
	  //"don't know"
   	  //pop_context not needed
	  continue;
	}
	else
	{
	  if(!is_contained(deduced,_old_value))
	  {
	    deductions.push_back(deductiont());
	    deductions.back().first = deduced;
  	    get_antecedents(*solver,_old_value,value_literals,
			    deductions.back().second);
	  }
	}

	//pop_context not needed
      }
      else //bottom
      {
	deductions.push_back(deductiont());
	deductions.back().first = false_exprt();
	get_antecedents(*solver,_old_value,value_literals,
			deductions.back().second);
	
	break; //at this point we have a conflict, we return
      }
    }

    // inference for numerical variables using templates
    else if (it->type().id() == ID_signedbv ||
        it->type().id() == ID_unsignedbv ||
        it->type().id() == ID_floatbv)
    {
      template_generator_acdlt template_generator(
          options,ssa_db,ssa_local_unwinder); 
      template_generator(SSA,*it);

      ssa_analyzer(*solver, SSA, and_exprt(conjunction(old_value),statement),
          template_generator);
      exprt var_value;
      ssa_analyzer.get_result(var_value,template_generator.all_vars());
      valuet var_values;
      if(var_value.id()==ID_and)
	var_values = var_value.operands();
      else
	var_values.push_back(var_value);

      if(var_values.empty())
	continue;

      //get deductions
      //ENHANCE: make assumptions persistent in incremental_solver
      // so that we can reuse value+statement from above
      bvt value_literals;
      value_literals.resize(old_value.size());
      *solver << statement;
      for(unsigned i=0; i<old_value.size(); i++)
      {
	value_literals[i] = solver->convert(old_value[i]);
	solver->solver->set_frozen(value_literals[i]);
      }
      for(unsigned i=0; i<var_values.size(); ++i)
      {
	if(!is_contained(var_values[i],_old_value))
	{
	  solver->new_context();
	  *solver << not_exprt(var_values[i]);
          solver->set_assumptions(value_literals);

	  deductions.push_back(deductiont());
	  deductions.back().first = var_values[i];
	  get_antecedents(*solver,_old_value,value_literals,
			  deductions.back().second);
	  solver->pop_context();
	}
      }	
    }
    else
    {
      warning() << "WARNING: do not know how to propagate " 
        << it->get_identifier()
        << " of type " << from_type(SSA.ns, "", it->type()) 
        << eom;
    }


#ifdef DEBUG
    std::cout << "[ACDL-DOMAIN] deductions(" << it->get_identifier() << "): ";
    output(std::cout, deductions) << std::endl;
#endif
  }
}


/*******************************************************************\

Function: acdl_domaint::get_antecedents()

  Inputs:

 Outputs:

 Purpose:

\*******************************************************************/

void acdl_domaint::get_antecedents(incremental_solvert &solver,
				   const valuet &value,
				   const bvt &value_literals,
				   antecedentst &antecedents)
{
  for(unsigned i=0; i<value_literals.size(); ++i)
  {
    if(solver.is_in_conflict(value_literals[i]))
       antecedents.push_back(value[i]);
  }
 }

/*******************************************************************\

Function: acdl_domaint::meet()

  Inputs:

 Outputs:

 Purpose:

\*******************************************************************/

void acdl_domaint::meet(const valuet &old_value,
	    valuet &new_value)
{
  new_value.insert(new_value.end(), old_value.begin(), old_value.end());
}

/*******************************************************************\

Function: acdl_domaint::meet()

  Inputs:

 Outputs:

 Purpose:

\*******************************************************************/

void acdl_domaint::meet(const meet_irreduciblet &old_value,
	    valuet &new_value)
{
  new_value.push_back(old_value);
}

/*******************************************************************\

Function: acdl_domaint::join()

  Inputs:

 Outputs:

 Purpose:

\*******************************************************************/

void acdl_domaint::join(const std::vector<valuet> &old_values,
	    valuet &new_value)
{
  assert(false);
}

/*******************************************************************\

Function: acdl_domaint::is_contained()

  Inputs:

 Outputs:

 Purpose:

\*******************************************************************/

bool acdl_domaint::is_contained(const meet_irreduciblet &m, 
				const valuet &value) const
{
  if(m.type().id()==ID_bool)
  {
    for(unsigned i=0; i<value.size(); i++)
    {
      if(m == value[i]) 
	return true;
    }
    return false;
  }
  else if (m.type().id() == ID_signedbv ||
	   m.type().id() == ID_unsignedbv ||
	   m.type().id() == ID_floatbv)
  {
    for(unsigned i=0; i<value.size(); i++)
    {
      //check whether this is not a boolean
      if(value[i].id()==ID_symbol ||
         (value[i].id()==ID_not && value[i].op0().id()==ID_symbol))
	continue;
      if(m == value[i]) 
	return true;

      //maybe the simplifier does the job
      exprt f = simplify_expr(and_exprt(value[i],not_exprt(m)),SSA.ns);
      if(f.is_false())
	return true;

      std::unique_ptr<incremental_solvert> solver(
	 incremental_solvert::allocate(SSA.ns,true));
      *solver << and_exprt(value[i],not_exprt(m));
      if((*solver)()==decision_proceduret::D_UNSATISFIABLE) 
	return true;
    }
    return false;
  }
  
  assert(false);
}


/*******************************************************************\

Function: acdl_domaint::is_bottom()

  Inputs:

 Outputs:

 Purpose:

\*******************************************************************/

bool acdl_domaint::is_bottom(const valuet &value) const
{
  return value.size()==1 && value[0].is_false();
}

/*******************************************************************\

Function: acdl_domaint::is_complete()

  Inputs:

 Outputs:

 Purpose: This is a very stupid and restrictive gamma-completeness check

\*******************************************************************/

bool acdl_domaint::is_complete(const valuet &value, const varst& symbols) const
{
#ifdef DEBUG
  std::cout << "[ACDL-DOMAIN] is_complete? "
	    << from_expr(SSA.ns, "", conjunction(value));
//	    << std::endl;
#endif

    
  std::unique_ptr<incremental_solvert> solver(
    incremental_solvert::allocate(SSA.ns,true));
  *solver << conjunction(value);
    
#if 0
  // find symbols in value
  std::set<symbol_exprt> symbols;
  find_symbols (conjunction(value), symbols);
#endif

  for(std::set<symbol_exprt>::const_iterator it = symbols.begin();
      it != symbols.end(); ++it)
  {
    decision_proceduret::resultt res = (*solver)();
    assert(res==decision_proceduret::D_SATISFIABLE);
    // if value == (x=[2,2]) and (*it is x), then 'm' below contains the
    // value of x which is 2
    exprt m = (*solver).get(*it);
    solver->new_context();

#if 0
    std::cout << "  check "
	      << from_expr(SSA.ns, "", not_exprt(equal_exprt(*it,m)))
	      << std::endl;
#endif
  
    // and push !(x=m) into the solver
    *solver << not_exprt(equal_exprt(*it,m));
  
    if((*solver)()!=decision_proceduret::D_UNSATISFIABLE)
    {
#ifdef DEBUG
      std::cout << " is not complete" << std::endl;
#endif
      return false;
    }

    solver->pop_context();
  }
  
#ifdef DEBUG
  std::cout << " is complete" << std::endl;
#endif
  return true;
}

/*******************************************************************\

Function: acdl_domaint::remove_var()

  Inputs: example:
          Old_value = (1 <= x && x <= 5) && (0 <= y && y <= 10) vars = x

 Outputs: example:
          (0 <= y && y <= 10)

 Purpose:

\*******************************************************************/

void acdl_domaint::remove_var(const valuet &old_value, 
			      const symbol_exprt &var,
                              valuet &new_value)
{
  for(valuet::const_iterator it = old_value.begin();
      it != old_value.end(); ++it)
  {
    find_symbols_sett symbols;
    find_symbols(*it,symbols);
    if(symbols.find(var.get_identifier()) == symbols.end())
      new_value.push_back(*it);
  }
}

/*******************************************************************\

Function: acdl_domaint::split()

  Inputs: example: 
            expr: x-y
            value: -(x-y) <= 1 && x-y <= 5 && -y <= 0 && y <= 10 && ...
          This is very generic, can be easily extended to octagons and 
          other richer domains
 Outputs: example:
            2 <= x-y (for upper=true)

 Purpose: splits constant-bounded expressions in half
          If the expression is already a singleton then we cannot split
          and we return false. 

\*******************************************************************/

exprt acdl_domaint::split(const valuet &value, const exprt &expr, 
			  bool upper)
{ 
  std::cout << "[ACDL-DOMAIN] Split: "; output(std::cout, value);
        
  if(expr.type().id()==ID_bool)
  {
    exprt v_true = simplify_expr(and_exprt(conjunction(value),expr),SSA.ns);
    if(v_true.is_false())
      return false_exprt();
    exprt v_false = simplify_expr(and_exprt(conjunction(value),
					    not_exprt(expr)),SSA.ns);
    if(v_false.is_true())
      return false_exprt();
    if(upper) 
      return expr;
    else
      return not_exprt(expr);
  }

  if(!(expr.type().id() == ID_signedbv ||
     expr.type().id() == ID_unsignedbv ||
       expr.type().id() == ID_floatbv))
  {
    warning() << "WARNING: do not know how to split " 
	      << from_expr(SSA.ns, "", expr)
	      << " of type " << from_type(SSA.ns, "", expr.type()) 
	      << eom;
    return false_exprt(); 
  }

  if(value.size()<2)
    return false_exprt(); //cannot split

  //match template expression
  constant_exprt u;
  for(unsigned i=0; i<value.size(); i++)
  {
    const exprt &e = value[i];
    if(e.id() != ID_le)
      continue;
    if(to_binary_relation_expr(e).lhs() == expr)
    {
      u = to_constant_expr(to_binary_relation_expr(e).rhs());
      break;
    }
  }
  constant_exprt l;
  for(unsigned i=0; i<value.size(); i++)
  {
    const exprt &e = value[i];
    if(e.id() != ID_le)
      continue;
    const exprt &lhs = to_binary_relation_expr(e).lhs();
    if(lhs.id()==ID_unary_minus && 
       lhs.op0().id()==ID_typecast &&
       lhs.op0().op0() == expr)
    {
      l = to_constant_expr(to_binary_relation_expr(e).rhs());
      break;
    }
  }

  //TODO: check whether we have a singleton, then we cannot split anymore
  exprt m = tpolyhedra_domaint::between(l,u);

  if(upper) 
  {
#ifdef DEBUG
    std::cout << "[ACDL-DOMAIN] decision: "
	      << from_expr(SSA.ns, "", binary_relation_exprt(m,ID_le,expr)) << std::endl;
#endif
    return binary_relation_exprt(m,ID_le,expr);
  }
  else 
  {
#ifdef DEBUG
    std::cout << "[ACDL-DOMAIN] decision: "
	      << from_expr(SSA.ns, "", binary_relation_exprt(expr,ID_le,m)) << std::endl;
#endif
    return binary_relation_exprt(expr,ID_le,m);
  }
  return true_exprt();
}

/*******************************************************************\

Function: acdl_domaint::normalize()

  Inputs:

 Outputs:

 Purpose:

\*******************************************************************/

void acdl_domaint::normalize(valuet &value, const varst &vars)
{
  for(unsigned i=0; i<value.size(); i++)
  {
    if(value[i].is_false())
    {
      set_bottom(value);
      return;
    }
  }

#if 0 //I don't think this is needed anymore
  valuet old_value = value;

  std::vector<symbol_exprt> clean_vars;
  
  //project out vars
  for(varst::const_iterator it = vars.begin();
      it != vars.end(); ++it)
  {
    // we only normalize what the abstract domain currently handles
    if(it->type().id() == ID_signedbv ||
       it->type().id() == ID_unsignedbv ||
       it->type().id() == ID_floatbv)
    {
      value = remove_var(value,*it);
      clean_vars.push_back(*it);
    }
  }
    
  ssa_analyzert ssa_analyzer;
  std::unique_ptr<incremental_solvert> solver(incremental_solvert::allocate(SSA.ns,true));

  template_generator_acdlt template_generator(options,ssa_db,ssa_local_unwinder); 
  template_generator(SSA,clean_vars);
    
  ssa_analyzer(*solver, SSA, old_value,template_generator);
  valuet new_values;
  ssa_analyzer.get_result(new_values,template_generator.all_vars());

    
  value = and_exprt(new_values,value);
#endif
}
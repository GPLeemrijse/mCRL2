#ifndef MCRL2_ATERMPP_DETAIL_ATERM_APPL_IMPLEMENTATION_H
#define MCRL2_ATERMPP_DETAIL_ATERM_APPL_IMPLEMENTATION_H

#include "mcrl2/utilities/exception.h"
#include "mcrl2/utilities/detail/memory_utility.h"
#include "mcrl2/atermpp/detail/atypes.h"
#include "mcrl2/atermpp/aterm_appl.h"

namespace atermpp
{

namespace detail
{

template <class Term, class InputIterator, class ATermConverter>
_aterm* local_term_appl_with_converter(const function_symbol &sym, 
                                       const InputIterator begin, 
                                       const InputIterator end, 
                                       const ATermConverter &convert_to_aterm)
{
  const size_t arity = sym.arity();

  HashNumber hnr = sym.number();
  
  /* The term is already partly constructed initially. If
     it turns out that the term already exists, this skeleton is freed
     using simple_free_term. Otherwise, the new_term is finished
     and a it is returned. */ 

  detail::_aterm* new_term = (detail::_aterm_appl<Term>*) detail::allocate_term(TERM_SIZE_APPL(arity));
  
  size_t j=0;
  for (InputIterator i=begin; i!=end; ++i, ++j)
  {
    new (&(reinterpret_cast<detail::_aterm_appl<Term>*>(new_term)->arg[j])) Term(convert_to_aterm(*i));
    const aterm &arg = reinterpret_cast<detail::_aterm_appl<Term>*>(new_term)->arg[j];
    CHECK_TERM(arg);
    hnr = COMBINE(hnr, arg);
  }
  assert(j==arity);

  hnr &= detail::aterm_table_mask;
  detail::_aterm* cur = detail::aterm_hashtable[hnr];
  while (cur)
  {
    if (cur->function()==sym)
    {
      bool found = true;
      for (size_t i=0; i<arity; i++)
      {
        if (reinterpret_cast<detail::_aterm_appl<Term>*>(cur)->arg[i] != reinterpret_cast<detail::_aterm_appl<Term>*>(new_term)->arg[i])
        {
          found = false;
          break;
        }
      }
      if (found)
      {
        simple_free_term(new_term,arity);
        return cur;
      }
    }
    cur = cur->next();
  }

  if (!cur)
  {
    new (&new_term->function()) function_symbol(sym);
    
    new_term->next() = detail::aterm_hashtable[hnr];
    detail::aterm_hashtable[hnr] = new_term;
  }
  
  return new_term;
}

// The functions below are used to obtain an address of objects that due to
// template arguments can have different types.

inline const _aterm* ADDRESS(const _aterm* a)
{
  return a;
}

inline _aterm* ADDRESS(const aterm &a)
{
  return a.address();
}

template <class Term, class ForwardIterator>
_aterm* local_term_appl(const function_symbol &sym, const ForwardIterator begin, const ForwardIterator end)
{
  const size_t arity = sym.arity();
  HashNumber hnr = sym.number();
  size_t j=0;
  for (ForwardIterator i=begin; i!=end; ++i, ++j)
  {
    assert(j<arity);
    CHECK_TERM(*i);
    hnr = COMBINE(hnr, reinterpret_cast<size_t>(ADDRESS(*i)));
  }
  assert(j==arity);

  detail::_aterm* cur = detail::aterm_hashtable[hnr & detail::aterm_table_mask];
  while (cur)
  {
    if (cur->function()==sym)
    {
      bool found = true;
      ForwardIterator i=begin;
      for (size_t j=0; j<arity; ++i,++j)
      {
        if (reinterpret_cast<detail::_aterm_appl<Term>*>(cur)->arg[j].address() != detail::ADDRESS(*i)) 
        {
          found = false;
          break;
        }
      }
      if (found)
      {
        break;
      }
    }
    cur = cur->next();
  }

  if (!cur)
  {
    cur = (detail::_aterm_appl<Term>*) detail::allocate_term(TERM_SIZE_APPL(arity));
    /* Delay masking until after allocate_term */
    hnr &= detail::aterm_table_mask;
    new (&cur->function()) function_symbol(sym);
    
    ForwardIterator i=begin;
    for (size_t j=0; j<arity; ++i, ++j)
    {
      new (&(reinterpret_cast<detail::_aterm_appl<Term>*>(cur)->arg[j])) Term(*i);
    }
    cur->next() = detail::aterm_hashtable[hnr];
    detail::aterm_hashtable[hnr] = cur;
  }
  
  return cur;
}

template <class Term>
_aterm* term_appl1(const function_symbol &sym, const Term &arg0)
{
  assert(sym.arity()==1);
  CHECK_TERM(arg0);

  HashNumber hnr = COMBINE(sym.number(), arg0);

  detail::_aterm *cur = detail::aterm_hashtable[hnr & detail::aterm_table_mask];
  while (cur)
  {
    if ((sym==cur->function()) && 
         reinterpret_cast<detail::_aterm_appl<Term>*>(cur)->arg[0] == arg0)
    {
      return cur;
    }
    cur = cur->next();
  }

  cur = detail::allocate_term(TERM_SIZE_APPL(1));
  /* Delay masking until after allocate_term */
  hnr &= detail::aterm_table_mask;

  new (&cur->function()) function_symbol(sym);
  new (&(reinterpret_cast<detail::_aterm_appl<Term>*>(cur)->arg[0])) Term(arg0);
  cur->next() = detail::aterm_hashtable[hnr];
  detail::aterm_hashtable[hnr] = cur;

  return cur;
}

template <class Term>
_aterm* term_appl2(const function_symbol &sym, const Term &arg0, const Term &arg1)
{
  assert(sym.arity()==2);

  CHECK_TERM(arg0);
  CHECK_TERM(arg1);
  HashNumber hnr = COMBINE(COMBINE(sym.number(), arg0),arg1);

  detail::_aterm *cur = detail::aterm_hashtable[hnr & detail::aterm_table_mask];
  while (cur)
  {
    if (cur->function()==sym &&
        reinterpret_cast<detail::_aterm_appl<Term>*>(cur)->arg[0] == arg0 && 
        reinterpret_cast<detail::_aterm_appl<Term>*>(cur)->arg[1] == arg1)
    {
      return cur;
    }
    cur = cur->next();
  }

  cur = detail::allocate_term(TERM_SIZE_APPL(2));
  /* Delay masking until after allocate_term */
  hnr &= detail::aterm_table_mask;
  new (&cur->function()) function_symbol(sym);
  new (&(reinterpret_cast<detail::_aterm_appl<Term>*>(cur)->arg[0])) Term(arg0);
  new (&(reinterpret_cast<detail::_aterm_appl<Term>*>(cur)->arg[1])) Term(arg1);

  cur->next() = detail::aterm_hashtable[hnr];
  detail::aterm_hashtable[hnr] = cur;

  return cur;
}

template <class Term>
_aterm* term_appl3(const function_symbol &sym, const Term &arg0, const Term &arg1, const Term &arg2)
{

  assert(sym.arity()==3);

  CHECK_TERM(arg0);
  CHECK_TERM(arg1);
  CHECK_TERM(arg2);
  HashNumber hnr = COMBINE(COMBINE(COMBINE(sym.number(), arg0),arg1),arg2);

  detail::_aterm *cur = detail::aterm_hashtable[hnr & detail::aterm_table_mask];
  while (cur)
  {
    if (cur->function()==sym &&
        reinterpret_cast<detail::_aterm_appl<Term>*>(cur)->arg[0] == arg0 &&
        reinterpret_cast<detail::_aterm_appl<Term>*>(cur)->arg[1] == arg1 &&
        reinterpret_cast<detail::_aterm_appl<Term>*>(cur)->arg[2] == arg2)
    {
      return cur;
    }
    cur = cur->next();
  }

  cur = detail::allocate_term(TERM_SIZE_APPL(3));
  /* Delay masking until after allocate_term */
  hnr &= detail::aterm_table_mask;
  new (&cur->function()) function_symbol(sym);
  new (&(reinterpret_cast<detail::_aterm_appl<Term>*>(cur)->arg[0])) Term(arg0);
  new (&(reinterpret_cast<detail::_aterm_appl<Term>*>(cur)->arg[1])) Term(arg1);
  new (&(reinterpret_cast<detail::_aterm_appl<Term>*>(cur)->arg[2])) Term(arg2);

  cur->next() = detail::aterm_hashtable[hnr];
  detail::aterm_hashtable[hnr] = cur;

  return cur;
}

template <class Term>
_aterm *term_appl4(const function_symbol &sym, const Term &arg0, const Term &arg1, const Term &arg2, const Term &arg3)
{
  CHECK_TERM(arg0);
  CHECK_TERM(arg1);
  CHECK_TERM(arg2);
  CHECK_TERM(arg3);
  assert(sym.arity()==4);

  HashNumber hnr = COMBINE(COMBINE(COMBINE(COMBINE(sym.number(), arg0), arg1), arg2), arg3);

  detail::_aterm* cur = detail::aterm_hashtable[hnr & detail::aterm_table_mask];
  while (cur && (cur->function()!=sym ||
     reinterpret_cast<detail::_aterm_appl<Term>*>(cur)->arg[0] != arg0 ||
     reinterpret_cast<detail::_aterm_appl<Term>*>(cur)->arg[1] != arg1 ||
     reinterpret_cast<detail::_aterm_appl<Term>*>(cur)->arg[2] != arg2 ||
     reinterpret_cast<detail::_aterm_appl<Term>*>(cur)->arg[3] != arg3))
  {
    cur = cur->next();
  }

  if (!cur)
  {
    cur = detail::allocate_term(TERM_SIZE_APPL(4));
    /* Delay masking until after allocate_term */
    hnr &= detail::aterm_table_mask;
    new (&cur->function()) function_symbol(sym);
    new (&(reinterpret_cast<detail::_aterm_appl<Term>*>(cur)->arg[0])) Term(arg0);
    new (&(reinterpret_cast<detail::_aterm_appl<Term>*>(cur)->arg[1])) Term(arg1);
    new (&(reinterpret_cast<detail::_aterm_appl<Term>*>(cur)->arg[2])) Term(arg2);
    new (&(reinterpret_cast<detail::_aterm_appl<Term>*>(cur)->arg[3])) Term(arg3);

    cur->next() = detail::aterm_hashtable[hnr];
    detail::aterm_hashtable[hnr] = cur;
  }

  return cur;
}

template <class Term>
_aterm* term_appl5(const function_symbol &sym, const Term &arg0, const Term &arg1, const Term &arg2,
                                      const Term &arg3, const Term &arg4)
{

  assert(sym.arity()==5);
  CHECK_TERM(arg0);
  CHECK_TERM(arg1);
  CHECK_TERM(arg2);
  CHECK_TERM(arg3);


  HashNumber hnr = COMBINE(COMBINE(COMBINE(COMBINE(COMBINE(sym.number(), arg0), arg1), arg2), arg3), arg4);

  detail::_aterm *cur = detail::aterm_hashtable[hnr & detail::aterm_table_mask];
  while (cur && (cur->function()!=sym ||
     reinterpret_cast<detail::_aterm_appl<Term>*>(cur)->arg[0] != arg0 ||
     reinterpret_cast<detail::_aterm_appl<Term>*>(cur)->arg[1] != arg1 ||
     reinterpret_cast<detail::_aterm_appl<Term>*>(cur)->arg[2] != arg2 ||
     reinterpret_cast<detail::_aterm_appl<Term>*>(cur)->arg[3] != arg3 ||
     reinterpret_cast<detail::_aterm_appl<Term>*>(cur)->arg[4] != arg4))
  {
    cur = cur->next();
  }

  if (!cur)
  {
    cur = detail::allocate_term(TERM_SIZE_APPL(5));
    /* Delay masking until after allocate_term */
    hnr &= detail::aterm_table_mask;
    new (&cur->function()) function_symbol(sym);
    new (&(reinterpret_cast<detail::_aterm_appl<Term>*>(cur)->arg[0])) Term(arg0);
    new (&(reinterpret_cast<detail::_aterm_appl<Term>*>(cur)->arg[1])) Term(arg1);
    new (&(reinterpret_cast<detail::_aterm_appl<Term>*>(cur)->arg[2])) Term(arg2);
    new (&(reinterpret_cast<detail::_aterm_appl<Term>*>(cur)->arg[3])) Term(arg3);
    new (&(reinterpret_cast<detail::_aterm_appl<Term>*>(cur)->arg[4])) Term(arg4);

    cur->next() = detail::aterm_hashtable[hnr];
    detail::aterm_hashtable[hnr] = cur;
  }

  return cur;
}

template <class Term>
_aterm *term_appl6(const function_symbol &sym, const Term &arg0, const Term &arg1, const Term &arg2,
                                      const Term &arg3, const Term &arg4, const Term &arg5)
{
  assert(sym.arity()==6);
  CHECK_TERM(arg0);
  CHECK_TERM(arg1);
  CHECK_TERM(arg2);
  CHECK_TERM(arg3);
  CHECK_TERM(arg4);
  CHECK_TERM(arg5);

  HashNumber hnr = COMBINE(COMBINE(COMBINE(COMBINE(COMBINE(COMBINE(sym.number(), arg0), arg1), arg2), arg3), arg4), arg5);

  detail::_aterm* cur = detail::aterm_hashtable[hnr & detail::aterm_table_mask];
  while (cur && (cur->function()!=sym ||
  reinterpret_cast<detail::_aterm_appl<Term>*>(cur)->arg[0] != arg0 ||
  reinterpret_cast<detail::_aterm_appl<Term>*>(cur)->arg[1] != arg1 ||
  reinterpret_cast<detail::_aterm_appl<Term>*>(cur)->arg[2] != arg2 ||
  reinterpret_cast<detail::_aterm_appl<Term>*>(cur)->arg[3] != arg3 ||
  reinterpret_cast<detail::_aterm_appl<Term>*>(cur)->arg[4] != arg4 ||
  reinterpret_cast<detail::_aterm_appl<Term>*>(cur)->arg[5] != arg5))
  {
    cur = cur->next();
  }

  if (!cur)
  {
    cur = detail::allocate_term(TERM_SIZE_APPL(6));
    /* Delay masking until after allocate_term */
    hnr &= detail::aterm_table_mask;
    
    new (&cur->function()) function_symbol(sym);
    new (&(reinterpret_cast<detail::_aterm_appl<Term>*>(cur)->arg[0])) Term(arg0);
    new (&(reinterpret_cast<detail::_aterm_appl<Term>*>(cur)->arg[1])) Term(arg1);
    new (&(reinterpret_cast<detail::_aterm_appl<Term>*>(cur)->arg[2])) Term(arg2);
    new (&(reinterpret_cast<detail::_aterm_appl<Term>*>(cur)->arg[3])) Term(arg3);
    new (&(reinterpret_cast<detail::_aterm_appl<Term>*>(cur)->arg[4])) Term(arg4);
    new (&(reinterpret_cast<detail::_aterm_appl<Term>*>(cur)->arg[5])) Term(arg5);

    cur->next() =detail::aterm_hashtable[hnr];
    detail::aterm_hashtable[hnr] = cur;
  }

  return cur;
}
} //namespace detail

template <class Term>
term_appl<Term> term_appl<Term>::set_argument(const Term &arg, const size_t n) 
{
  bool found;

  size_t arity = function().arity();
  assert(n < arity);

  HashNumber hnr = function().number();
  for (size_t i=0; i<arity; i++)
  {
    if (i!=n)
    {
      hnr = detail::COMBINE(hnr, (*this)(i));
    }
    else
    {
      hnr = detail::COMBINE(hnr, arg);
    }
  }


  detail::_aterm *cur = detail::aterm_hashtable[hnr & detail::aterm_table_mask];
  while (cur)
  {
    if (cur->function()==function())
    {
      found = true;
      for (size_t i=0; i<arity; i++)
      {
        if (i!=n)
        {
          if (reinterpret_cast<detail::_aterm_appl<Term>*>(cur)->arg[i]!=(*this)(i))  
          {
            found = false;
            break;
          }
        }
        else
        {
          if (reinterpret_cast<detail::_aterm_appl<Term>*>(cur)->arg[i]!=arg)
          {
            found = false;
            break;
          }
        }
      }
      if (found)
      {
        break;
      }
    }
    cur = cur->next();
  }

  if (!cur)
  {
    cur = detail::allocate_term(detail::TERM_SIZE_APPL(arity));
    /* Delay masking until after allocate_term */
    hnr &= detail::aterm_table_mask;
    new (&cur->function()) function_symbol((*this).function());
    for (size_t i=0; i<arity; i++)
    {
      if (i!=n)
      {
        new (&(reinterpret_cast<detail::_aterm_appl<Term>*>(cur)->arg[i])) Term((*this)(i));
      }
      else
      {
        new (&(reinterpret_cast<detail::_aterm_appl<Term>*>(cur)->arg[i])) Term(arg);
      }
    }
    cur->next() = detail::aterm_hashtable[hnr];
    detail::aterm_hashtable[hnr] = cur;
  }

  return reinterpret_cast<detail::_aterm_appl<Term>*>(cur);
}


} // namespace atermpp

#endif // MCRL2_ATERMPP_DETAIL_ATERM_APPL_IMPLEMENTATION_H

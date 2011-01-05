// Author(s): Wieger Wesselink
// Copyright: see the accompanying file COPYING or copy at
// https://svn.win.tue.nl/trac/MCRL2/browser/trunk/COPYING
//
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)
//
/// \file mcrl2/modal_formula/traverser.h
/// \brief add your file description here.

#ifndef MCRL2_MODAL_FORMULA_TRAVERSER_H
#define MCRL2_MODAL_FORMULA_TRAVERSER_H

#include "mcrl2/modal_formula/action_formula.h"
#include "mcrl2/modal_formula/regular_formula.h"
#include "mcrl2/modal_formula/state_formula.h"
#include "mcrl2/lps/traverser.h"

namespace mcrl2 {

namespace action_formulas {

  /// \brief Traversal class for action formula data types
  template <typename Derived>
  class traverser: public lps::traverser<Derived>
  {
    public:
      typedef lps::traverser<Derived> super;
      using super::operator();
      using super::enter;
      using super::leave;

// Include file with traverse member functions. This is to avoid duplication.
#include "mcrl2/modal_formula/detail/action_formula_traverser.inc.h"
  };

  template <typename Derived>
  class binding_aware_traverser_helper: public lps::binding_aware_traverser<Derived>
  {
    public:
      typedef lps::binding_aware_traverser<Derived> super;     
      using super::operator();
      using super::enter;
      using super::leave;

#include "mcrl2/modal_formula/detail/action_formula_traverser.inc.h"
  };

  /// \brief Handle binding variables.
  template <typename Derived>
  class binding_aware_traverser : public binding_aware_traverser_helper<Derived>
  {
    public:
      typedef binding_aware_traverser_helper<Derived> super;      
      using super::operator();
      using super::enter;
      using super::leave;
      using super::increase_bind_count;
      using super::decrease_bind_count;
      
      void operator()(exists const& x)
      {
        increase_bind_count(x.variables());
        super::operator()(x);
        decrease_bind_count(x.variables());
      }
      
      void operator()(forall const& x)
      {
        increase_bind_count(x.variables());
        super::operator()(x);
        decrease_bind_count(x.variables());
      }
  };

  template <typename Derived, typename AdaptablePredicate>
  class selective_traverser : public core::selective_traverser<Derived, AdaptablePredicate, action_formulas::traverser>
  {
    public:
      typedef core::selective_traverser<Derived, AdaptablePredicate, action_formulas::traverser> super;
      using super::operator();
      using super::enter;
      using super::leave;

      selective_traverser()
      { }

      selective_traverser(AdaptablePredicate predicate) : super(predicate)
      { }
  };

//  template <typename Derived, typename AdaptablePredicate>
//  class selective_binding_aware_traverser: public action_formulas::selective_traverser<Derived, AdaptablePredicate, action_formulas::binding_aware_traverser>
//  {
//    public:
//      typedef action_formulas::selective_traverser<Derived, AdaptablePredicate, action_formulas::binding_aware_traverser> super;
//      using super::enter;
//      using super::leave;
//      using super::operator();
//
//      selective_binding_aware_traverser()
//      { }
//
//      selective_binding_aware_traverser(AdaptablePredicate predicate): super(predicate)
//      { }
//  };

} // namespace action_formulas

namespace regular_formulas {

  /// \brief Traversal class for regular formula data types
  template <typename Derived>
  class traverser: public action_formulas::traverser<Derived>
  {
    public:
      typedef action_formulas::traverser<Derived> super;
      using super::operator();
      using super::enter;
      using super::leave;

// Include file with traverse member functions. This is to avoid duplication.
#include "mcrl2/modal_formula/detail/regular_formula_traverser.inc.h"
  };

  template <typename Derived>
  class binding_aware_traverser: public action_formulas::binding_aware_traverser<Derived>
  {
    public:
      typedef action_formulas::binding_aware_traverser<Derived> super;     
      using super::operator();
      using super::enter;
      using super::leave;

#include "mcrl2/modal_formula/detail/regular_formula_traverser.inc.h"
  };

  template <typename Derived, typename AdaptablePredicate>
  class selective_traverser : public core::selective_traverser<Derived, AdaptablePredicate, regular_formulas::traverser>
  {
    public:
      typedef core::selective_traverser<Derived, AdaptablePredicate, regular_formulas::traverser> super;
      using super::operator();
      using super::enter;
      using super::leave;

      selective_traverser()
      { }

      selective_traverser(AdaptablePredicate predicate) : super(predicate)
      { }
  };

//  template <typename Derived, typename AdaptablePredicate>
//  class selective_binding_aware_traverser: public regular_formulas::selective_traverser<Derived, AdaptablePredicate, regular_formulas::binding_aware_traverser>
//  {
//    public:
//      typedef regular_formulas::selective_traverser<Derived, AdaptablePredicate, regular_formulas::binding_aware_traverser> super;
//      using super::enter;
//      using super::leave;
//      using super::operator();
//
//      selective_binding_aware_traverser()
//      { }
//
//      selective_binding_aware_traverser(AdaptablePredicate predicate): super(predicate)
//      { }
//  };

} // namespace regular_formulas

namespace state_formulas {

  /// \brief Traversal class for state formula data types
  template <typename Derived>
  class traverser: public regular_formulas::traverser<Derived>
  {
    public:
      typedef regular_formulas::traverser<Derived> super;
      using super::operator();
      using super::enter;
      using super::leave;

// Include file with traverse member functions. This is to avoid duplication.
#include "mcrl2/modal_formula/detail/state_formula_traverser.inc.h"
  };

  template <typename Derived>
  class binding_aware_traverser_helper: public regular_formulas::binding_aware_traverser<Derived>
  {
    public:
      typedef regular_formulas::binding_aware_traverser<Derived> super;
      using super::operator();
      using super::enter;
      using super::leave;

#include "mcrl2/modal_formula/detail/state_formula_traverser.inc.h"
  };

  /// \brief Handle binding variables.
  template <typename Derived>
  class binding_aware_traverser : public binding_aware_traverser_helper<Derived>
  {
    public:
      typedef binding_aware_traverser_helper<Derived> super;
      using super::operator();
      using super::enter;
      using super::leave;
      using super::increase_bind_count;
      using super::decrease_bind_count;

    void operator()(exists const& x)
    {
      increase_bind_count(x.variables());
      super::operator()(x);
      decrease_bind_count(x.variables());
    }

    void operator()(forall const& x)
    {
      increase_bind_count(x.variables());
      super::operator()(x);
      decrease_bind_count(x.variables());
    }
  };

  template <typename Derived, typename AdaptablePredicate>
  class selective_traverser : public core::selective_traverser<Derived, AdaptablePredicate, state_formulas::traverser>
  {
    public:
      typedef core::selective_traverser<Derived, AdaptablePredicate, state_formulas::traverser> super;
      using super::operator();
      using super::enter;
      using super::leave;

      selective_traverser()
      { }

      selective_traverser(AdaptablePredicate predicate) : super(predicate)
      { }
  };

//  template <typename Derived, typename AdaptablePredicate>
//  class selective_binding_aware_traverser: public state_formulas::selective_traverser<Derived, AdaptablePredicate, state_formulas::binding_aware_traverser>
//  {
//    public:
//      typedef state_formulas::selective_traverser<Derived, AdaptablePredicate, state_formulas::binding_aware_traverser> super;
//      using super::enter;
//      using super::leave;
//      using super::operator();
//
//      selective_binding_aware_traverser()
//      { }
//
//      selective_binding_aware_traverser(AdaptablePredicate predicate): super(predicate)
//      { }
//  };

} // namespace state_formulas

} // namespace mcrl2

#endif // MCRL2_MODAL_FORMULA_TRAVERSER_H

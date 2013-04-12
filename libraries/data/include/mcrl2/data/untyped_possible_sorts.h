// Author(s): Wieger Wesselink
// Copyright: see the accompanying file COPYING or copy at
// https://svn.win.tue.nl/trac/MCRL2/browser/trunk/COPYING
//
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)
//
/// \file mcrl2/data/untyped_possible_sorts.h
/// \brief add your file description here.

#ifndef MCRL2_DATA_UNTYPED_POSSIBLE_SORTS_H
#define MCRL2_DATA_UNTYPED_POSSIBLE_SORTS_H

#include "mcrl2/atermpp/container_utility.h"
#include "mcrl2/core/detail/soundness_checks.h"
#include "mcrl2/data/sort_expression.h"

namespace mcrl2 {

namespace data {

//--- start generated class untyped_possible_sorts ---//
/// \brief Multiple possible sorts
class untyped_possible_sorts: public sort_expression
{
  public:
    /// \brief Default constructor.
    untyped_possible_sorts()
      : sort_expression(core::detail::constructUntypedSortsPossible())
    {}

    /// \brief Constructor.
    /// \param term A term
    untyped_possible_sorts(const atermpp::aterm& term)
      : sort_expression(term)
    {
      assert(core::detail::check_term_UntypedSortsPossible(*this));
    }

    /// \brief Constructor.
    untyped_possible_sorts(const sort_expression_list& sorts)
      : sort_expression(core::detail::gsMakeUntypedSortsPossible(sorts))
    {}

    /// \brief Constructor.
    template <typename Container>
    untyped_possible_sorts(const Container& sorts, typename atermpp::detail::enable_if_container<Container, sort_expression>::type* = 0)
      : sort_expression(core::detail::gsMakeUntypedSortsPossible(sort_expression_list(sorts.begin(), sorts.end())))
    {}

    const sort_expression_list& sorts() const
    {
      return atermpp::aterm_cast<const sort_expression_list>(atermpp::list_arg1(*this));
    }
};
//--- end generated class untyped_possible_sorts ---//

} // namespace data

} // namespace mcrl2

#endif // MCRL2_DATA_UNTYPED_POSSIBLE_SORTS_H

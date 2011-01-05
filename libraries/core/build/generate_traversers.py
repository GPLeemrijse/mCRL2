#~ Copyright 2009, 2010 Wieger Wesselink.
#~ Distributed under the Boost Software License, Version 1.0.
#~ (See accompanying file LICENSE_1_0.txt or http://www.boost.org/LICENSE_1_0.txt)

import os
import re
from path import *
from mcrl2_classes import *
from mcrl2_utility import *

BUILDER_FUNCTION = r'''EXPRESSION operator()(const QUALIFIED_NODE& x)
{
  static_cast<Derived&>(*this).enter(x);
  VISIT_FUNCTIONS
  static_cast<Derived&>(*this).leave(x);
  return result;
}
'''

def compare_classes(x, y):
    if 'X' in x.modifiers() and 'X' in y.modifiers():
        return cmp(x.index, y.index)
    return cmp('X' in x.modifiers(), 'X' in y.modifiers())

def make_traverser(filename, classnames, all_classes):
    result = []
    classes = [all_classes[name] for name in classnames]

    # preserve the same order as old generation
    classes.sort(compare_classes)

    for c in classes:
        result.append(c.traverse_function(all_classes))
    text = '\n'.join(result)

    #----------------------------------------------------------------------------------------#
    # N.B. THIS IS AN UGLY HACK to deal with the optional time function in some LPS classes
    # TODO: investigate if the time interface can be improved
    text = re.sub(r'static_cast<Derived&>\(\*this\)\(x.time\(\)\)', 'if (x.has_time()) static_cast<Derived&>(*this)(x.time());', text)
    #----------------------------------------------------------------------------------------#

    insert_text_in_file(filename, text, 'generated code')

def create_builder_file(filename, namespace):
    text = '''// Author(s): Wieger Wesselink
// Copyright: see the accompanying file COPYING or copy at
// https://svn.win.tue.nl/trac/MCRL2/browser/trunk/COPYING
//
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)
//
/// \\file FILENAME
/// \\brief The content of this file is included in other header
/// files, to prevent duplication.

//--- start generated code ---//
//--- end generated code ---//
'''

    #if path(filename).exists():
    #    path(filename).remove()
    if not path(filename).exists():
        name = re.sub(r'\.\./\.\.', 'mcrl2', filename)
        text = re.sub('FILENAME', name, text)
        path(filename).write_text(text)
    #os.system('svn add %s' % filename)

def make_builder(filename, class_map, all_classes, namespace, expression, dependencies, modifiability_map):
    classnames = parse_classnames(class_map[namespace], namespace)
    create_builder_file(filename, namespace)

    result = []
    classes = [all_classes[name] for name in classnames]   

    # preserve the same order as old generation
    classes.sort(compare_classes)

    for c in classes:
        if is_dependent_type(dependencies, c.classname(True)):
            result.append(c.builder_function(all_classes, dependencies, modifiability_map))
    text = '\n'.join(result)

    insert_text_in_file(filename, text, 'generated code')

if __name__ == "__main__":
    class_map = {
          'core'             : CORE_CLASSES,
          'data'             : DATA_EXPRESSION_CLASSES + ASSIGNMENT_EXPRESSION_CLASSES + SORT_EXPRESSION_CLASSES + CONTAINER_TYPES + BINDER_TYPES + ABSTRACTION_EXPRESSION_CLASSES + STRUCTURED_SORT_ELEMENTS + DATA_CLASSES,
          'state_formulas'   : STATE_FORMULA_CLASSES,
          'regular_formulas' : REGULAR_FORMULA_CLASSES,
          'action_formulas'  : ACTION_FORMULA_CLASSES,
          'lps'              : LPS_CLASSES,
          'process'          : PROCESS_CLASSES + PROCESS_EXPRESSION_CLASSES,
          'pbes_system'      : PBES_CLASSES + PBES_EXPRESSION_CLASSES,
          'bes'              : BOOLEAN_CLASSES + BOOLEAN_EXPRESSION_CLASSES
        }

    all_classes = parse_class_map(class_map)

    make_traverser('../../bes/include/mcrl2/bes/detail/traverser.inc.h'                          , parse_classnames(class_map['bes'], 'bes'), all_classes)
    make_traverser('../../lps/include/mcrl2/lps/detail/traverser.inc.h'                          , parse_classnames(class_map['lps'], 'lps'), all_classes)
    make_traverser('../../process/include/mcrl2/process/detail/traverser.inc.h'                  , parse_classnames(class_map['process'], 'process'), all_classes)
    make_traverser('../../data/include/mcrl2/data/detail/traverser.inc.h'                        , parse_classnames(class_map['data'], 'data'), all_classes)
    make_traverser('../../pbes/include/mcrl2/pbes/detail/traverser.inc.h'                        , parse_classnames(class_map['pbes_system'], 'pbes_system'), all_classes)
    make_traverser('../../lps/include/mcrl2/modal_formula/detail/action_formula_traverser.inc.h' , parse_classnames(class_map['action_formulas'], 'action_formulas'), all_classes)
    make_traverser('../../lps/include/mcrl2/modal_formula/detail/regular_formula_traverser.inc.h', parse_classnames(class_map['regular_formulas'], 'regular_formulas'), all_classes)
    make_traverser('../../lps/include/mcrl2/modal_formula/detail/state_formula_traverser.inc.h'  , parse_classnames(class_map['state_formulas'], 'state_formulas'), all_classes)

    boolean_expression_dependencies = find_dependencies(all_classes, 'bes::boolean_expression')
    data_expression_dependencies    = find_dependencies(all_classes, 'data::data_expression')
    pbes_expression_dependencies    = find_dependencies(all_classes, 'pbes_system::pbes_expression')
    process_expression_dependencies = find_dependencies(all_classes, 'process::process_expression')
    sort_expression_dependencies    = find_dependencies(all_classes, 'data::sort_expression')
    action_formula_dependencies     = find_dependencies(all_classes, 'action_formulas::action_formula')
    regular_formula_dependencies    = find_dependencies(all_classes, 'regular_formulas::regular_formula')
    state_formula_dependencies      = find_dependencies(all_classes, 'state_formulas::state_formula')

    modifiability_map = make_modifiability_map(all_classes)

    # sort_expression_builder
    make_builder('../../data/include/mcrl2/data/detail/sort_expression_builder.inc.h', class_map, all_classes, 'data', 'data::sort_expression', sort_expression_dependencies, modifiability_map)
    make_builder('../../lps/include/mcrl2/lps/detail/sort_expression_builder.inc.h', class_map, all_classes, 'lps', 'data::sort_expression', sort_expression_dependencies, modifiability_map)
    make_builder('../../process/include/mcrl2/process/detail/sort_expression_builder.inc.h', class_map, all_classes, 'process', 'data::sort_expression', sort_expression_dependencies, modifiability_map)
    make_builder('../../pbes/include/mcrl2/pbes/detail/sort_expression_builder.inc.h', class_map, all_classes, 'pbes_system', 'data::sort_expression', sort_expression_dependencies, modifiability_map)
    make_builder('../../lps/include/mcrl2/modal_formula/detail/action_formula_sort_expression_builder.inc.h', class_map, all_classes, 'action_formulas', 'data::sort_expression', sort_expression_dependencies, modifiability_map)
    make_builder('../../lps/include/mcrl2/modal_formula/detail/regular_formula_sort_expression_builder.inc.h', class_map, all_classes, 'regular_formulas', 'data::sort_expression', sort_expression_dependencies, modifiability_map)
    make_builder('../../lps/include/mcrl2/modal_formula/detail/state_formula_sort_expression_builder.inc.h', class_map, all_classes, 'state_formulas', 'data::sort_expression', sort_expression_dependencies, modifiability_map)

    # data_expression_builder
    make_builder('../../data/include/mcrl2/data/detail/data_expression_builder.inc.h', class_map, all_classes, 'data', 'data::data_expression', data_expression_dependencies, modifiability_map)
    make_builder('../../lps/include/mcrl2/lps/detail/data_expression_builder.inc.h', class_map, all_classes, 'lps', 'data::data_expression', data_expression_dependencies, modifiability_map)
    make_builder('../../process/include/mcrl2/process/detail/data_expression_builder.inc.h', class_map, all_classes, 'process', 'data::data_expression', data_expression_dependencies, modifiability_map)
    make_builder('../../pbes/include/mcrl2/pbes/detail/data_expression_builder.inc.h', class_map, all_classes, 'pbes_system', 'data::data_expression', data_expression_dependencies, modifiability_map)
    make_builder('../../lps/include/mcrl2/modal_formula/detail/action_formula_data_expression_builder.inc.h', class_map, all_classes, 'action_formulas', 'data::data_expression', data_expression_dependencies, modifiability_map)
    make_builder('../../lps/include/mcrl2/modal_formula/detail/regular_formula_data_expression_builder.inc.h', class_map, all_classes, 'regular_formulas', 'data::data_expression', data_expression_dependencies, modifiability_map)
    make_builder('../../lps/include/mcrl2/modal_formula/detail/state_formula_data_expression_builder.inc.h', class_map, all_classes, 'state_formulas', 'data::data_expression', data_expression_dependencies, modifiability_map)

    # pbes_expression_builder
    make_builder('../../pbes/include/mcrl2/pbes/detail/pbes_expression_builder.inc.h', class_map, all_classes, 'pbes_system', 'pbes_system::pbes_expression', pbes_expression_dependencies, modifiability_map)

    # boolean_expression_builder
    make_builder('../../bes/include/mcrl2/bes/detail/boolean_expression_builder.inc.h', class_map, all_classes, 'bes', 'bes::boolean_expression', boolean_expression_dependencies, modifiability_map)

    # process_expression_builder
    make_builder('../../process/include/mcrl2/process/detail/process_expression_builder.inc.h', class_map, all_classes, 'process', 'process::process_expression', process_expression_dependencies, modifiability_map)

    # state_formula_builder
    make_builder('../../lps/include/mcrl2/modal_formula/detail/state_formula_builder.inc.h', class_map, all_classes, 'state_formulas', 'state_formulas::state_formula', state_formula_dependencies, modifiability_map)

    # action_formula_builder
    make_builder('../../lps/include/mcrl2/modal_formula/detail/action_formula_builder.inc.h', class_map, all_classes, 'action_formulas', 'action_formulas::action_formula', action_formula_dependencies, modifiability_map)

    # regular_formula_builder
    make_builder('../../lps/include/mcrl2/modal_formula/detail/regular_formula_builder.inc.h', class_map, all_classes, 'regular_formulas', 'regular_formulas::regular_formula', regular_formula_dependencies, modifiability_map)

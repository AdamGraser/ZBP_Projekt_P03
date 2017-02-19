/* Automaton - C++ implementation of a sample program for the paper:
** MG Ciura, S Deorowicz, "How to squeeze a lexicon".
**
** This program uses st_tree library: https://github.com/erikerlandson/st_tree
** Author: Erik Erlandson
** License: Apache 2.0 license (full text of license may be found in "LICENSE" file)
*/

#pragma once

#include <list>
#include "st_tree.h"
#include <string>
#include <type_traits>

using std::list;
using std::string;
using st_tree::tree;

template <class ListContainer = list<string>, class TreeContainer = tree<string>> class Automaton
{
    static_assert(std::is_base_of<list<string>, ListContainer>::value, "ListContainer must inherit from list<string>");
    static_assert(std::is_base_of<tree<string>, TreeContainer>::value, "TreeContainer must inherit from tree<string>");

private:
    ListContainer listContainer;
    TreeContainer treeContainer;

public:
    Automaton()
    { }

    ~Automaton()
    { }
};

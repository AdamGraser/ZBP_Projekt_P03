/* Automaton - C++ implementation of a sample program for the paper:
** MG Ciura, S Deorowicz, "How to squeeze a lexicon".
** 2001-05-04 - 2006-03-01
**
** Created by Grzegorz Kuünik and Adam Gr‰ser
** as a project "Dictionary on FSA", "Advanced Programming Libraries", 2017
*/

#pragma once

#include <string>
#include <type_traits>
#include <cstddef>
#include <iostream>
#include <ctime>
#include "targetver.h"

using std::string;
using std::size_t;
using std::cout;



/// <summary>
/// Finite-State Automaton, that represents its states in form of lists or complete binary trees.
/// </summary>
template <bool use_tree> class Automaton
{ };



/// <summary>
/// Finite-State Automaton, that represents its states in form of lists.
/// </summary>
template <> class Automaton<false>
{
public:
    /// <summary>Defines whether statistics should be printed on the screen.</summary>
    bool print_statistics;

    /// <summary>Default constructor.</summary>
    /// <param name="print_statistics">Defines whether statistics should be printed on the screen.</param>
    Automaton(bool print_statistics = true)
        :print_statistics(print_statistics)
    {}

    /// <summary>Default destructor.</summary>
    ~Automaton()
    {}
};



/// <summary>
/// Finite-State Automaton, that represents its states in form of complete binary trees.
/// </summary>
template <> class Automaton<true>
{
public:
    /// <summary>Defines whether statistics should be printed on the screen.</summary>
    bool print_statistics;

    /// <summary>Default constructor.</summary>
    /// <param name="print_statistics">Defines whether statistics should be printed on the screen.</param>
    Automaton(bool print_statistics = true)
        :print_statistics(print_statistics)
    {}

    /// <summary>Default destructor.</summary>
    ~Automaton()
    {}
};

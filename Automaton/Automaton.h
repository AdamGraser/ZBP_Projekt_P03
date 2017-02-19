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
    /// <summary>Maximal lexicon string length.</summary>
    int max_str_len;
    /// <summary></summary>
    int max_chars;
    /// <summary></summary>
    int ht_size;
    /// <summary></summary>
    int ht_elem_size;
    /// <summary></summary>
    int max_aut_size;

    /// <summary>Default constructor.</summary>
    /// <param name="print_statistics">Defines whether statistics should be printed on the screen.</param>
    /// <param name="max_str_len">Maximal lexicon string length.</param>
    /// <param name="max_chars"></param>
    /// <param name="ht_size"></param>
    Automaton(bool print_statistics = true, int max_str_len = 300, int max_chars = 256, int ht_size = (1 << 20), int ht_elem_size = (1 << 10),
              int max_aut_size = (1 << 22))
        :print_statistics(print_statistics), max_str_len(max_str_len), max_chars(max_chars), ht_size(ht_size), ht_elem_size(ht_elem_size), max_aut_size(max_aut_size)
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
    /// <summary>Maximal lexicon string length.</summary>
    int max_str_len;
    /// <summary></summary>
    int max_chars;
    /// <summary></summary>
    int ht_size;
    /// <summary></summary>
    int ht_elem_size;
    /// <summary></summary>
    int max_aut_size;

    /// <summary>Default constructor.</summary>
    /// <param name="print_statistics">Defines whether statistics should be printed on the screen.</param>
    /// <param name="max_str_len">Maximal lexicon string length.</param>
    /// <param name="max_chars"></param>
    /// <param name="ht_size"></param>
    Automaton(bool print_statistics = true, int max_str_len = 300, int max_chars = 256, int ht_size = (1 << 20), int ht_elem_size = (1 << 10),
              int max_aut_size = (1 << 21))
        :print_statistics(print_statistics), max_str_len(max_str_len), max_chars(max_chars), ht_size(ht_size), ht_elem_size(ht_elem_size), max_aut_size(max_aut_size)
    {}

    /// <summary>Default destructor.</summary>
    ~Automaton()
    {}
};

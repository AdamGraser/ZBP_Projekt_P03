/* Automaton - C++ version of a sample program for the paper:
** MG Ciura, S Deorowicz, "How to squeeze a lexicon".
** 2001-05-04 - 2006-03-01
**
** Created by Grzegorz Kuünik and Adam Gr‰ser
** as a project "Dictionary on FSA", "Advanced Programming Libraries", 2017
*/

#pragma once

#include <limits.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include "targetver.h"



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

    /// <summary>Default constructor.</summary>
    /// <param name="dictionary_file">Lexicon file.</param>
    /// <param name="automaton_file">Automaton file.</param>
    /// <param name="print_statistics">Defines whether statistics should be printed on the screen.</param>
    Automaton(char *lexicon_file, char *automaton_file, bool print_statistics = true)
        :print_statistics(print_statistics)
    {
        open_dict(lexicon_file, "r");
        make_automat();
        save_automat(automaton_file);
        read_automat(automaton_file);
    }

    /// <summary>Default destructor. Closes lexicon file.</summary>
    ~Automaton()
    {
        fclose(lex_file);
    }

private:
    // -- consts

    /// <summary>Maximal lexicon string length.</summary>
    static const int MAX_STR_LEN = 300;
    /// <summary></summary>
    static const int MAX_CHARS = 256;
    /// <summary>Size of hash table.</summary>
    static const int HT_SIZE = (1 << 20);
    /// <summary>Hash table element's size.</summary>
    static const int HT_ELEM_SIZE = (1 << 10);
    /// <summary>Maximal size of automaton.</summary>
    static const int MAX_AUT_SIZE = (1 << 21);

    // -- types

    typedef union
    {
        unsigned all_fields;
        struct
        {
            unsigned llast : 1;
            unsigned rlast : 1;
            unsigned dest : 21;
            unsigned attr : 8;
            unsigned term : 1;
        } b;
    } transition;

    typedef int sizeof_unsigned_int_must_match_sizeof_transition
        [2 * (sizeof(unsigned) == sizeof(transition)) - 1];

    typedef struct tbucket
    {
        unsigned addr;
        size_t size;
        struct tbucket *next;
    } bucket;

    // -- fields

    /// <summary>Number of strings.</summary>
    unsigned long n_strings;
    /// <summary>Number of characters.</summary>
    unsigned long n_chars;

    bucket *hash_table[HT_SIZE];
    bucket *ht_elem[MAX_AUT_SIZE / HT_ELEM_SIZE];
    int ht_next_elem, ht_last_pos;

    /// <summary>The automaton.</summary>
    transition automat[MAX_AUT_SIZE];
    /// <summary>Size of the automaton.</summary>
    unsigned aut_size;
    /// <summary>Position of the start state.</summary>
    unsigned start_state;
    transition larval_state[MAX_STR_LEN + 1][MAX_CHARS];
    size_t l_state_len[MAX_STR_LEN + 1];
    int is_terminal[MAX_STR_LEN + 1];
    /// <summary>String for listing.</summary>
    unsigned char temp_str[MAX_STR_LEN + 1];
    transition temp_state[MAX_CHARS + 1];

    /// <summary>Lexicon file.</summary>
    FILE *lex_file;
    /// <summary>Automaton file.</summary>
    FILE *aut_file;

    // -- statistics fields

    /// <summary>Number of terminal transitions.</summary>
    unsigned n_term_trans;
    /// <summary>Number of states.</summary>
    unsigned n_states;
    /// <summary>Number of transitions.</summary>
    unsigned n_trans;

    // -- methods

    /// <summary>Print an error message and terminate the program.</summary>
    /// <param name="message">Error message.</param>
    void error(char *message)
    {
        fprintf(stderr, "%s\n", message);
        exit(EXIT_FAILURE);
    }

    /// <summary>Prepare data structures before program run.</summary>
    void prepare_tables()
    {
        int i;

        for (i = HT_SIZE - 1; i >= 0; i--)
        {
            hash_table[i] = NULL;
        }

        for (i = MAX_AUT_SIZE / HT_ELEM_SIZE - 1; i >= 0; i--)
            ht_elem[i] = NULL;
        ht_last_pos = -1;
        ht_next_elem = HT_ELEM_SIZE;

        aut_size = 0;
        for (i = 0; i < MAX_STR_LEN + 1; i++)
            l_state_len[i] = 0;
    }

    /// <summary>Set buffer for I/O stream.</summary>
    /// <param name="file">I/O stream.</param>
    /// <param name="size">Buffer size.</param>
    void set_io_buffer(FILE *file, size_t size)
    {
        if (setvbuf(file, NULL, _IOFBF, size) != 0)
            error("Cannot set input buffer.");
    }

    /// <summary>Read next string from input file and return its length.</summary>
    /// <param name="str">Array of chars to put the string into.</param>
    /// <returns>Length of the read string.</returns>
    int read_string(unsigned char *str)
    {
        int c, i;

        for (i = 0; (c = getc(lex_file)) != '\n'; str[i++] = (unsigned char) c)
        {
            if (c == EOF)
                return 0;
            if (i > MAX_STR_LEN)
                error("Lexicon string too long.");
        }
        str[i] = '\0';
        n_strings++;
        n_chars += i + 1;

        return i;
    }

    /// <summary>Hash function for states.</summary>
    /// <param name="state">States to hash.</param>
    /// <param name="state_len">Size of "state" array.</param>
    /// <returns>Hash of states.</returns>
    unsigned hash_state(transition *state, unsigned state_len)
    {
        unsigned r = 0;
        int i;

        for (i = state_len - 1; i >= 0; i--)
            r += state[i].all_fields;

        return ((r * 324027) >> 13) % HT_SIZE;
    }

    /// <summary>Make a tree-shaped state from a larval state.</summary>
    /// <param name="state">States to build a tree of.</param>
    /// <param name="left"></param>
    /// <param name="right"></param>
    /// <param name="pos"></param>
    /// <param name="full"></param>
    void make_tree(transition *state, int left, int right, unsigned pos, int full)
    {
        int size, sel, rest;

        size = right - left + 1;

        if (full == -1)
        {
            /* first time count of elements in full part of the tree (full) */
            full = 0;
            while (2 * full + 1 < size)
                full = 2 * full + 1;
        }

        sel = left + full / 2;     /* sel - number of root element */
        rest = size - full;         /* number of elements in last row */
        if (rest > (full + 1) / 2)  /* are elts in last row right > root */
            sel += (full + 1) / 2;  /* add only `left' elements */
        else
            sel += rest;            /* add all elements in last row */

                                    /* put transition to root of current subtree */
        temp_state[pos] = state[sel];

        /* recursively create rest of the tree */
        if (left > sel - 1)
            temp_state[pos].b.llast = 1;   /* there are no `left' elements */
        else
            make_tree(state, left, sel - 1, pos + pos + 1, full / 2);

        if (sel + 1 > right)
            temp_state[pos].b.rlast = 1;  /* there are no `right' elements */
        else
            make_tree(state, sel + 1, right, pos + pos + 2, full / 2);
    }

    /// <summary>Seek an identical state in the automaton or create a new state.</summary>
    /// <param name="state">Array of current automaton states.</param>
    /// <param name="state_len">Size of "state" array.</param>
    /// <returns>Index of a found or created state.</returns>
    unsigned make_state(transition *state, unsigned state_len)
    {
        bucket *ptr;
        int i;
        unsigned pos;
        unsigned hash_addr;

        /* make zero state */
        if (state_len == 0)
            state[state_len++].all_fields = 0;

        make_tree(state, 0, state_len - 1, 0, -1);

        /* check if an identical state is in automat */
        hash_addr = hash_state(state, state_len);
        for (ptr = hash_table[hash_addr]; ptr; ptr = ptr->next)
        {
            if (ptr->size == state_len)
            {
                for (i = state_len - 1; i >= 0; i--)
                    if (automat[ptr->addr + i].all_fields != temp_state[i].all_fields)
                        break;

                if (i < 0)
                    return ptr->addr;       /* identical state found */
            }
        }

        if (print_statistics)
            for (i = state_len - 1; i >= 0; i--)
                n_term_trans += state[i].b.term;

        if (aut_size + state_len >= MAX_AUT_SIZE)
            error("The automaton grew too large.");

        /* put state into automat */
        for (i = state_len - 1; i >= 0; i--)
            automat[aut_size + i] = temp_state[i];

        if (ht_next_elem >= HT_ELEM_SIZE)
        {
            ht_next_elem = 0;
            if ((ht_elem[++ht_last_pos] =
                (bucket *) malloc(sizeof(bucket) * HT_ELEM_SIZE)) == NULL)
                error("Not enough memory.");
        }
        ptr = &ht_elem[ht_last_pos][ht_next_elem++];

        /* put pointer to the state into hash table */
        ptr->addr = aut_size;
        ptr->size = state_len;
        ptr->next = hash_table[hash_addr];
        hash_table[hash_addr] = ptr;

        pos = aut_size;
        aut_size += state_len;

        if (print_statistics)
        {
            n_states++;
            n_trans += state_len;
        }

        return pos;
    }

    /// <summary>Create the automaton.</summary>
    void make_automat()
    {
        unsigned char s0[MAX_STR_LEN + 1] = "";
        unsigned char s1[MAX_STR_LEN + 1];
        size_t i = 0, p, q;
        transition new_trans;

        new_trans.b.llast = 0;
        new_trans.b.rlast = 0;

        prepare_tables();   /* prepare data structures for algorithm */

        while ((q = read_string(s1)) != 0)
        {
            /* find common prefix */
            for (p = 0; s1[p] == s0[p]; p++)
                ;
            if (s1[p] < s0[p])
                error("Strings in the lexicon file are unsorted.");

            /* emit states for suffix of previous string */
            while (i > p)
            {
                new_trans.b.dest = make_state(larval_state[i], l_state_len[i]);
                new_trans.b.attr = s0[--i];
                new_trans.b.term = is_terminal[i + 1];
                larval_state[i][l_state_len[i]++].all_fields = new_trans.all_fields;
            }

            /* copy suffix of s1 to s0 */
            while (i < q)
            {
                s0[i] = s1[i];
                is_terminal[++i] = 0;
                l_state_len[i] = 0;
            }
            s0[q] = '\0';
            is_terminal[q] = 1;

            if(print_statistics)
                if (n_strings % 65536 == 0)
                    printf("%lu strings read\t%u transitions created\n", n_strings, aut_size);
        }
        while (i > 0)
        {
            new_trans.b.dest = make_state(larval_state[i], l_state_len[i]);
            new_trans.b.term = is_terminal[i];
            new_trans.b.attr = s0[--i];
            larval_state[i][l_state_len[i]++].all_fields = new_trans.all_fields;
        }
        start_state = make_state(larval_state[0], l_state_len[0]);
        automat[aut_size].b.dest = start_state;   /* put pseudo state */
    }

    /// <summary>Check if the given string exists in the automaton.</summary>
    /// <param name="str">String to look for.</param>
    /// <returns>Non-zero value, if the given string has been found in the automaton. Otherwise zero is returned.</returns>
    int check_string(unsigned char *str)
    {
        unsigned pos = 0;
        int i;
        unsigned char w;

        int found;
        int offset;
        transition e;

        pos = automat[pos].b.dest;

        for (i = 0; str[i]; i++)
        {
            if (pos > aut_size)
                error("Error in automaton file.");
            found = 0;
            w = str[i];
            offset = 1;

            /* search the tree for current character */
            while (1)
            {
                e = automat[pos + offset - 1];
                if (e.b.attr == w)
                {
                    found = 1;
                    break;
                }
                if (e.b.attr > w)
                {
                    if (e.b.llast)
                        break;
                    offset = offset * 2;
                }
                else
                {
                    if (e.b.rlast)
                        break;
                    offset = offset * 2 + 1;
                }
            }
            if (!found)
                return 0;
            if (str[i + 1])          /* if not last character in string */
                pos = e.b.dest;    /* get index of new state */
        }
        return e.b.term;
    }

    /// <summary>Check if the automaton is correct (test all the strings from a lexicon).</summary>
    void test_automat()
    {
        n_strings = 0;
        n_chars = 0;

        while (read_string(temp_str))
            if (!check_string(temp_str))
                printf("String %s not found!\n", temp_str);
    }

    /// <summary>
    /// Recursively list all the strings recognized by an automaton,
    /// beginning at the given position in the automaton and in the string.
    /// </summary>
    /// <param name="pos"></param>
    /// <param name="str_pos"></param>
    /// <param name="tree_pos"></param>
    void list_strings(unsigned pos, int str_pos, int tree_pos)
    {
        int i;

        if (pos == 0)
            return;

        if (pos > aut_size)
            error("Error in automat file.");

        /* go left */
        if (!automat[pos + tree_pos].b.llast)
            list_strings(pos, str_pos, tree_pos + tree_pos + 1);

        /* add new character */
        temp_str[str_pos] = (unsigned char) (automat[pos + tree_pos].b.attr);
        if (automat[pos + tree_pos].b.term)
        {
            /* when string terminates at this character write the string */
            for (i = 0; i <= str_pos; i++)
                putc(temp_str[i], lex_file);
            putc('\n', lex_file);
            n_strings++;
            n_chars += str_pos + 2;
        }
        /* execute recursively for all characters in current state */
        list_strings(automat[pos + tree_pos].b.dest, str_pos + 1, 0);

        /* go right */
        if (!automat[pos + tree_pos].b.rlast)
            list_strings(pos, str_pos, tree_pos + tree_pos + 2);
    }

    /// <summary>Show some statistics, including execution time.</summary>
    /// <param name="exec_time">Execution time.</param>
    void show_stat(double exec_time)
    {
        printf("%lu strings\t%lu characters\n", n_strings, n_chars);

        if(print_statistics)
            printf("%u states\t%u transitions\t%u terminal transitions\n",
                   n_states, n_trans, n_term_trans);

        printf("Execution time: %.3f seconds\t", exec_time);
        if (exec_time != 0.0)
        {
            printf("\nExecution speed: %.lf wps, %.lf cps\n",
                   n_strings / exec_time, n_chars / exec_time);
        }
        printf("Size of the automaton: %u bytes\n", aut_size * sizeof automat[0]);
    }

    /// <summary>Read an automaton from a file.</summary>
    /// <param name="fname">File name.</param>
    void read_automat(char *fname)
    {
        if ((aut_file = fopen(fname, "rb")) == NULL)
            error("Cannot open input file.");

        aut_size = fread(automat, sizeof automat[0], MAX_AUT_SIZE, aut_file);
        fclose(aut_file);
        if (aut_size >= 2)
        {
            /* create a pseudo state pointing to the start state */
            memcpy(&start_state, &automat[0], sizeof automat[0]);
            automat[0].b.dest = automat[0].all_fields;
            if (start_state < aut_size)
                return;
        }
        error("Error in input file.");
    }

    /// <summary>Save the automaton to a file.</summary>
    /// <param name="fname">Name of destination file.</param>
    void save_automat(char *fname)
    {
        if ((aut_file = fopen(fname, "wb")) == NULL)
            error("Cannot open output file.");

        /* create a pseudo state pointing to the start state */
        automat[0].all_fields = start_state;
        if (fwrite(automat, sizeof automat[0], aut_size, aut_file) < aut_size)
            error("Error writing to file.");
        fclose(aut_file);
    }

    /// <summary>Open the lexicon file.</summary>
    /// <param name="fname">File name.</param>
    /// <param name="attr">Attributes for opening-file-function.</param>
    void open_dict(char *fname, char *attr)
    {
        if ((lex_file = fopen(fname, attr)) == NULL)
            error("Cannot open lexicon file.");
        set_io_buffer(lex_file, 8192);
    }

    /// <summary>Print usage info.</summary>
    void show_info(void)
    {
        printf("Usage: am -m automaton_file lexicon_file -- make an automaton\n"
               "       am -l automaton_file lexicon_file -- list an automaton\n"
               "       am -t automaton_file lexicon_file -- test an automaton\n"
               "\nPress any key to exit...\n");
        fgetc(stdin);
        exit(EXIT_SUCCESS);
    }

    /// <summary>Check if the given string exists in the automaton.</summary>
    /// <param name="keyword">String to look for.</param>
    /// <returns>Boolean value that determines whether the given string has been found.</returns>
    bool exists(unsigned char *keyword)
    {
        n_strings = 0;
        n_chars = 0;

        if (!check_string(keyword))
            return false;

        return true;
    }
};

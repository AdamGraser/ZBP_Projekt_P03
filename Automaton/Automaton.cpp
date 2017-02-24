/* Automaton - C++ implementation of a sample program for the paper:
** MG Ciura, S Deorowicz, "How to squeeze a lexicon".
** 2001-05-04 - 2006-03-01
**
** Created by Grzegorz Kuünik and Adam Gr‰ser
** as a project "Dictionary on FSA", "Advanced Programming Libraries", 2017
*/

#include "Automaton.h"

// PRIVATE

/*
** Open the lexicon file.
*/
void Automaton<true>::open_dict(char *fname, char *attr)
{
    if ((lex_file = fopen(fname, attr)) == NULL)
        error("Cannot open lexicon file.");
    set_io_buffer(lex_file, 8192);
}

/*
** Save the automaton to a file of given name.
*/
void Automaton<true>::save_automat(char *fname)
{
    if ((aut_file = fopen(fname, "wb")) == NULL)
        error("Cannot open output file.");

    /* create a pseudo state pointing to the start state */
    automat[0].all_fields = start_state;
    if (fwrite(automat, sizeof automat[0], aut_size, aut_file) < aut_size)
        error("Error writing to file.");
    fclose(aut_file);
}

/*
** Read an automaton from a file fname.
*/
void Automaton<true>::read_automat(char *fname)
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

/*
** Recursively list all the strings recognized by an automaton,
** beginning at the given position in the automaton and in the
** string.
*/
void Automaton<true>::list_strings(iterator it, int str_pos)
{
    int i;
    unsigned char currentLetter = *it;

    /* go left */
    if (!it.isEnd(currentLetter - 1))
    {
        iterator left(it);
        ++left;
        list_strings(left, str_pos);
    }

    /* add new character */
    temp_str[str_pos] = currentLetter;

    if (it.isTerm())
    {
        /* when string terminates at this character write the string */
        for (i = 0; i <= str_pos; i++)
            putc(temp_str[i], lex_file);

        putc('\n', lex_file);
        n_strings++;
        n_chars += str_pos + 2;
    }

    /* execute recursively for all characters in current state */
    if (!it.isRoot())
    {
        iterator next = it.localBegin();
        list_strings(next, str_pos + 1);
    }

    /* go right */
    if (!it.isEnd(currentLetter + 1))
    {
        iterator right(it);
        ++right += 1;
        list_strings(right, str_pos);
    }
}

void Automaton<true>::print_strings(iterator it, int str_pos)
{
    int i;
    unsigned char currentLetter = *it;

    /* go left */
    if (!it.isEnd(currentLetter - 1))
    {
        iterator left(it);
        ++left;
        print_strings(left, str_pos);
    }

    /* add new character */
    tempString.seekp(str_pos, std::ios_base::beg);
    tempString << currentLetter;

    std::string curString = tempString.str().substr(0, tempString.tellp());

    if (it.isTerm())
    {
        /* when string terminates at this character write the string */
        std::cout << curString << std::endl;

        n_strings++;
        n_chars += str_pos + 2;
    }

    /* execute recursively for all characters in current state */
    if (!it.isRoot())
    {
        iterator next = it.localBegin();
        print_strings(next, str_pos + 1);
    }

    /* go right */
    if (!it.isEnd(currentLetter + 1))
    {
        iterator right(it);
        ++right += 1;
        print_strings(right, str_pos);
    }
}

/*
** Check if the automaton is correct
** (test all the strings from a lexicon).
*/
void Automaton<true>::test_automat()
{
    n_strings = 0;
    n_chars = 0;

    while (read_string(temp_str))
        if (!check_string(temp_str))
            printf("String %s not found!\n", temp_str);
}

/*
** Check if the given string exists in the automaton.
*/
bool Automaton<true>::check_string(unsigned char *str)
{
    unsigned char *searchWord = str;
    unsigned char currentLetter;
    bool found = false;

    Automaton<true>::AutomatonLetterIterator it = this->letterBegin();

    while (!it.isEnd(*searchWord))
    {
        currentLetter = *it;

        if (currentLetter == *searchWord)
        {
            ++searchWord;

            if ((unsigned) *searchWord == 0)
            {
                found = it.isTerm();
                break;
            }

            it = it.localBegin();
        }
        else
        {
            ++it += (currentLetter < *searchWord ? 1 : 0);
        }
    }

    return found;
}

/*
** Create the automaton.
*/
void Automaton<true>::make_automat()
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

        if (print_statistics)
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

/*
** Seek an identical state in the automaton
** or create a new state. Return its index.
*/
unsigned Automaton<true>::make_state(transition *state, unsigned state_len)
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

/*
** Make a tree-shaped state from a larval state.
*/
void Automaton<true>::make_tree(transition *state, int left, int right, unsigned pos, int full)
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

/*
** Hash function for states.
*/
unsigned Automaton<true>::hash_state(transition *state, unsigned state_len)
{
    unsigned r = 0;
    int i;

    for (i = state_len - 1; i >= 0; i--)
        r += state[i].all_fields;

    return ((r * 324027) >> 13) % HT_SIZE;
}

/*
** Read next string from input file and return its length.
*/
int Automaton<true>::read_string(unsigned char *str)
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

/*
** Set buffer for io stream.
*/
void Automaton<true>::set_io_buffer(FILE *file, size_t size)
{
    if (setvbuf(file, NULL, _IOFBF, size) != 0)
        error("Cannot set input buffer.");
}

/*
** Print an error message and terminate the program.
*/
void Automaton<true>::error(char *message)
{
    fprintf(stderr, "%s\n", message);
    exit(EXIT_FAILURE);
}

/*
** Prepare data structures before program run.
*/
void Automaton<true>::prepare_tables()
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

/*
** Show some statistics, including execution time.
*/
void Automaton<true>::show_stat(double exec_time)
{
    printf("%lu strings\t%lu characters\n", n_strings, n_chars);

    if (print_statistics)
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

bool Automaton<true>::exists(unsigned char *keyword)
{
    n_strings = 0;
    n_chars = 0;

    return check_string(keyword);
}

void Automaton<true>::rewind()
{
    /* create a pseudo state pointing to the start state */
    memcpy(&start_state, &automat[0], sizeof automat[0]);
    automat[0].b.dest = automat[0].all_fields;
    currentStatePos = automat[0].b.dest;
    if (start_state < aut_size)
        return;
}

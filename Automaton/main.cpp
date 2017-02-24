#include "Automaton.h"

int main(int argc, char **argv)
{
    clock_t t1, t2;

    if (argc == 4)
	{
        if (!strcmp(argv[1], "-t"))
        { /* test an existing automaton */

			Automaton<true> *a = new Automaton<true>(false);
            a->test_automat(argv[2]);
			cout << "\n";
			cout << " -------\n";
			cout << "Iterator:\n";
			cout << " -------\n";


			std::string keyword;

			while (true)
			{
				cin >> keyword;

                if (keyword == ":q")
                {
                    break;
                }
                else
                {
                    if (a->exists((unsigned char*)const_cast<char*>(keyword.c_str())))
                    {
                        cout << "FOUND: " << keyword << "\n";
                    }
                    else
                    {
                        cout << "NOT FOUND: " << keyword << "\n";
                    }
                }
			}
        }
        else if (!strcmp(argv[1], "-l"))
        { /* create a dictionary, basing on an existing automaton */
            Automaton<true> *a = new Automaton<true>(false);
            a->list_automat(argv[3], argv[2]);
            cout << "Dictionary " << argv[3] << " created!\n";
        }
        else if (!strcmp(argv[1], "-m"))
        { /* create an automaton, basing on an existing dictionary */
            Automaton<true> *a = new Automaton<true>(false);
            a->make_automat(argv[3], argv[2]);
            cout << "Automaton " << argv[2] << " created!\n";
        }
		else
		{
            cout << "Invalid parameter\n";
		}
        t2 = clock();
    }
	else
	{
        cout << "Wrong parameters number\n";

	}

    cout << "Press any key to continue..." << endl;
    fgetc(stdin);
    return EXIT_SUCCESS;
}
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
			std::cout << std::endl;
			std::cout << " -------" << std::endl;
			std::cout << "Iterator:" << std::endl;
			std::cout << " -------" << std::endl;


			std::string keyword;

			while (true)
			{
				std::cin >> keyword;

                if (keyword == ":q")
                {
                    break;
                }
                else
                {
                    if (a->exists((unsigned char*)const_cast<char*>(keyword.c_str())))
                    {
                        std::cout << "FOUND: " << keyword << std::endl;
                    }
                    else
                    {
                        std::cout << "NOT FOUND: " << keyword << std::endl;
                    }
                }
			}
        }
        else if (!strcmp(argv[1], "-l"))
        { /* create a dictionary, basing on an existing automaton */
            Automaton<true> *a = new Automaton<true>(false);
            a->list_automat(argv[3], argv[2]);
            printf("Dictionary %s created!\n", argv[3]);
        }
        else if (!strcmp(argv[1], "-m"))
        { /* create an automaton, basing on an existing dictionary */
            Automaton<true> *a = new Automaton<true>(false);
            a->make_automat(argv[3], argv[2]);
            printf("Automaton %s created!\n", argv[2]);
        }
		else
		{
			printf("Invalid parameter\n");
		}
        t2 = clock();
    }
	else
	{

		printf("Wrong parameters number\n");

	}

    printf("Press any key to continue...\n");
    fgetc(stdin);
    return EXIT_SUCCESS;
}
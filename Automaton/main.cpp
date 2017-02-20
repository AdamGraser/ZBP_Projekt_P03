#include "Automaton.h"

int main(int argc, char **argv)
{
    clock_t t1, t2;

    if (argc == 4)
	{
        if (!strcmp(argv[1], "-m"))
        { /* make a new automaton */

			Automaton<false> *a = new Automaton<false>(argv[3], argv[2], false);
			unsigned char *text1 = (unsigned char *)"Radom";
			bool test1 = a->exists(text1);

			if (test1)
			{
				std::cout << "yep1" << std::endl;
			}
			else
			{
				std::cout << "Nope1" << std::endl;
			}

			unsigned char *text2 = (unsigned char *)"zbita";
			bool test2 = a->exists(text2);

			if (test2)
			{
				std::cout << "yep2" << std::endl;
			}
			else
			{
				std::cout << "Nope2" << std::endl;
			}

        }
		else
		{
			printf("blad1\n");
		}
        t2 = clock();
    }
	else
	{

		printf("blad2");

	}

    fgetc(stdin);
    return EXIT_SUCCESS;
}
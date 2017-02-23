#include "Automaton.h"

int main(int argc, char **argv)
{
    clock_t t1, t2;

    if (argc == 4)
	{
        if (strcmp(argv[1], "-m"))
        { /* make a new automaton */

			Automaton<true> *a = new Automaton<true>(argv[3], argv[2], false);
			//unsigned char *text1 = (unsigned char *)"Radom";
			//bool test1 = a->exists(text1);

			//if (test1)
			//{
			//	std::cout << "yep1" << std::endl;
			//}
			//else
			//{
			//	std::cout << "Nope1" << std::endl;
			//}

			//unsigned char *text2 = (unsigned char *)"zbita";
			//bool test2 = a->exists(text2);

			//if (test2)
			//{
			//	std::cout << "yep2" << std::endl;
			//}
			//else
			//{
			//	std::cout << "Nope2" << std::endl;
			//}

			//std::cout << " -----------" << std::endl;
			//std::cout << "List String:" << std::endl;
			//std::cout << " -----------" << std::endl;
			//a->print_strings(26, 0);
			std::cout << std::endl;
			std::cout << " -------" << std::endl;
			std::cout << "Iterator:" << std::endl;
			std::cout << " -------" << std::endl;


			std::string keyword;

			while (true)
			{
				std::cin >> keyword;

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
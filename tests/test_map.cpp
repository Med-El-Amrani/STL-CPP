/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   main_map.cpp                                       :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: mbadaoui <marvin@42.fr>                    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/02/19 14:41:11 by mbadaoui          #+#    #+#             */
/*   Updated: 2023/02/19 14:41:12 by mbadaoui         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "test_map.hpp"

int main()
{

    std::cout << RED << "________________________________________________________________________________________________________" << std::endl;
    std::cout << RED << "**** The test is taking so much time to test the all cases and the time complexity of each method ****" << std::endl;
    std::cout << RED << "--------------------------------------------------------------------------------------------------------" << RESET << std::endl;
    signal(SIGALRM, alarm_handler);

    std::cout << YELLOW << "Testing Iterators;" << RESET << std::endl;
    TEST_CASE(iterator_tests);
    TEST_CASE(const_iterator_tests);
    TEST_CASE(reverse_iterator_tests);
    std::cout << std::endl;

    std::cout << YELLOW << "Testing Constructors;" << RESET << std::endl;
    TEST_CASE(testConstructors);
    std::cout << std::endl;

    std::cout << YELLOW << "Testing Iterator Methods;" << RESET << std::endl;
    TEST_CASE(testIterators);
    std::cout << std::endl;

    std::cout << YELLOW << "Testing Capacity Methods;" << RESET << std::endl;
    TEST_CASE(testCapacityMethods)
    std::cout << std::endl;

    std::cout << YELLOW << "Testing Access Element Methods; " << RESET << std::endl;
    TEST_CASE(testElementAccess);
    std::cout << std::endl;

    std::cout << YELLOW << "Testing Modifiers Methods;" << RESET << std::endl;
    TEST_CASE(testModifiers)
    std::cout << std::endl;

    std::cout << YELLOW << "Testing Observers Methods;" << RESET << std::endl;
    TEST_CASE(testObservers)
    std::cout << std::endl;

    std::cout << YELLOW << "Testing Operations Methods;" << RESET << std::endl;
    TEST_CASE(testOperations)
    std::cout << std::endl;

    std::cout << YELLOW << "Testing Allocator Methods;" << RESET << std::endl;
    TEST_CASE(testAllocatorMethodes)
    std::cout << std::endl;

    std::cout << YELLOW << "Testing Retional Operators; " << RESET << std::endl;
    TEST_CASE(testRetionalOperators);
    std::cout << std::endl;

    std::cout << YELLOW << "Testing Non-Member Swap  ; " << RESET << std::endl;
    TEST_CASE(testNonMemberSwap);
    std::cout << std::endl;
    return 0;
}

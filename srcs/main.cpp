/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   main.cpp                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: victorviterbo <victorviterbo@student.42    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/09/21 10:44:51 by victorviter       #+#    #+#             */
/*   Updated: 2025/09/29 16:22:43 by victorviter      ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "WebServ.hpp"

/*int	main(void)
{
	std::string	raw("POST /submit HTTP/1.1\r\nHost: localhost:8080\r\nContent-Length: 16\r\n\r\nname=Alice&age=5");
	Request	test(raw);

	std::cout << test << std::endl;
}
*/


int main(int argc, char *argv[])
{
	WebServ	myServer;

	if (argc != 2)
	{
		std::cerr << "Please provide config file" << std::endl;
		return (1);
	}
	if (myServer.WebServInit(argv[1]) == SERV_ERROR)
		return (1);
	return (myServer.WebServRun());
}

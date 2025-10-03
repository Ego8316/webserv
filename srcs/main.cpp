/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   main.cpp                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: victorviterbo <victorviterbo@student.42    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/09/21 10:44:51 by victorviter       #+#    #+#             */
/*   Updated: 2025/10/03 14:23:29 by victorviter      ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "WebServ.hpp"

int main(int argc, char *argv[])
{
	if (argc != 2)
	{
		std::cerr << "Please provide config file" << std::endl;
		return (1);
	}
	WebServ	myServer(argv[1]);
	if (myServer.WebServInit() == SERV_ERROR)
		return (1);
	return (myServer.WebServRun());
}

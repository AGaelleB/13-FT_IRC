#ifndef USER_HPP

#include "../includes/Library.hpp"
#include "../includes/Client.hpp"


class User
{
	private:
		std::string	_userName;
		std::string	_nickName;

	public:
		User();
		~User();
};

User::User() { }

User::~User() { }

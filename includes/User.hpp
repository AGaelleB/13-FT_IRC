#ifndef USER_HPP
# define USER_HPP

#include "./Library.hpp"
#include "./Utils.hpp"

class User {
	private:
		int			_index;
		std::string	_userName;
		std::string	_nickName;

	public:
		User();
		User(int index, const std::string& username, const std::string& nickname);
		~User();

		int			getIndex() const;
		std::string	getUsername() const;
		std::string	getNickname() const;

		void		setIndex(int index);
		void		setUsername(const std::string& username);
		void		setNickname(const std::string& nickname);
};

#endif // USER_HPP
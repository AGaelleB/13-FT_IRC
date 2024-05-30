// User.cpp

#include "../includes/User.hpp"

User::User() : _index(0), _userName("username"), _nickName("nickname") {}

User::User(int index, const std::string& username, const std::string& nickname) : _index(index), _userName(username), _nickName(nickname) {}

User::~User() {}

int User::getIndex() const {
	return _index;
}

std::string User::getUsername() const {
	return _userName;
}

std::string User::getNickname() const {
	return (_nickName);
}

void User::setIndex(int index) {
	_index = index;
}

void User::setUsername(const std::string& username) {
	_userName = username;
}

void User::setNickname(const std::string& nickname) {
	_nickName = nickname;
}

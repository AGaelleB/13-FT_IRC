#ifndef TOPIC_HPP
# define TOPIC_HPP

# include "./Utils.hpp"
# include "./RPL.hpp"

const int MIN_TOPIC_SIZE = 1;
const int MAX_TOPIC_SIZE = 307;

class Topic {
	private:
		std::string			_topicName;

	public:
		Topic();
		~Topic();

		const std::string&	getTopicName() const;
		void				setTopicName(const std::string& topicName);
};

#endif // TOPIC_HPP

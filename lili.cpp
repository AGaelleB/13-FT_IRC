	memset(buffer, 0, sizeof(buffer)); //-> clear the buffer
	ssize_t bytes = recv(fd, buffer, sizeof(buffer) - 1 , 0); //-> receive the data
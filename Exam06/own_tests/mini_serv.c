#include <string.h>
#include <unistd.h>
#include <stdlib.h>
#include <stdio.h>
#include <netinet/ip.h>

int		count = 0, max_fd = 0;
int		ids[FD_SETSIZE];
char	*msgs[FD_SETSIZE];

fd_set	rfds, wfds, afds;
char	buf_read[1001], buf_write[42];



// COPY

int extract_message(char **buf, char **msg)
{
	char	*newbuf;
	int	i;

	*msg = 0;
	if (*buf == 0)
		return (0);
	i = 0;
	while ((*buf)[i])
	{
		if ((*buf)[i] == '\n')
		{
			newbuf = calloc(1, sizeof(*newbuf) * (strlen(*buf + i + 1) + 1));
			if (newbuf == 0)
				return (-1);
			strcpy(newbuf, *buf + i + 1);
			*msg = *buf;
			(*msg)[i + 1] = 0;
			*buf = newbuf;
			return (1);
		}
		i++;
	}
	return (0);
}

char *str_join(char *buf, char *add)
{
	char	*newbuf;
	int		len;

	if (buf == 0)
		len = 0;
	else
		len = strlen(buf);
	newbuf = malloc(sizeof(*newbuf) * (len + strlen(add) + 1));
	if (newbuf == 0)
		return (0);
	newbuf[0] = 0;
	if (buf != 0)
		strcat(newbuf, buf);
	free(buf);
	strcat(newbuf, add);
	return (newbuf);
}

// COPY END



void fatal_error( void ) {

	write(2, "Fatal error\n", 12);
	exit(1);
}

void notify(int author, char *msg) {

	for (int fd = 0; fd < max_fd; fd++) {
		if (FD_ISSET(fd, &wfds) && fd != author)
			send(fd, msg, strlen(msg), 0);
	}
}

void send_msg(int client) {

	char *msg;

	while(extract_message(&(msgs[client]), &msg)) {
		sprintf(buf_write, "client %d: ", ids[client]);
		notify(client, buf_write);
		notify(client, msg);
		free(msg);
	}
}

void disconnect_client(int client) {

	sprintf(buf_write, "server: client %d just left\n", ids[client]);
	notify(client, buf_write);
	free(msgs[client]);
	FD_CLR(client, &afds);

	if (client == max_fd) {
		for (int fd = max_fd - 1; fd > 2; fd--) {
			if (FD_ISSET(fd, &afds)) {
				max_fd = fd;
				break ;
			}
		}
	}

	close(client);
}

void register_client(int client) {

	max_fd = client > max_fd ? client : max_fd;
	ids[client] = count++;
	msgs[client] = NULL;
	FD_SET(client, &afds);
	sprintf(buf_write, "server: client %d just arrived\n", ids[client]);
	notify(client, buf_write);
}

int create_socket( void ) {

	max_fd = socket(AF_INET, SOCK_STREAM, 0);
	if (max_fd < 0)
		fatal_error();
	FD_SET(max_fd, &afds);
	return max_fd;
}

int main(int argc, char **argv) {

	if (argc < 2) {
		write(2, "Wrong number of arguments\n", 26);
		exit(1);
	}

	FD_ZERO(&afds);
	int sockfd = create_socket();

	struct sockaddr_in servaddr; 
	bzero(&servaddr, sizeof(servaddr)); 

	servaddr.sin_family = AF_INET;
	servaddr.sin_addr.s_addr = htonl(2130706433);
	servaddr.sin_port = htons(atoi(argv[1]));

	if ((bind(sockfd, (const struct sockaddr *)&servaddr, sizeof(servaddr))) != 0)
		fatal_error();
	if (listen(sockfd, SOMAXCONN))
		fatal_error();

	while(1) {
		rfds = wfds = afds;

		if (select(max_fd + 1, &rfds, &wfds, NULL, NULL) < 0)
			fatal_error();

		for (int fd = 0; fd <= max_fd; fd++) {
			if (!FD_ISSET(fd, &rfds))
				continue ;

			if (fd == sockfd) {
				int client_fd = accept(sockfd, NULL, NULL);
				if (client_fd > 0) {
					register_client(client_fd);
					break ;
				}
			} else {
				int bytes = recv(fd, buf_read, 1001, 0);
				if (bytes <= 0) {
					disconnect_client(fd);
					break ;
				}
				buf_read[bytes] = '\0';
				msgs[fd] = str_join(msgs[fd], buf_read);
				send_msg(fd);
			}
		}
	}
	return 0;
}

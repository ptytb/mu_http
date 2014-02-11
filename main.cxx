/** Автор: Пронин И.С. <luminex@bk.ru>
 *  Группа И-5-4
 *  2011 г.
 */

#include "main.hpp"
#include "ustring.hpp"
#include "Log.hpp"
#include "Client.hpp"

#include <cstdlib>
#include <cstring>
#include <errno.h>
#include <pthread.h>
#include <signal.h>
#include <sys/inotify.h>
#include <unistd.h>

#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <netdb.h>
#include <fcntl.h>

#include <iostream>
#include <fstream>
#include <sstream>
#include <iomanip>
#include <locale>
#include <map>

#include "ustring.hpp"

#define MAX(a,b) ((a)>(b)?(a):(b))

Config config;
DirReader dirReader;

sig_atomic_t signalCaught = 0;

void sigint_handler(int arg)
{
	signalCaught = 1;
}

int init_listener();
void update_dir();
void accept_client(int sfd);

int main(int argc, char **argv)
{
	setlocale(LC_ALL, SERVER_LOCALE);

	signal(SIGINT, sigint_handler);

	/* Блокирую сигнал SIGPIPE чтобы сервер не падал из-за клиентов,
	 * "кидающих" сокеты */

	sigset_t sigmask, origSigmask;
	sigemptyset(&sigmask);
	sigaddset(&sigmask, SIGPIPE);
	sigprocmask(SIG_BLOCK, &sigmask, &origSigmask);

	log(SERVER_INFO);

	/* Читаю конфигурационный файл */

	int cr = config.readConfig(SERVER_CONFIG_FILE, SERVER_LOCALE);

	switch (cr)
	{
		case CONFIG_NOT_FOUND:
			log("Файл конфигурации не найден.");
			break;
		case CONFIG_SYNTAX_ERROR:
			log("Ошибка синтаксиса в файле конфигурации.");
			exit(EXIT_FAILURE);
			break;
		case CONFIG_LOADED:
			log("Файл конфигурации загружен.");
			std::map<std::string, std::string>::iterator i;
			for (i = config.begin(); i != config.end(); i++) {
				log(i->first + "=" + i->second);
			}
			break;
	}

	/* Кэширую директории */

	log("Кэширование директорий");
	dirReader.cache(config["server.root"]);

	/* Создаю сокет и пытаюсь слушать */

	int sfd = init_listener();

	/* Главный цикл */

	for (; !signalCaught;)
	{
		int r, nfds = 0;
		fd_set rd, er;
	
		FD_ZERO(&rd);
		FD_ZERO(&er);
		FD_SET(sfd, &rd);
		FD_SET(dirReader.fd, &rd);
		FD_SET(sfd, &er);
		nfds = MAX(sfd, dirReader.fd) + 1;

		r = select(nfds, &rd, NULL, &er, NULL);
		
		if (r == -1)
		{
			if (errno == EINTR)
			{
				continue;
			}
			else
			{
				break;
			}
		}

		if (FD_ISSET(dirReader.fd, &rd))
		{
			update_dir();
		}

		if (FD_ISSET(sfd, &rd))
		{
			accept_client(sfd);
		}
	}

	log("Прерывание...");
	return 0;
}

int init_listener()
{
	addrinfo hints;
	addrinfo *result, *rp;
	int sfd;

	memset(&hints, 0, sizeof (addrinfo));
	hints.ai_family = AF_UNSPEC;
	hints.ai_socktype = SOCK_STREAM;
	hints.ai_flags = AI_PASSIVE;

	std::string strAddr = config["server.addr"];
	std::string strPort = config["server.listen"];

	int egai;
	if ((egai = getaddrinfo(strAddr.empty() ? NULL : strAddr.c_str(),
					strPort.c_str(), &hints, &result)) != 0) {
		log(gai_strerror(egai));
		exit(EXIT_FAILURE);
	}

	for (rp = result; rp != NULL; rp = rp->ai_next)
	{
		sfd = socket(rp->ai_family, rp->ai_socktype, rp->ai_protocol);
		if (sfd == -1)
			continue;

		int reuse = 1;
		setsockopt(sfd, SOL_SOCKET, SO_REUSEADDR, &reuse, sizeof(reuse));

		if (bind(sfd, rp->ai_addr, rp->ai_addrlen) != -1) {
			int elsn = listen(sfd, SERVER_BACKLOG);
			if (elsn)
			{
				std::cerr << strerror(errno) << std::endl;
				exit(EXIT_FAILURE);
			}
			break;
		}

		close(sfd);
	}

	if (rp == NULL)
	{
		exit(EXIT_FAILURE);
	}

	freeaddrinfo(result);

	int opts = fcntl(sfd, F_GETFL);
	opts |= O_NONBLOCK;
	fcntl(sfd, F_SETFL, opts);

	std::string iface = config["server.interface"];
	if (iface != "all") {
		int esopt = setsockopt(sfd, SOL_SOCKET, SO_BINDTODEVICE,
				iface.c_str(), iface.length());
		if (esopt) {
			std::cerr << strerror(errno) << std::endl;
			exit(EXIT_FAILURE);
		}
	}

	log("Слушаю порт " + strPort + " на интерфейсах " + iface);
	return sfd;
}

void update_dir()
{
	struct inotify_event
	{
		int      wd;       /* Watch descriptor */
		uint32_t mask;     /* Mask of events */
		uint32_t cookie;   /* Unique cookie associating related
				      events (for rename(2)) */
		uint32_t len;      /* Size of name field */
		char     name[];   /* Optional null-terminated name */
	} * ie;

	char buffer[16384];
	ie = reinterpret_cast<struct inotify_event *> (buffer);

	int r = read(dirReader.fd, buffer, sizeof(buffer));

	if (r < 0)
	{
		log("read(): %s", strerror_r(errno, buffer,
					sizeof(buffer)));
		return;
	}

	if (ie->mask & IN_Q_OVERFLOW)
	{
		log("Переполнение очереди inotify");
		return;
	}

	dirReader.update(ie->wd, ie->mask);
}

void accept_client(int sfd)
{
	socklen_t alen = sizeof(sockaddr_in);
	int ptc, r;
	sockaddr_in addr;
	pthread_t clientThread;

	memset(&addr, '\0', alen);

	r = accept(sfd, (sockaddr*) &addr, &alen);

	/* Поток клиента должен освободить cinfo! */

	ClientInfo *cinfo = new ClientInfo(r, addr);

	ptc = pthread_create(&clientThread, NULL, &Client::start,
			(void*) cinfo);
	if (ptc)
	{
		log("Не удалось создать поток клиента");
		exit(EXIT_FAILURE);
	}

	pthread_detach(clientThread);
}



#ifndef __CEMAIL
#define __CEMAIL

#ifdef WIN32
#include <winsock2.h>
#include <io.h>
#else
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <netdb.h>
#include <unistd.h>

#define INVALID_SOCKET	-1
#define SOCKET_ERROR	-1
#endif

#ifndef FALSE
#define FALSE	0
#endif

#ifndef TRUE
#define TRUE	1
#endif

#ifndef	NULL
#define	NULL	0
#endif

#ifndef ERROR
#define ERROR	-1
#endif

/* Mail server status codes */
#define	MAIL_WELCOME	220
#define MAIL_OK			250
#define MAIL_GO_AHEAD	354
#define MAIL_GOODBYE	221

/* Error codes returned by send_mail */
#define E_NO_SOCKET_CONN	-1
#define E_PROTOCOL_ERROR	-2

#include "CServer.h"

class CServer;

class CEMail
{
public:
	CEMail(CServer *Server);
	~CEMail();

	void SendWelcome(string Address);
	void SendPassword(string Email, string User, string Password);

	int send_mail(const char *smtpserver, const char *from, const char *to, 
					const char *subject, const char *replyto, const char *msg);
	int connect_to_server(const char *server);
	int send_command(int n_sock, const char *prefix, const char *cmd, 
						const char *suffix, int ret_code);
	int send_mail_message(int n_sock, const char *from, const char *to, 
							const char *subject, const char *replyto, const char *msg);


	#ifdef WIN32
	int startup_sockets_lib(void);
	int cleanup_sockets_lib(void);
	#define snprintf  _snprintf
	#endif
private:
	CServer *p;
protected:
};

#endif

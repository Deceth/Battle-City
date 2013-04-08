/*
===============================================================================

    Battle City - CEmail file
    Copyright (C) 2005-2013  battlecity.org

    This file is part of Battle City.

    Battle City is free software: you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation, either version 3 of the License, or
    (at your option) any later version.

    Battle City is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with Battle City.  If not, see <http://www.gnu.org/licenses/>.
===============================================================================
*/
#include "CEMail.h"

void *CEmailPointer;

string MailServer;

void *SendWelcomeEmail(void *threadarg);
void *SendPasswordEmail(void *threadarg);
string tmpEmail;
string tmpUser;
string tmpPassword;

/***************************************************************
 * Constructor
 *
 * @param Server
 **************************************************************/
CEMail::CEMail(CServer *Server) {
	this->p = Server;
	CEmailPointer = Server;

#ifdef _DEBUG
	MailServer = "k2smtpout.secureserver.net";
#else
	MailServer = "k2smtpout.secureserver.net";
#endif
}

/***************************************************************
 * Destructor
 *
 **************************************************************/
CEMail::~CEMail() {
}

/***************************************************************
 * Function:	SendWelcome
 *
 * @param Address
 **************************************************************/
void CEMail::SendWelcome(string Address) {
	pthread_t thread;
	int rc = 0;

	// Fill the temp variables
	tmpEmail = Address;

	// Spawn a thread to send the email
    rc = pthread_create(&thread, NULL, SendWelcomeEmail, (void *)0);
    if (rc) {
		cout << "SendWelcome - ERROR: return code from pthread_create() is " << rc << endl;
    }
}

/***************************************************************
 * Function:	SendPassword
 *
 * @param Email
 * @param User
 * @param Password
 **************************************************************/
void CEMail::SendPassword(string Email, string User, string Password) {
	pthread_t thread;
	int rc = 0;

	// Fill the temp variables
	tmpEmail = Email;
	tmpPassword = Password;
	tmpUser = User;

	// Spawn a thread to send the email
    rc = pthread_create(&thread, NULL, SendPasswordEmail, (void *)0);
    if (rc) {
		cout << "SendPassword - ERROR: return code from pthread_create() is " << rc << endl;
    }
}

/***************************************************************
 * Function:	SendWelcomeEmail
 *
 * @param tr
 * @param td
 **************************************************************/
void *SendWelcomeEmail(void *threadarg) {
	string EMail = tmpEmail;
	string EmailMessage;
	CServer *p = (CServer *)CEmailPointer;

	// Create the welcome email
	EmailMessage = "Welcome to BattleCity!";
	EmailMessage += "\r\n\r\nBe sure to check out the online community at www.battlecity.net !";
	cout << "Mail::SendWelcome::" << EMail << endl;

	// Send the email
	if (p->EMail->send_mail(MailServer.c_str(), "remote@battlecity.net", EMail.c_str(), "Welcome to BattleCity!", "remote@battlecity.net", EmailMessage.c_str()) != 0) {
		cout << "Mail::Failed::Welcome::" << EMail << endl;
	}
	else {
		cout << "Mail::Succeeded::Welcome::" << EMail << endl;
	}

	// Exit the thread
	pthread_exit(NULL);
	return 0;
}

/***************************************************************
 * Function:	SendPasswordEmail
 *
 * @param tr
 * @param td
 **************************************************************/
void *SendPasswordEmail(void *threadarg) {
	string EMail = tmpEmail;
	string User = tmpUser;
	string Password = tmpPassword;
	string EmailMessage;
	CServer *p = (CServer *)CEmailPointer;

	// Create the password email
	EmailMessage = "An account information recovery request has been made for your email address.  Your account information is as follows:\r\n\r\nUser: ";
	EmailMessage += User;
	EmailMessage += "\r\nPassword: ";
	EmailMessage += Password;
	cout << "Mail::SendPassword::" << EMail << endl;

	// Send the email
	if (p->EMail->send_mail(MailServer.c_str(), "remote@battlecity.net", EMail.c_str(), "Password Recovery",  "remote@battlecity.net",  EmailMessage.c_str()) != 0) {
		cout << "Mail::Failed::Password::" << EMail << endl;
	}
	else {
		cout << "Mail::Succeeded::Password::" << EMail << endl;
	}

	// Exit the thread
	pthread_exit(NULL);
	return 0;
}

/***************************************************************
 * Function:	send_mail
 *
 * @param smtpserver
 * @param from
 * @param to
 * @param subject
 * @param replyto
 * @param msg
 **************************************************************/
int CEMail::send_mail(const char *smtpserver, const char *from, const char *to, 
					const char *subject, const char *replyto, const char *msg) {
	int n_socket;
	int n_retval = 0;

#ifdef WIN32
	startup_sockets_lib();
#endif

	/* First connect the socket to the SMTP server */
	if ((n_socket = connect_to_server(smtpserver)) == ERROR) 
		n_retval = E_NO_SOCKET_CONN;

	/* All connected. Now send the relevant commands to initiate a mail transfer */
	if (n_retval == 0 && send_command(n_socket, "MAIL From:<", from, ">\r\n", MAIL_OK) == ERROR)
		n_retval = E_PROTOCOL_ERROR;
	if (n_retval == 0 && send_command(n_socket, "RCPT To:<", to, ">\r\n", MAIL_OK) == ERROR) 
		n_retval = E_PROTOCOL_ERROR;

	/* Now send the actual message */
	if (n_retval == 0 && send_command(n_socket, "", "DATA", "\r\n", MAIL_GO_AHEAD) == ERROR) 
		n_retval = E_PROTOCOL_ERROR;
	if (n_retval == 0 && send_mail_message(n_socket, from, to, subject, replyto, msg) == ERROR) 
		n_retval = E_PROTOCOL_ERROR;

	/* Now tell the mail server that we're done */
	if (n_retval == 0 && send_command(n_socket, "", "QUIT", "\r\n", MAIL_GOODBYE) == ERROR) 
		n_retval = E_PROTOCOL_ERROR;

	/* Now close up the socket and clean up */
	shutdown(n_socket, 2);

#ifdef WIN32
	cleanup_sockets_lib();
#endif

	return n_retval;
}

/***************************************************************
 * Function:	connect_to_server
 *
 * @param server
 **************************************************************/
int CEMail::connect_to_server(const char *server) {
	struct hostent *host;
	struct in_addr	inp;
	struct protoent *proto;
	struct sockaddr_in sa;
	int n_sock;
#define SMTP_PORT	   25
#define BUFSIZE		4096
	char s_buf[BUFSIZE] = "";
	int n_ret;

	/* First resolve the hostname */
	host = gethostbyname(server);
	if (host == NULL) {
		fprintf(stderr, "Could not resolve hostname %s. Aborting...\n", server);
		return ERROR;
	}

	memcpy(&inp, host->h_addr_list[0], host->h_length);

	/* Next get the entry for TCP protocol */
	if ((proto = getprotobyname("tcp")) == NULL) {
		fprintf(stderr, "Could not get the protocol for TCP. Aborting...\n");
		return ERROR;	
	}

	/* Now create the socket structure */
#ifdef WIN32
	if ((n_sock = (int)socket(PF_INET, SOCK_STREAM, proto->p_proto)) == INVALID_SOCKET) {
#else
	if ((n_sock = socket(PF_INET, SOCK_STREAM, proto->p_proto)) == INVALID_SOCKET) {
#endif
		fprintf(stderr, "Could not create a TCP socket. Aborting...\n");
		return ERROR;
	}

	/* Now connect the socket */
	memset(&sa, 0, sizeof(sa));
	sa.sin_addr = inp;
	sa.sin_family = host->h_addrtype;
	sa.sin_port = htons(SMTP_PORT);
	if (connect(n_sock, (struct sockaddr *)&sa, sizeof(sa)) == SOCKET_ERROR) {
		fprintf(stderr, "Connection refused by host %s.", server);
		return ERROR;
	}

	/* Now read the welcome message */
	n_ret = recv(n_sock, s_buf, BUFSIZE, 0);
	
	return n_sock;
}

/***************************************************************
 * Function:	send_command
 *
 * @param tr
 * @param td
 **************************************************************/
int CEMail::send_command(int n_sock, const char *prefix, const char *cmd, 
						const char *suffix, int ret_code) {
#define BUFSIZE		4096
	char s_buf[BUFSIZE] = "";
	char s_buf2[50];
	
 	strncpy(s_buf, prefix, BUFSIZE);
	strncat(s_buf, cmd, BUFSIZE);
	strncat(s_buf, suffix, BUFSIZE);

	if (send(n_sock, s_buf, (int)strlen(s_buf), 0) == SOCKET_ERROR) {
		fprintf(stderr, "Could not send command string %s to server.", s_buf);
		return ERROR;
	}

	/* Now read the response. */
	recv(n_sock, s_buf, BUFSIZE, 0);

	/* Now check if the ret_code is in the buf */
	sprintf(s_buf2, "%d", ret_code);

	if (strstr(s_buf, s_buf2) != NULL)
		return TRUE;
	else
		return ERROR;
}

/***************************************************************
 * Function:	send_mail_message
 *
 * @param tr
 * @param td
 **************************************************************/
int CEMail::send_mail_message(int n_sock, const char *from, const char *to,
							const char *subject, const char *replyto, const char *msg) {
#define BUFSIZE		4096
#define BUFSIZE2	100
#define MSG_TERM	"\r\n.\r\n"
#define MAIL_AGENT	"Mayukh's SMTP code (http://www.mayukhbose.com/freebies/c-code.php)"
	char s_buf[BUFSIZE];
	char s_buf2[BUFSIZE2];
	time_t t_now = time(NULL);
	int n_ret;

	/* First prepare the envelope */
	strftime(s_buf2, BUFSIZE2, "%a, %d %b %Y  %H:%M:%S +0000", gmtime(&t_now));

	snprintf(s_buf, BUFSIZE, "Date: %s\r\nFrom: %s\r\nTo: %s\r\nSubject: %s\r\nX-Mailer: %s\r\nReply-To: %s\r\n\r\n",
				s_buf2, from, to, subject, MAIL_AGENT, replyto); 

	/* Send the envelope */
	if (send(n_sock, s_buf, (int)strlen(s_buf), 0) == SOCKET_ERROR) {
		fprintf(stderr, "Could not send message header: %s", s_buf);
		return ERROR;
	}

	/* Now send the message */
	if (send(n_sock, msg, (int)strlen(msg), 0) == SOCKET_ERROR) {
		fprintf(stderr, "Could not send the message %s\n", msg);
		return ERROR;
	}

	/* Now send the terminator*/
	if (send(n_sock, MSG_TERM, (int)strlen(MSG_TERM), 0) == SOCKET_ERROR) {
		fprintf(stderr, "Could not send the message terminator.\n", msg);
		return ERROR;
	}

	/* Read and discard the returned message ID */
	n_ret = recv(n_sock, s_buf, BUFSIZE, 0);

	return TRUE;
}

#ifdef WIN32

/***************************************************************
 * Function:	startup_sockets_lib
 *
 **************************************************************/
int CEMail::startup_sockets_lib(void) {
	WORD wVersionRequested;
	WSADATA wsaData;
	int n_err;
 
	wVersionRequested = MAKEWORD( 1, 1 );
 
	n_err = WSAStartup( wVersionRequested, &wsaData );
	if (n_err != 0) {
		fprintf(stderr, "Could not find winsock.dll.Aborting...\n");
		return FALSE;
	}
  
	if (LOBYTE( wsaData.wVersion ) != 1 ||
			HIBYTE( wsaData.wVersion ) != 1) {
		fprintf(stderr, "Could not find w1.1 version of winsock.dll.Aborting...\n");
		WSACleanup();
		return FALSE; 
	}
 
	return TRUE; 
}

/***************************************************************
 * Function:	cleanup_sockets_lib
 *
 **************************************************************/
int CEMail::cleanup_sockets_lib(void) {
	WSACleanup();
	return TRUE;
}

#endif

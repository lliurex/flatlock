/*
This piece of code bind mounts flatpak runtime in the user folder
effectively blocking any attempts to manage sofware for non privileged users
*/
#include <dirent.h>
#include <errno.h>
#include <stdarg.h>
#include <stdio.h>
#include <ctype.h>
#include <stdlib.h>
#include <string.h>
#include <sys/mount.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>

int main () {
    struct dirent *pDirent;
    struct stat stats;
    DIR *pDir;
    DIR *flatpakUserDir;
	char * runtimesys="/var/lib/flatpak/runtime";
	char * rundir="/.local/share/flatpak/app";
    pDir = opendir ("/home");
    if (pDir == NULL) {
        printf ("Cannot open directory\n");
        return 1;
    }
    while ((pDirent = readdir(pDir)) != NULL) {
		if (isalpha(pDirent->d_name[0])==0)
		{
			continue;
		}
		char * userdir="";
		userdir=pDirent->d_name;
		size_t len=strlen(userdir);
		char * newdir=(char *)malloc(len + strlen("/home/") + strlen(rundir) +1);
		strcpy(newdir,"/home/");
		strcat(newdir,userdir);
		strcat(userdir,newdir);
		stat(newdir, &stats);

		strcat(newdir,rundir);
    	flatpakUserDir = opendir (newdir);
    	if (flatpakUserDir == NULL) {
        	continue;
    	}
		printf("Mounting '%s'\n%m\n", newdir); 
		mount(runtimesys,newdir,"",MS_BIND,"");
    }
    closedir (pDir);
    return 0;
}

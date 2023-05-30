/*
This piece of code bind mounts flatpak runtime in the user folder
effectively blocking any attempts to manage sofware for non privileged users
*/
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <dirent.h>
#include <sys/mount.h>

int main () {
    struct dirent *pDirent;
    DIR *pDir;
    pDir = opendir ("/home");
	char * runtimesys="/var/lib/flatpak/runtime";
	char * rundir="/.local/share/flatpak/app";
    if (pDir == NULL) {
        printf ("Cannot open directory\n");
        return 1;
    }
    while ((pDirent = readdir(pDir)) != NULL) {
		if ((strcmp(pDirent->d_name,".")==0) ||(strcmp(pDirent->d_name,"..")==0))
		{
			continue;
		}
		char * tmp;
		tmp=pDirent->d_name;
		size_t len=strlen(tmp);
		char * newdir=(char *)malloc(len + strlen("/home/") + strlen(rundir) +1);
		strcpy(newdir,"/home/");
		strcat(newdir,tmp);
		strcat(newdir,rundir);
		mount(runtimesys,newdir,"",MS_BIND,"");
    }
    closedir (pDir);
    return 0;
}

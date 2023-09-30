/*
This piece of code bind mounts flatpak runtime in the user folder
effectively blocking any attempts to manage sofware for non privileged users
*/
#include <dirent.h>
#include <errno.h>
#include <libgen.h>
#include <limits.h>
#include <stdarg.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/mount.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>

static int mkpath(const char *path, mode_t mode, char *userdir,int uid,int gid) {
	char opath[PATH_MAX];
	char *p;
	size_t len;
	char user[sizeof(userdir)];

	strncpy(opath, path, sizeof(opath));
	strncpy(user, userdir, sizeof(user));
	opath[sizeof(opath) - 1] = '\0';
	len = strlen(opath);
	if (len == 0)
		return (0);
	else if (opath[len - 1] == '/')
		opath[len - 1] = '\0';
	for(p = opath; *p; p++)
		if (*p == '/') 
		{
			*p = '\0';
			if (access(opath, F_OK))
				mkdir(opath, mode);
				if (strlen(opath)>strlen(userdir))
					chown(opath, uid, gid);
			*p = '/';
		}
	if (access(opath, F_OK)) 
		mkdir(opath, mode);
	return(0);
}


int main () {
    struct dirent *pDirent;
    struct stat stats;
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
		char * userdir;
		userdir=pDirent->d_name;
		size_t len=strlen(userdir);
		char * newdir=(char *)malloc(len + strlen("/home/") + strlen(rundir) +1);
		strcpy(newdir,"/home/");
		strcat(newdir,userdir);
		strcat(userdir,newdir);
		stat(newdir, &stats);

		strcat(newdir,rundir);
		if(mkpath(newdir, 0777,userdir,stats.st_uid,stats.st_gid) && errno != EEXIST)
			printf("Error creating '%s'\n%m\n", newdir); 
		printf("Mounting '%s'\n%m\n", newdir); 
		mount(runtimesys,newdir,"",MS_BIND,"");
    }
    closedir (pDir);
    return 0;
}

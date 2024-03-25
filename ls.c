#define _DEFAULT_SOURCE

#include <stdlib.h>
#include <stdio.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <sys/sysmacros.h>
#include <dirent.h>
#include <stdbool.h>
#include <string.h>
#include <errno.h>
#include <pwd.h>
#include <grp.h>
#include <time.h>

char get_file_type(const struct stat sb);
void print_verbose_for_file(const char *filename);
char* get_permissions(const struct stat sb);
char* get_owner_name(const struct stat sb);
char* get_group_name(const struct stat sb);

int main(int argc, char** argv) {
    bool l_opt = false;
    bool is_a_dir = true;

    if(argc > 3) {
        fprintf(stderr, "Usage: %s [-l] [filename]");
        return EXIT_FAILURE;
    }

    char *filename = NULL;
    DIR *dir = opendir("./");
    for(int i = 1; i < argc; ++i) {
        char* arg = argv[i];
        if(strcmp(arg, "-l") == 0) l_opt = true;
        else {
            DIR* _dir = opendir(arg);
            if(errno == ENOTDIR) {
                is_a_dir = false;
                fopen(arg, "r"); // "r" utilisé pour causer un errno = ENOENT dans le cas où le fichier n'existe pas.
                if(errno == ENOENT) {
                    fprintf(stderr, "Error, %s does not exists.", arg);
                    return EXIT_FAILURE;
                } else if(errno != 0){
                    perror("fopen");
                    return EXIT_FAILURE;
                } else {
                    filename = arg;
                }
            } else {
                closedir(dir);
                dir = _dir;
            }
        }
    }
    if(dir == NULL) {
        perror("Unable to open the directory.");
        return EXIT_FAILURE;
    }

    if(is_a_dir) {
        if(l_opt) {
            print_verbose_for_file(".");
            print_verbose_for_file("..");
        } else {
            printf(".\t..\t");
        }
        struct dirent* entry;
        while((entry = readdir(dir)) != NULL) {
            char* fname = entry->d_name;
            if(l_opt) {
                print_verbose_for_file(fname);
            } else {
                printf("%s\t", fname);
            }
        }
        closedir(dir);
    } else {
        if(l_opt) {
            print_verbose_for_file(filename);
        } else {
            printf("%s\n", filename);
        }
    }

    return EXIT_SUCCESS;
}

/**
 * Imprime dans `stdout` le contenu verbeux des informations du fichier `filename`
 * @param filename le nom du fichier.
 */
void print_verbose_for_file(const char *filename) {
    struct stat sb;
    if(lstat(filename, &sb) == -1) {
        perror("lstat");
        exit(EXIT_FAILURE);
    }

    char* date = ctime(&sb.st_mtime);
    date[strcspn(date, "\n")] = '\0'; // supprime le \n de fin du résultat de ctime


    printf("%c%s\t%ju\t%s\t%s\t%jd\t%s\t%s\n",
           get_file_type(sb), // d, s, l, ...
           get_permissions(sb), // les permissions du fichier.
           sb.st_nlink, // le nombre de liens dur du fichier.
           get_owner_name(sb), // le nom de l'utilisateur propriétaire.
           get_group_name(sb), // le nom du groupe propriétaire.
           sb.st_size, // la taille du fichier.
           date, // la date de dernière modification du fichier.
           filename // le nom du fichier.
           );
}

/**
 * @param sb `struct stat` d'un fichier.
 * @return le nom de l'utilisateur propriétaire du fichier correspondant au `struct stat` donné.
 */
char* get_owner_name(const struct stat sb) {
    struct passwd *pwd = getpwuid(sb.st_uid);
    return pwd != 0 ? pwd->pw_name : "???";
}

/**
 * @param sb `struct stat` d'un fichier.
 * @return le nom du groupe propriétaire du fichier correspondant au `struct stat` donné.
 */
char* get_group_name(const struct stat sb) {
    struct group *grp = getgrgid(sb.st_gid);
    return grp != 0 ? grp->gr_name : "???";
}

/**
 * @param sb `struct stat` d'un fichier.
 * @return la chaine de caractère correpondantes aux permissions du fichier correspondant au `struct stat` donné.
 */
char* get_permissions(const struct stat sb) {
    static char perms[11];
    __mode_t perm_oct = sb.st_mode;

    // Owner
    perms[0] = (perm_oct & S_IRUSR) ? 'r' : '-';
    perms[1] = (perm_oct & S_IWUSR) ? 'w' : '-';
    perms[2] = (perm_oct & S_IXUSR) ? 'x' : '-';

    // Group Owner
    perms[3] = (perm_oct & S_IRGRP) ? 'r' : '-';
    perms[4] = (perm_oct & S_IWGRP) ? 'w' : '-';
    perms[5] = (perm_oct & S_IXGRP) ? 'x' : '-';

    // Others
    perms[6] = (perm_oct & S_IROTH) ? 'r' : '-';
    perms[7] = (perm_oct & S_IWOTH) ? 'w' : '-';
    perms[8] = (perm_oct & S_IXOTH) ? 'x' : '-';

    perms[9] = '\0';

    if (perm_oct & S_ISUID) { // set user id
        perms[2] = (perms[2] == 'x') ? 's' : 'S';
    }
    if (perm_oct & S_ISGID) { // set group id
        perms[5] = (perms[5] == 'x') ? 's' : 'S';
    }
    if (perm_oct & S_ISVTX) { // Sticky bit
        perms[8] = (perms[8] == 'x') ? 't' : 'T';
    }

    return perms;
}


/**
 * @param sb `struct stat` d'un fichier.
 * @return le type du fichier correspondant au `struct stat` donné.
 */
char get_file_type(const struct stat sb) {
    switch(sb.st_mode & S_IFMT) {
        case S_IFBLK: return 'b';
        case S_IFCHR: return 'c';
        case S_IFDIR: return 'd';
        case S_IFIFO: return 'P';
        case S_IFLNK: return 'l';
        case S_IFREG: return '-';
        case S_IFSOCK: return 's';
        default: return '?';
    }
}
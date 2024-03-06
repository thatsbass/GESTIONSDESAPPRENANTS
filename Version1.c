#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#ifdef _WIN32
#include <conio.h>
#else
#include <termios.h>
#endif
#include <time.h>

#define MAX_LENGTH 100




typedef struct {
    char username[MAX_LENGTH];
    char password[MAX_LENGTH];
} User;




    void MenuAdmin() {
    printf("\n\t\t###################################################\n");
    printf("\t\t#               Espace Administrateur             #\n");
    printf("\t\t###################################################\n");
    printf("\t\t#                                                 #\n");
    printf("\t\t#  1. GESTION DES ÉTUDIANTS                       #\n");
    printf("\t\t#  3. MARQUER LES PRÉSENCES                       #\n");
    printf("\t\t#  5. Quitter                                     #\n");
    printf("\t\t#                                                 #\n");
    printf("\t\t###################################################\n\n");
}
   void MenuEtudiant() {
    printf("\n\t\t###################################################\n");
    printf("\t\t#               Espace Etudiant                   #\n");
    printf("\t\t###################################################\n");
    printf("\t\t#                                                 #\n");
    printf("\t\t#  1. MARQUER LES PRÉSENCES                       #\n");
    printf("\t\t#  2. NOMBRE DE MINUTES D'ABSENCE                 #\n");   
    printf("\t\t#  4. Quitter                                     #\n");
    printf("\t\t#                                                 #\n");
    printf("\t\t###################################################\n\n");
}

int authenticate(const char *filename, const char *username, const char *password, const char *role_username, const char *role_password) {
    FILE *file = fopen(filename, "r");
    if (file == NULL) {
        printf("Impossible d'ouvrir le fichier\n");
        return 0;
    }

    char line[MAX_LENGTH];
    User user;
    while (fgets(line, sizeof(line), file) != NULL) {
        sscanf(line, "%s %s", user.username, user.password);
        if (strcmp(username, user.username) == 0 && strcmp(password, user.password) == 0) {
            fclose(file);
            if (strcmp(username, role_username) == 0 && strcmp(password, role_password) == 0) {
                return 1; 
            } else {
                return 2; 
            }
        }
    }

    fclose(file);
    return 0; // Si Authentification échouée
}

void writeFileConnection(const char *filename, const char *username, const char *password, const char *mode) {
    FILE *file = fopen(filename, mode);
    if (file == NULL) {
        printf("Impossible d'ouvrir le fichier\n");
        return;
    }
    fprintf(file, "%s %s\n", username, password);
    fclose(file);
}

#ifdef _WIN32
char getch() {
    return _getch();
}
#else
char getch() {
    char buf = 0;
    struct termios old = {0};
    fflush(stdout);
    if (tcgetattr(0, &old) < 0)
        perror("tcsetattr()");
    old.c_lflag &= ~ICANON;
    old.c_lflag &= ~ECHO;
    old.c_cc[VMIN] = 1;
    old.c_cc[VTIME] = 0;
    if (tcsetattr(0, TCSANOW, &old) < 0)
        perror("tcsetattr ICANON");
    if (read(0, &buf, 1) < 0)
        perror("read()");
    old.c_lflag |= ICANON;
    old.c_lflag |= ECHO;
    if (tcsetattr(0, TCSADRAIN, &old) < 0)
        perror("tcsetattr ~ICANON");
    return buf;
}
#endif

void getHiddenInput(char *password, int maxLength) {
    int i = 0;
    char ch;
    while (1) {
        ch = getch();
        if (ch == '\n' || ch == '\r') {
            password[i] = '\0';
            break;
        } else if (ch == 127 && i > 0) {  
            printf("\b \b");
            i--;
        } else if (isprint(ch) && i < maxLength - 1) {
            password[i++] = ch;
            printf("*");
        }
    }
}

int main() {
    const char *filename = "login.txt";
    const char *admin_username = "admin";
    const char *admin_password = "Passer1";
    const char *student_username = "etudiant";
    const char *student_password = "Passer2";

    
    writeFileConnection(filename, admin_username, admin_password, "w");
    writeFileConnection(filename, student_username, student_password, "a");

    char username[MAX_LENGTH], password[MAX_LENGTH];

    do {
        printf("Nom d'utilisateur : ");
        if (fgets(username, MAX_LENGTH, stdin) == NULL || strlen(username) == 0 || strspn(username, " \n") == strlen(username) || isspace((unsigned char)username[0])) {
            printf("Vous n'avez rien saisi, réessayez SVP\n");
        }
    } while (strlen(username) == 0 || strspn(username, " \n") == strlen(username) || isspace((unsigned char)username[0]));
    username[strcspn(username, "\n")] = '\0'; 

   
    do {
        printf("Mot de passe : ");
        getHiddenInput(password, MAX_LENGTH);
        printf("\n");
        if (strlen(password) == 0 || strspn(password, " \n") == strlen(password) || isspace((unsigned char)password[0])) {
            printf("Vous n'avez rien saisi, réessayez SVP\n");
        }
    } while (strlen(password) == 0 || strspn(password, " \n") == strlen(password) || isspace((unsigned char)password[0]));

    // Tenter de se connecter en tant qu'administrateur
    int auth_result = authenticate(filename, username, password, admin_username, admin_password);
    if (auth_result == 1) {
        printf("Chargement...\n\n");
        printf("Vous êtes connecté en tant qu'administrateur\n");
        MenuAdmin();
        

    } else if (auth_result == 2) {
        // Tenter de se connecter en tant qu'étudiant
        auth_result = authenticate(filename, username, password, student_username, student_password);
        if (auth_result == 1) {
            printf("Chargement...\n\n");
            printf("Vous êtes connecté en tant qu'étudiant\n");
            MenuEtudiant();
            
           
        
        } else {
            printf("Identifiants invalides.\n");
        }
    } else {
        printf("Identifiants invalides.\n");
    }

    return 0;
}








#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <signal.h>

#define MAX_BUF 256

int getPID(const char* program_name) 
{
    char command[MAX_BUF];
    FILE *fp;
    int pid = -1;

    // Shell komutu oluştur
    snprintf(command, sizeof(command), "pidof %s", program_name);

    // Komutu çalıştır ve çıktısını oku
    fp = popen(command, "r");
   
    if (fp == NULL) 
    {
        perror("popen");
        return -1;
    }

    // Çıktıyı oku ve PID'yi al
    if (fgets(command, sizeof(command), fp) != NULL) 
    {
        // PID'yi al
        pid = atoi(command);
    }

    // Dosya akışını kapat
    pclose(fp);

    return pid;
}

int main() 
{
    const char* program_name = "sublime_text"; // PID'sini almak istediğiniz programın adı
    //const char* program_name = "chrome"; // PID'sini almak istediğiniz programın adı
    
    int pid = getPID(program_name);

    if (pid != -1) 
    {
        printf("Programın PID'si: %d\n", pid);
        kill(pid, 15);
    } 

    else 
    {
        printf("Program bulunamadı veya bir hata oluştu.\n");
    }

    return 0;
}

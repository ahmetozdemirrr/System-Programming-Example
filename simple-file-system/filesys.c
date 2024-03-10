#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <fcntl.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>

#define BUFFER_SIZE 256

int main(int argc, char const *argv[])
{
	const char * message = "\nPlease enter a input : ";
	const char * content = "Hello world!\nHello world!\nHello world!\nHello world!\nHello world!\nHello world!\nHello world!\n";
	char buffer[BUFFER_SIZE]; //Buffer aç
	char bufferInput[BUFFER_SIZE];

	/* 
		- Dosya eğer yoksa oluştur
		- Okuma ve yazmaya açık olsun
		- Dosya sahibine okuma ve yazma izni açık olsun
	*/
	int file = open("example.txt", O_CREAT | O_RDWR, S_IRUSR | S_IWUSR); 

	if (file == -1)
	{
		//write(2, "Cannot create e new file.\n", 27);
		perror("Cannot create e new file.\n");
		exit(EXIT_FAILURE);
	}

	else
	{
		write(file, content, strlen(content)); // dosyaya content'i yaz.
	}
	
	ssize_t readedByte; // dosyadan okunan blok burada tutulacak
	int lineCount = 0;

	// okumadan önce imleci başa alıyoruz.
	lseek(file, 0, SEEK_SET);
	
	while ((readedByte = read(file, buffer, BUFFER_SIZE)) > 0) // line sayısı bulunacak algoritma
	{
		for (ssize_t i = 0; i < readedByte; ++i)
		{
			if (buffer[i] == '\n')
			{
				lineCount++;
			}
		}

		if (readedByte == 0)
		{
			break; // Dosyanın sonuna gelindi...
		}
	}
	
	// okumadan önce imleci başa alıyoruz.
	lseek(file, 0, SEEK_SET);
	readedByte = read(file, buffer, BUFFER_SIZE);

	if (readedByte == -1)
	{
		perror("Reading fail");
		close(file);
		exit(EXIT_FAILURE);
	}

	else
	{
		printf("\n-- Before input -- \n");
		printf("Line count : %d\n\n", lineCount);
		write(STDOUT_FILENO, buffer, readedByte);
	}
	
	// input girilmesi için talimat ve input alımı
	write(STDOUT_FILENO, message, strlen(message)); // >
	fgets(bufferInput, BUFFER_SIZE, stdin);			// <

	if (strlen(bufferInput) == BUFFER_SIZE - 1 && bufferInput[BUFFER_SIZE - 2] != '\n') 
	{
    	fprintf(stderr, "Input too long, buffer overflow!\n");
    	exit(EXIT_FAILURE);
	}

	write(file, bufferInput, strlen(bufferInput)); // alınan input dosyaya yazılsın

	lineCount = 0; // girdi alındıktan sonra tekrar satır sayısını doğru bulabilmek için sıfırlıyoruz...

	// okumadan önce imleci başa alıyoruz.
	lseek(file, 0, SEEK_SET);
	
	while ((readedByte = read(file, buffer, BUFFER_SIZE)) > 0)
	{
		for (ssize_t i = 0; i < readedByte; ++i)
		{
			if (buffer[i] == '\n')
			{
				lineCount++;
			}
		}

		if (readedByte == 0)
		{
			break; // Dosyanın sonuna gelindi...
		}
	}

	// son çıktıyı basmak için tekrar okuma yapıyoruz
	lseek(file, 0, SEEK_SET);
	readedByte = read(file, buffer, BUFFER_SIZE);

	if (readedByte == -1)
	{
		perror("Reading fail");
		close(file);
		exit(EXIT_FAILURE);
	}

	else
	{
		printf("\n-- After input --\n");
		printf("Line count : %d\n\n", lineCount);
		write(STDOUT_FILENO, buffer, strlen(buffer));
	}
	close(file);
	return 0;
}

/*
	open(const char *path, int flag, mode_t mode)

	flags:

		O_RDONLY: Dosyayı sadece okuma modunda açar.
		O_WRONLY: Dosyayı sadece yazma modunda açar.
		O_RDWR: Dosyayı hem okuma hem yazma modunda açar.
		O_CREAT: Dosya yoksa oluşturur.
		O_EXCL: O_CREAT ile birlikte kullanılır ve dosya zaten varsa hata döndürür.
		O_TRUNC: Dosyayı açarken içeriğini keser (sıfırlar).
		O_APPEND: Dosyaya yazma işlemi yapılırken dosyanın sonuna ekler.
		O_NONBLOCK: Dosyayı bloklamadan (non-blocking) açar.
		O_SYNC: Dosyanın verilerini doğrudan disk üzerine yazar (asenkron olmayan yazma).
		O_ASYNC: Asenkron modda dosya açar (UNIX sinyalleri kullanarak olaylara yanıt verir).

	modes: mode_t mode parametresi sadece O_CREAT kullanıldığında geçerlidir 
		   ve dosyanın izinlerini belirler. 
		S_IRUSR: Dosya sahibine okuma izni verir.
		S_IWUSR: Dosya sahibine yazma izni verir.
		S_IXUSR: Dosya sahibine çalıştırma (execute) izni verir.
		S_IRGRP: Grup üyelerine okuma izni verir.
		S_IROTH: Diğer kullanıcılara okuma izni verir.

---------------------------------------------------------------------------------------------------------

	ssize_t write(int file_descriptor, const void *buffer, size_t count);

	* file_descriptor: İşlem yapılacak dosya veya soketin dosya tanımlayıcısıdır. 
	Örneğin, standart hata çıkışını ifade eden 2, hata mesajını yazdığınız dosya 
	tanımlayıcısını belirtir.
	
	* buffer: Yazılacak verinin bulunduğu bellek bloğunun başlangıç adresidir. 
	"Cannot create e new file.\n" string ifadesi burada bir karakter dizisidir.
	
	* count: Yazılacak verinin boyutudur. 27 değeri, verinin uzunluğunu ifade eder.

---------------------------------------------------------------------------------------------------------

	ssize_t read(int file_descriptor, void *buffer, size_t count);

	* file_descriptor: İşlem yapılacak dosyanın veya soketin dosya tanımlayıcısıdır.
	* buffer: Okunan verinin depolanacağı bellek bloğunun başlangıç adresidir.
	* count: Okunacak byte sayısını belirtir.

---------------------------------------------------------------------------------------------------------
	
	write formats: (unistd.h)

	STDIN_FILENO  -> 0
	STDOUT_FILENO -> 1
	STDERR_FILENO -> 2

	write(format, message, size);
*/
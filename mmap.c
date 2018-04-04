#define _GNU_SOURCE
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>
#include <sys/mman.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>


#define PATH "./archivo.txt"
int main() {
	struct stat filestat;
	char *p;
	int fd;
	int removed;
	size_t size;

	fd = open(PATH, O_CREAT | O_RDWR);
	if (fd == -1) {
		perror("open");
		exit(1);
	}

	if (fstat(fd, &filestat) == -1) {
		perror("stat");
		exit(1);
	}
	size = filestat.st_size;

	p = mmap (0, size, PROT_READ | PROT_WRITE, MAP_SHARED, fd, 0);
	if (p == MAP_FAILED) {
		perror("mmap");
		exit(1);
	}

	int i;
	removed = 0;
	for (i = 0; i < size; i++) {
		if (p[i] == 'b') {
			memmove(&p[i], &p[i+2], strlen(p) - i);
			removed++;
			continue;
		}
		printf("%c", p[i - removed]);
	}
	
	int new_size = ((size - removed * sizeof(char)) * 2) + 1;
	if (ftruncate(fd, new_size) == -1) {
		perror("ftruncate");
		exit(1);
	}

	p = mremap(p, size, new_size, MREMAP_MAYMOVE, 0);
	if (p == MAP_FAILED) {
		perror("mremap");
		exit(1);
	}
	
	int j = 0;
	for (i = i + 1; i < new_size; i++) {
		p[i] = toupper(p[j]);
		printf("%c", p[i]);
		j++;
	}

	if (munmap(p, new_size) == -1) {
		perror("munmap");
		exit(1);
	}

	close(fd);

	exit(0);
}

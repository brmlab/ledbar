#include <stdio.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <string.h>

#define READS_THR	20
#define RAINBOW_DIVISOR 30

int main() {
	int rainbow, equalizer, serial;
	fd_set readers;
	struct timeval tv;
	int r, s, w;
	int reads_eq = 0, reads_ra = 0;
	int source;
	char buf_eq[65], buf_ra[65];
	char *srcbuf = buf_eq;
	int c = 'a';
	int tmp;

	rainbow = open("/tmp/ledbar/rainbow", O_RDWR);
	equalizer = open("/tmp/ledbar/equalizer", O_RDWR);
	serial = open("/tmp/ledbar/serial", O_RDWR);

	if(rainbow == -1 || equalizer == -1 || serial == -1) {
		perror("Open failed");
		return 1;
	}

	printf(	"****************************************\n"
		"* Usage: press [r] for rainbow mode	*\n"
		"*	  press [e] for equalizer mode	*\n"
		"*	  press [a] for automatic mode	*\n"
		"****************************************\n");

	while(1) {
		tv.tv_sec = 3;
		tv.tv_usec = 0;
		FD_ZERO(&readers);
		FD_SET(rainbow, &readers);
		FD_SET(equalizer, &readers);
		FD_SET(fileno(stdin), &readers);

		s = select(equalizer+1, &readers, NULL, NULL, &tv);
		if(s == -1) {
			perror("select failed");
			return 2;
		}
		if(!s)
			continue;

		if(FD_ISSET(fileno(stdin), &readers)) {
			tmp = getc(stdin);
			if (tmp != '\r' && tmp != '\n') {
				if(tmp != 'a' && tmp != 'r' && tmp != 'e')
					printf("Invalid command!\n");
				else
					c = tmp;
			}
		}

		if(FD_ISSET(equalizer, &readers)) {
			r=read(equalizer, buf_eq, sizeof(buf_eq) - 1);
			if(r == -1) {
				perror("eq read");
				return 3;
			}
			int i;
			for(i = 0; i < r; i++) {
				if(buf_eq[i]) {
					reads_eq++;
					break;
				}
			}
		}
		if(FD_ISSET(rainbow, &readers)) {
			r=read(rainbow, buf_ra, sizeof(buf_ra) - 1);
			if(r == -1) {
				perror("ra read");
				return 3;
			}
			reads_ra++;
		}
		
		if (c == 'a') {
			if(reads_eq > READS_THR) {
				reads_eq = 0;
				reads_ra = 0;
				srcbuf = buf_eq;
			} else if(reads_ra > READS_THR*RAINBOW_DIVISOR) {
				reads_eq = 0;
				reads_ra = 0;
				srcbuf = buf_ra;
			}
		} else if (c == 'r') {
			srcbuf = buf_ra;
		} else if (c == 'e') {
			srcbuf = buf_eq;
		}
		
		if(write(serial, srcbuf, r) == -1) {
			perror("write failed");
			return 4;
		}
	}
	close(serial);
	close(rainbow);
	close(equalizer);
	return 0;
	
}


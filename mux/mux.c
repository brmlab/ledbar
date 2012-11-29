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
	int r,s,w;
	int reads_eq=0, reads_ra=0;
	int source;
	char buf_eq[65], buf_ra[65];
	char *srcbuf=buf_eq;


	rainbow=open("/tmp/ledbar/rainbow", O_RDWR);
	equalizer=open("/tmp/ledbar/equalizer", O_RDWR);
	serial=open("/tmp/ledbar/serial", O_RDWR);

	if(rainbow==-1 || equalizer==-1 || serial==-1) {
		perror("Open failed");
		return 1;
	}

	while(1) {
		tv.tv_sec=3;
		tv.tv_usec=0;
		FD_ZERO(&readers);
		FD_SET(rainbow, &readers);
		FD_SET(equalizer, &readers);
		s=select(equalizer+1, &readers, NULL, NULL, &tv);
		if(s==-1) {
			perror("select failed");
			return 2;
		} else if(s) {
			if(FD_ISSET(equalizer, &readers)) {
				r=read(equalizer, buf_eq, sizeof(buf_eq)-1);
				if(r==-1) {
					perror("eq read");
					return 3;
				}
				int i;
				for(i=0; i<r; i++) {
					if(buf_eq[i]) {
						reads_eq++;
						break;
					}
				}
			}
			if(FD_ISSET(rainbow, &readers)) {
				r=read(rainbow, buf_ra, sizeof(buf_ra)-1);
				if(r==-1) {
					perror("ra read");
					return 3;
				}
				reads_ra++;
			}
					
	//	printf("eq: %i ra: %i r: %i\n", reads_eq, reads_ra, r);

			if(reads_eq>READS_THR) {
				reads_eq=0;
				reads_ra=0;
				if(srcbuf!=buf_eq)
					printf("switching to equalizer\n");
				srcbuf=buf_eq;
			} else if(reads_ra>READS_THR*RAINBOW_DIVISOR) {
				reads_eq=0;
				reads_ra=0;
				if(srcbuf!=buf_ra)
					printf("switching to rainbow\n");
				srcbuf=buf_ra;
			}
	
			if(write(serial, srcbuf, r)==-1) {
				perror("write failed");
				return 4;
			}
	//		usleep(500);

		}
	}
	close(serial);
	close(rainbow);
	close(equalizer);
	return 0;
}


//u32 kHeld = hidKeysDown();
//if (kHeld & KEY_B) exit(0);
//if a key is down and its KEY_B
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <malloc.h>
#include <errno.h>
#include <stdarg.h>
#include <unistd.h>

#include <fcntl.h>

#include <sys/types.h>

#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>

#include <3ds.h>

#define SOC_ALIGN       0x1000
#define SOC_BUFFERSIZE  0x100000

static u32 *SOC_buffer = NULL;
s32 sock = -1, csock = -1;

__attribute__((format(printf,1,2)))
void failExit(const char *fmt, ...);

const static char http_200[] = "HTTP/1.1 200 OK\r\n";

const static char indexdata[] = "<html> \
                               <head><title>A test page</title></head> \
                               <body> \
							   <table> \
									<tr>    <th>a</th>  <th>b</th>  <th>x</th>  <th>y</th>  <th>start</th>  <th>select</th> <th>l</th>  <th>r</th>  <th>dup</th>    <th>ddown</th>  <th>dleft</th>  <th>dright</th>	<th>zrdown</th>	<th>zldown</th>			<th>padup</th>    <th>paddown</th>  <th>padleft</th>  <th>padright</th></tr> \
									<tr>    <td>%d</td>  <td>%d</td>  <td>%d</td>  <td>%d</td>  <td>%d</td>      <td>%d</td>      <td>%d</td>  <td>%d</td>  <td>%d</td>      <td>%d</td>      <td>%d</td>      <td>%d</td>		<td>%d</td>		<td>%d</td> 	<td>%d</td>      <td>%d</td>		<td>%d</td>		<td>%d</td></tr> \
									<tr>    <th> Touch X </th> <th> Touch Y </th> </tr> \
									<tr>    <td> %03d </td> <td> %03d </td> </tr> \
								</table> \
							   <script> \
									setTimeout(function() {window.location.reload();}, 5);\
							   </script> \
                               </body> \
                               </html>";

const static char http_html_hdr[] = "Content-type: text/html\r\n\r\n";
const static char http_get_index[] = "GET / HTTP/1.1\r\n";


//---------------------------------------------------------------------------------
void socShutdown() {
//---------------------------------------------------------------------------------
	printf("waiting for socExit...\n");
	socExit();

}

//---------------------------------------------------------------------------------
int main(int argc, char **argv) {
//---------------------------------------------------------------------------------
	int ret;
	int adown = 0;
	int bdown = 0;
	int xdown = 0;
	int ydown = 0;
	int startdown = 0;
	int selectdown = 0;
	int ldown = 0;
	int rdown = 0;
	int dup = 0;
	int ddown = 0;
	int dleft = 0;
	int dright = 0;
	int padup = 0;
	int paddown = 0;
	int padleft = 0;
	int padright = 0;
	int zrdown = 0;
	int zldown = 0;

	u32	clientlen;
	struct sockaddr_in client;
	struct sockaddr_in server;
	char temp[1026];
	static int hits=0;

	gfxInitDefault();

	// register gfxExit to be run when app quits
	// this can help simplify error handling
	atexit(gfxExit);

	consoleInit(GFX_TOP, NULL);

	printf ("\nlibctru sockets demo\n");

	// allocate buffer for SOC service
	SOC_buffer = (u32*)memalign(SOC_ALIGN, SOC_BUFFERSIZE);

	if(SOC_buffer == NULL) {
		failExit("memalign: failed to allocate\n");
	}

	// Now intialise soc:u service
	if ((ret = socInit(SOC_buffer, SOC_BUFFERSIZE)) != 0) {
    	failExit("socInit: 0x%08X\n", (unsigned int)ret);
	}

	// register socShutdown to run at exit
	// atexit functions execute in reverse order so this runs before gfxExit
	atexit(socShutdown);

	// libctru provides BSD sockets so most code from here is standard
	clientlen = sizeof(client);

	sock = socket (AF_INET, SOCK_STREAM, IPPROTO_IP);

	if (sock < 0) {
		failExit("socket: %d %s\n", errno, strerror(errno));
	}

	memset (&server, 0, sizeof (server));
	memset (&client, 0, sizeof (client));

	server.sin_family = AF_INET;
	server.sin_port = htons (80);
	server.sin_addr.s_addr = gethostid();

	printf("Point your browser to http://%s/\n",inet_ntoa(server.sin_addr));
		
	if ( (ret = bind (sock, (struct sockaddr *) &server, sizeof (server))) ) {
		close(sock);
		failExit("bind: %d %s\n", errno, strerror(errno));
	}

	// Set socket non blocking so we can still read input to exit
	fcntl(sock, F_SETFL, fcntl(sock, F_GETFL, 0) | O_NONBLOCK);

	if ( (ret = listen( sock, 5)) ) {
		failExit("listen: %d %s\n", errno, strerror(errno));
	}

	while (aptMainLoop()) {
		gspWaitForVBlank();
		hidScanInput();

		csock = accept (sock, (struct sockaddr *) &client, &clientlen);

		if (csock<0) {
			if(errno != EAGAIN) {
				failExit("accept: %d %s\n", errno, strerror(errno));
			}
		} else {
			//check for inputs if so set to 1
			u32 kHeld = hidKeysHeld();
			if (kHeld & KEY_A) adown = 1;
			if (kHeld & KEY_B) bdown = 1;
			if (kHeld & KEY_X) xdown = 1;
			if (kHeld & KEY_Y) ydown = 1;

			//start and selct
			if (kHeld & KEY_START) startdown = 1;
			if (kHeld & KEY_SELECT) selectdown = 1;

			// bumpers
			if (kHeld & KEY_L) ldown = 1;
			if (kHeld & KEY_R) rdown = 1;
			if (kHeld & KEY_ZR) zrdown = 1;
			if (kHeld & KEY_ZL) zldown = 1;

			//cStick def
			if (kHeld & KEY_CPAD_DOWN) ddown = 1;
			if (kHeld & KEY_CPAD_UP) dup = 1;
			if (kHeld & KEY_CPAD_RIGHT) dright = 1;
			if (kHeld & KEY_CPAD_LEFT) dleft = 1;

			// I Think the dPad?
			if (kHeld & KEY_DDOWN) paddown = 1;
			if (kHeld & KEY_DUP) padup = 1;
			if (kHeld & KEY_DLEFT) padleft = 1;
			if (kHeld & KEY_DRIGHT) padright = 1;
			
			// Touchpad
			touchPosition touch;

			//Read the touch screen coordinates
			hidTouchRead(&touch);

			// //Print the touch screen coordinates
			// printf("\x1b[2;0H%03d; %03d", touch.px, touch.py);


			//-------------------------------
			// set client socket to blocking to simplify sending data back
			fcntl(csock, F_SETFL, fcntl(csock, F_GETFL, 0) & ~O_NONBLOCK);
			//printf("Connecting port %d from %s\n", client.sin_port, inet_ntoa(client.sin_addr));
			memset (temp, 0, 1026);

			ret = recv (csock, temp, 1024, 0);

			if ( !strncmp( temp, http_get_index, strlen(http_get_index) ) ) {
				hits++;

				send(csock, http_200, strlen(http_200),0);
				send(csock, http_html_hdr, strlen(http_html_hdr),0);
				//										<th>dup</th>    <th>ddown</th>  <th>dleft</th>  <th>dright</th><th>padup</th>    <th>paddown</th>  <th>padleft</th>  <th>padright</th>
				sprintf(temp, indexdata, adown, bdown, xdown, ydown, startdown, selectdown, ldown, rdown, dup, ddown, dleft, dright, zrdown, zldown, padup, paddown, padleft, padright, touch.px, touch.py);
				send(csock, temp, strlen(temp),0);
			}

			close (csock);
			csock = -1;
			// set all input variables back to zero
			adown = 0;
			bdown = 0;
			xdown = 0;
			ydown = 0;
			startdown = 0;
			selectdown = 0;
			ldown = 0;
			rdown = 0;
			dup = 0;
			ddown = 0;
			dleft = 0;
			dright = 0;
			zrdown = 0;
			zldown = 0;
			padup = 0;
			paddown = 0;
			padleft = 0;
			padright = 0;
			// -------------------------
		}

		// dont want to accidentally break the connecting playing games
		// if (kHeld & KEY_START) break;
	}

	close(sock);

	return 0;
}

//---------------------------------------------------------------------------------
void failExit(const char *fmt, ...) {
//---------------------------------------------------------------------------------

	if(sock>0) close(sock);
	if(csock>0) close(csock);

	va_list ap;

	printf(CONSOLE_RED);
	va_start(ap, fmt);
	vprintf(fmt, ap);
	va_end(ap);
	printf(CONSOLE_RESET);
	printf("\nPress B to exit\n");

	while (aptMainLoop()) {
		gspWaitForVBlank();
		hidScanInput();

		u32 kHeld = hidKeysDown();
		if (kHeld & KEY_B) exit(0);
	}
}

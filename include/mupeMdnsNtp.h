#ifndef MUPEMDNSNTP
#define MUPEMDNSNTP



void mupeMdnsNtpInit(void );

uint64_t getNowMs() ;
void waitForNTPConnect();

#endif

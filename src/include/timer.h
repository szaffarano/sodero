#ifndef _INCLUDE_H_
#define _INCLUDE_H_

#define MICRO(m) ((long) (m/15.08573727))
#define SEGUNDOS(m) (((long) (m/15.08573727) * 1000000L))

void delay (const long);

#endif //_INCLUDE_H_

////////////////////////////////////////////////////////////////
// File: macros.h
// Desc: Useful macros.
////////////////////////////////////////////////////////////////

#define SAFERELEASE(Obj)		if(Obj != NULL) { Obj->Release(); Obj = NULL; }
#define SAFEDELETE(A)			if(A != NULL){delete A; A = NULL;}
#define SAFEDELETEARRAY(A)  	if(A != NULL){delete [] A ;A = NULL;}
#define TODEGREE(rad)			((rad * 180.0f) / 3.1415f)
#define TORADIAN(deg)			((deg * 3.1415f) / 180.0f)
#define PI						(180.0f)
#define AsyncKey(k)				(GetAsyncKeyState(k) & 0x8000f)
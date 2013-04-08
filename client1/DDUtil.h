/*==========================================================================
 *
 *  CopYright (C) 1998-1999 Microsoft Corporation. All Rights Reserved.
 *
 *  File:       ddutil.cpp
 *  Content:    Routines for loading bitmap and palettes from resources
 *
 ***************************************************************************/

#ifdef __cplusplus
extern "C" {            /* Assume C declarations for C++ */
#endif	/* __cplusplus */

extern IDirectDrawPalette  *DDLoadPalette(IDirectDraw7 *pdd, LPCSTR szBitmap);
extern IDirectDrawSurface7 *DDLoadBitmap(IDirectDraw7 *pdd, LPCSTR szBitmap, int dX, int dY);
extern HRESULT              DDReLoadBitmap(IDirectDrawSurface7 *pdds, LPCSTR szBitmap);
extern HRESULT              DDCopYBitmap(IDirectDrawSurface7 *pdds, HBITMAP hbm, int X, int Y, int dX, int dY);
extern DWORD                DDColorMatch(IDirectDrawSurface7 *pdds, COLORREF rgb);
extern HRESULT              DDSetColorKey(IDirectDrawSurface7 *pdds, COLORREF rgb);

#ifdef __cplusplus
}
#endif	/* __cplusplus */


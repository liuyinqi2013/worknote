#include "CCBase64Encode.h"
static unsigned char *base64 = (unsigned char*)("ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789+/=");

string ToBase64Encode(const string& src) {
	int i, j, srclength  = src.length();
	string dest((srclength/3*4) + 4, 0);

	for  (i = 0, j = 0; i < srclength/3*3; i += 3, j += 4) {
		dest[j] = ((src[i] & 0xfc) >> 2);
		dest[j + 1] = ((src[i] & 0x03) << 4) + ((src[i + 1] & 0xf0) >> 4);
		dest[j + 2] = ((src[i + 1] & 0x0f) << 2) + ((src[i + 2] & 0xc0) >> 6);
		dest[j + 3] = (src[i + 2] & 0x3f);
	}

	if ( srclength % 3 == 1) {
		dest[j ] = ((src[i] & 0xfc) >> 2);
		dest[j + 1 ] = ((src[i] & 0x03) << 4);
		dest[j + 2] = 64;
		dest[j + 3] = 64;
	} 
	else if ( srclength % 3 == 2) {
		dest[j] = ((src[i] & 0xfc) >> 2);
		dest[j + 1] = ((src[i] & 0x03) << 4) + ((src[i + 1] & 0xf0) >> 4);
		dest[j + 2] = ((src[i + 1] & 0x0f) << 2) ;
		dest[j + 3] = 64;
	}
	else {
		dest.erase(dest.length() - 4, 4);
	}

	for (int j = 0; j < dest.length(); j++)
		dest[j] = base64[(int)dest[j]];
	return dest;
}

unsigned long ToBase64Encode(const char* src, unsigned long len, string &des)
{
	if (len == 0 ) return 0;
	unsigned long i, j, srclength  = len;
	string dest((srclength/3*4) + 4, 0);
	for  (i = 0, j = 0; i < srclength/3*3; i += 3, j += 4) {
		dest[j] = ((src[i] & 0xfc) >> 2);
		dest[j + 1] = ((src[i] & 0x03) << 4) + ((src[i + 1] & 0xf0) >> 4);
		dest[j + 2] = ((src[i + 1] & 0x0f) << 2) + ((src[i + 2] & 0xc0) >> 6);
		dest[j + 3] = (src[i + 2] & 0x3f);
	}

	if ( srclength % 3 == 1) {
		dest[j ] = ((src[i] & 0xfc) >> 2);
		dest[j + 1 ] = ((src[i] & 0x03) << 4);
		dest[j + 2] = 64;
		dest[j + 3] = 64;
	} 
	else if ( srclength % 3 == 2) {
		dest[j] = ((src[i] & 0xfc) >> 2);
		dest[j + 1] = ((src[i] & 0x03) << 4) + ((src[i + 1] & 0xf0) >> 4);
		dest[j + 2] = ((src[i + 1] & 0x0f) << 2) ;
		dest[j + 3] = 64;
	}
	else {
		dest.erase(dest.size() - 4, 4);
	}

	for (j = 0; j < dest.size(); j++)
		dest[j] = base64[(int)dest[j]];
	des = dest;
	return des.size();
}

string UnBase64Encode(const string& src)
{
	unsigned int i, j, n = 0,leng = src.length();
	string temp(leng, 0);
	string dest(leng/4*3, 0);

	for(i = 0; i < leng; i++) {
		for(j = 0; j < 65; j++) {
			if(src[i] == base64[j]){
				temp[i] = j;
				break;
			}
		}
	}

	if (temp[leng - 1] == 64) {
		n++;
	}

	if(temp[leng - 2] == 64) {
		n++;
	}

	for (i = 0, j = 0; i <= leng - 4 ; i += 4, j += 3) {
		dest[j] = ( (temp[i]<<2) & 0xfc) + ((temp[i + 1] >> 4) & 0x03);
		dest[j + 1] = ((temp[i + 1] << 4) &  0xf0) + ((temp[i + 2] >> 2) & 0x0f);
		dest[j + 2] = ((temp[i + 2] << 6) & 0xc0) +  temp[i + 3] ;
	}
	int len = dest.length(); 
	dest.erase(len - n, n);
	return dest;
}

unsigned long UnBase64Encode(const char *src, unsigned long length, string &des)
{	
	if (length == 0) return 0;
	unsigned long i, j, n = 0,leng = length;
	string temp(leng, 0);
	string dest(leng/4*3, 0);
	for(i = 0; i < leng; i++) {
		for(j = 0; j < 65; j++) {
			if(src[i] == base64[j]) {
				temp[i] = j;
				break;
			}
		}
	}

	if (temp[leng - 1] == 64) {
		n++;
	}
	if(temp[leng - 2] == 64) {
		n++;
	}

	for (i = 0, j = 0; i <= leng - 4 ; i += 4, j += 3) {
		dest[j] = ( (temp[i]<<2) & 0xfc) + ((temp[i + 1] >> 4) & 0x03);
		dest[j + 1] = ((temp[i + 1] << 4) &  0xf0) + ((temp[i + 2] >> 2) & 0x0f);
		dest[j + 2] = ((temp[i + 2] << 6) & 0xc0) +  temp[i + 3] ;
	}
	unsigned long len = leng/4*3; 
	dest.erase(len - n, n);
	des = dest;
	return len - n;
}

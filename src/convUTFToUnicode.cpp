#include "stdafx.h"
#include "convToUnicode.h"
#include "convUTFToUnicode.h"

#include <string>

#define		UTF16BUF(x)						dst+=writeUTF16Buf(dbuf,maxbufsize,dst,x,status.bXCS)
#define		UTF16BUF2(x)					dst+=writeUTF16Buf(dbuf,maxbufsize,dst,x,false)
#define		__USE_UTF_CODE_CRLF__			// ユニコード出力の改行に CR+LF を使用する。定義しなければ LF のみになる。
#define		__USE_8BITCODE_CR__				// 8単位符号出力の改行コードに0x0Dを使用する。定義しなければ0x0Aを使用する



size_t conv_utf_to_unicode(char16_t* dbuf, const size_t maxbufsize, const uint8_t* sbuf, const size_t total_length, const bool bCharSize, const bool bIVS) {
	//		UTF-8 -> UNICODE(UTF-16LE)文字列への変換
	//
	//		sbuf				変換元buf
	//		total_length		その長さ(uint8_t単位, NULL文字分を含まない)
	//		dbuf				変換先buf
	//		maxbufsize			変換先bufの最大サイズ(char16_t単位), 越えた分は書き込まれず無視される
	//		bCharSize			スペース及び英数文字の変換に文字サイズ指定(NSZ, MSZ)を反映させるか否か．trueなら反映させる
	//		bIVS				字体の異なる漢字(葛, 辻, 祇など)存在する場合、その区別に異体字セレクタを使用する．
	//
	//		戻り値				変換して生成したUTF-16文字列の長さ(char16_t単位)
	//							dbufにNULLを指定すると変換して生成した文字列の長さ(char16_t単位)だけ返す

	ConvStatus	status;
	initConvStatus(&status);

	size_t		src = 0;
	size_t		dst = 0;

	while (src < total_length)
	{
		if (isControlChar(sbuf[src]))
		{
			// 0x00～0x20, 0x7F～0xA0, 0xFFの場合

			switch (sbuf[src])
			{
			case 0x08:						// APB (BS)
			case 0x09:						// APF (TAB)
				UTF16BUF(sbuf[src]);												// BS, TAB出力
				src++;
				break;
			case 0x0A:						// APD (LF)
			case 0x0D:						// APR (CR)
#ifdef __USE_UTF_CODE_CRLF__
				UTF16BUF(0x000D);													// CR出力
#endif
				UTF16BUF(0x000A);													// LF出力
				if ((sbuf[src] == 0x0D) && ((src + 1) < total_length) && (sbuf[src + 1] == 0x0A)) src++;
				src++;
				break;
			case 0x20:						// SP
				UTF16BUF((bCharSize && status.bNormalSize) ? 0x3000 : 0x0020);	// 全角, 半角SP出力
				src++;
				break;
			case 0x7F:						// DEL
				UTF16BUF(0x007F);													// DEL出力．（DEL文字は前景色塗りつぶしなので、本来は U+25A0 や U+25AE を出力するのが正しい動作かと思いますが、都合によりこうしてあります）
				src++;
				break;
			case 0x9B:						// CSI処理
				src += csiProc(sbuf + src, total_length - src, &status);
				break;
			default:						// それ以外の制御コード
				src += changeConvStatus(sbuf + src, total_length - src, &status);
				break;
			}
		}
		else
		{

			char  sConv[4] = {0,0,0,0};// sbuf[src];
				//可変長の処理
			wchar_t dConv[1] = { 0 };
				if (sbuf[src] < 0x80) {
					char sConv[1]{ (char)sbuf[src] };

					MultiByteToWideChar(CP_UTF8, 0, sConv, 1, dConv, 1);
					src = src + 1;
					dst++;
					dbuf[dst - 1] = dConv[0];
				}
				else if ((sbuf[src] >= 0xC2) && (sbuf[src] < 0xDF)) {
					char sConv[2]{ (char)sbuf[src],(char)sbuf[src + 1] };

					MultiByteToWideChar(CP_UTF8, 0, sConv, 2, dConv, 1);
					src = src + 2;
					dst++;
					dbuf[dst - 1] = dConv[0];
				}
				else if (sbuf[src] >= 0xE0 && sbuf[src] < 0xF0) {
					char sConv[3]{ (char)sbuf[src],(char)sbuf[src + 1],(char)sbuf[src + 2] };

					MultiByteToWideChar(CP_UTF8, 0, sConv, 3, dConv, 1);
					src = src + 3;
					dst++;
					dbuf[dst - 1] = dConv[0];
				}
				else if (sbuf[src] >= 0xF0 && sbuf[src] < 0xF5) {
					char sConv[4]{ (char)sbuf[src],(char)sbuf[src + 1],(char)sbuf[src + 2],(char)sbuf[src + 3] };
					uint32_t sEsc = ((sConv[0] & 0xFF) << 24 | (sConv[1] & 0xFF) << 16 | (sConv[2] & 0xFF) << 8 | (sConv[3] & 0xFF));
					if ( (sEsc >= 0xF09F8400) & (sEsc <= 0xF09F8900) ){
					//if ((sConv[0] == 0xF0) && (sConv[1] == 0x9F) && (sConv[2] > 0x84) && (sConv[2] < 0x89) ) {
						//getUTF8toARIBAdditionalSynbol(sConv[0],);
						dst++;
						dbuf[dst - 1] = 0x005B;
						if (sEsc == 0xF09F889A) {
							dst++;
							dbuf[dst - 1] = 0x7121;
						}
						if (sEsc == 0xF09F8891) {
							dst++;
							dbuf[dst - 1] = 0x5B57;
						}
						if (sEsc == 0xF09F869E) {
							dst++;
							dbuf[dst - 1] = 0x0034;
							dst++;
							dbuf[dst - 1] = 0x004B;
						}
						dst++;
						dbuf[dst - 1] = 0x005D;
					}
					else {
						MultiByteToWideChar(CP_UTF8, 0, sConv, 4, dConv, 1);
						dst++;
						dbuf[dst - 1] = dConv[0];
					}

					src = src + 4;

				}
				else UTF16BUF(0x0020);

		}
	}
		UTF16BUF2(0x0000);

		dst--;
		if (dst > maxbufsize) dst = maxbufsize;

		return dst;			// 変換後の長さを返す(char16_t単位), 終端のnull文字分を含まない
	}

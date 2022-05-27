#include "stdafx.h"
#include "convToUnicode.h"
#include "convUTFToUnicode.h"

#include <string>

#define		UTF16BUF(x)						dst+=writeUTF16Buf(dbuf,maxbufsize,dst,x,status.bXCS)
#define		UTF16BUF2(x)					dst+=writeUTF16Buf(dbuf,maxbufsize,dst,x,false)
#define		__USE_UTF_CODE_CRLF__			// ���j�R�[�h�o�͂̉��s�� CR+LF ���g�p����B��`���Ȃ���� LF �݂̂ɂȂ�B
#define		__USE_8BITCODE_CR__				// 8�P�ʕ����o�͂̉��s�R�[�h��0x0D���g�p����B��`���Ȃ����0x0A���g�p����



size_t conv_utf_to_unicode(char16_t* dbuf, const size_t maxbufsize, const uint8_t* sbuf, const size_t total_length, const bool bCharSize, const bool bIVS) {
	//		UTF-8 -> UNICODE(UTF-16LE)������ւ̕ϊ�
	//
	//		sbuf				�ϊ���buf
	//		total_length		���̒���(uint8_t�P��, NULL���������܂܂Ȃ�)
	//		dbuf				�ϊ���buf
	//		maxbufsize			�ϊ���buf�̍ő�T�C�Y(char16_t�P��), �z�������͏������܂ꂸ���������
	//		bCharSize			�X�y�[�X�y�щp�������̕ϊ��ɕ����T�C�Y�w��(NSZ, MSZ)�𔽉f�����邩�ۂ��Dtrue�Ȃ甽�f������
	//		bIVS				���̂̈قȂ銿��(��, ��, �_�Ȃ�)���݂���ꍇ�A���̋�ʂɈّ̎��Z���N�^���g�p����D
	//
	//		�߂�l				�ϊ����Đ�������UTF-16������̒���(char16_t�P��)
	//							dbuf��NULL���w�肷��ƕϊ����Đ�������������̒���(char16_t�P��)�����Ԃ�

	ConvStatus	status;
	initConvStatus(&status);

	size_t		src = 0;
	size_t		dst = 0;

	while (src < total_length)
	{
		if (isControlChar(sbuf[src]))
		{
			// 0x00�`0x20, 0x7F�`0xA0, 0xFF�̏ꍇ

			switch (sbuf[src])
			{
			case 0x08:						// APB (BS)
			case 0x09:						// APF (TAB)
				UTF16BUF(sbuf[src]);												// BS, TAB�o��
				src++;
				break;
			case 0x0A:						// APD (LF)
			case 0x0D:						// APR (CR)
#ifdef __USE_UTF_CODE_CRLF__
				UTF16BUF(0x000D);													// CR�o��
#endif
				UTF16BUF(0x000A);													// LF�o��
				if ((sbuf[src] == 0x0D) && ((src + 1) < total_length) && (sbuf[src + 1] == 0x0A)) src++;
				src++;
				break;
			case 0x20:						// SP
				UTF16BUF((bCharSize && status.bNormalSize) ? 0x3000 : 0x0020);	// �S�p, ���pSP�o��
				src++;
				break;
			case 0x7F:						// DEL
				UTF16BUF(0x007F);													// DEL�o�́D�iDEL�����͑O�i�F�h��Ԃ��Ȃ̂ŁA�{���� U+25A0 �� U+25AE ���o�͂���̂����������삩�Ǝv���܂����A�s���ɂ�肱�����Ă���܂��j
				src++;
				break;
			case 0x9B:						// CSI����
				src += csiProc(sbuf + src, total_length - src, &status);
				break;
			default:						// ����ȊO�̐���R�[�h
				src += changeConvStatus(sbuf + src, total_length - src, &status);
				break;
			}
		}
		else
		{

			char  sConv[4] = {0,0,0,0};// sbuf[src];
				//�ϒ��̏���
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

		return dst;			// �ϊ���̒�����Ԃ�(char16_t�P��), �I�[��null���������܂܂Ȃ�
	}

#pragma once
#include <stdint.h>

size_t conv_utf_to_unicode(char16_t* dbuf, const size_t maxbufsize, const uint8_t* sbuf, const size_t total_length, const bool bCharSize, const bool bIVS);
size_t getUTF8toARIBAdditionalSynbol(char* sConv, wchar_t* dConv);

typedef struct UTFToUnicodeTable
{
	int utf8;
	int utf16;
};

const struct UTFToUnicodeTable tables[] =
{
	//2バイト毎にLSBとして扱う
	//ex: HVのHは0x4800, Vは0x5600
	//出来たら前半はもう少しきれいなテーブルにしたい
	{0xF09F858A, 0x56004800},//HV
	{0xF09F858B, 0x56004C00},//MV
	{0xF09F858C, 0x44005300},//SD
	{0xF09F858D, 0x53005300},//SS
	{0xF09F858E, 0x56004F004F00},//PPV
	{0xF09F869B, 0x44000033},//3D
	{0xF09F869D, 0x004B0032},// 2K
	{0xF09F869E, 0x004B0034},//4K
	{0xF09F869F, 0x004B0038},// 8K
	{0xF09F86A0, 0x0031002C0035},//5.1
	{0xF09F86A1, 0x0031002C0037},//7.1
	{0xF09F86A2, 0x0032002C00320032},//22.2
	{0xF09F86A3, 0x4F0000300036},//60P
	{0xF09F86A4, 0x4F00003000320031},//60P
	{0xF09F86A5, 0x6400},//[d]
	{0xF09F86A6, 0x43004800},//HybridCast
	{0xF09F86A7, 0x510044004800},//HDR
	{0xF09F86AA, 0x560048005300},//SHV
	{0xF09F86AB, 0x440048005500},//UHD
	{0xF09F86AC, 0x44004E005600},//VOD

	{0xF09F8890, 0x624B},//手
	{0xF09F8891, 0x5B57},// 字
	{0xF09F8892, 0x53CC},// 双
	{0xF09F8893, 0x30C7},// デ
	{0xF09F8894, 0x4E8C},// 二
	{0xF09F8895, 0x591A},// 多
	{0xF09F8896, 0x89E3},// 解
	{0xF09F8897, 0x5929},// 天
	{0xF09F8898, 0x4EA4},// 交
	{0xF09F8899, 0x6620},// 映
	{0xF09F889A, 0x7121},// 無
	{0xF09F889B, 0x6599},// 料
	{0xF09F889C, 0x524D},// 前
	{0xF09F889D, 0x5F8C},// 後
	{0xF09F889E, 0x518D},// 再
	{0xF09F889F, 0x65B0},// 新

	{0xF09F88A0, 0x521D},// 初
	{0xF09F88A1, 0x7D42},// 終
	{0xF09F88A2, 0x751F},// 生
	{0xF09F88A3, 0x8CA9},// 販
	{0xF09F88A4, 0x58F0},// 声
	{0xF09F88A5, 0x5439},// 吹
	{0xF09F88A6, 0x6F14},// 演



	{ -1, -1 },
};
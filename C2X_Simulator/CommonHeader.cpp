#include "commonheader.h"

CommonHeader::CommonHeader(int ht, int hst, int pl, int mhl)
{
	HT = ht;
	HST = hst;
	PL = pl;
	MHL = mhl;
}
CommonHeader::~CommonHeader(){}

int CommonHeader::GetHT()
{
	return HT;
}
int CommonHeader::GetHST()
{
	return HST;
}
int CommonHeader::GetPL()
{
	return PL;
}
int CommonHeader::GetMHL()
{
	return MHL;
}
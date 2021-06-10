#include "basicheader.h"

BasicHeader::BasicHeader(int lt, int rhl)
{
	LT = lt;
	RHL = rhl;
}
BasicHeader::~BasicHeader(){}

int BasicHeader::GetLT()
{
	return LT;
}
int BasicHeader::GetRHL()
{
	return RHL;
}
void BasicHeader::DecrementLT()
{
	LT--;
}
void BasicHeader::DecrementRHL()
{
	RHL--;
}
void BasicHeader::SetRHL(int rhl)
{
	RHL = rhl;
}
void BasicHeader::SetLT(int lt)
{
	LT = lt;
}
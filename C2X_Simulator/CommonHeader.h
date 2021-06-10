#ifndef HEADERS_COMMONHEADER_H_
#define HEADERS_COMMONHEADER_H_


class CommonHeader
{
public:
	CommonHeader(int ht, int hst, int pl, int mhl);
	~CommonHeader();
	
	int GetHT();
	int GetHST();
	int GetPL();
	int GetMHL();

private:
	int HT;		//type
	int HST;	//subtype
	int PL;		//payload length
	int MHL;	//max hop limit
};

#endif /*HEADERS_COMMONHEADER_H_*/

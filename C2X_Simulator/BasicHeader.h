#ifndef HEADERS_BASICHEADER_H_
#define HEADERS_BASICHEADER_H_


class BasicHeader
{
public:
	BasicHeader(int lt, int rhl);
	~BasicHeader();

	int GetLT();
	int GetRHL();
	void DecrementLT();
	void DecrementRHL();
	void SetRHL(int rhl);
	void SetLT(int lt);

private:
	int LT;		//lifetime of packet
	int RHL;	//remaining hop limit
};

#endif /*HEADERS_BASICHEADER_H_*/

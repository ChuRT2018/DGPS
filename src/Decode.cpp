#include "DGPS_CHRT/GPSCurriculumDesign.h"

bool GPSCurriculumDesign::Findhead(unsigned char* pdata, int dataLength, int& headPosition)
{
	//unsigned char buff[MAXRAWLEN];
	if (dataLength < 3) return false;
	unsigned char temp[3];
	int position = 0;
	temp[0] = *(pdata); position++;
	temp[1] = *(pdata++); position++;
	temp[2] = *(pdata++); position++;
	while (position < dataLength) {
		if (temp[0] == 0xAA && temp[1] == 0x44 && temp[2] == 0x12) {
			headPosition = position - 1;
			return true;
		}
		else {
			temp[0] = temp[1];
			temp[1] = temp[2];
			temp[2] = *(pdata++); 
			position++;
		}
	}
	return false;
}

/*
typedef struct head{
	unsigned char headLength; // 1
	unsigned short messageID; // 2
	char messageType;  // 1
	unsigned char portAddress;  //1
	unsigned short messageLength; // 2
	unsigned short sequence; // 2
	unsigned char idleTime; // 1
	unsigned char timeStatus; // 1
	unsigned short week; // 2
	long gpsSecondBin = -1; // 4
	float gpsSecondASCII = -1; // 4
	unsigned long receiverStatus; // 4
	unsigned short reserved; // 2
	unsigned short receiver; // 2
};
*/
bool GPSCurriculumDesign::DecodeHead(unsigned char* pdata, head& header)
{
	//unsigned char buff[MAXRAWLEN];
	unsigned char buff[MAXRAWLEN] = "\n";
	memcpy(buff, pdata, MAXRAWLEN * sizeof(unsigned char));
	int num = 0;

	//Head Length 1
	//Hex2UChar(buff, header.headLength, MAXRAWLEN, num);
	unsigned char* pbuff = buff + num;
	Hex2Dec(&pbuff, header.headLength, MAXRAWLEN, num);
	
	//Message ID 2
	//Hex2UShort(buff, header.messageID, totalLen, position);
	pbuff = buff + num;
	Hex2Dec(&pbuff, header.messageID, MAXRAWLEN, num);

	//message type 1 
	//Hex2Char(buff, header.messageType, totalLen, position);
	pbuff = buff + num;
	Hex2Dec(&pbuff, header.messageType, MAXRAWLEN, num);
	
	//port address 1
	//Hex2UChar(buff, header.portAddress, totalLen, position);
	pbuff = buff + num;
	Hex2Dec<unsigned char>(&pbuff, header.portAddress, MAXRAWLEN, num);

	//message length 2
	//Hex2UShort(buff, header.messageLength, totalLen, position);
	pbuff = buff + num;
	Hex2Dec(&pbuff, header.messageLength, MAXRAWLEN, num);

	//sequence 2
	//Hex2UShort(buff, header.sequence, totalLen, position);
	pbuff = buff + num;
	Hex2Dec(&pbuff, header.sequence, MAXRAWLEN, num);

	//idle time 1
	//Hex2UChar(buff, header.idleTime, totalLen, position);
	pbuff = buff + num;
	Hex2Dec(&pbuff, header.idleTime, MAXRAWLEN, num);

	//time status 1
	/////////////////data type unknown///////////////////////
	//Hex2UChar(buff, header.timeStatus, totalLen, position);
	pbuff = buff + num;
	Hex2Dec(&pbuff, header.timeStatus, MAXRAWLEN, num);
	
	//week 2
	//Hex2UShort(buff, header.week, totalLen, position);
	pbuff = buff + num;
	Hex2Dec(&pbuff, header.week, MAXRAWLEN, num);

	//ms 4
	//Hex2Long(buff, header.gpsSecondBin, totalLen, position);
	pbuff = buff + num;
	Hex2Dec(&pbuff, header.gpsSecondBin, MAXRAWLEN, num);

	//receiverStatus 4
	//Hex2ULong(buff, header.receiverStatus, totalLen, position);
	pbuff = buff + num;
	Hex2Dec(&pbuff, header.receiverStatus, MAXRAWLEN, num);

	//reserved 2
	//Hex2UShort(buff, header.reserved, totalLen, position);
	pbuff = buff + num;
	Hex2Dec(&pbuff, header.reserved, MAXRAWLEN, num);

	//receiver 2
	//Hex2UShort(buff, header.receiver, totalLen, position);
	pbuff = buff + num;
	Hex2Dec(&pbuff, header.receiver, MAXRAWLEN, num);

	return true;
}

/*
typedef struct satellitemessage {
	unsigned short PRN; //2
	unsigned short glofreq; //2
	double psr; //8
	float psrStd; //4
	double adr; //8
	float adrStd; //4
	float dopp; //4
	float C; //4
	float locktime; //4
	unsigned long ch_tr_status; //4
};
*/
bool GPSCurriculumDesign::DecodeRange(unsigned char*pdata, int headerLen, int messageLen, std::vector<range>& ranges)
{
	if (pdata == nullptr) return false;
	if (messageLen <= sizeof(long)) return false;
	int position = headerLen;
	int totalLen = headerLen + messageLen;
	pdata += position;
	ranges.clear();
	long obs = 0;
	Hex2Dec<long>(&pdata, obs, totalLen, position);

	for (int i = 0; i < obs; i++) {
		range range_;
		//PRN 2
		Hex2Dec<unsigned short>(&pdata, range_.PRN, totalLen, position);

		//glofreq 2
		Hex2Dec(&pdata, range_.glofreq, totalLen, position);

		//psr 8
		Hex2Dec(&pdata, range_.psr, totalLen, position);


		//psrStd 4
		Hex2Dec(&pdata, range_.psrStd, totalLen, position);

		//adr 8
		Hex2Dec(&pdata, range_.adr, totalLen, position);

		//adrStd 4
		Hex2Dec(&pdata, range_.adrStd, totalLen, position);

		//dopp 4
		Hex2Dec(&pdata, range_.dopp, totalLen, position);


		//C 4
		Hex2Dec(&pdata, range_.C, totalLen, position);


		//locktime 4
		Hex2Dec(&pdata, range_.locktime, totalLen, position);


		//ch_tr_status 4
		Hex2Dec(&pdata, range_.ch_tr_status, totalLen, position);


		ranges.push_back(range_);
	}
	return true;
}


//typedef struct gpsephem {
//	unsigned long PRN; // 4
//	double tow; // 8
//	unsigned long health; // 4
//	unsigned long iode1; // 4
//	unsigned long iode2; // 4
//	unsigned long week; // 4
//	unsigned long zweek; // 4
//	double toe; // 8
//	double A; // 8
//	double deltaN; // 8
//	double M0; // 8
//	double ecc; // 8
//	double omega; // 8
//	double cuc; // 8
//	double cus; // 8
//	double crc; // 8
//	double crs; // 8
//	double cic; // 8
//	double cis; // 8
//	double l0; // 8
//	double I; // 8
//	double omega0; // 8
//	double oomega; // 8
//	unsigned long iodc; //4
//	double toc; // 8
//	double tgd; // 8
//	double af0; // 8
//	double af1; // 8
//	double af2; // 8
//	antispoofing AS; // 4
//	double N; // 8
//	double URA; // 8
//};
bool GPSCurriculumDesign::DecodeGPSEphem(unsigned char* pdata, int headerLen, int messageLen, std::unordered_map<unsigned long, gpsephem>& gpsephems, unsigned long& PRN)
{
	if (pdata == nullptr) return false;
	if (messageLen == 0) return false;
	int position = headerLen;
	int totalLen = headerLen + messageLen;
	pdata += headerLen;

	gpsephem gpsephem_;

	//	unsigned long PRN 4
	if (!Hex2Dec(&pdata, gpsephem_.PRN, totalLen, position)) {
		return false;
	}

	//	double tow 8
	if (!Hex2Dec(&pdata, gpsephem_.tow, totalLen, position)) {
		return false;
	}

	//	unsigned long health  4
	if (!Hex2Dec(&pdata, gpsephem_.health, totalLen, position)) {
		return false;
	}

	//	unsigned long iode1 4
	if (!Hex2Dec(&pdata, gpsephem_.iode1, totalLen, position)) {
		return false;
	}

	//	unsigned long iode2 4
	if (!Hex2Dec(&pdata, gpsephem_.iode2, totalLen, position)) {
		return false;
	}

	//	unsigned long week 4
	if (!Hex2Dec(&pdata, gpsephem_.week, totalLen, position)) {
		return false;
	}

	//	unsigned long zweek 4
	if (!Hex2Dec(&pdata, gpsephem_.zweek, totalLen, position)) {
		return false;
	}


	//	double toe 8
	if (!Hex2Dec(&pdata, gpsephem_.toe, totalLen, position)) {
		return false;
	}

	//	double A 8
	if (!Hex2Dec(&pdata, gpsephem_.A, totalLen, position)) {
		return false;
	}

	//	double deltaN 8
	if (!Hex2Dec(&pdata, gpsephem_.deltaN, totalLen, position)) {
		return false;
	}

	//	double M0 8
	if (!Hex2Dec(&pdata, gpsephem_.M0, totalLen, position)) {
		return false;
	}

	//	double ecc 8
	if (!Hex2Dec(&pdata, gpsephem_.ecc, totalLen, position)) {
		return false;
	}

	//	double omega 8
	if (!Hex2Dec(&pdata, gpsephem_.omega, totalLen, position)) {
		return false;
	}

	//	double cuc 8
	if (!Hex2Dec(&pdata, gpsephem_.cuc, totalLen, position)) {
		return false;
	}

	//	double cus 8
	if (!Hex2Dec(&pdata, gpsephem_.cus, totalLen, position)) {
		return false;
	}

	//	double crc 8
	if (!Hex2Dec(&pdata, gpsephem_.crc, totalLen, position)) {
		return false;
	}

	//	double crs 8
	if (!Hex2Dec(&pdata, gpsephem_.crs, totalLen, position)) {
		return false;
	}

	//	double cic 8
	if (!Hex2Dec(&pdata, gpsephem_.cic, totalLen, position)) {
		return false;
	}

	//	double cis 8
	if (!Hex2Dec(&pdata, gpsephem_.cis, totalLen, position)) {
		return false;
	}

	//	double l0 8
	if (!Hex2Dec(&pdata, gpsephem_.i0, totalLen, position)) {
		return false;
	}

	//	double I 8
	if (!Hex2Dec(&pdata, gpsephem_.dotI, totalLen, position)) {
		return false;
	}

	//	double omega0 8
	if (!Hex2Dec(&pdata, gpsephem_.omega0, totalLen, position)) {
		return false;
	}

	//	double oomega 8
	if (!Hex2Dec(&pdata, gpsephem_.dotOmega, totalLen, position)) {
		return false;
	}

	//	unsigned long iodc 4
	if (!Hex2Dec(&pdata, gpsephem_.iodc, totalLen, position)) {
		return false;
	}

	//	double toc 8
	if (!Hex2Dec(&pdata, gpsephem_.toc, totalLen, position)) {
		return false;
	}

	//	double tgd 8
	if (!Hex2Dec(&pdata, gpsephem_.tgd, totalLen, position)) {
		return false;
	}

	//	double af0 8
	if (!Hex2Dec(&pdata, gpsephem_.af0, totalLen, position)) {
		return false;
	}

	//	double af1 8
	if (!Hex2Dec(&pdata, gpsephem_.af1, totalLen, position)) {
		return false;
	}

	//	double af2 8
	if (!Hex2Dec(&pdata, gpsephem_.af2, totalLen, position)) {
		return false;
	}

	//	antispoofing AS 4
	long temp;
	if (!Hex2Dec(&pdata, temp, totalLen, position)) {
		return false;
	}
	gpsephem_.AS = temp == 0 ? FalseAntispoofing : TrueAntispoofing;

	//	double N 8
	if (!Hex2Dec(&pdata, gpsephem_.N, totalLen, position)) {
		return false;
	}

	//	double URA 8
	if (!Hex2Dec(&pdata, gpsephem_.URA, totalLen, position)) {
		return false;
	}

	std::unordered_map<unsigned long, gpsephem>::iterator it = gpsephems.find(gpsephem_.PRN);
	PRN = gpsephem_.PRN;

	if (it == gpsephems.end()) {
		std::pair<unsigned long, gpsephem> tempgpsephem(gpsephem_.PRN, gpsephem_);
		gpsephems.insert(tempgpsephem);
	}
	else {
		gpsephems.at(gpsephem_.PRN) = gpsephem_;
	}

	return true;
}

//typedef struct ionutc {
//	double a0; // 8
//	double a1; // 8
//	double a2; // 8
//	double a3; // 8
//	double b0; // 8
//	double b1; // 8
//	double b2; // 8
//	double b3; // 8
//	unsigned long utcwn; // 4
//	unsigned long tot; // 4
//	double A0; // 8
//	double A1; // 8
//	unsigned long wnlsf; // 4
//	unsigned long dn; // 4
//	long deltatls; // 4
//	long deltatutc; //4
//};
bool GPSCurriculumDesign::DecodeIonutc(unsigned char* pdata, int headerLen, int messageLen, ionutc& ionutc_)
{
	if (pdata == nullptr) return false;
	if (messageLen == 0) return false;

	int position = headerLen;
	int totalLen = headerLen + messageLen;
	pdata += position;
	//	double a0 8
	if (!Hex2Dec(&pdata, ionutc_.a0, totalLen, position)) {
		return false;
	}

	//	double a1; // 8
	if (!Hex2Dec(&pdata, ionutc_.a1, totalLen, position)) {
		return false;
	}

	//	double a2; // 8
	if (!Hex2Dec(&pdata, ionutc_.a2, totalLen, position)) {
		return false;
	}

	//	double a3; // 8
	if (!Hex2Dec(&pdata, ionutc_.a3, totalLen, position)) {
		return false;
	}

	//	double b0; // 8
	if (!Hex2Dec(&pdata, ionutc_.b0, totalLen, position)) {
		return false;
	}

	//	double b1; // 8
	if (!Hex2Dec(&pdata, ionutc_.b1, totalLen, position)) {
		return false;
	}

	//	double b2; // 8
	if (!Hex2Dec(&pdata, ionutc_.b2, totalLen, position)) {
		return false;
	}

	//	double b3; // 8
	if (!Hex2Dec(&pdata, ionutc_.b3, totalLen, position)) {
		return false;
	}

	//	unsigned long utcwn; // 4
	if (!Hex2Dec(&pdata, ionutc_.utcwn, totalLen, position)) {
		return false;
	}

	//	unsigned long tot; // 4
	if (!Hex2Dec(&pdata, ionutc_.tot, totalLen, position)) {
		return false;
	}

	//	double A0; // 8
	if (!Hex2Dec(&pdata, ionutc_.A0, totalLen, position)) {
		return false;
	}

	//	double A1; // 8
	if (!Hex2Dec(&pdata, ionutc_.A1, totalLen, position)) {
		return false;
	}

	//	unsigned long wnlsf; // 4
	if (!Hex2Dec(&pdata, ionutc_.wnlsf, totalLen, position)) {
		return false;
	}

	//	unsigned long dn; // 4
	if (!Hex2Dec(&pdata, ionutc_.dn, totalLen, position)) {
		return false;
	}

	//	long deltatls; // 4
	if (!Hex2Dec(&pdata, ionutc_.deltatls, totalLen, position)) {
		return false;
	}

	//	long deltatutc; //4
	if (!Hex2Dec(&pdata, ionutc_.deltatutc, totalLen, position)) {
		return false;
	}

	return true;

}


//typedef struct rawephem {
//	unsigned long prn; //4
//unsigned long refweek; // 4
//unsigned long refsec; // 4
//subframe subframe1; // 30
//subframe subframe2; // 30
//subframe subframe3; // 30
//};

//bool GPSCurriculumDesign::DecodeRawEphem(unsigned char* pdata, int headerLen, int messageLen, std::unordered_map<unsigned long, rawephem>& rawephems)
//{
//	rawephem rawephem_;
//	if (pdata == nullptr) return false;
//	if (messageLen == 0) return false;
//
//	int position = headerLen;
//	int totalLen = headerLen + messageLen;
//	pdata += position;
//
//	//unsigned long prn; //4
//	if (!Hex2Dec(&pdata, rawephem_.prn, totalLen, position)) {
//		return false;
//	}
//
//	//unsigned long refweek; // 4
//	if (!Hex2Dec(&pdata, rawephem_.refweek, totalLen, position)) {
//		return false;
//	}
//
//	//unsigned long refsec; // 4
//	if (!Hex2Dec(&pdata, rawephem_.refsec, totalLen, position)) {
//		return false;
//	}
//
//	memcpy(&rawephem_.subframe1, pdata, sizeof(unsigned char) * 30);
//	pdata += sizeof(unsigned char) * 30;
//
//	memcpy(&rawephem_.subframe2, pdata, sizeof(unsigned char) * 30);
//	pdata += sizeof(unsigned char) * 30;
//
//	memcpy(&rawephem_.subframe3, pdata, sizeof(unsigned char) * 30);
//	pdata += sizeof(unsigned char) * 30;
//	////subframe subframe1; // 30
//	//if (!Hex2Dec(&pdata, rawephem_.subframe1, totalLen, position)) {
//	//	return false;
//	//}
//
//	////subframe subframe2; // 30
//	//if (!Hex2Dec(&pdata, rawephem_.subframe2, totalLen, position)) {
//	//	return false;
//	//}
//
//	////subframe subframe3; // 30
//	//if (!Hex2Dec(&pdata, rawephem_.subframe3, totalLen, position)) {
//	//	return false;
//	//}
//	
//	std::unordered_map<unsigned long, rawephem>::iterator it = rawephems.find(rawephem_.prn);
//
//	if (it == rawephems.end()) {
//		std::pair<unsigned long, rawephem> temprawephem(rawephem_.prn, rawephem_);
//		rawephems.insert(temprawephem);
//	}
//
//	rawephems.at(rawephem_.prn) = rawephem_;
//
//	return true;
//}

//bool GPSCurriculumDesign::DecodePsrPos(unsigned char* pdata, int headerLen, int messageLen, psrPOS& psrPos_)
//{
//	if (pdata == nullptr) return false;
//	if (messageLen == 0) return false;
//
//	int position = headerLen;
//	int totalLen = headerLen + messageLen;
//	pdata += position;
//	//	enum solStat 4
//	if (!Hex2Dec(&pdata, psrPos_.solStat, totalLen, position)) {
//		return false;
//	}
//
//	//	enum posType 4
//	if (!Hex2Dec(&pdata, psrPos_.posType, totalLen, position)) {
//		return false;
//	}
//
//	//	double lat 8
//	if (!Hex2Dec(&pdata, psrPos_.lat, totalLen, position)) {
//		return false;
//	}
//
//	//	double lon 8
//	if (!Hex2Dec(&pdata, psrPos_.lon, totalLen, position)) {
//		return false;
//	}
//
//	//	double hgt 8
//	if (!Hex2Dec(&pdata, psrPos_.hgt, totalLen, position)) {
//		return false;
//	}
//
//	return true;
//}

bool GPSCurriculumDesign::CheckCRC(FILE* FObs, const unsigned char* pdata, const int len)
{
	const unsigned char* pdata_ = pdata;
	unsigned char buff[MAXRAWLEN];

	if (fread(buff, 4 * sizeof(unsigned char), 1, FObs) < 1) {
		return false;
	}

	unsigned int crcFormate = (unsigned int)(buff[0] | buff[1] << 8 | buff[2] << 16 | buff[3] << 24);
	if (crcFormate == crc32(pdata, len)) {
		pdata = pdata_;
		return true;
	}
	pdata = pdata_;
	return false;
}

unsigned int GPSCurriculumDesign::crc32(const unsigned char *buff, int len)
{
	int i, j;
	unsigned int crc = 0;
	for (i = 0; i < len; i++)
	{
		crc ^= buff[i];
		for (j = 0; j < 8; j++)
		{
			if (crc & 1) crc = (crc >> 1) ^ POLYCRC32;
			else crc >>= 1;
		}
	}
	return crc;
}

bool GPSCurriculumDesign::RollByte(unsigned char input, int s, unsigned char& output)
{
	if (input > 128 || input < 63) {
		return false;
	}
	
	output = input << 1;
	//µûÊ½½»»»
	output = (output << 4) | (output >> 4);
	output = ((output << 2) & 0xcc) | ((output >> 2) & 0x33);
	output = ((output << 1) & 0xaa) | ((output >> 1) & 0x55);

	output = (output >> 1);
	if (s == 1) {
		//output = (output >= 0)? output : output + 0x01;
		output = ~output;
	}
	output = output & 0x3f;
	return true;
}

bool GPSCurriculumDesign::StitchingWord(std::string input, std::string& output)
{
	unsigned char in[5] = "\0";
	unsigned char out[3] = "\0";
	input.copy((char*)in, 5, 0);
	out[0] =
		(in[0] << 2 & 0xfc) | (in[1] >> 4 & 0x03);
	out[1] = 
		(in[1] << 4 & 0xf0) | (in[2] >> 2 & 0x0f);
	out[2] = 
		(in[2] << 4 & 0xc0) | (in[3] & 0x3f);
	output = std::string((char*)out);
	return true;
}

bool GPSCurriculumDesign::CheckParity(unsigned char * input)
{
	char b[30] = "\0";
	char result[6] = "\0";
	unsigned char temp = input[4];

	if (byte30th == 1) {
		temp = (~input[4]) & 0x3f;
	}

	for (int i = 0; i < 24; i++) {
		b[i] = (input[i / 6] >> (23 - i) % 6) & 0x01;
	}

	b[24] = (temp >> 5) & 0x01;
	b[25] = (temp >> 4) & 0x01;
	b[26] = (temp >> 3) & 0x01;
	b[27] = (temp >> 2) & 0x01;
	b[28] = (temp >> 1) & 0x01;
	b[29] = (temp) & 0x01;


	result[0] = b[0] ^ b[1] ^ b[2] ^ b[4] ^ b[5] ^ b[9] ^ b[10] ^ b[11] ^ b[12] ^ b[13] ^ b[16] ^ b[17] ^ b[19] ^ b[22] ^ byte29th;
	result[1] = b[1] ^ b[2] ^ b[3] ^ b[5] ^ b[6] ^ b[10] ^ b[11] ^ b[12] ^ b[13] ^ b[14] ^ b[17] ^ b[18] ^ b[20] ^ b[23] ^ byte30th;
	result[2] = b[0] ^ b[2] ^ b[3] ^ b[4] ^ b[6] ^ b[7] ^ b[11] ^ b[12] ^ b[13] ^ b[14] ^ b[15] ^ b[18] ^ b[19] ^ b[21] ^ byte29th;
	result[3] = b[1] ^ b[3] ^ b[4] ^ b[5] ^ b[7] ^ b[8] ^ b[12] ^ b[13] ^ b[14] ^ b[15] ^ b[16] ^ b[19] ^ b[20] ^ b[22] ^ byte30th;
	result[4] = b[0] ^ b[2] ^ b[4] ^ b[5] ^ b[6] ^ b[8] ^ b[9] ^ b[13] ^ b[14] ^ b[15] ^ b[16] ^ b[17] ^ b[20] ^ b[21] ^ b[23] ^ byte30th;
	result[5] = b[2] ^ b[4] ^ b[5] ^ b[7] ^ b[8] ^ b[9] ^ b[10] ^ b[12] ^ b[14] ^ b[18] ^ b[21] ^ b[22] ^ b[23] ^ byte29th;

	byte29th = b[28];
	byte30th = b[29];

	if (b[24] == result[0]
		&& b[25] == result[1]
		&& b[26] == result[2]
		&& b[27] == result[3]
		&& b[28] == result[4]	
		&& b[29] == result[5]) {
		return true;
	}
	return false;
}

bool GPSCurriculumDesign::DecodeDGPS(unsigned char* pdata,int length,
	 DGPSHead& dgpsHead, std::unordered_map<char,DGPSMessage>& dgpsmessages)
{
	byte29th = 1;
	byte30th = 1;
	if (length < 6) return false;
	DGPSMessage dgpsmessage;
	unsigned char wordBits[5] = "\0";
	unsigned char oneword[3] = "\0";
	unsigned char parityControl = '0';
	int position = 0;
	
	short bit1 = 0;
	short bit2 = 0;
	
	double scale = 0;

	for (int i = 0; i < 4; i++) {
		if (!RollByte(*(pdata + position), byte30th, wordBits[i])) {
			position++;
			continue;
		}
		position++;
	}

	while (position < length - 10) {
		
		if (!RollByte(*(pdata + position), byte30th, wordBits[4])) {
			position++;
			continue;
		}
		position++;

		unsigned char preamble =
			(wordBits[0] << 2 & 0xfc) | (wordBits[1] >> 4 & 0x03);
		
		//find preamble and check parity of the 1st word
		if (preamble != 102 && preamble != 153 || !CheckParity(wordBits)) {
			wordBits[0] = wordBits[1];
			wordBits[1] = wordBits[2];
			wordBits[2] = wordBits[3];
			wordBits[3] = wordBits[4];
			continue;
		}

		
		//decode the 1st word in the message head
		dgpsHead.messageType = ((wordBits[1] << 2) & 0x3c) | ((wordBits[2] >> 4) & 0x03);
		if (dgpsHead.messageType != 1) 
		{ 
			continue; 
		}

		bit1 = (wordBits[2] << 2) & 0x03;
		bit2 = ((wordBits[2] << 6) & 0xc0) | (wordBits[3]) & 0x3f;
		dgpsHead.baseStation = bit1 * 256 + bit2;

		//check parity of 2nd word in the message head and decode it
		for (int i = 0; i < 5; i++) {
			if (!RollByte(*(pdata + position), byte30th, wordBits[i])) {
				position++;
				continue;
			}
			position++;
		}

		if (!CheckParity(wordBits)) {
			continue;
		}

		bit1 = (wordBits[0] >> 1) & 0x1f;
		bit2 = ((wordBits[0] << 7) & 0x70) 
			| ((wordBits[1] << 1) & 0x3f) 
			| ((wordBits[2] >> 5) & 0x01);

		dgpsHead.zCounter = 0.6 * (bit1 * 256 + bit2);
		dgpsHead.numOfSeq = (wordBits[2] >> 2) & 0x07;
		dgpsHead.lengthOfMessage = ((wordBits[2] << 3) & 0x18) 
			| ((wordBits[3] >> 3) & 0x07);
		dgpsHead.healthState = (wordBits[3]) & 0x07;

		//decode message data
		if (position + dgpsHead.lengthOfMessage > length + 1) {
			continue;
		}
		int satelliteNum = 0;
		int findSatelliteNum = 0;
		switch ((dgpsHead.lengthOfMessage - 2) % 5) {
		case 0:
			satelliteNum = 0;
			break;
		case 2:
			satelliteNum = 1;
			break;
		case 4:
			satelliteNum = 2;
			break;
		default:
			satelliteNum = 0;
		}
		satelliteNum += (dgpsHead.lengthOfMessage - 2) / 5 * 3;
		for (int i = 0; i < (dgpsHead.lengthOfMessage - 2) / 5 + 1; i++) {
			
			/*--------------------1st word-------------------------------*/
			for (int i = 0; i < 5; i++) {
				if (!RollByte(*(pdata + position), byte30th, wordBits[i])) {
					position++;
					continue;
				}
				position++;
			}

			if (!CheckParity(wordBits)) {
				break;
			}

			dgpsmessage.s = (wordBits[0] >> 5) & 0x01;
			if (dgpsmessage.s == 1) {
				scale = 0.32;
			}
			else if (dgpsmessage.s == 0) {
				scale = 0.02;
			}
			else {
				continue;
			}

			dgpsmessage.UDRE = (wordBits[0] >> 3) & 0x03;
			dgpsmessage.NumOfSatellite = 
				((wordBits[0] << 2) & 0x1c) | ((wordBits[1] >> 4) & 0x03);
			
			
			bit1 = ((wordBits[1] << 4) & 0x00f0) | ((wordBits[2] >> 2) & 0x000f);
			bit2 = ((wordBits[2] << 6) & 0x00c0) | ((wordBits[3]) & 0x003f);
			
			dgpsmessage.pseCorrect = scale * (short)((bit1 << 8) & 0xff00 | (bit2 & 0x00ff));
			
			/*---------------------------2nd word--------------------*/
			for (int i = 0; i < 5; i++) {
				if (!RollByte(*(pdata + position), byte30th, wordBits[i])) {
					position++;
					continue;
				}
				position++;
			}

			if (!CheckParity(wordBits)) {
				break;
			}
	
			bit1 = ((wordBits[0] << 2) & 0xfc) |((wordBits[1] >> 4) & 0x03);
			dgpsmessage.rangeRateCorrect = scale * (char)bit1 * 0.1;
			bit1 = ((wordBits[1] << 4) & 0xf0) | ((wordBits[2] >> 2) & 0x0f);
			dgpsmessage.ageOfData =  (char)bit1;
			//1st satellite of 5 words
			findSatelliteNum++;
			
			std::unordered_map<char, DGPSMessage>::iterator it 
				= dgpsmessages.find(dgpsmessage.NumOfSatellite);

			if (it == dgpsmessages.end()) {
				std::pair<char, DGPSMessage> tempgpsephem(
					dgpsmessage.NumOfSatellite, dgpsmessage);
				dgpsmessages.insert(tempgpsephem);
			}
			else {
				dgpsmessages.at(dgpsmessage.NumOfSatellite) = dgpsmessage;
			}
			
			if (findSatelliteNum >= satelliteNum) {
				break;
			}
			dgpsmessage.s = (wordBits[2] >> 1) & 0x01;
			if (dgpsmessage.s == 1) {
				scale = 0.32;
			}
			else if (dgpsmessage.s == 0) {
				scale = 0.02;
			}
			else {
				continue;
			}

			dgpsmessage.UDRE = ((wordBits[2] << 1) & 0x02) | ((wordBits[3] >> 5) & 0x01);
			dgpsmessage.NumOfSatellite = wordBits[3] & 0x1f;

			/*-----------------------3rd word---------------------------*/
			for (int i = 0; i < 5; i++) {
				if (!RollByte(*(pdata + position), byte30th, wordBits[i])) {
					position++;
					continue;
				}
				position++;
			}

			if (!CheckParity(wordBits)) {
				break;
			}

			bit1 = ((wordBits[0] << 2) & 0x00fc) | ((wordBits[1] >> 4) & 0x0003);
			bit2 = ((wordBits[1] << 4) & 0x00f0) | ((wordBits[2] >> 2) & 0x000f);
			dgpsmessage.pseCorrect = scale * (short)((bit1 << 8) & 0xff00 | (bit2 & 0x00ff));
			
			bit1 = ((wordBits[2] << 6) & 0xc0) | ((wordBits[3]) & 0x3f);
			dgpsmessage.rangeRateCorrect = scale * (char)bit1 * 0.1;
			
			/*------------------4th word----------------------------*/
			for (int i = 0; i < 5; i++) {
				if (!RollByte(*(pdata + position), byte30th, wordBits[i])) {
					position++;
					continue;
				}
				position++;
			}

			if (!CheckParity(wordBits)) {
				continue;
			}

			bit1 = ((wordBits[0] << 2) & 0xfc) | ((wordBits[1] >> 4) & 0x03);
			dgpsmessage.ageOfData =  (char)bit1;
			//   2nd satellite of 5 word
			findSatelliteNum++;
			 it = dgpsmessages.find(dgpsmessage.NumOfSatellite);

			if (it == dgpsmessages.end()) {
				std::pair<char, DGPSMessage> tempgpsephem(
					dgpsmessage.NumOfSatellite, dgpsmessage);
				dgpsmessages.insert(tempgpsephem);
			}
			else {
				dgpsmessages.at(dgpsmessage.NumOfSatellite) = dgpsmessage;
			}
			if (findSatelliteNum >= satelliteNum) {
				break;
			}

			dgpsmessage.s = (wordBits[1] >> 3) & 0x01;
			if (dgpsmessage.s == 1) {
				scale = 0.32;
			}
			else if (dgpsmessage.s == 0) {
				scale = 0.02;
			}
			else {
				continue;
			}

			dgpsmessage.UDRE = (wordBits[1] >> 1) & 0x03;
			dgpsmessage.NumOfSatellite = ((wordBits[1] << 4) & 0x10) | ((wordBits[2] >> 2) & 0x0f);

			bit1 = ((wordBits[2] << 6) & 0x00c0) | ((wordBits[3]) & 0x003f);

			/*--------------------------5th word------------------------*/
			for (int i = 0; i < 5; i++) {
				if (!RollByte(*(pdata + position), byte30th, wordBits[i])) {
					position++;
					continue;
				}
				position++;
			}

			if (!CheckParity(wordBits)) {
				continue;
			}

			bit2 = ((wordBits[0] << 2) & 0x00fc) | ((wordBits[1] >> 4) & 0x0003);
			dgpsmessage.pseCorrect = scale * (short)((bit1 << 8) & 0xff00 | (bit2 & 0x00ff));

			bit1 = ((wordBits[1] << 4) & 0xf0) | ((wordBits[2] >> 2) & 0x0f);
			dgpsmessage.rangeRateCorrect = scale * (char)bit1 * 0.1;

			bit1 = ((wordBits[2] << 6) & 0xc0) | (wordBits[3] & 0x3f);
			dgpsmessage.ageOfData = (char)bit1;
			// 3rd satellite of 5 word;
			findSatelliteNum++;
			it = dgpsmessages.find(dgpsmessage.NumOfSatellite);

			if (it == dgpsmessages.end()) {
				std::pair<char, DGPSMessage> tempgpsephem(
					dgpsmessage.NumOfSatellite, dgpsmessage);
				dgpsmessages.insert(tempgpsephem);
			}
			else {
				dgpsmessages.at(dgpsmessage.NumOfSatellite) = dgpsmessage;
			}
			if (findSatelliteNum >= satelliteNum) {
				break;
			}
		}
	}

	return true;
}

//inline bool GPSDCurriculumDesign::Hex2Char(unsigned char* pdata, char& c, int totalLen, int& position)
//{
//	if (pdata == nullptr) return false;
//	if (position + sizeof(char) > totalLen) return false;
//	c = *((char*)pdata);
//	position += sizeof(char);
//	pdata += sizeof(char);
//	return true;
//}
//
//inline bool GPSDCurriculumDesign::Hex2UChar(unsigned char* pdata, unsigned char& uc, int totalLen, int& position)
//{
//	if (pdata == nullptr) return false;
//	if (position + sizeof(unsigned char) > totalLen) return false;
//	uc = *((unsigned char*)pdata);
//	position += sizeof(char);
//	pdata += sizeof(char);
//	return true;
//}
//
//inline bool GPSDCurriculumDesign::Hex2Short(unsigned char* pdata, short& s, int totalLen, int& position)
//{
//	if (pdata == nullptr) return false;
//	if (position + sizeof(short) > totalLen) return false;
//	s = *((short*)pdata);
//	position += sizeof(short);
//	pdata += sizeof(short);
//	return true;
//}
//
//inline bool GPSDCurriculumDesign::Hex2UShort(unsigned char* pdata, unsigned short& us, int totalLen, int& position)
//{
//	if (pdata == nullptr) return false;
//	if (position + sizeof(unsigned short) > totalLen) return false;
//	us = *((unsigned short*)pdata);
//	position += sizeof(unsigned short);
//	pdata += sizeof(unsigned short);
//	return true;
//}
//
//inline bool GPSDCurriculumDesign::Hex2Int(unsigned char* pdata, int& i, int totalLen, int& position)
//{
//	if (pdata == nullptr) return false;
//	if (position + sizeof(int) > totalLen) return false;
//	i = *((int*)pdata);
//	position += sizeof(int);
//	pdata += sizeof(int);
//	return true;
//}
//
//inline bool GPSDCurriculumDesign::Hex2UInt(unsigned char* pdata, unsigned int& ui, int totalLen, int& position)
//{
//	if (pdata == nullptr) return false;
//	if (position + sizeof(unsigned int) > totalLen) return false;
//	ui = *((unsigned int*)pdata);
//	position += sizeof(unsigned int);
//	pdata += sizeof(unsigned int);
//	return true;
//}
//
//inline bool GPSDCurriculumDesign::Hex2Long(unsigned char* pdata, long& l, int totalLen, int& position)
//{
//	if (pdata == nullptr) return false;
//	if (position + sizeof(long) > totalLen) return false;
//	l = *((long *)pdata);
//	position += sizeof(long);
//	pdata += sizeof(long);
//	return true;
//}
//
//inline bool GPSDCurriculumDesign::Hex2ULong(unsigned char* pdata, unsigned long& ul, int totalLen, int& position)
//{
//	if (pdata == nullptr) return false;
//	if (position + sizeof(unsigned long) > totalLen) return false;
//	ul = *((unsigned long *)pdata);
//	position += sizeof(unsigned long);
//	pdata += sizeof(unsigned long);
//	return true;
//}
//
//inline bool GPSDCurriculumDesign::Hex2Float(unsigned char* pdata, float& f, int totalLen, int& position)
//{
//	if (pdata == nullptr) return false;
//	if (position + sizeof(float) > totalLen) return false;
//	f = *((float *)pdata);
//	position += sizeof(float);
//	pdata += sizeof(float);
//	return true;
//}
//
//inline bool GPSDCurriculumDesign::Hex2Double(unsigned char* pdata, double& d, int totalLen, int& position)
//{
//	if (pdata == nullptr) return false;
//	if (position + sizeof(double) > totalLen) return false;
//	d = *((double *)pdata);
//	position += sizeof(double);
//	pdata += sizeof(double);
//	return true;
//}


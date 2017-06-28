// DecodeFile.cpp : 定义控制台应用程序的入口点。
//

#include "../include/DGPS_CHRT/GPSCurriculumDesign.h"
#include "../3rdparty/Serial.h"
#include "../3rdparty/sockets.h"
#include <Windows.h>
#include <fstream>
#include <iostream>

#define LOGOBSP

#define BUFFSIZE 2048

using namespace GPSCurriculumDesign;

FILE* GPSCurriculumDesign::fout = nullptr;
int GPSCurriculumDesign::byte29th = 1;
int GPSCurriculumDesign::byte30th = 1;

int main()
{
	//FILE* fout;
	if ((fout = fopen(std::string(".\\data\\ObsPos.txt").c_str(), "wb")) == NULL) {
		return 0;
	}

	DGPSHead dgpsHead;
	std::unordered_map<unsigned long, gpsephem> gpsephems;
	std::unordered_map<char, DGPSMessage> dgpsmessages;

	gpsephems.reserve(32);
	dgpsmessages.reserve(32);
	//std::unordered_map<unsigned long, rawephem> rawephems;
	ionutc ionutc_;
	head header;
	std::vector<range> ranges;
	std::vector<satellitePandV> satellitePandVs;
	observationPAndV obsPandV = { 0 , 0 , 0 , 0 , 0 , 0 , 0, 0 };
	//observationPAndV obsPandV = { -2248554.290 , 5050335.624 , 3170387.565 , 0 , 0 , 0 };
	/*zero for not update this epoch and one for updated in this epoch*/
	//int PRNIndex[33] = { 0 };
	int count = 0;
	bool getIonutc = false;
	bool getGPSEphem = false;

#ifdef OPENCOM

	CSerial serial;
	if (!serial.Open(5, 115200)) {
		std::cout << "COM open failed" << std::endl;
		return 1;
	}

	char* log = "unlogall\n       ";
	while (!serial.SendData(log, 16)) {
		Sleep(2000);
	}
	Sleep(2000);
	/*--------------get and decode ephem----------------------*/
	{
		unsigned char ephemBuffer[20480];
		unsigned char aLittleBuffer[1024];
		log = "log gpsephemb onchanged\n        ";
		while (!serial.SendData(log, 32)) {
			Sleep(2000);
		}
		Sleep(2000);
		int lengthOfALittleBuffer = 0;
		int positionOfEphemBuffer = 0;
		while ((lengthOfALittleBuffer = serial.ReadData(aLittleBuffer, 1024)) != 0) {
			memcpy(ephemBuffer + positionOfEphemBuffer, aLittleBuffer,
				lengthOfALittleBuffer);
			positionOfEphemBuffer += lengthOfALittleBuffer;
		}

		//fwrite(ephemBuffer, positionOfEphemBuffer, 1, fout);

		int sizeOfEphem = positionOfEphemBuffer;
		int headOfEphemMessage = 0;
		positionOfEphemBuffer = 0;
		unsigned char* _ephemBuffer = nullptr;
		int movePos = 0;
		while (positionOfEphemBuffer <= sizeOfEphem) {
			positionOfEphemBuffer += movePos;
			bool getHeader = Findhead(ephemBuffer + positionOfEphemBuffer, sizeOfEphem - positionOfEphemBuffer, headOfEphemMessage)
				&& DecodeHead(
					_ephemBuffer = ephemBuffer
					+ positionOfEphemBuffer
					+ headOfEphemMessage,
					header);
			if (!getHeader) {
				std::cout << "find no head of ephem" << std::endl;
				continue;;
			}
			/*if (positionOfEphemBuffer < header.headLength + header.messageLength) {
				std::cout << "something wrong with ephem message" << std::endl;
				return 1;}
			*/


			movePos = header.messageLength + header.headLength - 3;
			unsigned int crcFormate =
				(unsigned int)(*(_ephemBuffer + movePos)
					| *(_ephemBuffer + movePos + 1) << 8
					| *(_ephemBuffer + movePos + 2) << 16
					| *(_ephemBuffer + movePos + 3) << 24);
			//unsigned char* _gpsdata = gpsData;
			if (crcFormate != crc32(_ephemBuffer - 3, header.messageLength + header.headLength))
			{
				std::cout << "crc32 correct not past" << std::endl;
				continue;
			}

			if (header.messageID == 7) {
				//GPSEPHEM
				unsigned long PRN = 0;
				getGPSEphem = DecodeGPSEphem(
					_ephemBuffer - 3,
					header.headLength,
					header.messageLength,
					gpsephems, PRN);
				//std::cout << PRN << std::endl;
			}
			
		}
	}
	/*---------------end the decode of ephem--------------------*/

	log = "log ionutcb onchanged\n          ";
	while (!serial.SendData(log, 32)) {
		Sleep(2000);
	}
	Sleep(2000);

	log = "log rangeb ontime 1\n            ";
	while (!serial.SendData(log, 32)) {
		Sleep(2000);
	}
	Sleep(2000);


	log = "log psrposb ontime 2\n           ";
	while (!serial.SendData(log, 32)) {
		Sleep(2000);
	}
	Sleep(2000);

#endif

#ifdef OPENDGPS

	SOCKET socket;
	if (!OpenDGPSSocket(socket)) {
		std::cout << "socket open failed" << std::endl;
		return 1;
	}
#endif	
	std::cout << "init successed" << std::endl;
	unsigned char gpsData[GPSDATASIZE];
	unsigned char dgpsData[DGPSDATASIZE];

	int gpsBufferLength = 0;
	int dgpsBufferLength = 0;
	int headPosition = 0;


	/*----------------while start here---------------------*/
	while (1) {
		Sleep(1000);

#ifdef OPENCOM
		gpsBufferLength = serial.ReadData(gpsData, GPSDATASIZE);
#endif

#ifdef OPENDGPS
		dgpsBufferLength = recv(socket, (char*)dgpsData, DGPSDATASIZE, 0);
#endif
		std::cout << gpsBufferLength << " " << dgpsBufferLength << std::endl;
		if (gpsBufferLength == 0) {
			continue;
		}

		if (dgpsBufferLength != 0) {
			bool decodeDGPS = DecodeDGPS((unsigned char*)dgpsData,
				dgpsBufferLength, dgpsHead, dgpsmessages);
		}
		/*if (dgpsBufferLength == 0) {
			for (int i = 0; i < dgpsMessages.size(); i++) {
				std::cout << (int)dgpsMessages[i].NumOfSatellite << " ";
			}
			std::cout << std::endl;
		}*/

		unsigned char* _gpsData = nullptr;
		bool getHeader = Findhead(gpsData, gpsBufferLength, headPosition)
			&& DecodeHead(_gpsData = gpsData + headPosition, header);
		if (!getHeader) {
			std::cout << "find no head" << std::endl;
			continue;
		}
		if (gpsBufferLength < header.headLength + header.messageLength) {
			continue;
		}

		int movePos = header.messageLength + header.headLength - 3;
		unsigned int crcFormate =
			(unsigned int)(*(_gpsData + movePos)
				| *(_gpsData + movePos + 1) << 8
				| *(_gpsData + movePos + 2) << 16
				| *(_gpsData + movePos + 3) << 24);
		//unsigned char* _gpsdata = gpsData;
		if (crcFormate != crc32(_gpsData - 3, header.messageLength + header.headLength))
		{
			std::cout << "crc32 correct not past" << std::endl;
			continue;
		}
		std::cout << header.messageID << std::endl;
		//gpsData = _gpsdata;

		if (header.messageID == 7) {
			//GPSEPHEM
			unsigned long PRN = 0;
			getGPSEphem = DecodeGPSEphem(
				_gpsData - 3,
				header.headLength,
				header.messageLength,
				gpsephems, PRN);
			//std::cout << PRN << std::endl;
		}
		else if (header.messageID == 8) {
			//IONUTC
			getIonutc = DecodeIonutc(
				_gpsData - 3,
				header.headLength,
				header.messageLength,
				ionutc_);
		}
		//else if (header.messageID == 41) {
		//	//RAWEPHEM
		//	bool getRawEphem = DecodeRawEphem(
		//		pdata,
		//		header.headLength,
		//		header.messageLength,
		//		rawephems
		//	);
		//}

		else if (header.messageID == 43) {
			//RANGE
			ranges.clear();
			bool getRange = DecodeRange(
				_gpsData - 3,
				header.headLength,
				header.messageLength,
				ranges);
			//double x, y, z, Vx, Vy, Vz;


			if (!(getRange && getGPSEphem/* && getIonutc*/)) {
				continue;
			}

			std::cout << ". ";
			//if (count == 2700)
			//	std::cout << " stop here ";
			if (!ObservationPositionAndVelocity(
				ionutc_,
				gpsephems,
				dgpsmessages,
				ranges,
				header.week,
				header.gpsSecondBin / 1000, dgpsHead.zCounter,
				satellitePandVs,
				obsPandV)) {
				continue;
			}
			count++;
			std::cout << "locating succeed" << std::endl;
#ifdef LOGOBSP
			XYZ xyz = { obsPandV.x, obsPandV.y, obsPandV.z };
			BLH blh;
			XYZ2BLH(xyz, blh);
			fprintf(fout, "%015.7f %015.7f %015.7f %015.7f %015.7f %015.7f %015.7f\n",
				blh.longitude * 180 / PI + 180, blh.latitude * 180 / PI, blh.height,
				obsPandV.vx, obsPandV.vy, obsPandV.vz, obsPandV.pdop);
#endif
			ranges.clear();
			satellitePandVs.clear();
		}
		//else if (header.messageID == 47) {
		//	psrPOS psrPOS_;
		//	bool getBestPos = DecodePsrPos(pdata,
		//		header.headLength,
		//		header.messageLength,
		//		psrPOS_);
		//	/*if (getBestPos) {
		//		std::cout << psrPOS_.lat
		//			<< " " << psrPOS_.lon
		//			<< " " << psrPOS_.hgt << std::endl;
		//	}*/
		//}
		//else if (header.messageID == 140) {
		//	//RANGECMP
		//}
	}
	fclose(fout);
	system("pause");
	return 0;
}

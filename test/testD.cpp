#include "../include/DGPS_CHRT/GPSCurriculumDesign.h"
#include "../3rdparty/Serial.h"
#include "../3rdparty/sockets.h"
#include <Windows.h>
#include <fstream>
#include <iostream>

//#define LOGOBSP
//#define OPENCOM
#define OPENDGPS
#define BUFFSIZE 2048

using namespace GPSCurriculumDesign;

FILE* GPSCurriculumDesign::fout = nullptr;
int GPSCurriculumDesign::byte29th = 1;
int GPSCurriculumDesign::byte30th = 1;

int not_main()
{

	//unsigned char buff[MAXRAWLEN];
	FILE* FObs;
	//byte29th = byte30th = 1;
	DGPSHead dgpsHead;
	std::vector<DGPSMessage> dgpsMessages;
	dgpsMessages.resize(1);
	std::string  fileName = ".\\data\\20160620.bin.rtcm";
	if ((FObs = fopen(fileName.c_str(), "rb")) == NULL) {
		printf("Cannot open GPS obs file.\n");
		return 0;
	}
#ifdef OPENCOM

	CSerial serial;
	if (!serial.Open(4, 115200)) {
		std::cout << "COM open failed" << std::endl;
		return 1;
	}

	char* log = "unlogall\n";
	serial.SendData(log,9);
	Sleep(1000);

	log = "log gpsephemb onchanged\n";
	serial.SendData(log, 24);
	Sleep(1000);

	log = "log rangeb ontime 1\n";
	serial.SendData(log, 24);
	Sleep(1000);

	log = "log psrposb ontime 1\n";
	serial.SendData(log, 24);
	Sleep(1000);
#endif

#ifdef OPENDGPS

	SOCKET socket;
	if (!OpenDGPSSocket(socket)) {
		std::cout << "socket open failed" << std::endl;
		return 1;
	}
#endif	


	unsigned char buff[BUFFSIZE];
	int bufferLength = 0;
	while (1) {
		/*if (fread(buff, sizeof(unsigned char), 1024, FObs) < 1) {
			continue;
		}*/
#ifdef OPENCOM
		serial.ReadData(buff, 1024);
#endif

#ifdef OPENDGPS
		bufferLength = recv(socket, (char*)buff, 1024, 0);
#endif
		
		bool decodeDGPS = DecodeDGPS((unsigned char*)buff, bufferLength, dgpsMessages, dgpsHead);
		for (int i = 0; i < dgpsMessages.size(); i++) {
			std::cout << dgpsHead.zCounter << " "
				<< (int)dgpsMessages[i].NumOfSatellite<< " "
				<< (double)dgpsMessages[i].pseCorrect << " "
				<< (double)dgpsMessages[i].rangeRateCorrect << " "
				<< (int)dgpsMessages[i].UDRE << " "
				<< (int)dgpsMessages[i].ageOfData
				<< std::endl <<" " << std::endl;
		}
	}
	fclose(FObs);
	system("pause");
	return 0;
}

#ifndef SMSMANAGER_H_
#define SMSMANAGER_H_

#pragma warning(disable:4996)
#include <iostream>
#include <conio.h>
#include <string>
#include <vector>

#define FILE_NUMBER "./phoneNumber.txt"

class SMSmanager {
private:
	std::vector<std::string> numberList;
	std::string typeName; // CallBack 관련 기기 타입명
	std::string appName;  // CallBack 관련 기기명
public:
	SMSmanager();
	~SMSmanager();
	void showList();
	void addNumber();
	void delNumber();
	void setName(const std::string& typeName, const std::string& appName);
	void setTypeName(const std::string& typeName);
	void setAppName(const std::string& appName);
	void sendMsg(const std::string& str);
	const bool checkNumberDup(const std::string& number);
	static SMSmanager& getInstance();
	static const bool bufCheck();
};

#endif


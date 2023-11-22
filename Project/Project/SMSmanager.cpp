#include "SMSmanager.h"

// 생성 시 기 저장된 전화번호 목록 Load 후 저장
SMSmanager::SMSmanager() {
	FILE* fp;
	char phoneNumber[20];
	fp = fopen(FILE_NUMBER, "rt");
	if (fp != nullptr) {
		while (fgets(phoneNumber, sizeof(phoneNumber), fp) != nullptr) {
			phoneNumber[strlen(phoneNumber) - 1] = '\0';
			numberList.push_back(static_cast<std::string>(phoneNumber));
		}
		fclose(fp);
	}
}
// 소멸 시 전화번호 목록 저장
SMSmanager::~SMSmanager() {
	FILE* fp;
	fp = fopen(FILE_NUMBER, "wt");
	if (fp != nullptr) {
		for (auto& pn : numberList) {
			pn += '\n';
			fputs(pn.c_str(), fp);
		}
		fclose(fp);
	}
}
// 저장된 번호 목록 출력
void SMSmanager::showList() {
	for (int i = 0; i < numberList.size(); i++) {
		std::cout << i + 1 << ". " << numberList[i] << std::endl;
	}
}
// 휴대 전화 번호 저장
void SMSmanager::addNumber() {
	std::string number;
	while (true) {
		system("cls");
		showList();
		std::cout << "---------------------------------------------" << std::endl;
		std::cout << "휴대전화 번호를 \"010\"을 제외한 숫자 8자리만 입력하세요 (종료 : 0) >> ";
		std::cin >> number;
		if (bufCheck()) continue;
		if (number.length() == 1 && number[0] == '0') return;
		if (number.length() != 8) {
			std::cout << "입력 범위 오류! 다시 진행하려면 아무 버튼이나 입력하세요." << std::endl;
			getch();
			continue;
		}
		if (checkNumberDup(number)) continue;
		for (int i = 0; i < number.length(); i++) {
			if (isdigit(number[i]) == false) {
				std::cout << "입력 오류! 다시 진행하려면 아무 버튼이나 입력하세요." << std::endl;
				getch();
				continue;
			}
		}
		number = "010" + number;
		break;
	}
	numberList.push_back(number);
	std::cout << number << " 번호 등록 완료! 다시 진행하려면 아무 버튼이나 입력하세요." << std::endl;
	getch();
}
// 휴대 전화 번호 삭제
void SMSmanager::delNumber() {
	if (numberList.size() == 0) return;
	std::string number;
	int cmd;
	while (true) {
		system("cls");
		showList();
		std::cout << "---------------------------------------------" << std::endl;
		std::cout << "삭제할 번호를 선택하세요 (종료 : 0) >> ";
		std::cin >> cmd;
		if (bufCheck()) continue;
		if (cmd == 0) return;
		if (cmd < 0 || cmd > numberList.size()) {
			std::cout << "입력 범위 초과! 다시 진행하려면 아무 버튼이나 입력하세요." << std::endl;
			getch();
			continue;
		}
		break;
	}
	number = numberList[--cmd];
	numberList.erase(numberList.begin() + cmd);
	std::cout << number << " 번호 삭제 완료! 다시 진행하려면 아무 버튼이나 입력하세요." << std::endl;
	getch();
}
void SMSmanager::setName(const std::string& typeName, const std::string& appName) { setTypeName(typeName); setAppName(appName); }
void SMSmanager::setTypeName(const std::string& typeName) { this->typeName = typeName; }
void SMSmanager::setAppName(const std::string& appName) { this->appName = appName; }
void SMSmanager::sendMsg(const std::string& str) {
	if (numberList.size() == 0) return;
	std::cout << "---------------------------------------------" << std::endl;
	std::cout << typeName << ") " << appName << " " << str << " 되었습니다." << std::endl;
	std::cout << "아래 등록된 번호 목록으로 메세지 송부됩니다." << std::endl;
	std::cout << "---------------------------------------------" << std::endl;
	showList();
}
// 번호 중복 검사
const bool SMSmanager::checkNumberDup(const std::string& number) {
	for (auto& pn : numberList) {
		if (pn == ("010" + number)) {
			std::cout << "번호 중복! 다시 진행하려면 아무 버튼이나 입력하세요." << std::endl;
			getch();
			return true;
		}
	}
	return false;
}
SMSmanager& SMSmanager::getInstance() {
	static SMSmanager sms;
	return sms;
}
const bool SMSmanager::bufCheck() {
	if (std::cin.get() != '\n') {
		std::cin.clear();
		while (std::cin.get() != '\n') { ; }
		std::cout << "입력 오류! 다시 진행하려면 아무 버튼이나 입력하세요." << std::endl;
		getch();
		return true;
	}
	return false;
}
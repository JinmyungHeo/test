#include "SMSmanager.h"

// ���� �� �� ����� ��ȭ��ȣ ��� Load �� ����
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
// �Ҹ� �� ��ȭ��ȣ ��� ����
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
// ����� ��ȣ ��� ���
void SMSmanager::showList() {
	for (int i = 0; i < numberList.size(); i++) {
		std::cout << i + 1 << ". " << numberList[i] << std::endl;
	}
}
// �޴� ��ȭ ��ȣ ����
void SMSmanager::addNumber() {
	std::string number;
	while (true) {
		system("cls");
		showList();
		std::cout << "---------------------------------------------" << std::endl;
		std::cout << "�޴���ȭ ��ȣ�� \"010\"�� ������ ���� 8�ڸ��� �Է��ϼ��� (���� : 0) >> ";
		std::cin >> number;
		if (bufCheck()) continue;
		if (number.length() == 1 && number[0] == '0') return;
		if (number.length() != 8) {
			std::cout << "�Է� ���� ����! �ٽ� �����Ϸ��� �ƹ� ��ư�̳� �Է��ϼ���." << std::endl;
			getch();
			continue;
		}
		if (checkNumberDup(number)) continue;
		for (int i = 0; i < number.length(); i++) {
			if (isdigit(number[i]) == false) {
				std::cout << "�Է� ����! �ٽ� �����Ϸ��� �ƹ� ��ư�̳� �Է��ϼ���." << std::endl;
				getch();
				continue;
			}
		}
		number = "010" + number;
		break;
	}
	numberList.push_back(number);
	std::cout << number << " ��ȣ ��� �Ϸ�! �ٽ� �����Ϸ��� �ƹ� ��ư�̳� �Է��ϼ���." << std::endl;
	getch();
}
// �޴� ��ȭ ��ȣ ����
void SMSmanager::delNumber() {
	if (numberList.size() == 0) return;
	std::string number;
	int cmd;
	while (true) {
		system("cls");
		showList();
		std::cout << "---------------------------------------------" << std::endl;
		std::cout << "������ ��ȣ�� �����ϼ��� (���� : 0) >> ";
		std::cin >> cmd;
		if (bufCheck()) continue;
		if (cmd == 0) return;
		if (cmd < 0 || cmd > numberList.size()) {
			std::cout << "�Է� ���� �ʰ�! �ٽ� �����Ϸ��� �ƹ� ��ư�̳� �Է��ϼ���." << std::endl;
			getch();
			continue;
		}
		break;
	}
	number = numberList[--cmd];
	numberList.erase(numberList.begin() + cmd);
	std::cout << number << " ��ȣ ���� �Ϸ�! �ٽ� �����Ϸ��� �ƹ� ��ư�̳� �Է��ϼ���." << std::endl;
	getch();
}
void SMSmanager::setName(const std::string& typeName, const std::string& appName) { setTypeName(typeName); setAppName(appName); }
void SMSmanager::setTypeName(const std::string& typeName) { this->typeName = typeName; }
void SMSmanager::setAppName(const std::string& appName) { this->appName = appName; }
void SMSmanager::sendMsg(const std::string& str) {
	if (numberList.size() == 0) return;
	std::cout << "---------------------------------------------" << std::endl;
	std::cout << typeName << ") " << appName << " " << str << " �Ǿ����ϴ�." << std::endl;
	std::cout << "�Ʒ� ��ϵ� ��ȣ ������� �޼��� �ۺε˴ϴ�." << std::endl;
	std::cout << "---------------------------------------------" << std::endl;
	showList();
}
// ��ȣ �ߺ� �˻�
const bool SMSmanager::checkNumberDup(const std::string& number) {
	for (auto& pn : numberList) {
		if (pn == ("010" + number)) {
			std::cout << "��ȣ �ߺ�! �ٽ� �����Ϸ��� �ƹ� ��ư�̳� �Է��ϼ���." << std::endl;
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
		std::cout << "�Է� ����! �ٽ� �����Ϸ��� �ƹ� ��ư�̳� �Է��ϼ���." << std::endl;
		getch();
		return true;
	}
	return false;
}
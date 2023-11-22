#include "Menu.h"

BaseMenu::BaseMenu(const std::string& s) : title(s) { ; }
std::string BaseMenu::getTitle() { return title; }
BaseMenu::~BaseMenu() {}

PopupMenu::PopupMenu(const std::string& s) : BaseMenu(s) { ; }
void PopupMenu::add(std::shared_ptr<BaseMenu> p) { v.push_back(p); }
void PopupMenu::command() {
	while (true) {
		int cmd;
		int sz = v.size();
		while (true) {
			system("cls");
			showMenu();
			std::cout << "---------------------------------------------" << std::endl;
			std::cout << "�޴� ��ȣ�� �Է��ϼ���(���� : 0) >> ";
			std::cin >> cmd;
			if (bufCheck()) continue;
			if (cmd == 0) return;
			if (cmd < 0 || cmd > v.size()) {
				std::cout << "�Է� ���� �ʰ�! �ٽ� �����Ϸ��� �ƹ� ��ư�̳� �Է��ϼ���." << std::endl;
				getch();
				continue;
			}
			break;
		}
		v[--cmd]->command();
	}
}
void PopupMenu::showMenu() {
	int sz = v.size();
	for (int i = 0; i < sz; i++) {
		std::cout << i + 1 << ". " << v[i]->getTitle() << std::endl;
	}
}
const bool PopupMenu::bufCheck() {
	if (std::cin.get() != '\n') {
		std::cin.clear();
		while (std::cin.get() != '\n') { ; }
		std::cout << "�߸��� �Է�! �ٽ� �����Ϸ��� �ƹ� ��ư�̳� �Է��ϼ���." << std::endl;
		getch();
		return true;
	}
	return false;
}

MenuItem::MenuItem(const std::string& s, const voidHandler_t h) : BaseMenu(s), handler(h) { ; }
void MenuItem::command() {
	if (handler != nullptr) { handler(); }
	else { std::cout << "��� ����! �ٽ� �����Ϸ��� �ƹ� ��ư�̳� �Է��ϼ���." << std::endl; getch(); }
}
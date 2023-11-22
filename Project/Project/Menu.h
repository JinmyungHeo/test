#ifndef MENU_H_
#define MENU_H_

#pragma warning(disable:4996)
#include <iostream>
#include <vector>
#include <string>
#include <conio.h>
#include <functional>

using namespace std::placeholders;
using voidHandler_t = std::function<void()>;

class BaseMenu {
	std::string title;
public:
	BaseMenu(const std::string& s);
	std::string getTitle();
	virtual ~BaseMenu();
	virtual void command() = 0;
};

class PopupMenu : public BaseMenu {
	std::vector<std::shared_ptr<BaseMenu>> v;
public:
	PopupMenu(const std::string& s);
	void add(std::shared_ptr<BaseMenu> p);
	void command();
	void showMenu();
	static const bool bufCheck();
};

class MenuItem : public BaseMenu {
	voidHandler_t handler;
public:
	MenuItem(const std::string& s, const voidHandler_t h = nullptr);
	void command();
};

#endif // MENU_H_


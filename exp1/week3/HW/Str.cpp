#include <iostream>
#include <cstring>
#include "Str.h"

using namespace std;

// 클래스 생성자, len: 문자열의 길이, str: 문자열
Str::Str(int leng) {
	if (leng >= 0) {
		len = leng;
		str = new char[len + 1];
	}
	else cout << "Invalid length" << endl;
}

// 클래스 생성자, len: 문자열의 길이, str: 문자열
Str::Str(char *neyong) {
	len = strlen(neyong);
	str = new char[len + 1]; // '\0'를 고려하여 +1
	strcpy(str, neyong); // strcpy로 str에 neyong 저장
}

// 클래스 소멸자
Str::~Str() {delete[] str;}

// 문자열의 길이 반환
int Str::length(void) {return len;}

// 문자열 반환
char *Str::contents(void) {return str;}

// 문자열과 a 비교, 같으면 0 반환
int Str::compare(class Str& a) {return strcmp(str, a.contents());}

// 문자열과 a 비교, 같으면 0 반환
int Str::compare(char *a) {return strcmp(str, a);}

// 문자열에 a 대입
void Str::operator=(char *a) {
	if (a) { // a가 NULL이 아닌 경우
		delete[] str; // 원래 문자열 제거
		len = strlen(a); // a의 길이
		str = new char[len + 1]; // '\0'을 고려하여 +1
		strcpy(str, a); // strcpy로 str에 a 저장
	}
	else cout << "Invalid string" << endl;
}

// 문자열에 a의 내용 대입
void Str::operator=(class Str& a) {
	delete[] str; // 원래 문자열 제거
	len = a.len; // a의 내용의 길이
	str = new char[len + 1]; // '\0'을 고려하여 +1
	strcpy(str, a.contents()); // strcpy로 str에 a의 내용 저장
}


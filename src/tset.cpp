// ННГУ, ВМК, Курс "Методы программирования-2", С++, ООП
//
// tset.cpp - Copyright (c) Гергель В.П. 04.10.2001
//   Переработано для Microsoft Visual Studio 2008 Сысоевым А.В. (19.04.2015)
//
// Множество - реализация через битовые поля

#include "tset.h"

// Fake variables used as placeholders in tests
static const int FAKE_INT = -1;
static TBitField FAKE_BITFIELD(1);
static TSet FAKE_SET(1);

TSet::TSet(int mp) : bf(mp)
{
    MaxPower = mp;
}

// конструктор копирования
TSet::TSet(const TSet &s) : bf(s.bf)
{
    MaxPower = s.MaxPower;
}

// конструктор преобразования типа
TSet::TSet(const TBitField &bf) : bf(bf)
{
    MaxPower = bf.GetLength();
}

TSet::operator TBitField()
{
    TBitField tmp(this -> bf);
    return tmp;
}

int TSet::GetMaxPower(void) const // получить макс. к-во эл-тов
{
    return MaxPower;
}

int TSet::IsMember(const int n) const // элемент множества?
{
    return bf.GetBit(n);
}

void TSet::InsElem(const int n) // включение элемента множества
{
    bf.SetBit(n);
}

void TSet::DelElem(const int n) // исключение элемента множества
{
    bf.ClrBit(n);
}

// теоретико-множественные операции

TSet& TSet::operator=(const TSet &s) // присваивание
{
    bf = s.bf;
    MaxPower = s.MaxPower;
    return *this;
}

int TSet::operator==(const TSet &s) const // сравнение
{
    return bf == s.bf;
}

int TSet::operator!=(const TSet &s) const // сравнение
{
    return bf != s.bf;
}

TSet TSet::operator+(const TSet &s) // объединение
{
    TSet tmp(bf | s.bf);          // характеристические векторы OR-ятся
    tmp.MaxPower = (MaxPower > s.MaxPower) ? MaxPower : s.MaxPower; // берём макс. универс
    return tmp;
}

TSet TSet::operator+(const int Elem) // объединение с элементом
{
    if (Elem < 0 || Elem >= MaxPower) { // элемент должен быть из того же универса
        throw Elem;
    }
    TSet tmp(*this);
    tmp.InsElem(Elem);
    return tmp;
}

TSet TSet::operator-(const int Elem) // разность с элементом
{
    if (Elem < 0 || Elem >= MaxPower) { // элемент должен быть из того же универса
        throw Elem;
    }
    TSet tmp(*this);
    tmp.DelElem(Elem);
    return tmp;
}

TSet TSet::operator*(const TSet &s) // пересечение
{
    TSet tmp(bf & s.bf);          // характеристические векторы AND-ятся
    tmp.MaxPower = (MaxPower > s.MaxPower) ? MaxPower : s.MaxPower;
    return tmp;
}

TSet TSet::operator~(void) // дополнение
{
    TSet tmp(~bf);                // инвертируем весь характеристический вектор
    tmp.MaxPower = MaxPower;      // универс остаётся прежним
    return tmp;
}

// перегрузка ввода/вывода

istream &operator>>(istream &istr, TSet &s) // ввод
{
    istr >> s.bf; // читаем характеристический вектор как битовое поле
    return istr;
}

ostream& operator<<(ostream &os, const TSet &s) // вывод
{
    os << '{';
    int n = s.MaxPower;
    for (int i = 0; i < n; i++) {
        if (s.IsMember(i)) {
            os <<' ' << i << ',';
        }
    }
    os << "}";
    return os;
}

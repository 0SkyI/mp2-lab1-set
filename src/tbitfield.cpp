// ННГУ, ВМК, Курс "Методы программирования-2", С++, ООП
//
// tbitfield.cpp - Copyright (c) Гергель В.П. 07.05.2001
//   Переработано для Microsoft Visual Studio 2008 Сысоевым А.В. (19.04.2015)
//
// Битовое поле

#include "tbitfield.h"

// Fake variables used as placeholders in tests
static const int FAKE_INT = -1;
static TBitField FAKE_BITFIELD(1);

TBitField::TBitField(int n)
{
    if (n < 0) { // длина не может быть отрицательной
        throw n;
    }
    BitLen = n;                       // всего битов в поле
    MemLen = (n + 31) / 32;           // к-во 32-битных слов (округление вверх)
    if (MemLen == 0) {                // частный случай: пустое поле
        MemLen = 1;
    }
    pMem = new TELEM[MemLen];
    for (int i = 0; i < MemLen; i++) { // все биты изначально очищены
        pMem[i] = 0;
    }
}

TBitField::TBitField(const TBitField &bf) // конструктор копирования
{
    BitLen = bf.BitLen;
    MemLen = bf.MemLen;
    pMem   = new TELEM[MemLen];
    for (int i = 0; i < MemLen; i++) {
        pMem[i] = bf.pMem[i];
    }
}

TBitField::~TBitField()
{
    delete [] pMem; // освобождаем динамическую память
    pMem = nullptr;
}

int TBitField::GetMemIndex(const int n) const // индекс Мем для бита n
{
    return n >> 5;
}

TELEM TBitField::GetMemMask(const int n) const // битовая маска для бита n
{
    return 1 << (n&31);
}

// доступ к битам битового поля

int TBitField::GetLength(void) const // получить длину (к-во битов)
{
  return BitLen;
}

void TBitField::SetBit(const int n) // установить бит
{
    if (n < 0 || n >= BitLen) {
        throw n;
    }
    else {
        pMem[GetMemIndex(n)] |= GetMemMask(n);
    }
}

void TBitField::ClrBit(const int n) // очистить бит
{
    if (n < 0 || n >= BitLen) {
        throw n;
    }
    else {
        pMem[GetMemIndex(n)] &= ~GetMemMask(n);
    }
}

int TBitField::GetBit(const int n) const // получить значение бита
{
  if (n < 0 || n >= BitLen) {
    throw n;
  }
  else {
    // сдвигаем найденный бит в младший разряд - получаем 0 или 1
    return (pMem[GetMemIndex(n)] & GetMemMask(n)) >> (n & 31);
  }
}

// битовые операции

TBitField& TBitField::operator=(const TBitField &bf) // присваивание
{
    if (this == &bf) { // защита от самоприсваивания bf = bf
        return *this;
    }
    if (MemLen < bf.MemLen) { // старый массив меньше нового - пересоздаём
        delete [] pMem;
        pMem   = new TELEM[bf.MemLen];
    }
    BitLen = bf.BitLen;
    MemLen = bf.MemLen;
    for (int i = 0; i < MemLen; i++) {
        pMem[i] = bf.pMem[i];
    }
    return *this;
}

int TBitField::operator==(const TBitField &bf) const // сравнение
{
    if (BitLen != bf.BitLen) { // разные длины - поля не равны
        return 0;
    }
    for (int i = 0; i < MemLen; i++) {
        if (pMem[i] != bf.pMem[i]) {
            return 0;
        }
    }
    return 1;
}

int TBitField::operator!=(const TBitField &bf) const // сравнение
{
    return !(*this == bf); // неравенство - отрицание равенства
}

TBitField TBitField::operator|(const TBitField &bf) // операция "или"
{
    int len = (BitLen > bf.BitLen) ? BitLen : bf.BitLen; // макс. длина
    TBitField tmp(len);
    for (int i = 0; i < tmp.MemLen; i++) {
        TELEM x = (i < MemLen) ? pMem[i] : 0;    // у bf может быть меньше слов
        TELEM y = (i < bf.MemLen) ? bf.pMem[i] : 0;
        tmp.pMem[i] = x | y;
    }
    return tmp;
}

TBitField TBitField::operator&(const TBitField &bf) // операция "и"
{
    int len = (BitLen > bf.BitLen) ? BitLen : bf.BitLen; // макс. длина
    TBitField tmp(len);
    for (int i = 0; i < tmp.MemLen; i++) {
        TELEM x = (i < MemLen) ? pMem[i] : 0;
        TELEM y = (i < bf.MemLen) ? bf.pMem[i] : 0;
        tmp.pMem[i] = x & y;
    }
    return tmp;
}

TBitField TBitField::operator~(void) // отрицание
{
    TBitField tmp(BitLen);
    for (int i = 0; i < MemLen; i++) {
        tmp.pMem[i] = ~pMem[i];
    }
    // очищаем "лишние" биты последнего слова (свыше BitLen),
    // иначе сравнение с другими полями может работать неверно
    int extra = MemLen * 32 - BitLen; // к-во лишних битов (0..31)
    if (extra > 0) {
        tmp.pMem[MemLen - 1] &= ~0u >> extra;
    }
    return tmp;
}

// ввод/вывод

istream &operator>>(istream &istr, TBitField &bf) // ввод
{
    istr >> ws;                      // пропускаем пробелы
    char c;
    for (int i = 0; i < bf.BitLen; i++) {
        istr.get(c);                 // читаем по одному символу
        if (!istr.good()) {          // поток закончился раньше
            break;
        }
        if (c == '1') {
            bf.SetBit(i);
        } else if (c == '0') {
            bf.ClrBit(i);
        } else {                     // недопустимый символ
            istr.setstate(std::ios_base::failbit);
            break;
        }
    }
    return istr;
}

ostream &operator<<(ostream &ostr, const TBitField &bf) // вывод
{
    for (int i = 0; i < bf.BitLen; i++) {
        ostr << (bf.GetBit(i) ? '1' : '0');
    }
    return ostr;
}

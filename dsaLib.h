/*
 * =========================================================================================
 * Name        : dsaLib.h
 * Author      : Duc Dung Nguyen
 * Email       : nddung@hcmut.edu.vn
 * Copyright   : Faculty of Computer Science and Engineering - Bach Khoa University
 * Description : library for Assignment 2 - Data structures and Algorithms - Fall 2017
 * =========================================================================================
 */

#ifndef A02_DSALIB_H
#define A02_DSALIB_H

#include <string>
#include <math.h>
#include <vector>

using namespace std;


class DSAException {
    int     _error;
    string  _text;
public:

    DSAException() : _error(0), _text("Success") {}
    DSAException(int err) : _error(err), _text("Unknown Error") {}
    DSAException(int err, const char* text) : _error(err), _text(text) {}

    int getError() { return _error; }
    string& getErrorText() { return _text; }
};

template <class T>
struct L1Item {
    T data;
    L1Item<T> *pNext;
    L1Item() : pNext(NULL) {}
    L1Item(T &a) : data(a), pNext(NULL) {}
};

template <class T>
class L1List
{
    L1Item<T> *_pHead; // The head pointer of linked list
    L1Item<T> *_pLast; // The last pointer of linked list
    size_t _size;      // number of elements in this list
  public:
    L1List() : _pHead(NULL), _pLast(NULL), _size(0) {}
    L1List(T &a) : _pHead(a), _pLast(a), _size(1) {}
    ~L1List()
    {
        destroy();
        _size = 0;
    }

    void destroy();

    bool isEmpty()
    {
        return _pHead == NULL;
    }
    size_t getSize()
    {
        return _size;
    }

    T &at(int i);
    T &operator[](int i);

    bool find(T &a, int &idx);
    int insert(int i, T &a);
    int insertTail(T &a);
    int remove(int i);

    int push_back(T &a);
    int insertHead(T &a);

    int removeHead();
    int removeLast();

    void reverse();

    T &getLast();

    L1Item<T> *&pointerHead()
    {
        return _pHead;
    }

    L1Item<T> *&pointerLast()
    {
        return _pLast;
    }

    bool traverse(bool (*op)(T, T, T, T), T n4)
    {
        L1Item<T> *p = _pHead;
        while (p->pNext != NULL)
        {
            if (op(p->data, p->pNext->data, _pLast->data, n4))
                return true;
            p = p->pNext;
        }
        return false;
    }

    bool traverse(bool (*op)(T &, T &), T &id)
    {
        L1Item<T> *p = _pHead;
        while (p != NULL)
        {
            if (op(p->data, id))
                return true;
            p = p->pNext;
        }
        return false;
    }

    void traverseGreater(bool (*op)(T &, T &), T &n)
    {
        L1Item<T> *p = _pHead;
        while (p != NULL)
        {
            if (op(p->data, n))
            {
                n = p->data;
            }
            p = p->pNext;
        }
    }

    void traverse(void (*op)(T &))
    {
        L1Item<T> *p = _pHead;
        while (p)
        {
            op(p->data);
            p = p->pNext;
        }
    }
    void traverse(void (*op)(T &, T &), T &a)
    {
        L1Item<T> *p = _pHead;
        while (p)
        {
            op(p->data, a);
            p = p->pNext;
        }
    }
    void traverse(void (*op)(T &, void *), void *pParam)
    {
        L1Item<T> *p = _pHead;
        while (p)
        {
            op(p->data, pParam);
            p = p->pNext;
        }
    }
    void removeAll(T &a)
    {
        L1Item<T> *pre = _pHead;
        L1Item<T> *cur = _pHead->pNext;
        while (cur)
        {
            if (cur->data == a)
            {
                L1Item<T> *del = cur;
                pre->pNext = cur->pNext;
                delete del;
                cur = pre->pNext;
                _size--;
                continue;
            }
            pre = cur;
            cur = cur->pNext;
        }
        _pLast = pre;
        if (_pHead->data == a)
        {
            removeHead();
        }
    }
};

/// Insert item to the end of the list
/// Return 0 if success
template <class T>
int L1List<T>::push_back(T &a)
{
    if (_pHead == NULL)
    {
        _pHead = new L1Item<T>(a);
    }
    else
    {
        L1Item<T> *p = _pHead;
        while (p->pNext)
            p = p->pNext;
        p->pNext = new L1Item<T>(a);
    }

    _size++;
    return 0;
}

/// Insert item to the front of the list
/// Return 0 if success
template <class T>
int L1List<T>::insertHead(T &a)
{
    L1Item<T> *p = new L1Item<T>(a);
    p->pNext = _pHead;
    _pHead = p;
    _size++;
    return 0;
}

/// Remove the first item of the list
/// Return 0 if success
template <class T>
int L1List<T>::removeHead()
{
    if (_pHead)
    {
        L1Item<T> *p = _pHead;
        _pHead = p->pNext;
        delete p;
        _size--;
        if (_size == 0)
            _pLast = _pHead;
        return 0;
    }
    return -1;
}

/// Remove the last item of the list
/// Return 0 if success
template <class T>
int L1List<T>::removeLast()
{
    if (_pHead)
    {
        if (_pHead->pNext)
        {
            L1Item<T> *prev = _pHead;
            L1Item<T> *pcur = prev->pNext;
            while (pcur->pNext)
            {
                prev = pcur;
                pcur = pcur->pNext;
            }
            delete pcur;
            prev->pNext = NULL;
            _pLast = prev;
        }
        else
        {
            delete _pHead;
            _pHead = NULL;
        }
        _size--;
        return 0;
    }
    return -1;
}

///////////////======================Self-define======================////////////////

/// Reverse the list
template <class T>
void L1List<T>::reverse()
{
    L1Item<T> *temp = new L1Item<T>();
    while (_pHead != NULL)
    {
        temp->data = _pHead->data;
        L1Item<T> *t = new L1Item<T>();
        t->pNext = temp;
        temp = t;
        _pHead = _pHead->pNext;
    }
    _pHead = temp->pNext;
    delete temp;
}

/// Remove the ith item of the list
/// Return 0 if success, else return -1
template <class T>
int L1List<T>::remove(int i)
{
    if (i == 0)
    {
        removeHead();
        return 0;
    }
    else if (i == _size - 1)
    {
        removeLast();
        return 0;
    }
    else if (i > 0 & i < _size - 1)
    {
        L1Item<T> *pre = _pHead;
        L1Item<T> *curr = _pHead;
        while (i != 0)
        {
            pre = curr;
            curr = curr->pNext;
            i--;
        }
        pre->pNext = curr->pNext;
        delete curr;
        return 0;
    }
    else
    {
        return -1;
    }
}

/// Insert an item into the list
/// Return 0 if success
template <class T>
int L1List<T>::insert(int i, T &a)
{
    if (i == 0)
    {
        insertHead(a);
        return 0;
    }
    if (i > 0 && i < _size)
    { //from 1 to size - 1
        L1Item<T> *pre = _pHead;
        L1Item<T> *curr = _pHead;
        while (i != 0)
        {
            pre = curr;
            curr = curr->pNext;
            i--;
        }
        pre->pNext = new L1Item<T>(a);
        pre->pNext->pNext = curr;
        _size++;
        return 0;
    }
    else if (i == _size)
    {
        insertTail(a);
        return 0;
    }
    else
    {
        return -1;
    }
}

/// Insert an item into the tail of linked list
/// Return 0 if success
template <class T>
int L1List<T>::insertTail(T &a)
{
    if (_pLast == NULL)
    {
        insertHead(a);
        _pLast = _pHead;
        return 0;
    }
    _pLast->pNext = new L1Item<T>(a);
    _pLast = _pLast->pNext;
    _size++;
    return 0;
}

/// Find the item a
/// Get the index if found
template <class T>
bool L1List<T>::find(T &a, int &idx)
{
    L1Item<T> *temp = _pHead;
    idx = 0;
    while (temp != NULL)
    {
        if (a == temp->data)
        {
            return true;
        }
        temp = temp->pNext;
        idx++;
    }
    return false;
}

/// Operator[]
template <class T>
T &L1List<T>::operator[](int i)
{
    if (i <= _size - 1 && i >= 0)
    {
        L1Item<T> *temp = _pHead;
        while (i != 0)
        {
            temp = temp->pNext;
            i--;
        }
        T &ans = temp->data;
        return ans;
    }
}

/// At
/// if out-of-range then throw the exception
template <class T>
T &L1List<T>::at(int i)
{
    if (i <= _size - 1 && i >= 0)
    {
        operator[](i);
    }
    else
        throw out_of_range("Out-of-range");
}

/// Destroy the list
template <class T>
void L1List<T>::destroy()
{
    if (_pHead == NULL)
        return;
    while (_pHead != NULL)
    {
        L1Item<T> *del = _pHead;
        _pHead = _pHead->pNext;
        delete del;
    }
    _pLast = _pHead;
}

/// Get the last node
template <class T>
T &L1List<T>::getLast()
{
    T &ans = _pLast->data;
    return ans;
}

/************************************************************************
 * This section is for AVL tree
 ************************************************************************/
template <class T>
struct AVLNode {
    T           _data;
    AVLNode<T>   *_pLeft, *_pRight;
#ifdef AVL_USE_HEIGHT
    int         _height;
    AVLNode(T &a) : _data(a), _pLeft(NULL), _pRight(NULL), _height(1) {}
#else
    int         _bFactor;
    AVLNode(T &a) : _data(a), _pLeft(NULL), _pRight(NULL), _bFactor(0) {}
#endif
};

template <class T>
class AVLTree {
    AVLNode<T> *_pRoot;
public:
    AVLTree() : _pRoot(NULL) {}
    ~AVLTree() { destroy(_pRoot); }

    bool find(T& key, T* &ret) { return find(_pRoot, key, ret); }
    bool insert(T& key) { return insert(_pRoot, key); }
    bool remove(T& key) { return remove(_pRoot, key); }
    void traverseNLR(void (*op)(T&)) { traverseNLR(_pRoot, op); }
    void traverseLNR(void (*op)(T&)) { traverseLNR(_pRoot, op); }
    void traverseLRN(void (*op)(T&)) { traverseLRN(_pRoot, op); }

protected:
    void destroy(AVLNode<T>* &pR)
    {
        if(pR == NULL) return;
        destroy(pR->_pLeft);
        destroy(pR->_pRight);
        delete pR;
        pR = NULL;
    }
    bool find(AVLNode<T> *pR, T& key, T* &ret)
    {
        if(pR == NULL) return false;
        else if(pR->_data == key)
        {
            ret = &(pR->data);
            return true;
        }
        else if(pR->data > key)
        {
            find(pR->_pLeft, key, ret);
        }
        else find(pR->_pRight, key, ret);
    }
    bool insert(AVLNode<T>* &pR, T& a)
    {
        if(pR == NULL)
        {
            pR = new AVLNode<T>(a);
            return true;
        }
        else if(pR->_data < a)
        {
            insert(pR->_pLeft, a);
            balanceLeft(pR);
        }
        else 
        {
            insert(pR->_pRight, a);
            balanceRight(pR);
        }
    }
    bool remove(AVLNode<T>* &pR, T& a)
    {
        if(pR == NULL) return false;
        if(a < pR->_data)
        {
            remove(pR->_pLeft, a);
        }
        else if(a > pR->_data)
        {
            remove(pR->_pRight, a);
        }
        else 
        {
            if(pR->_pRight == NULL && pR->_pLeft == NULL)
            {
                AVLNode<T>* p = pR;
                pR = NULL;
                delete p;
                return true;
            }
            else if(pR->_pRight == NULL)
            {
                AVLNode<T> *t = pR;
                pR = pR->_pLeft;
                delete t;
                return true;
            }
            else 
            {
                AVLNode<T>* t = pR->_pRight;
                while(t->_pLeft)
                    t = t->_pLeft;
                pR->data = t->data;
                remove(pR->_pRight, t->data);
            }
        }
        reBalance(pR);
    }
    void traverseNLR(AVLNode<T> *pR, void (*op)(T&))
    {
        if(pR == NULL) return;
        op(pR->data);
        traverseNLR(pR->_pLeft, op);
        traverseNLR(pR->_pRight, op);
    }
    void traverseLNR(AVLNode<T> *pR, void (*op)(T&))
    {
        traverseLNR(pR->_pLeft, op);
        if(pR == NULL) return;
        op(pR->data);
        traverseLNR(pR->_pRight, op);
    }
    void traverseLRN(AVLNode<T> *pR, void (*op)(T&))
    {
        traverseLRN(pR->_pLeft, op);
        traverseLRN(pR->_pRight, op);
        if(pR == NULL) return;
        op(pR->data);
    }
    void reBalance(AVLNode<T> *&pR)
    {
        balanceLeft(pR);
        balanceRight(pR);
    }

    void rotLeft(AVLNode<T>* &pR)
    {
        AVLNode<T>* x = pR->_pRight;
        AVLNode<T>* y = x->_pLeft;
        x->_pLeft = pR;
        pR->_pRight = y;
        pR = x;
    }
    void rotRight(AVLNode<T>* &pR)
    {
        AVLNode<T>* x = pR->_pLeft;
        AVLNode<T>* y = x->_pRight;
        x->_pRight = pR;
        pR->_pLeft = y;
        pR = x;
    }
    void rotLR(AVLNode<T>* &pR)
    {
        rotLeft(pR->_pLeft);
        rotRight(pR);
    }
    void rotRL(AVLNode<T>* &pR)
    {
        rotRight(pR->_pRight);
        rotLeft(pR);
    }

    bool balanceLeft(AVLNode<T>* &pR)
    {
        // > 1 means the height of the left is greater then the right 2 units
        if(pR->_bFactor > 1 && pR->_pLeft->_bFactor >= 0)
        {
            /*           9                        7
             *         /   \                   /     \
             *        7     10               5         9
             *      /   \        ==>       /   \      /  \
             *     5     8                4     6    8    10
             *   /   \                   
             *  4     6                      
             */                                 
            rotRight(pR);
        }
        else if(pR->_bFactor > 1 && pR->_pLeft->_bFactor < 0)
        {
            /*              11                      11                      9
             *            /   \                    /  \                   /    \
             *           7     12                 9    12                7     11
             *         /   \            ==>      /  \           ==>     / \   /   \ 
             *        5     9                   7   10                 5   8 10   12
             *             /  \                / \                          
             *            8    10             5   8                         
             */
            rotLR(pR);
        }
    }
    bool balanceRight(AVLNode<T>* &pR)
    {
        //< -1 means the height of the right is greater then the left 2 units
        if(pR->_bFactor < -1 && pR->_pRight->_bFactor <= 0)
        {
            /*             5                           7 
             *           /   \                      /     \
             *          4     7                   5        9
             *               /  \        ==>     / \      /  \
             *              6    9              4   6    8   10
             *                  /  \                
             *                 8    10                 
             */
            rotLeft(pR);
        }
        else if(pR->_bFactor < -1 && pR->_pRight->_bFactor > 0)
        {
            /*              7                           7                           9
             *             / \                         /  \                       /    \
             *            5   11                      5    9                    7       11
             *               /  \       ==>               /  \          ==>    / \     /   \
             *              9   12                       8    11              5   8   10   12
             *             / \                               /  \
             *            8   10                            10  12
             */
            rotRL(pR);
        }
    }
};

#endif //A02_DSALIB_H

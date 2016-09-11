//
//
// Description: This file is part of FET
//
//
// Author: Liviu Lalescu <Please see http://lalescu.ro/liviu/ for details about contacting Liviu Lalescu (in particular, you can find here the e-mail address)>
// Copyright (C) 2009 Liviu Lalescu <http://lalescu.ro/liviu/>
//
/***************************************************************************
 *                                                                         *
 *   This program is free software: you can redistribute it and/or modify  *
 *   it under the terms of the GNU Affero General Public License as        *
 *   published by the Free Software Foundation, either version 3 of the    *
 *   License, or (at your option) any later version.                       *
 *                                                                         *
 ***************************************************************************/

//REFERENCES:
//
//		Hints from Ted Jensen's Tutorial on Pointers and Arrays in C -
//			- Chapter 9: Pointers and Dynamic Allocation of Memory - improvement so that the elements of a matrix are in a contiguous memory location.
//
//		Hints from C++ FAQ LITE by Marshall Cline -
//			- Section [13] - Operator overloading, article [13.12] - advice about the () operator for matrices.
//
//		You may find more information on the FET documentation web page, http://lalescu.ro/liviu/fet/doc/

#ifndef MATRIX_H
#define MATRIX_H

#include "timetable_defs.h"

template <typename T>
class Matrix3D
{
private:
	int d1;
	int d2;
	int d3;
	
	T*** a;
	T* content;
	
public:
	Matrix3D();
	~Matrix3D();

	void clear();
	void resize(int _d1, int _d2, int _d3);
	T** operator[](int i);
	T& operator()(int i, int j, int k);
};

template <typename T>
class Matrix2D
{
private:
	int d1;
	int d2;
	
	T** a;
	T* content;
	
public:
	Matrix2D();
	~Matrix2D();

	void clear();
	void resize(int _d1, int _d2);
	T* operator[](int i);
	T& operator()(int i, int j);
};

template <typename T>
class Matrix1D
{
private:
	int d1;
	
	T* a;
	
public:
	Matrix1D();
	~Matrix1D();

	void clear();
	void resize(int _d1);
	T& operator[](int i);
	//T& operator()(int i);
};


template <typename T> Matrix3D<T>::Matrix3D()
{
	d1=d2=d3=-1;
}

template <typename T> Matrix3D<T>::~Matrix3D()
{
	this->clear();
}

template <typename T> void Matrix3D<T>::clear()
{
	if(d1>=0 || d2>=0 || d3>=0){
		assert(d1>0 && d2>0 && d3>0);
		
		for(int i=0; i<d1; i++)
			delete[] a[i];
		delete[]a;
		
		delete[] content;
	}
	d1=d2=d3=-1;
}

template <typename T> void Matrix3D<T>::resize(int _d1, int _d2, int _d3)
{
	if(_d1<=0 || _d2<=0 || _d3<=0){
		this->clear();
		return;
	}
	
	if(d1!=_d1 || d2!=_d2 || d3!=_d3){
		this->clear();
		
		d1=_d1;
		d2=_d2;
		d3=_d3;

		content=new T[d1*d2*d3];
		a=new T**[d1];
		for(int i=0; i<d1; i++){
			a[i]=new T*[d2];
			for(int j=0; j<d2; j++)
				a[i][j]=content+i*d2*d3+j*d3;
		}
	}
}

template <typename T> inline T** Matrix3D<T>::operator[](int i)
{
	return a[i];
}

template <typename T> inline T& Matrix3D<T>::operator()(int i, int j, int k)
{
	//return content[i*d2*d3+j*d3+k];
	return content[(i*d2+j)*d3+k];
}


template <typename T> Matrix2D<T>::Matrix2D()
{
	d1=d2=-1;
}

template <typename T> Matrix2D<T>::~Matrix2D()
{
	this->clear();
}

template <typename T> void Matrix2D<T>::clear()
{
	if(d1>=0 || d2>=0){
		assert(d1>0 && d2>0);
		
		delete[] a;
		
		delete[] content;
	}
	d1=d2=-1;
}

template <typename T> void Matrix2D<T>::resize(int _d1, int _d2)
{
	if(_d1<=0 || _d2<=0){
		this->clear();
		return;
	}

	if(d1!=_d1 || d2!=_d2){
		this->clear();

		d1=_d1;
		d2=_d2;

		content=new T[d1*d2];
		a=new T*[d1];
		for(int i=0; i<d1; i++)
			a[i]=content+i*d2;
	}
}

template <typename T> inline T* Matrix2D<T>::operator[](int i)
{
	return a[i];
}

template <typename T> inline T& Matrix2D<T>::operator()(int i, int j)
{
	return content[i*d2+j];
}


template <typename T> Matrix1D<T>::Matrix1D()
{
	d1=-1;
}

template <typename T> Matrix1D<T>::~Matrix1D()
{
	this->clear();
}

template <typename T> void Matrix1D<T>::clear()
{
	if(d1>=0){
		assert(d1>0);
		delete[] a;
	}
	d1=-1;
}

template <typename T> void Matrix1D<T>::resize(int _d1)
{
	if(_d1<=0){
		this->clear();
		return;
	}

	if(d1!=_d1){
		this->clear();

		d1=_d1;

		a=new T[d1];
	}
}

template <typename T> inline T& Matrix1D<T>::operator[](int i)
{
	return a[i];
}

/*template <typename T> inline T& Matrix1D<T>::operator()(int i)
{
	return a[i];
}*/

#endif

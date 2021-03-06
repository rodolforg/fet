//
//
// Description: This file is part of FET
//
//
// Author: Liviu Lalescu <Please see https://lalescu.ro/liviu/ for details about contacting Liviu Lalescu (in particular, you can find here the e-mail address)>
// Copyright (C) 2009 Liviu Lalescu <https://lalescu.ro/liviu/>
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
//		You may find more information on the FET documentation web page, https://lalescu.ro/liviu/fet/doc/

#ifndef MATRIX_H
#define MATRIX_H

#include <cassert>

/**
 * @brief Matrix3D is a simple 3D vector class with proper resource deallocation
 */
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

	/**
	 * @brief clear all matrix contents and size
	 */
	void clear();
	/**
	 * @brief change the matrix size, DESTROYING original content
	 * @param _d1 the new size for the first dimension
	 * @param _d2 the new size for the second dimension
	 * @param _d3 the new size for the third dimension
	 */
	void resize(int _d1, int _d2, int _d3);
	T** operator[](int i);
	const T * const * operator[](int i) const;
	T& operator()(int i, int j, int k);
	const T& operator()(int i, int j, int k) const;

	/**
	 * @brief getD1 the size of the first dimension
	 * @return the current size of first (main) dimension of 3-D matrix
	 */
	int getD1() const;
	/**
	 * @brief getD2 the size of the second dimension
	 * @return the current size of second dimension of 3-D matrix
	 */
	int getD2() const;
	/**
	 * @brief getD3 the size of the third dimension
	 * @return the current size of third dimension of 3-D matrix
	 */
	int getD3() const;
};

/**
 * @brief Matrix2D is a simple 2D vector class with proper resource deallocation
 */
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

	/**
	 * @brief clear all matrix contents and size
	 */
	void clear();
	/**
	 * @brief change the matrix size, DESTROYING original content
	 * @param _d1 the new size for the first dimension
	 * @param _d2 the new size for the second dimension
	 */
	void resize(int _d1, int _d2);
	T* operator[](int i);
	const T* operator[](int i) const;
	T& operator()(int i, int j);
	const T& operator()(int i, int j) const;

	/**
	 * @brief getD1 the size of the first dimension
	 * @return the current size of first (main) dimension of 3-D matrix
	 */
	int getD1() const;
	/**
	 * @brief getD2 the size of the second dimension
	 * @return the current size of second dimension of 3-D matrix
	 */
	int getD2() const;
};

/**
 * @brief Matrix1D is a simple vector class with proper resource deallocation
 */
template <typename T>
class Matrix1D
{
private:
	int d1;
	
	T* a;
	
public:
	Matrix1D();
	~Matrix1D();

	/**
	 * @brief clear all matrix contents and size
	 */
	void clear();
	/**
	 * @brief change the matrix size, DESTROYING original content
	 * @param _d1 the new dimension size
	 */
	void resize(int _d1);
	T& operator[](int i);
	const T& operator[](int i) const;
	/**
	 * @brief getD1 the current matrix dimension
	 * @return the current 1-D matrix size
	 */
	int getD1() const;
};


template <typename T> Matrix3D<T>::Matrix3D()
	: d1(-1), d2(-1), d3(-1), a(NULL), content(NULL)
{
}

template <typename T> Matrix3D<T>::~Matrix3D()
{
	this->clear();
}

template <typename T> void Matrix3D<T>::clear()
{
	if(a != NULL){
		assert(d1>0 && d2>0 && d3>0);
		
		for(int i=0; i<d1; i++)
			delete[] a[i];
		delete[]a;
		a = NULL;
	}
	if (content != NULL) {
		assert(d1>0 && d2>0 && d3>0);

		delete[] content;
		content = NULL;
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
				a[i][j]=content+(i*d2+j)*d3;
		}
	}
}

template <typename T> inline T** Matrix3D<T>::operator[](int i)
{
	return a[i];
}

template <typename T> inline const T * const * Matrix3D<T>::operator[](int i) const
{
	return a[i];
}

template <typename T> inline T& Matrix3D<T>::operator()(int i, int j, int k)
{
	return content[(i*d2+j)*d3+k];
}

template <typename T> inline const T& Matrix3D<T>::operator()(int i, int j, int k) const
{
	return content[(i*d2+j)*d3+k];
}

template <typename T> int Matrix3D<T>::getD1() const
{
	return d1;
}

template <typename T> int Matrix3D<T>::getD2() const
{
	return d2;
}

template <typename T> int Matrix3D<T>::getD3() const
{
	return d3;
}


template <typename T> Matrix2D<T>::Matrix2D()
	: d1(-1), d2(-1), a(NULL), content(NULL)
{
}

template <typename T> Matrix2D<T>::~Matrix2D()
{
	this->clear();
}

template <typename T> void Matrix2D<T>::clear()
{
	if(a != NULL){
		assert(d1>0 && d2>0);
		
		delete[] a;
		a = NULL;
	}
	if (content != NULL) {
		assert(d1>0 && d2>0);
		delete[] content;
		content = NULL;
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

template <typename T> inline const T& Matrix2D<T>::operator()(int i, int j) const
{
	return content[i*d2+j];
}

template <typename T> inline T& Matrix2D<T>::operator()(int i, int j)
{
	return content[i*d2+j];
}

template <typename T> int Matrix2D<T>::getD1() const
{
	return d1;
}

template <typename T> int Matrix2D<T>::getD2() const
{
	return d2;
}


template <typename T> Matrix1D<T>::Matrix1D()
	: d1(-1), a(NULL)
{
}

template <typename T> Matrix1D<T>::~Matrix1D()
{
	this->clear();
}

template <typename T> void Matrix1D<T>::clear()
{
	if(a != NULL){
		assert(d1>0);
		delete[] a;
		a = NULL;
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

template <typename T> inline const T& Matrix1D<T>::operator[](int i) const
{
	return a[i];
}

template <typename T> inline T& Matrix1D<T>::operator[](int i)
{
	return a[i];
}

template <typename T> int Matrix1D<T>::getD1() const
{
	return d1;
}

#endif

/**
 * ARX - Advanced Resource Archives
 * Copyright (C) 2005-2026  Hagen Möbius
 * SPDX-License-Identifier: MIT
**/

#ifndef ARX__INCLUDE__ARXX_H
#define ARX__INCLUDE__ARXX_H

#include <arx/archive.h>
#include <arx/common.h>
#include <arx/item.h>

/**
 * @mainpage
 *
 * Welcome to Advanced Resource file for C++.
 * This library provides classes and functions to open, modify and save ARX archives.
 * 
 * ARX is just another file format.
 * It is designed to contain an arbitrary amount of data chunks in a compressed state.
 * Additionally the data chunks may be named, typed, versioned and structured in a directory tree.
 * It therefore very much resembles any form of tar.bz2, tar.gz, rar or zip with the files beeing the data chunks.
 * 
 * From a pragmatic point of view this very much covers the functionality, but I wouldn't have done this library if it didn't provide at least some advantage or additional feature.
 *
 * To start reading and learning about libarxx you could start with ARX::Archive which is the entry point for almost every ARX session.
 * Also the ARX::Item is very important.
 * Additionally the Modules page is recommended for reading.
 **/

/**
 * @brief The ARX namespace which contains all the C++ classes you need to work with ARX archives.
 *
 * In this namespace the most important classes are ARX::Item and ARX::Archive.
 **/
namespace ARX
{
};

#endif

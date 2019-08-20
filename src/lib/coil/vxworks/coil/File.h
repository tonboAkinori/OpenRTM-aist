﻿// -*- C++ -*-
/*!
 * @file File_vxworks.h
 * @brief File functions
 * @date $Date$
 * @author Nobuhiko Miyamoto <n-miyamoto@aist.go.jp>
 *
 * Copyright (C) 2017
 *     Nobuhiko Miyamoto
 *     Robot Innovation Research Center
 *     National Institute of
 *         Advanced Industrial Science and Technology (AIST), Japan
 *     All rights reserved.
 *
 * $Id$
 *
 */

#ifndef COIL_FILE_H
#define COIL_FILE_H

#include <cstring>
#include <sys/types.h>
#include <dirent.h>


#include <coil/config_coil.h>
#include <coil/stringutil.h>

#ifdef __RTP__
#else
#include <pathLib.h>
#endif



namespace coil
{
const unsigned int MaxPathLength(256);
  /*!
   * @if jp
   *
   * @brief ファイルパスよりディレクトリ部分を取得する
   *
   * ファイルパスよりディレクトリ部分を取得する。
   *
   * @param path ファイルパス
   *
   * @return ディレクトリ名称
   *
   * @else
   *
   * @brief Get a directory part than a file pass
   *
   * Get a directory part than a file pass.
   *
   * @param path File path
   *
   * @return Directory name
   *
   * @endif
   */
  inline std::string dirname(const char* path)
  {
#ifdef __RTP__
    return "";
#else
    unsigned int len = strlen(path);
    char path_name[MaxPathLength+1];
    memset( path_name , '\0' , len+1 );
    char dir_name[MaxPathLength+1];
    memset( dir_name , '\0' , len+1 );
    char file_name[MaxPathLength+1];
    memset( file_name , '\0' , len+1 );
    if(len>1 && strncmp("/", &path[len-1], 1)==0)
    {
      strncpy(path_name, path, len-1);   
    }
    else
    {
      strcpy(path_name, path);
    }
    pathSplit(path_name, dir_name, file_name);
    std::string ret = dir_name;
    if(ret == "")
    {
      if(strncmp("/", &path_name[0], 1) == 0)
      {
        ret = "/";
      }
      else
      {
        ret = ".";
      }
    }
    return ret;
#endif
  }

  /*!
   * @if jp
   *
   * @brief ファイルパスよりファイル名部分を取得する
   *
   * ファイルパスよりファイル名部分を取得する。
   *
   * @param path ファイルパス
   *
   * @return ファイル名称
   *
   * @else
   *
   * @brief Get a file name part than a file pass
   *
   * Get a directory part than a file pass.
   *
   * @param path File path
   *
   * @return File name
   *
   * @endif
   */
  inline std::string basename(const char* path)
  {
#ifdef __RTP__
    return "";
#else
    unsigned int len = strlen(path);
    char path_name[MaxPathLength+1];
    memset( path_name , '\0' , len+1 );
    char dir_name[MaxPathLength+1];
    memset( dir_name , '\0' , len+1 );
    char file_name[MaxPathLength+1];
    memset( file_name , '\0' , len+1 );
    if(len>1 && strncmp("/", &path[len-1], 1)==0)
    {
      strncpy(path_name, path, len-1);   
    }
    else
    {
      strcpy(path_name, path);
    }
    pathSplit(path_name, dir_name, file_name);
    std::string ret = file_name;
    if(ret == "")
    {
      if(strncmp("/", &path_name[0], 1) == 0)
      {
        ret = "/";
      }
      else
      {
        ret = ".";
      }
    }
    return ret;
#endif
  }

  /*!
   * @if jp
   *
   * @brief ファイルリストを取得する
   *
   * ディレクトリパスの中で指定ファイルにマッチするリストを取得する。
   *
   * @param path ディレクトリパス
   * @param glob_str ファイル名
   *
   * @return ファイルリスト
   *
   * @else
   *
   * @brief Get file list
   *
   * Get a list matching a file designated than a directory path.
   *
   * @param path Directory path
   * @param glob_str File name
   *
   * @return File list
   *
   * @endif
   */
  inline coil::vstring filelist(const char* path, const char* glob_str = "")
  {
    struct dirent* ent; 
    coil::vstring flist;
    bool has_glob(false);
    std::string pattern;

    if (path == 0) { return flist; }
    if (glob_str[0] != '\0') { has_glob = true; }

    DIR* dir_ptr(::opendir(path));
    if (dir_ptr == 0) { return flist; }
    
    while ((ent = ::readdir(dir_ptr)) != 0)
      {
        bool match(true);
        if (has_glob)
          {
            const char* globc(glob_str);
            std::string fname(ent->d_name);
            for (size_t i(0); i < fname.size() && *globc != '\0'; ++i, ++globc)
              {
                if (*globc == '*')
                  {
                    // the last '*' matches every thing
                    if (globc[1] == '\0') { break; }
                    // consecutive * or + are skiped, but fname keeps pointer
                    if (globc[1] == '*' || globc[1] == '+') { --i; continue; }

                    // advance pointer and find normal characters
                    ++globc;
                    size_t pos(fname.find(*globc, i));
                    if (pos == std::string::npos) { match = false; break; }
                    // matched, and advance i to pos
                    i = pos;
                  }
                else if (*globc == '+')
                  {
                    // the last '+' matches last one or more characters
                    if (globc[1] == '\0' && !(i + 1 < fname.size())) { break; }
                    // consecutive * or + are skiped, but fname keeps pointer
                    if (globc[1] == '*' || globc[1] == '+') { --i; continue; }

                    // advance pointer and find normal characters
                    ++globc;
                    size_t pos(fname.find(*globc, i + 1));
                    if (pos == std::string::npos) { match = false; break; }
                    // matched, and advance i to pos
                    i = pos;
                  }
                else
                  {
                    if (fname[i] != *globc) { match = false; }
                  }
                
                // in the last fname character, if glob is not end,
                // or *, fname is not matched.
                if (i + 1 == fname.size() && 
                    globc[1] != '\0' && globc[1] != '*') { match = false; }
              }
          }
        if (match) { flist.push_back(ent->d_name); }
      }
    ::closedir(dir_ptr);

    return flist;
  }
};

#endif // COIL_FILE_H

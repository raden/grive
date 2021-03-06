/*
	grive: an GPL program to sync a local directory with Google Drive
	Copyright (C) 2012  Wan Wai Ho

	This program is free software; you can redistribute it and/or
	modify it under the terms of the GNU General Public License
	as published by the Free Software Foundation version 2
	of the License.

	This program is distributed in the hope that it will be useful,
	but WITHOUT ANY WARRANTY; without even the implied warranty of
	MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
	GNU General Public License for more details.

	You should have received a copy of the GNU General Public License
	along with this program; if not, write to the Free Software
	Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301, USA.
*/

#pragma once

#include <boost/format.hpp>

namespace gr {

namespace log
{
	enum Serverity { debug, verbose, info, warning, error, critical } ;
	typedef boost::format Fmt ;
}

/*!	\brief	Base class and singleton of log facilities
*/
class LogBase
{
public :
	virtual void Log( const log::Fmt& msg, log::Serverity s = log::info ) = 0 ;
	virtual void Enable( log::Serverity s, bool enable = true ) = 0 ;
	
	static LogBase* Inst( LogBase *log = 0 ) ;

protected :
	LogBase() ;
	~LogBase() ;
} ;

void Log( const std::string& str, log::Serverity s = log::info ) ;

template <typename P1>
void Log( const std::string& fmt, const P1& p1, log::Serverity s = log::info )
{
	LogBase::Inst()->Log( log::Fmt(fmt) % p1, s ) ;
}

template <typename P1, typename P2>
void Log( const std::string& fmt, const P1& p1, const P2& p2, log::Serverity s = log::info )
{
	LogBase::Inst()->Log( log::Fmt(fmt) % p1 % p2, s ) ;
}

template <typename P1, typename P2, typename P3>
void Log( const std::string& fmt, const P1& p1, const P2& p2, const P3& p3, log::Serverity s = log::info )
{
	LogBase::Inst()->Log( log::Fmt(fmt) % p1 % p2 % p3, s ) ;
}

void Trace( const std::string& str ) ;

template <typename P1>
void Trace( const std::string& fmt, const P1& p1 )
{
	LogBase::Inst()->Log( log::Fmt(fmt) % p1, log::debug ) ;
}

template <typename P1, typename P2>
void Trace( const std::string& fmt, const P1& p1, const P2& p2 )
{
	LogBase::Inst()->Log( log::Fmt(fmt) % p1 % p2, log::debug ) ;
}

template <typename P1, typename P2, typename P3>
void Trace( const std::string& fmt, const P1& p1, const P2& p2, const P3& p3 )
{
	LogBase::Inst()->Log( log::Fmt(fmt) % p1 % p2 % p3, log::debug ) ;
}

} // end of namespace

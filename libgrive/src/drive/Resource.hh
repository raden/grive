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

#include "Entry.hh"
#include "http/Agent.hh"
#include "util/Exception.hh"
#include "util/FileSystem.hh"

#include <string>
#include <vector>
#include <iosfwd>

namespace gr {

class Json ;

/*!	\brief	A resource can be a file or a folder in the google drive

	The google drive contains a number of resources, which is represented by this class.
	It also contains linkage to other resources, such as parent and childrens.
*/
class Resource
{
public :
	struct Error : virtual Exception {} ;

	typedef std::vector<Resource*> Children ;
	typedef Children::const_iterator iterator ;
	
public :
	Resource() ;
	explicit Resource( const xml::Node& entry ) ;
	explicit Resource( const Entry& entry, Resource *parent = 0 ) ;
	explicit Resource( const fs::path& path ) ;
	explicit Resource( const Json& json, Resource *parent = 0 ) ;
	void Swap( Resource& coll ) ;
	
	// default copy ctor & op= are fine

	bool IsFolder() const ;

	std::string Name() const ;
	std::string SelfHref() const ;
	std::string ResourceID() const ;
	std::string ParentHref() const ;
	
	const Resource* Parent() const ;
	Resource* Parent() ;
	void AddChild( Resource *child ) ;
	Resource* FindChild( const std::string& title ) ;
	
	fs::path Path() const ;
	bool IsInRootTree() const ;

	void FromRemote( const Entry& e ) ;
	void FromLocal() ;
	
	void Sync( http::Agent *http, const http::Headers& auth ) ;
	void Delete( http::Agent* http, const http::Headers& auth ) ;

	Json Serialize() const ;
	
	// children access
	iterator begin() const ;
	iterator end() const ;
	std::size_t size() const ;
	
private :
	/// State of the resource. indicating what to do with the resource
	enum State
	{
		/// The best state: the file is the same in remote and in local.
		sync,
		
		/// Resource created in local, but remote does not have it.
		/// We should create the resource in google drive and upload new content
		local_new,
		
		/// Resource exists in both local & remote, but changes in local is newer
		/// than remote. We should upload local copy to overwrite remote.
		local_changed,
		
		/// Resource deleted from local since last time grive has checked.
		local_deleted,
		
		/// Resource created in google drive, but not exist in local.
		/// We should download the file.
		remote_new,
		
		/// Resource exists in both local & remote, but remote is newer.		
		remote_changed
	} ;

	friend std::ostream& operator<<( std::ostream& os, State s ) ;
	
private :
	void Download( http::Agent* http, const fs::path& file, const http::Headers& auth ) const ;
	bool EditContent( http::Agent* http, const http::Headers& auth ) ;
	bool Create( http::Agent* http, const http::Headers& auth ) ;
	bool Upload( http::Agent* http, const std::string& link, const http::Headers& auth, bool post ) ;
	
private :
	Entry					m_entry ;
	
	// not owned
	Resource				*m_parent ;
	std::vector<Resource*>	m_child ;
	
	State					m_state ;
} ;

} // end of namespace

namespace std
{
	void swap( gr::Resource& c1, gr::Resource& c2 ) ;
}

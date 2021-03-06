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

#include "ResourceTree.hh"
#include "CommonUri.hh"

#include "protocol/Json.hh"
#include "util/Destroy.hh"
#include "util/Log.hh"

#include <algorithm>
#include <cassert>

namespace gr {

using namespace details ;

ResourceTree::ResourceTree( ) :
	m_root( new Resource )
{
	m_set.insert( m_root ) ;
}

ResourceTree::ResourceTree( const ResourceTree& fs ) :
	m_root( 0 )
{
	const Set& s = fs.m_set.get<ByIdentity>() ;
	for ( Set::const_iterator i = s.begin() ; i != s.end() ; ++i )
	{
		Resource *c = new Resource( **i ) ;
		if ( c->SelfHref() == root_href )
			m_root = c ;
		
		m_set.insert( c ) ;
	}
	
	assert( m_root != 0 ) ;
}

ResourceTree::~ResourceTree( )
{
	Clear() ;
}

void ResourceTree::Clear()
{
	// delete all pointers
	const Set& s = m_set.get<ByIdentity>() ;
	std::for_each( s.begin(), s.end(), Destroy() ) ;
	
	m_set.clear() ;
	m_root = 0 ;
}

Resource* ResourceTree::Root()
{
	assert( m_root != 0 ) ;
	return m_root ;
}

const Resource* ResourceTree::Root() const
{
	assert( m_root != 0 ) ;
	return m_root ;
}

void ResourceTree::Swap( ResourceTree& fs )
{
	m_set.swap( fs.m_set ) ;
}

ResourceTree& ResourceTree::operator=( const ResourceTree& fs )
{
	ResourceTree tmp( fs ) ;
	Swap( tmp ) ;
	return *this ;
}

Resource* ResourceTree::FindByHref( const std::string& href )
{
	if ( href.empty() )
		return 0 ;

	HrefMap& map = m_set.get<ByHref>() ;
	HrefMap::iterator i = map.find( href ) ;
	return i != map.end() ? *i : 0 ;
}

const Resource* ResourceTree::FindByHref( const std::string& href ) const
{
	const HrefMap& map = m_set.get<ByHref>() ;
	HrefMap::const_iterator i = map.find( href ) ;
	return i != map.end() ? *i : 0 ;
}

/// Unlike other search functions, this one does not depend on the multi-index
/// container. It traverses the tree instead.
Resource* ResourceTree::FindByPath( const fs::path& path )
{
	// not yet implemented
	assert( false ) ;
	return false ;
}

///	Reinsert should be called when the ID/HREF were updated
bool ResourceTree::ReInsert( Resource *coll )
{
	Set& s = m_set.get<ByIdentity>() ;
	Set::iterator i = s.find( coll ) ;
	if ( i != s.end() )
	{
		s.erase( i ) ;
		m_set.insert( coll ) ;
		return true ;
	}
	else
		return false ;
}

void ResourceTree::Insert( Resource *coll )
{
	m_set.insert( coll ) ;
}

void ResourceTree::Erase( Resource *coll )
{
	Set& s = m_set.get<ByIdentity>() ;
	s.erase( s.find( coll ) ) ;
}

void ResourceTree::Update( Resource *coll, const Entry& e )
{
	assert( coll != 0 ) ;

	coll->FromRemote( e ) ;
	ReInsert( coll ) ;
}

ResourceTree::iterator ResourceTree::begin()
{
	return m_set.get<ByIdentity>().begin() ;
}

ResourceTree::iterator ResourceTree::end()
{
	return m_set.get<ByIdentity>().end() ;
}

void ResourceTree::Read( const Json& json )
{
	Clear() ;
	m_root = new Resource ;
	AddTree( m_root, json ) ;
}

void ResourceTree::AddTree( Resource *node, const Json& json )
{
	assert( node != 0 ) ;
	m_set.insert( node ) ;
	
	std::vector<Json> array = json["child"].AsArray() ;
	for ( std::vector<Json>::iterator i = array.begin() ; i != array.end() ; ++i )
	{
		Resource *c = new Resource( *i, node ) ;
		node->AddChild( c ) ;
		AddTree( c, *i ) ;
	}
}

Json ResourceTree::Serialize() const
{
	return m_root->Serialize() ;
}

} // end of namespace

//  SuperTuxKart - a fun racing game with go-kart
//  Copyright (C) 2010 Lucas Baudin, Joerg Henrichs
//
//  This program is free software; you can redistribute it and/or
//  modify it under the terms of the GNU General Public License
//  as published by the Free Software Foundation; either version 3
//  of the License, or (at your option) any later version.
//
//  This program is distributed in the hope that it will be useful,
//  but WITHOUT ANY WARRANTY; without even the implied warranty of
//  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
//  GNU General Public License for more details.
//
//  You should have received a copy of the GNU General Public License
//  along with this program; if not, write to the Free Software
//  Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA  02111-1307, USA.
/**
  \page addons Addons
  */

#include "addons/addon.hpp"

#include <fstream>
#include <time.h>

#include "io/file_manager.hpp"
#include "io/xml_node.hpp"
#include "utils/string_utils.hpp"

Addon::SortOrder Addon::m_sort_order=Addon::SO_DEFAULT;

Addon::Addon(const XMLNode &xml)
{
    m_name               = "";
    m_id                 = "";
    m_designer           = "";
    m_status             = 0;
    m_installed          = false;
    m_installed_revision = 0;
    m_revision           = 0 ;
    m_zip_file           = "";
    m_description        = "";
    m_icon_url           = "";
    m_icon_basename      = "";
    m_icon_revision      = 0;
    m_size               = 0;
    m_date               = 0;
    m_icon_ready         = false;
    m_type               = xml.getName();

    // FIXME: temporarily till the web page is updated.
    if(m_type=="track")
    {
        int is_arena=0;
        xml.get("arena",          &is_arena            );
        if(is_arena) m_type="arena";
    }
    xml.get("name",               &m_name              );
    m_id                 = StringUtils::toLowerCase(m_name);
    xml.get("id",                 &m_id                );
    xml.get("designer",           &m_designer          );
    xml.get("status",             &m_status            );
    xml.get("date",               &m_date              );
    xml.get("installed",          &m_installed         );
    xml.get("installed-revision", &m_installed_revision);
    xml.get("revision",           &m_revision          );
    xml.get("file",               &m_zip_file          );
    xml.get("description",        &m_description       );
    
    // resolve XML entities
    m_description = StringUtils::replace(m_description, "&#10;", "\n");
    m_description = StringUtils::replace(m_description, "&#13;", ""); // ignore \r

    xml.get("image",              &m_icon_url          );
    xml.get("icon-revision",      &m_icon_revision     );
    xml.get("size",               &m_size              );
    m_icon_basename = StringUtils::getBasename(m_icon_url);
};   // Addon(const XML&)

// ----------------------------------------------------------------------------
/** Copies the installation data (like description, revision, icon) from the 
 *  downloaded online list to this entry.
*/
void Addon::copyInstallData(const Addon &addon)
{
    m_description   = addon.m_description;
    m_revision      = addon.m_revision;
    m_zip_file      = addon.m_zip_file;
    m_icon_url      = addon.m_icon_url;
    m_icon_basename = addon.m_icon_basename;
    m_icon_revision = addon.m_revision;
    m_designer      = addon.m_designer;
    m_status        = addon.m_status;
    m_date          = addon.m_date;
    // Support if the type of an addon changes, e.g. this ie necessary
    // when we introduce 'arena' as type (formerly arenas had type 'track').
    m_type          = addon.m_type;
}   // copyInstallData

// ----------------------------------------------------------------------------
/** Writes information about an installed addon (it is only called for 
 *  installed addons).
 *  \param out_stream Output stream to write to.
 */
void Addon::writeXML(std::ofstream *out_stream)
{
    (*out_stream) << "  <"                       << m_type 
                  << " name=\""                  << m_name 
                  << "\" id=\""                  << m_id 
                  << "\" designer=\""            << m_designer
                  << "\" status=\""              << m_status
                  << "\" date=\""                << m_date
                  << "\" installed=\""         
                  << (m_installed ? "true" : "false" )
                  << "\" installed-revision=\""  << m_installed_revision
                  << "\" size=\""                << m_size
                  << "\" icon-revision=\""       << m_icon_revision 
                  << "\"/>\n";
}   // writeXML

// ----------------------------------------------------------------------------
std::string Addon::getDateAsString() const
{    
    const struct tm *t = gmtime((time_t*)&m_date);
    char s[16];
    strftime(s, 128, "%d.%m.%Y", t);
    return s;
}   // getDateAsString

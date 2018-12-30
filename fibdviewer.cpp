#include "fibdviewer.h"

FiBDViewer::FiBDViewer()
    : m_path(""),
      m_folder(false),
      m_searched(false),
      m_searchFocused(false),
      m_selected(false)
{

}

FiBDViewer::FiBDViewer(const FiBDViewer &fi)
    : m_path(fi.m_path),
      m_folder(fi.m_folder),
      m_searched(fi.m_searched),
      m_searchFocused(fi.m_searchFocused),
      m_selected(fi.m_selected)
{

}

FiBDViewer::~FiBDViewer()
{
}

void FiBDViewer::setSearched(bool searched)
{
    m_searched = searched;
}
void FiBDViewer::setSearchFocused(bool focused)
{
    m_searchFocused = focused;
}
bool FiBDViewer::searched()
{
    return m_searched;
}
bool FiBDViewer::searchFocused()
{
    return m_searchFocused;
}

void FiBDViewer::setSelected(bool selected)
{
    m_selected = selected;
}
bool FiBDViewer::selected()
{
    return m_selected;
}

bool FiBDViewer::isFolder()
{
    return m_folder;
}

std::string FiBDViewer::path()
{
    return m_path;
}

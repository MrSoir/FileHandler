#ifndef FIBDVIEWER_H
#define FIBDVIEWER_H

#include <QObject>
#include <string>
#include "filesearcher.h"
#include "fileselector.h"

class FiBDViewer : public Searchable,
                   public Selectable
{
public:
    FiBDViewer();
    FiBDViewer(const FiBDViewer& fi);
    ~FiBDViewer();


    virtual void setSearched(bool searched);
    virtual void setSearchFocused(bool focused);
    bool searched();
    bool searchFocused();

    virtual void setSelected(bool selected);
    bool selected();

    bool isFolder();

    virtual std::string path();
private:
    std::string m_path;
    bool m_folder;
    bool m_searched;
    bool m_searchFocused;
    bool m_selected;
};

#endif // FIBDVIEWER_H

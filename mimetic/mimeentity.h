/***************************************************************************
    copyright            : (C) 2002-2008 by Stefano Barbato
    email                : stefano@codesink.org

    $Id: mimeentity.h,v 1.29 2008-10-07 11:06:25 tat Exp $
 ***************************************************************************/
#ifndef _MIMETIC_MIMEENTITY_H_
#define _MIMETIC_MIMEENTITY_H_
#include <string>
#include <iostream>
#include <streambuf>
#include <fstream>
#include <iterator>
#include <algorithm>
#include <memory>
#include <mimetic/strutils.h>
#include <mimetic/utils.h>
#include <mimetic/contenttype.h>
#include <mimetic/contenttransferencoding.h>
#include <mimetic/contentdisposition.h>
#include <mimetic/mimeversion.h>
#include <mimetic/mimeentitylist.h>
#include <mimetic/codec/codec.h>
#include <mimetic/os/file.h>
#include <mimetic/header.h>
#include <mimetic/body.h>
#include <mimetic/parser/itparserdecl.h>
#include <mimetic/streambufs.h>


namespace mimetic
{

class MimeEntity;


/// Represent a MIME entity    
class MimeEntity : public std::enable_shared_from_this<MimeEntity>
{
    friend class Body;
    friend class MimeEntityLoader;
    typedef std::list<std::string> BoundaryList;
    typedef unsigned long int size_type;
public:
    /**
     *  Blank MIME entity
     */
    static std::shared_ptr<MimeEntity> create();
    /**
     *  Parse [beg, end] and build entity based on content
     */
    template<typename Iterator>
    static std::shared_ptr<MimeEntity> create(Iterator beg, Iterator end, int mask = imNone);
    /**
     *  Parse istream and build entity based on content
     */
    static std::shared_ptr<MimeEntity> create(std::istream&);

#if __cplusplus >= 201103L
    MimeEntity(const MimeEntity&) = delete;
    MimeEntity& operator=(const MimeEntity&) = delete;
#endif

    virtual ~MimeEntity();

    /**
     * copy text rapresentation of the MimeEntity to the output iterator
     */
    template<typename OutputIt>
    size_type copy(OutputIt out);

    Header& header();
    const Header& header() const;

    Body& body();
    const Body& body() const;

    /** 
     * single step load functions: parse the input provided and build the
     * entity
     *
     * use load(..., mask) to ignore some part of the message when it's
     * not needed saving memory space and execution time
     */
    template<typename Iterator>
    void load(Iterator, Iterator, int mask = imNone);
    void load(std::istream&, int mask = imNone);

    /**
     * helper functions: return header().hasField(str)
     */
    bool hasField(const std::string&) const;

    /**
     * returns entity size 
     * Note: this function is slow, use it if you really need
     */
    size_type size() const;
    friend std::ostream& operator<<(std::ostream&, const MimeEntity&);

protected:
    MimeEntity();
    void commonInit();

    virtual std::ostream& write(std::ostream&, const char* eol = 0) const;

    template<class Type>
    static std::shared_ptr<Type> init(const std::shared_ptr<Type>& mimeEntity) {
        mimeEntity->commonInit();
        return mimeEntity;
    }

protected:
    Header m_header;
    Body m_body;
    size_type m_lines;
    size_type m_size;

private:
#if __cplusplus < 201103L
    MimeEntity(const MimeEntity&);
    MimeEntity& operator=(const MimeEntity&);
#endif
};

template<typename Iterator>
std::shared_ptr<MimeEntity> MimeEntity::create(Iterator bit, Iterator eit, int mask)
{
    std::shared_ptr<MimeEntity> entity = MimeEntity::create();
    entity->load(bit, eit, mask);
    return entity;
}


template<typename Iterator>
void MimeEntity::load(Iterator bit, Iterator eit, int mask)
{
    IteratorParser<Iterator, 
        typename std::iterator_traits<Iterator>::iterator_category> prs(shared_from_this());
    prs.iMask(mask);
    prs.run(bit, eit);
}

template<typename OutputIt>
MimeEntity::size_type MimeEntity::copy(OutputIt out)
{
    passthrough_streambuf<OutputIt> psb(out);
    std::ostream os(&psb);
    os << *this;
    return psb.size();
}

}

#endif

#include "metadataparser.h"
#include "nwutils.h"
#include "N0Slib.h"

MetaDataParser::MetaDataParser()
{
}


MetaData::MetaData() : duration(-1)
{
}

QString MetaData::toJson() {
    std::stringstream ss;
    nw::JsonWriter jw(ss);
    NwUtils::describe(jw, "duration", this->duration);
    jw.describeArray("chapters", "chapter", this->chapters.size());
    for (int i=0; jw.enterNextElement(i); ++i) {
        MetaDataChapter c = this->chapters[i];
        NwUtils::describe(jw, "start", c.start);
        NwUtils::describe(jw, "end", c.end);
        NwUtils::describe(jw, "title", c.title);
    }
    jw.close();
    return QString(ss.str().c_str());
}

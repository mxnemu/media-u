#include "metadataparser.h"
#include "nwutils.h"
#include "N0Slib.h"

MetaDataParser::MetaDataParser()
{
}


MetaData::MetaData() : duration(-1)
{
}

void MetaData::describe(nw::Describer* de) {
    NwUtils::describe(*de, "duration", this->duration);
    de->describeArray("chapters", "chapter", this->chapters.size());
    for (int i=0; de->enterNextElement(i); ++i) {
        MetaDataChapter c = this->chapters[i];
        NwUtils::describe(*de, "start", c.start);
        NwUtils::describe(*de, "end", c.end);
        NwUtils::describe(*de, "title", c.title);
    }
}

QString MetaData::toJson() {
    std::stringstream ss;
    nw::JsonWriter jw(ss);
    this->describe(&jw);
    jw.close();
    return QString(ss.str().c_str());
}

std::pair<int, int> MetaData::resolution() const {
    foreach (MetaDataTrack track, this->tracks) {
        if (track.type == video) {
            return std::pair<int,int>(track.track.video.resolutionX, track.track.video.resolutionY);
        }
    }
    return std::pair<int,int>(-1,-1);
}

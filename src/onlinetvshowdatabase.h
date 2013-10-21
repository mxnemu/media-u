#ifndef ONLINETVSHOWDATABASE_H
#define ONLINETVSHOWDATABASE_H

namespace OnlineTvShowDatabase {

enum UpdateFilter {
    ufInvalid = 0,
    ufSynopsis = 1 << 1,
    ufTitle = 1 << 2,
    ufRelations = 1 << 3,
    ufAiringDates = 1 << 4,
    ufSynonyms = 1 << 5,
    ufAll = ((unsigned int)-1)
};


class Client
{
public:
    Client();
};

}

#endif // ONLINETVSHOWDATABASE_H

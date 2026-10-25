#ifndef TIVIDY_FALLBACKHANDLER_H
#define TIVIDY_FALLBACKHANDLER_H

#include "RequestHandler.h"

class FallbackHandler : public RequestHandler {
public:
    FallbackHandler();
    ~FallbackHandler() override;

    void handle(ApprovalRequest& request) override;
};

#endif // TIVIDY_FALLBACKHANDLER_H

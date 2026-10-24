#ifndef TIVIDY_REQUESTHANDLER_H
#define TIVIDY_REQUESTHANDLER_H

#include <cstddef>

class ApprovalRequest;

class RequestHandler {
public:
    virtual ~RequestHandler();
    void setSuccessor(RequestHandler* successor);
    virtual void handle(ApprovalRequest& request) = 0;
protected:
    RequestHandler* successor;
};

#endif // TIVIDY_REQUESTHANDLER_H

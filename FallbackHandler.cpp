#include "FallbackHandler.h"
#include "ApprovalRequest.h"

FallbackHandler::FallbackHandler() : RequestHandler() {}

FallbackHandler::~FallbackHandler(){}

void FallbackHandler::handle(ApprovalRequest& request)
{
    if (request.isOpen())
    {
        request.markRejected();
    }
}

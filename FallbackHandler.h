#ifndef TIVIDY_FALLBACKHANDLER_H
#define TIVIDY_FALLBACKHANDLER_H

#include "RequestHandler.h"

/**
 * @brief The last handler in the approval chain.
 *
 * If a request reaches this handler and is still open, nobody above the
 * requester can take it, so it is rejected.
 */
class FallbackHandler : public RequestHandler
{
public:
    /**
     * @brief Creates the end-of-chain handler.
     */
    FallbackHandler();

    /**
     * @brief Destroys the handler.
     */
    ~FallbackHandler() override;

    /**
     * @brief Rejects the request if it is still open.
     * @param request The approval request to reject. It is changed in place.
     */
    void handle(ApprovalRequest &request) override;
};

#endif // TIVIDY_FALLBACKHANDLER_H

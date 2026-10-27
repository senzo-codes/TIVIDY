#ifndef TIVIDY_REQUESTHANDLER_H
#define TIVIDY_REQUESTHANDLER_H

#include <cstddef>

class ApprovalRequest;

/**
 * @brief Base class of the approval chain.
 *
 * Each handler covers one authority level. It either offers an approval
 * request to someone at that level, or passes the request to its successor.
 */

class RequestHandler
{
public:
    /**
     * @brief Creates a handler with no successor.
     */
    RequestHandler();

    /**
     * @brief Destroys the handler and deletes the rest of the chain.
     */
    virtual ~RequestHandler();

    /**
     * @brief Sets the next handler in the chain.
     * @param successor The handler to call when this one passes the request on.
     *                  The chain takes ownership of it.
     */
    void setSuccessor(RequestHandler *successor);

    /**
     * @brief Offers the request to someone at this level, or passes it on.
     * @param request The approval request to handle. It is changed in place.
     */
    virtual void handle(ApprovalRequest &request) = 0;

protected:
    RequestHandler *successor;
};

#endif // TIVIDY_REQUESTHANDLER_H

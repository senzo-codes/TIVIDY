#include "RequestHandler.h"

RequestHandler::RequestHandler()
    : successor(nullptr) {}

RequestHandler::~RequestHandler()
{
    delete successor;
}

void RequestHandler::setSuccessor(RequestHandler* successor)
{
    this->successor = successor;
}

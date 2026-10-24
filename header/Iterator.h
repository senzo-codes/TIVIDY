#ifndef TIVIDY_ITERATOR_H
#define TIVIDY_ITERATOR_H

class TaskComponent;

class Iterator {
public:
    virtual ~Iterator();
    virtual bool hasNext() const = 0;
    virtual TaskComponent* next() = 0;
};

#endif // TIVIDY_ITERATOR_H

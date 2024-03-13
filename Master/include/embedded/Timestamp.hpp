#include <elapsedMillis.h>

class Timestamp {
private:
    elapsedMillis timeElapsed;

public:
    void update() {
        timeElapsed = 0;
    }

    bool hasTimedOut(unsigned long timeout) const {
        return timeElapsed > timeout;
    }
};
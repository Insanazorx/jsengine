#pragma once
#include <cstdint>

#include "Forward.h"

namespace js::GarbageCollector {
    class Visitor;

    class Cell {
    public:
        Cell() = default;
        virtual ~Cell() = default;

        virtual bool is_cell() const { return true; }

        virtual void get_visited(Visitor& visitor) = 0;

        void mark () { marked = 1; }
        void unmark () { marked = 0; }
        bool is_marked () const { return marked; }

    private:
        Cell* next {nullptr};
        uint8_t marked : 1 {0};
        uint16_t size : 11 {0};
        uint8_t type_id : 4 {0};
    };

}

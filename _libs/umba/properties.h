#pragma once

//----------------------------------------------------------------------------

/*! \file
    \brief Реализация properties на чистом C++
*/

//----------------------------------------------------------------------------
//! Определяет указатель на структуру по указателю на поле экземпляра структуры
#define UMBA_CONTAINING_RECORD(address, type, field) ((type *)(\
    1+(char*)(address) - \
    (uintptr_t)(&((type *)1)->field)))


//! Определяет RW-property. Структура должна иметь методы getProp_Имя и setProp_ИМЯ
#define UMBA_DECLARE_PROPERTY_RW(OWNERNAME, TYPE, NAME)\
    struct propclass_##NAME {       \
        typedef TYPE property_type; \
        uint8_t dummy;              \
        /*propclass_##NAME() : dummy() {}*/ \
        inline operator property_type() {   \
            return UMBA_CONTAINING_RECORD(this, OWNERNAME, NAME)-> getProp_##NAME ();\
        }                                                 \
        inline void operator=(const property_type &src) { \
            UMBA_CONTAINING_RECORD(this, OWNERNAME, NAME)-> setProp_##NAME (src);\
        }             \
        /*private:*/  \
        /*explicit propclass_##NAME(const propclass_##NAME &src) {} */ \
        /*inline void operator=(const propclass_##NAME &src) {}     */ \
    } NAME

//! Определяет RO-property. Структура должна иметь метод getProp_Имя
#define UMBA_DECLARE_PROPERTY_R(OWNERNAME, TYPE, NAME) \
    struct propclass_##NAME {       \
        typedef TYPE property_type; \
        uint8_t dummy;              \
        inline operator property_type() { \
            return UMBA_CONTAINING_RECORD(this, OWNERNAME, NAME)-> getProp_##NAME ();\
        } \
    } NAME

//! Определяет RO-property. Структура должна иметь метод getProp_Имя (алиас для #UMBA_DECLARE_PROPERTY_R).
#define UMBA_DECLARE_PROPERTY_RO(OWNERNAME, TYPE, NAME) UMBA_DECLARE_PROPERTY_R(OWNERNAME, TYPE, NAME) \

//! Определяет WO-property. Структура должна иметь метод setProp_ИМЯ
#define UMBA_DECLARE_PROPERTY_W(OWNERNAME, TYPE, NAME) \
    struct propclass_##NAME {       \
        typedef TYPE property_type; \
        uint8_t dummy;              \
        inline void operator=(const property_type &src) { \
            UMBA_CONTAINING_RECORD(this, OWNERNAME, NAME)-> setProp_##NAME (src); \
        }        \
        private: \
        inline void operator=(const propclass_##NAME &src) {}\
    } NAME

//! Определяет WO-property. Структура должна иметь метод setProp_ИМЯ (алиас для #UMBA_DECLARE_PROPERTY_W).
#define UMBA_DECLARE_PROPERTY_WO(OWNERNAME, TYPE, NAME) UMBA_DECLARE_PROPERTY_W(OWNERNAME, TYPE, NAME)



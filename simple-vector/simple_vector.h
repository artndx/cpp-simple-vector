#pragma once
#include <initializer_list>
#include <stdexcept>
#include <utility>
#include "array_ptr.h"

using std::move;

class ReserveProxyObj {
public:
    ReserveProxyObj(size_t capacity)
        : capacity_(capacity)
    {
    }
    
    size_t GetCapacity() {
        return capacity_;
    }
    
private:
    size_t capacity_;
};

ReserveProxyObj Reserve(size_t capacity_to_reserve) {
    return ReserveProxyObj(capacity_to_reserve);
}

template <typename Type>
class SimpleVector {
public:
    using Iterator = Type*;
    using ConstIterator = const Type*;

    SimpleVector() noexcept = default;

    // Создаёт вектор из size элементов, инициализированных значением по умолчанию
    explicit SimpleVector(size_t size) {
        ArrayPtr<Type> new_items(size);
        
        for(size_t i = 0; i < size;++i){
            new_items[i] = move(Type());
        }

        items_.swap(new_items);
        size_ = size;
        capacity_ = size;
    }

    // Создаёт вектор из size элементов, инициализированных значением value
    SimpleVector(size_t size, const Type& value) {
        ArrayPtr<Type> new_items(size);
        
        for(size_t i = 0; i < size;++i){
            new_items[i] = move(value);
        }
        
        items_.swap(new_items);
        size_ = size;
        capacity_ = size;
    }

    // Создаёт вектор из std::initializer_list
    SimpleVector(std::initializer_list<Type> init) {
        size_t size = init.size();
        ArrayPtr<Type> new_items(size);
        
        size_t index = 0;
        for(Type element : init){
            new_items[index++] = move(element); 
        }
        
        items_.swap(new_items);
        size_ = size;
        capacity_ = size;
    }

    explicit SimpleVector(ReserveProxyObj obj)
    {
        Reserve(obj.GetCapacity());
    }

    SimpleVector(const SimpleVector<Type>& other){
        size_t new_size = other.GetSize();
        ArrayPtr<Type> new_items(new_size);
        for(size_t i = 0; i < new_size;++i){
            new_items[i] = other[i];
        }
        items_.swap(new_items);
        size_ = new_size;
        capacity_ = new_size;
    }

    SimpleVector(SimpleVector<Type>&& other){
        swap(other);
    }

    void swap(SimpleVector& other) noexcept {
        items_.swap(other.items_);
        std::swap(size_, other.size_);
        std::swap(capacity_, other.capacity_);

    }

    // Возвращает количество элементов в массиве
    size_t GetSize() const noexcept {
        return size_;
        return 0;
    }

    // Возвращает вместимость массива
    size_t GetCapacity() const noexcept {
        return capacity_;
        return 0;
    }

    // Сообщает, пустой ли массив
    bool IsEmpty() const noexcept {
        return (size_ == 0);
    }

    // Возвращает ссылку на элемент с индексом index
    Type& operator[](size_t index) noexcept {
        return items_[index];
    }

    // Возвращает константную ссылку на элемент с индексом index
    const Type& operator[](size_t index) const noexcept {
        return items_[index];
    }

    SimpleVector& operator=(SimpleVector rhs){
        if(this != &rhs){
            SimpleVector<Type> copy(rhs);
            swap(rhs);
        }
        return *this;
    }

    // Возвращает константную ссылку на элемент с индексом index
    // Выбрасывает исключение std::out_of_range, если index >= size
    Type& At(size_t index) {
        if(index >= size_){
            throw std::out_of_range("index >= size");
        }
        return items_[index];
    }

    // Возвращает константную ссылку на элемент с индексом index
    // Выбрасывает исключение std::out_of_range, если index >= size
    const Type& At(size_t index) const {
        if(index >= size_){
            throw std::out_of_range("index >= size");
        }
        return items_[index];
    }

    // Обнуляет размер массива, не изменяя его вместимость
    void Clear() noexcept {
        Resize(0);
    }

    // Изменяет размер массива.
    // При увеличении размера новые элементы получают значение по умолчанию для типа Type
    void Resize(size_t new_size) {
        if(new_size <= size_){
            size_ = new_size;
        } else if(new_size > size_ && new_size <= capacity_){
            for(size_t i = size_; i < new_size; ++i){
               items_[i] = move(Type()); 
            }
            size_ = new_size;
        } else{
            ArrayPtr<Type> new_items(new_size);
            for(size_t i = 0; i < size_;++i){
                new_items[i] = move(items_[i]);
            }
            for(size_t i = size_; i < new_size;++i){
                new_items[i] = move(Type());
            }
            items_.swap(new_items);
            size_ = new_size;
            capacity_ = std::max(new_size,capacity_ * 2);
        }
    }
    void Reserve(size_t new_capacity){
        if(new_capacity <= capacity_){
            return;
        } else{
            ArrayPtr<Type> new_items(new_capacity);
            for(size_t i = 0; i < size_;++i){
                new_items[i] = move(items_[i]); 
            }
            for(size_t i = size_; i < new_capacity;++i){
                new_items[i] = move(Type()); 
            }

            items_.swap(new_items);
            capacity_ = new_capacity;   
        }
        
    }

    void PushBack(const Type& value){
        if(size_ < capacity_){
            items_[size_++] = value;
        } else {
            if(capacity_ == 0){
                ++capacity_;
            } 
            ArrayPtr<Type> new_items(capacity_*2);
            for(size_t i = 0; i <size_;++i){
                new_items[i] = items_[i];
            }
            new_items[size_] = value;
            items_.swap(new_items);
            ++size_;
            capacity_ *= 2;
        }
    }

    void PushBack(Type&& value){
        if(size_ < capacity_){
            items_[size_++] = move(value);
        } else {
            if(capacity_ == 0){
                ++capacity_;
            } 
            ArrayPtr<Type> new_items(capacity_*2);
            for(size_t i = 0; i <size_;++i){
                new_items[i] = move(items_[i]);
            }
            new_items[size_] = move(value);
            items_.swap(new_items);
            ++size_;
            capacity_ *= 2;
        }
    }

    void PopBack() noexcept{
        if(size_){
            --size_;
        }
    }
    Iterator Insert(ConstIterator pos, const Type& value){
        if(pos < begin() || pos > end()){
            throw std::out_of_range("Uncorrect pos");
        }
        size_t pos_index = pos - items_.Get();
        if(size_ < capacity_){
            for(size_t i = size_; i > pos_index;--i){
                items_[i] = items_[i-1];
            }
            items_[pos_index] = value;
            ++size_;
        } else{
            if(size_){
                ArrayPtr<Type> new_items(capacity_*2);
                for(size_t i = 0; i < pos_index;++i){
                    new_items[i] = items_[i];
                }
                new_items[pos_index] = move(value);
                for(size_t i = pos_index + 1; i <= size_;++i){
                    new_items[i] = items_[i-1];
                }
                items_.swap(new_items);
                ++size_;
                capacity_ *= 2;
            } else {
                ArrayPtr<Type> new_items(1);
                new_items[0] = value;
                items_.swap(new_items);
                size_ = 1;
                capacity_ = 1; 
            }
        }

        return const_cast<Iterator>(begin()+pos_index); 
    }

    Iterator Insert(ConstIterator pos, Type&& value){
        if(pos < begin() || pos > end()){
            throw std::out_of_range("Uncorrect pos");
        }
        size_t pos_index = pos - items_.Get();
        if(size_ < capacity_){
            for(size_t i = size_; i > pos_index;--i){
                items_[i] = move(items_[i-1]);
            }
            items_[pos_index] = move(value);
            ++size_;
        } else{
            if(size_){
                ArrayPtr<Type> new_items(capacity_*2);
                for(size_t i = 0; i < pos_index;++i){
                    new_items[i] = move(items_[i]);
                }
                new_items[pos_index] = move(value);
                for(size_t i = pos_index + 1; i <= size_;++i){
                    new_items[i] = move(items_[i-1]);
                }
                items_.swap(new_items);
                ++size_;
                capacity_ *= 2;
            } else {
                ArrayPtr<Type> new_items(1);
                new_items[0] = move(value);
                items_.swap(new_items);
                size_ = 1;
                capacity_ = 1; 
            }
        }

        return const_cast<Iterator>(begin()+pos_index); 
    }
    

    Iterator Erase(ConstIterator pos){
        size_t pos_index = pos - items_.Get();
        for(size_t i = pos_index; i < size_;++i){
            items_[i] = move(items_[i + 1]);
        }
        --size_;
        return (begin() + pos_index);
    }

    // Возвращает итератор на начало массива
    // Для пустого массива может быть равен (или не равен) nullptr
    Iterator begin() noexcept {
        return items_.Get();
    }

    // Возвращает итератор на элемент, следующий за последним
    // Для пустого массива может быть равен (или не равен) nullptr
    Iterator end() noexcept {
        return items_.Get() + size_;
    }

    // Возвращает константный итератор на начало массива
    // Для пустого массива может быть равен (или не равен) nullptr
    ConstIterator begin() const noexcept {
        return items_.Get();
    }

    // Возвращает итератор на элемент, следующий за последним
    // Для пустого массива может быть равен (или не равен) nullptr
    ConstIterator end() const noexcept {
        return items_.Get() + size_;
    }

    // Возвращает константный итератор на начало массива
    // Для пустого массива может быть равен (или не равен) nullptr
    ConstIterator cbegin() const noexcept {
        return items_.Get();
    }

    // Возвращает итератор на элемент, следующий за последним
    // Для пустого массива может быть равен (или не равен) nullptr
    ConstIterator cend() const noexcept {
        return items_.Get() + size_;
    }
private:
    ArrayPtr<Type> items_;
    size_t size_ = 0;
    size_t capacity_ = 0;
};

template <typename Type>
bool operator==(const SimpleVector<Type>& lhs, const SimpleVector<Type>& rhs) {
    return std::equal(
        lhs.begin(),lhs.end(),
        rhs.begin(),rhs.end()
    );
}

template <typename Type>
bool operator!=(const SimpleVector<Type>& lhs, const SimpleVector<Type>& rhs) {
    return !(lhs == rhs);
}

template <typename Type>
bool operator<(const SimpleVector<Type>& lhs, const SimpleVector<Type>& rhs) {
    return std::lexicographical_compare(
        lhs.cbegin(),lhs.cend(),
        rhs.cbegin(),rhs.cend()
    );
}

template <typename Type>
bool operator<=(const SimpleVector<Type>& lhs, const SimpleVector<Type>& rhs) {
    return (lhs < rhs || lhs == rhs);
}

template <typename Type>
bool operator>(const SimpleVector<Type>& lhs, const SimpleVector<Type>& rhs) {
    return !(lhs <= rhs);
}

template <typename Type>
bool operator>=(const SimpleVector<Type>& lhs, const SimpleVector<Type>& rhs) {
    return (lhs > rhs || lhs == rhs);
}

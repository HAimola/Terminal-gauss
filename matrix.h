#pragma once
#include <vector>
#include <array>
#include <assert.h>

template<typename T>
class Matrix{
    private:

    void swap_lowest_error(size_t col, Matrix<T> coef){
        size_t biggest_el_index{};
        T biggest_el{};

        for(size_t line = 0; line < (size_t)coef.shape[0]; ++line){
            if(coef[line][col] > biggest_el){
                biggest_el = coef[line][col];
                biggest_el_index = line;
            }
        }

        if(biggest_el_index == 0) return;
        std::vector<T> tmp{coef[0]};
        coef[0] = coef[biggest_el_index];
        coef[biggest_el_index] = tmp;
    }

    public:
    std::vector<std::vector<T>> data;
    size_t pivot;
    std::array<int, 2> shape{};

    Matrix(): data({}), pivot(0){get_shape();}
    
    // Copy constructor
    Matrix(const Matrix& other): data{other.data}, pivot{other.pivot}, shape{other.shape}{}

    // Shape Constructor - Fill with zeros
    Matrix(std::array<size_t, 2> arr){
        fill_with_zeros(arr);
        get_shape();
    }

    Matrix<T> get_coeficients(){
        std::cout << shape[0];
        Matrix<T> coef{*this};
        for(auto &line: coef.data) line.resize(coef.shape[0]-1);
        coef.get_shape();
        std::cout << coef.shape[0];
        return coef;
    }

    void append(size_t line, T item){
        if(line < data.size()){
            for(auto &row: data) row.resize(shape[1]+1, 0);
            data[line][shape[1]] = item;
            get_shape();
        }else throw std::out_of_range("Index was out of range");
    }

    void append(T item){
        data[0].push_back(item);
    }

    void append(std::vector<T> vec){
        data.push_back(vec);
    }

    inline void clear(){
        data.clear();
    }

    void fill_with_zeros(std::array<size_t, 2> new_shape){   
        data.resize(new_shape[0], std::vector<T>{0});
        for(auto &line: data) line.resize(new_shape[1], 0);
    }
    
    std::array<int, 2> get_shape(){
        shape[0] = (int)data.size();
        shape[1] = (int)data[0].size();

        return shape;
    }

    std::vector<T>& operator[](size_t index){
        if(index < data.size()){
            return data[index];
        } else throw std::out_of_range("Index was out of range");
    }

    float det(){
        Matrix<T> coef = get_coeficients();
        [[maybe_unused]] std::array<int, 2> sh{coef.get_shape()};
        std::cout << coef.shape[0] << coef.shape[1];
        //assert(sh[0] == sh[1]);
        
        //swap_lowest_error(0, coef);

        return 0.0f;
    }

};

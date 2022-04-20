#pragma once
#include <vector>
#include <array>
#include <assert.h>

template<typename T>
class Matrix{
    private:

    void swap_lowest_error(size_t col, Matrix<T>& coef){
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
    std::array<int, 2> shape{};

    Matrix(): data({}){get_shape();}
    
    // Copy constructor
    Matrix(const Matrix& other): data{other.data}, shape{other.shape}{}

    // Shape Constructor - Fill with zeros
    Matrix(std::array<int, 2> arr){
        fill_with_zeros(arr);
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

    void fill_with_zeros(std::array<int, 2> new_shape){   
        data.resize((size_t)new_shape[0], std::vector<T>{0});
        for(auto &line: data) line.resize((size_t)new_shape[1], 0);
        shape[0] = new_shape[0];
        shape[1] = new_shape[1];
    }

    inline void resize(std::array<int, 2> new_shape){
        fill_with_zeros(new_shape);
    }

    std::array<int, 2> get_shape(){
        shape[0] = (int)data.size();
        shape[1] = (int)data[0].size();

        return shape;
    }

    double round_to(double value, int digits=10)
    {
        if (value == 0.0) // otherwise it will return 'nan' due to the log10() of zero
            return 0.0;

        double factor = pow(10.0, digits - ceil(log10(fabs(value))));
        return round(value * factor) / factor;   
    }

    void subtract_lines(std::vector<T>& l1, std::vector<T>& l2, double factor){
        for(size_t el = 0; el < l1.size(); ++el){
            l1[el] -= (l2[el]*factor);
            l1[el] = round_to(l1[el],5);
        }
    }

    Matrix<T> gauss_elimination(){
        Matrix<T> m = *this;
        for(size_t line = 0, col = 0; col < (size_t)m.shape[1] - 1; ++line, ++col){
            //swap_lowest_error(col, m);

            if(m[line][col] != 0){
                for(size_t cur_line = line+1; cur_line <(size_t)m.shape[0]; ++cur_line){
                    if(cur_line == 0) continue;
                    double factor = m[cur_line][col]/m[line][col];
                    subtract_lines(m[cur_line], m[line], factor);
                }
            }
        }
        return m;
    }

    // TODO
    std::vector<T> solve_system(){
        Matrix<T> m = gauss_elimination();
        int n_lines = m.shape[0];
        //int n_cols = m.shape[1];

        std::vector<double> ans(n_lines, 0);

        for(int line = n_lines - 1; line >= 0; --line){
            ans[line] = m[line][n_lines]; // Termo independentte
            for(int col = line+1; col <= n_lines; ++col){
                ans[line] -= m[line][col]*ans[col];
            }
        }

        //for(int nline = n_lines-2, col = n_cols-2; col >= 0; --col, --nline){
         //    ans[nline] =  (gauss_m[nline][n_cols-1] - sums[nline] )/ gauss_m[nline][col] ;
        //} 
        return ans;
    }

    double det(){
        double d = 1;
        if(shape[0] == shape[1]){
            for(size_t line = 0, col = 0; col < (size_t)shape[1]; ++line, ++col){
                d *= data[line][col];
            }
        }
        return d;
    }

    // OPERATORS

    std::vector<T>& operator[](size_t index){
        if(index < data.size()){
            return data[index];
        } else throw std::out_of_range("Index was out of range");
    }
};
